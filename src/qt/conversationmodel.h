#ifndef CONVERSATIONMODEL_H
#define CONVERSATIONMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QDateTime>

class MessageModel;

// ConversationModel (SMSG two-pane redesign, step 1).
//
// A list model that groups the flat MessageModel's rows by COUNTERPARTY (the other party in
// each message) and exposes one row per conversation for the left pane. It reads the flat
// model through its public role interface (FromAddressRole/ToAddressRole/TypeRole/
// ReceivedDateRole/LabelRole/MessageRole) -- it does NOT duplicate the SMSG store access, so
// the existing load/live-update path stays authoritative. It rebuilds on the flat model's
// rowsInserted / modelReset signals.
//
// This intentionally replaces the fragile flat-table + QSortFilterProxyModel(Ambiguous filter)
// plumbing for the inbox list: a concrete, fully-populated list model has no proxy initial-
// mapping or dynamic-sort-on-insert pitfalls.

class ConversationModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		explicit ConversationModel(MessageModel *messageModel, QObject *parent = 0);

		// One conversation = one (my-address <-> their-address) channel. Keyed on the full
		// address PAIR, not the counterparty alone: in SMSG your addresses are distinct
		// identities, and grouping by counterparty only would merge separate channels and
		// force a reply to guess which of your addresses to send from (risking mis-send).
		// Keying on the pair keeps each channel separate and lets a reply always use the
		// address that channel already uses.
		struct Conversation
		{
			QString		pairKey;			// direction-normalized "their+mine" channel key
			QString		counterparty;		// the other party's address
			QString		myAddress;			// MY address used in this channel
			QString		label;				// counterparty address-book label if known
			QString		myLabel;			// my-address address-book label if known
			QString		lastPreview;		// preview text of the most recent message
			QDateTime	lastMessageTime;	// time of the most recent message (for sort)
			int			messageCount;		// total messages in this conversation

			Conversation() : messageCount(0) {}
		};

		// Roles exposed to the left-pane view / delegate.
		enum ConversationRole
		{
			CounterpartyRole = Qt::UserRole + 1,	// QString their address
			MyAddressRole,							// QString my address used in this channel
			PairKeyRole,							// QString direction-normalized channel key
			LabelRole,								// QString their label (may be empty)
			MyLabelRole,							// QString my label (may be empty)
			DisplayNameRole,						// their label if present else truncated address
			CounterpartyDisplayRole,				// "LABEL (address)" or "address" for them
			MyAddressDisplayRole,					// "LABEL (address)" or "address" for me (full)
			LastPreviewRole,						// QString preview
			LastTimeRole,							// QDateTime
			MessageCountRole						// int
		};

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;

		// The counterparty / my-address / channel key for a given row (for wiring the
		// right-pane thread and ensuring a reply uses the correct sending address).
		QString counterpartyAt(int row) const;
		QString myAddressAt(int row) const;
		QString pairKeyAt(int row) const;

	public slots:
		// Rebuild the grouped list from the flat MessageModel. Cheap: one pass over the rows.
		void rebuild();

	private:
		MessageModel			*messageModel;
		QList<Conversation>		conversations;

		// Helper: shorten an address for display (e.g. "dWTe...cfvi6").
		static QString truncateAddress(const QString &address);
};

#endif // CONVERSATIONMODEL_H
