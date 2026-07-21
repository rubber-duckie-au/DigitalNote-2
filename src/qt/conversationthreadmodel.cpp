#include <algorithm>

#include "conversationthreadmodel.h"
#include "messagemodel.h"

// SMSG two-pane redesign, step 2. See conversationthreadmodel.h for rationale.

ConversationThreadModel::ConversationThreadModel(MessageModel *messageModel, QObject *parent) :
	QAbstractListModel(parent),
	messageModel(messageModel)
{
	if(messageModel)
	{
		// Rebuild when the flat model changes -- notably refreshed() after wallet unlock, and
		// rowsInserted when a new message arrives for the active channel.
		connect(messageModel, SIGNAL(refreshed()),                       this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),  this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(modelReset()),                      this, SLOT(rebuild()));
		connect(messageModel, SIGNAL(layoutChanged()),                   this, SLOT(rebuild()));
	}
}

void ConversationThreadModel::setPairKey(const QString &pairKey)
{
	if(activePairKey == pairKey)
	{
		return;
	}

	activePairKey = pairKey;
	rebuild();
}

QString ConversationThreadModel::pairKey() const
{
	return activePairKey;
}

void ConversationThreadModel::rebuild()
{
	beginResetModel();

	messages.clear();

	if(messageModel != NULL && !activePairKey.isEmpty())
	{
		int rows = messageModel->rowCount(QModelIndex());

		for(int i = 0; i < rows; i++)
		{
			QModelIndex idx = messageModel->index(i, 0, QModelIndex());

			// A message belongs to this channel iff its direction-normalized pair key
			// (their+mine) matches the active channel. FilterAddressRole is exactly that key.
			QString rowPairKey = messageModel->data(idx, MessageModel::FilterAddressRole).toString();

			if(rowPairKey != activePairKey)
			{
				continue;
			}

			ThreadMessage m;
			m.isSent = (messageModel->data(idx, MessageModel::TypeRole).toInt() == MessageTableEntry::Sent);
			m.text   = messageModel->data(idx, MessageModel::MessageRole).toString();
			m.time   = messageModel->data(idx, MessageModel::ReceivedDateRole).toDateTime();

			messages.append(m);
		}
	}

	// Oldest at top, newest at bottom (chat-log order).
	std::stable_sort(messages.begin(), messages.end(),
		[](const ThreadMessage &a, const ThreadMessage &b)
		{
			return a.time < b.time;
		});

	endResetModel();
}

int ConversationThreadModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
	{
		return 0;
	}

	return messages.size();
}

QVariant ConversationThreadModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() < 0 || index.row() >= messages.size())
	{
		return QVariant();
	}

	const ThreadMessage &m = messages.at(index.row());

	switch(role)
	{
		case IsSentRole:
			return m.isSent;

		case TextRole:
			return m.text;

		case TimeRole:
			return m.time;

		case Qt::DisplayRole:
			// Fallback for a plain view with no bubble delegate yet.
			return (m.isSent ? QString("> ") : QString("< ")) + m.text;

		default:
			return QVariant();
	}
}
