#ifndef CONVERSATIONLISTDELEGATE_H
#define CONVERSATIONLISTDELEGATE_H

#include <QStyledItemDelegate>

// ConversationListDelegate (SMSG two-pane redesign, step 2 polish).
//
// Paints one conversation row in the left/inbox list (ConversationModel), messenger-style:
//   line 1: contact display name (bold)            [right: last message time]
//   line 2: last message preview (muted, elided)
//   line 3: "via <my-address short>" (small)  -- distinguishes channels that share the same
//           counterparty but use different addresses of mine.
//
// Reads ConversationModel roles (DisplayNameRole/LastPreviewRole/LastTimeRole/MyAddressRole).

class ConversationListDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	public:
		explicit ConversationListDelegate(QObject *parent = 0);

		void paint(QPainter *painter, const QStyleOptionViewItem &option,
		           const QModelIndex &index) const;
		QSize sizeHint(const QStyleOptionViewItem &option,
		               const QModelIndex &index) const;

	private:
		static const int RowPadding = 8;
		static const int RowHeight  = 68;

};

#endif // CONVERSATIONLISTDELEGATE_H
