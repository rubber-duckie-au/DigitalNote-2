#ifndef CONVERSATIONTHREADMODEL_H
#define CONVERSATIONTHREADMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QDateTime>

class MessageModel;

// ConversationThreadModel (SMSG two-pane redesign, step 2).
//
// The right-pane model: the messages of ONE selected conversation channel, oldest -> newest.
// A channel is identified by its pair key (their-address + my-address, direction-normalized),
// the same key ConversationModel groups on and the same value as MessageModel's
// FilterAddressRole. Set the active channel with setPairKey(); the model then exposes just
// that channel's messages for a bubble-style thread view.
//
// Like ConversationModel it reads the flat MessageModel through its role interface (no proxy,
// no duplication of the SMSG store) and rebuilds on the flat model's refreshed()/row signals.

class ConversationThreadModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		explicit ConversationThreadModel(MessageModel *messageModel, QObject *parent = 0);

		// One message in the thread.
		struct ThreadMessage
		{
			bool		isSent;			// true = sent by me (right), false = received (left)
			QString		text;			// message body
			QDateTime	time;			// received_datetime (used for ordering + display)

			ThreadMessage() : isSent(false) {}
		};

		// Roles for the bubble delegate.
		enum ThreadRole
		{
			IsSentRole = Qt::UserRole + 1,	// bool
			TextRole,						// QString body
			TimeRole						// QDateTime
		};

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;

		// Which channel this thread shows (empty = none).
		QString pairKey() const;

	public slots:
		// Select the channel to display (pair key = their+mine). Triggers a rebuild.
		void setPairKey(const QString &pairKey);

		// Rebuild the thread for the current pair key from the flat MessageModel.
		void rebuild();

	private:
		MessageModel			*messageModel;
		QString					activePairKey;
		QList<ThreadMessage>	messages;
};

#endif // CONVERSATIONTHREADMODEL_H
