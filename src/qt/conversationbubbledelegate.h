#ifndef CONVERSATIONBUBBLEDELEGATE_H
#define CONVERSATIONBUBBLEDELEGATE_H

#include <QStyledItemDelegate>

// ConversationBubbleDelegate (SMSG two-pane redesign, step 2).
//
// Paints a chat-style message bubble for one ConversationThreadModel row:
//   * Sent messages  -> right-aligned, accent-coloured bubble.
//   * Received msgs   -> left-aligned, neutral-coloured bubble.
//   * Timestamp in a small muted font beneath the text.
//
// Replaces the old MessageViewDelegate's dated "<datetime><br><label><br><message>" HTML
// paragraph. Reads ConversationThreadModel roles (IsSentRole/TextRole/TimeRole). Text is drawn
// with the widget's default font stack, so Unicode emoji render in colour where the platform
// font supports it (e.g. Segoe UI Emoji on Windows) with no special handling.

class ConversationBubbleDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	public:
		explicit ConversationBubbleDelegate(QObject *parent = 0);

		void paint(QPainter *painter, const QStyleOptionViewItem &option,
		           const QModelIndex &index) const;
		QSize sizeHint(const QStyleOptionViewItem &option,
		               const QModelIndex &index) const;

	private:
		// Layout constants (device-independent pixels).
		static const int BubbleMaxWidthPct = 72;	// bubble max width as % of viewport
		static const int BubblePadding     = 10;	// text inset inside the bubble
		static const int BubbleMargin      = 8;		// gap around the bubble
		static const int BubbleRadius      = 10;	// corner radius
		static const int TimeGap           = 4;		// gap between text and timestamp

		// Compute the text rectangle (word-wrapped) for a given available width.
};

#endif // CONVERSATIONBUBBLEDELEGATE_H
