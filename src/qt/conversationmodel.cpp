#include <QHash>
#include <QTextDocument>
#include <QDebug>
#include <algorithm>

#include "conversationmodel.h"
#include "messagemodel.h"
#include "walletmodel.h"
#include "addresstablemodel.h"

// SMSG two-pane redesign, step 1. See conversationmodel.h for rationale.

// Format a party for display: "LABEL (address)" when a label exists, otherwise just the
// full address. Used for both the counterparty and my-address lines.
static QString formatParty(const QString &label, const QString &address)
{
	if(label.isEmpty())
	{
		return address;
	}

	return label + " (" + address + ")";
}

// Message bodies may be stored as rich text (a full HTML document from the compose editor).
// For the conversation preview we want plain text: if the body looks like HTML, extract the
// visible text via QTextDocument; otherwise use it as-is. Mirrors the bubble delegate.
static QString previewPlainText(const QString &raw)
{
	QString trimmed = raw.trimmed();

	bool looksLikeHtml = trimmed.startsWith("<!DOCTYPE", Qt::CaseInsensitive)
	                  || trimmed.startsWith("<html", Qt::CaseInsensitive)
	                  || (trimmed.contains("<p", Qt::CaseInsensitive) && trimmed.contains("</p>", Qt::CaseInsensitive))
	                  || (trimmed.contains("<body", Qt::CaseInsensitive));

	if(!looksLikeHtml)
	{
		return raw;
	}

	QTextDocument doc;
	doc.setHtml(raw);

	return doc.toPlainText().simplified();
}

ConversationModel::ConversationModel(MessageModel *messageModel, QObject *parent) :
	QAbstractListModel(parent),
	messageModel(messageModel)
{
	// Rebuild whenever the flat model changes (initial load already happened in the flat
	// model's ctor, so build once now too).
	if(messageModel)
	{
		connect(messageModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),  this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(modelReset()),                      this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(layoutChanged()),                   this, SLOT(rebuild()));
		// After the wallet is unlocked the flat model repopulates (messages are encrypted at
		// rest) and emits refreshed(); rebuild the conversation grouping then. This is the
		// key connection -- at construction the wallet is typically still locked, so the flat
		// model has 0 rows until unlock.
		connect(messageModel, SIGNAL(refreshed()),                       this, SLOT(rebuild()));
	}

	rebuild();
}

QString ConversationModel::truncateAddress(const QString &address)
{
	// "dWTePkmRjXd...Uxijkecfvi6" style short form.
	if(address.length() <= 16)
	{
		return address;
	}

	return address.left(8) + "..." + address.right(6);
}

void ConversationModel::rebuild()
{
	beginResetModel();

	conversations.clear();

	if(messageModel != NULL)
	{
		QHash<QString, int> indexByPairKey;	// pairKey -> index into conversations

		int rows = messageModel->rowCount(QModelIndex());

		for(int i = 0; i < rows; i++)
		{
			QModelIndex idx = messageModel->index(i, 0, QModelIndex());

			int type = messageModel->data(idx, MessageModel::TypeRole).toInt();
			QString fromAddress = messageModel->data(idx, MessageModel::FromAddressRole).toString();
			QString toAddress   = messageModel->data(idx, MessageModel::ToAddressRole).toString();
			QString label       = messageModel->data(idx, MessageModel::LabelRole).toString();
			QString message     = messageModel->data(idx, MessageModel::MessageRole).toString();
			QDateTime received  = messageModel->data(idx, MessageModel::ReceivedDateRole).toDateTime();

			// Counterparty = the OTHER party; myAddress = the address on MY side of this
			// message. For a Sent message: I am from_address, they are to_address. For a
			// Received message: they are from_address, I am to_address.
			QString counterparty = (type == MessageTableEntry::Sent) ? toAddress : fromAddress;
			QString myAddress    = (type == MessageTableEntry::Sent) ? fromAddress : toAddress;

			// Look up the address-book label for MY address (the message's LabelRole is the
			// counterparty's label). Guarded: wallet/address-book may be unavailable.
			QString myLabel;
			if(messageModel->getWalletModel() && messageModel->getWalletModel()->getAddressTableModel())
			{
				myLabel = messageModel->getWalletModel()->getAddressTableModel()->labelForAddress(myAddress);
			}

			// Channel key = direction-normalized address PAIR. Built as their+mine in both
			// directions so a Sent and a Received message on the same channel collapse
			// together, but different my-addresses (or different counterparties) stay
			// separate. This matches MessageModel::FilterAddressRole.
			QString pairKey = counterparty + myAddress;

			int convIndex;

			if(indexByPairKey.contains(pairKey))
			{
				convIndex = indexByPairKey.value(pairKey);
			}
			else
			{
				Conversation conv;
				conv.pairKey = pairKey;
				conv.counterparty = counterparty;
				conv.myAddress = myAddress;
				conv.label = label;
				conv.myLabel = myLabel;
				conv.messageCount = 0;

				conversations.append(conv);
				convIndex = conversations.size() - 1;
				indexByPairKey.insert(pairKey, convIndex);
			}

			Conversation &conv = conversations[convIndex];
			conv.messageCount++;

			// Keep a label if this row has one and we don't yet.
			if(conv.label.isEmpty() && !label.isEmpty())
			{
				conv.label = label;
			}

			// Track the most-recent message for preview + sort.
			if(!conv.lastMessageTime.isValid() || received >= conv.lastMessageTime)
			{
				conv.lastMessageTime = received;
				conv.lastPreview = previewPlainText(message);
			}
		}
	}

	// Most-recent conversation first.
	std::stable_sort(conversations.begin(), conversations.end(),
		[](const Conversation &a, const Conversation &b)
		{
			return a.lastMessageTime > b.lastMessageTime;
		});

	// Step-1 validation: report grouping result (remove once the two-pane view lands).
	qWarning() << "ConversationModel::rebuild ->" << conversations.size() << "conversations from"
	           << (messageModel ? messageModel->rowCount(QModelIndex()) : 0) << "messages";
	for(int c = 0; c < conversations.size(); c++)
	{
		const Conversation &cv = conversations.at(c);
		qWarning() << "  conv" << c << ":" << (cv.label.isEmpty() ? truncateAddress(cv.counterparty) : cv.label)
		           << "them=" << truncateAddress(cv.counterparty) << "me=" << truncateAddress(cv.myAddress)
		           << cv.messageCount << "msgs, last" << cv.lastMessageTime.toString();
	}

	endResetModel();
}

int ConversationModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
	{
		return 0;
	}

	return conversations.size();
}

QVariant ConversationModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() < 0 || index.row() >= conversations.size())
	{
		return QVariant();
	}

	const Conversation &conv = conversations.at(index.row());

	switch(role)
	{
		case CounterpartyRole:
			return conv.counterparty;

		case MyAddressRole:
			return conv.myAddress;

		case PairKeyRole:
			return conv.pairKey;

		case LabelRole:
			return conv.label;

		case MyLabelRole:
			return conv.myLabel;

		case DisplayNameRole:
			return conv.label.isEmpty() ? truncateAddress(conv.counterparty) : conv.label;

		case CounterpartyDisplayRole:
			return formatParty(conv.label, conv.counterparty);

		case MyAddressDisplayRole:
			return formatParty(conv.myLabel, conv.myAddress);

		case LastPreviewRole:
			return conv.lastPreview;

		case LastTimeRole:
			return conv.lastMessageTime;

		case MessageCountRole:
			return conv.messageCount;

		case Qt::DisplayRole:
			// Fallback for a plain QListView with no custom delegate: "Name - preview".
			return (conv.label.isEmpty() ? truncateAddress(conv.counterparty) : conv.label)
				+ "  -  " + conv.lastPreview;

		default:
			return QVariant();
	}
}

QString ConversationModel::counterpartyAt(int row) const
{
	if(row < 0 || row >= conversations.size())
	{
		return QString();
	}

	return conversations.at(row).counterparty;
}

QString ConversationModel::myAddressAt(int row) const
{
	if(row < 0 || row >= conversations.size())
	{
		return QString();
	}

	return conversations.at(row).myAddress;
}

QString ConversationModel::pairKeyAt(int row) const
{
	if(row < 0 || row >= conversations.size())
	{
		return QString();
	}

	return conversations.at(row).pairKey;
}
