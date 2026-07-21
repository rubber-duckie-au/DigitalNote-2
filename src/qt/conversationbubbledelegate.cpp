#include <QPainter>
#include <QApplication>
#include <QFontMetrics>
#include <QTextOption>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QImage>
#include <QUrl>
#include <QPalette>

#include "conversationbubbledelegate.h"
#include "conversationthreadmodel.h"
#include "xdnemoji.h"

// SMSG two-pane redesign, step 2 (+ step 3 custom emoji). See header for rationale.

// Build a QTextDocument for a message body, ready to lay out / paint. The body may be a full
// HTML document (from the compose editor), a fragment, or plain text. We resolve XDN emoji
// shortcodes (:xdn:) into <img> tags pointing at bundled resources, register those images on
// the document, set the wrap width and base font/colour, and hand back the document. Both
// paint() and sizeHint() use this so geometry and drawing always agree.
//
// Rendering via QTextDocument (instead of drawText) is what lets inline emoji images and
// basic rich text (bold/italic) show in bubbles.
static void buildBubbleDoc(QTextDocument &doc, const QString &raw, const QFont &font,
                           const QColor &textColor, int wrapWidth)
{
	QString trimmed = raw.trimmed();

	bool looksLikeHtml = trimmed.startsWith("<!DOCTYPE", Qt::CaseInsensitive)
	                  || trimmed.startsWith("<html", Qt::CaseInsensitive)
	                  || (trimmed.contains("<p", Qt::CaseInsensitive) && trimmed.contains("</p>", Qt::CaseInsensitive))
	                  || (trimmed.contains("<body", Qt::CaseInsensitive));

	// Start from the message as HTML. If it is plain text, escape it so stray < or & do not
	// get interpreted, then it is safe to inject emoji <img> tags.
	QString html = looksLikeHtml ? raw : raw.toHtmlEscaped();

	// Resolve emoji shortcodes to <img> tags (sized to the current font height).
	QFontMetrics fm(font);
	int emojiPx = fm.height() + 2;
	html = XdnEmoji::substituteShortcodesToHtml(html, emojiPx);

	// Register each bundled emoji image on the document so the <img> resource paths resolve.
	const QList<XdnEmoji::Entry> &entries = XdnEmoji::all();
	for(int i = 0; i < entries.size(); i++)
	{
		QImage img(entries.at(i).resourcePath);
		if(!img.isNull())
		{
			doc.addResource(QTextDocument::ImageResource, QUrl(entries.at(i).resourcePath), img);
		}
	}

	doc.setDefaultFont(font);

	// Base text colour via a wrapping span (QTextDocument has no single colour setter that
	// survives setHtml; a span is the simplest reliable way).
	QString colored = QString("<span style=\"color:%1;\">%2</span>")
	                      .arg(textColor.name())
	                      .arg(html);

	doc.setHtml(colored);
	doc.setTextWidth(wrapWidth);
}

ConversationBubbleDelegate::ConversationBubbleDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

void ConversationBubbleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	bool isSent      = index.data(ConversationThreadModel::IsSentRole).toBool();
	QString rawText  = index.data(ConversationThreadModel::TextRole).toString();
	QDateTime time   = index.data(ConversationThreadModel::TimeRole).toDateTime();
	QString timeText = time.isValid() ? time.toString("MMM d, h:mm ap") : QString();

	int viewportWidth = option.rect.width();
	int maxBubbleWidth = (viewportWidth * BubbleMaxWidthPct) / 100;
	int maxTextWidth = maxBubbleWidth - (2 * BubblePadding);

	if(maxTextWidth < 40)
	{
		maxTextWidth = 40;
	}

	QFont textFont = option.font;
	QFont timeFont = option.font;
	timeFont.setPointSizeF(timeFont.pointSizeF() > 1.0 ? timeFont.pointSizeF() - 1.5 : timeFont.pointSizeF());

	// Colours. Sent = accent blue, received = neutral grey. Selected = slightly darker.
	QColor bubbleColor = isSent ? QColor(0x2f, 0x6f, 0xed) : QColor(0xe9, 0xe9, 0xeb);
	QColor textColor   = isSent ? QColor(0xff, 0xff, 0xff) : QColor(0x11, 0x11, 0x11);
	QColor timeColor   = isSent ? QColor(0xd8, 0xe4, 0xff) : QColor(0x88, 0x88, 0x88);

	if(option.state & QStyle::State_Selected)
	{
		bubbleColor = bubbleColor.darker(110);
	}

	// Lay out the message (with inline emoji) to find its natural size.
	QTextDocument doc;
	buildBubbleDoc(doc, rawText, textFont, textColor, maxTextWidth);

	int textW = (int)doc.idealWidth();
	int textH = (int)doc.size().height();

	QFontMetrics timeFm(timeFont);
	int timeWidth = timeText.isEmpty() ? 0 : timeFm.horizontalAdvance(timeText);
	int timeHeight = timeText.isEmpty() ? 0 : (timeFm.height() + TimeGap);

	int contentWidth = qMax(textW, timeWidth);
	int bubbleWidth = contentWidth + (2 * BubblePadding);
	int bubbleHeight = textH + timeHeight + (2 * BubblePadding);

	// Position: sent -> right, received -> left.
	int bubbleX;

	if(isSent)
	{
		bubbleX = option.rect.right() - BubbleMargin - bubbleWidth;
	}
	else
	{
		bubbleX = option.rect.left() + BubbleMargin;
	}

	int bubbleY = option.rect.top() + BubbleMargin;
	QRect bubbleRect(bubbleX, bubbleY, bubbleWidth, bubbleHeight);

	// Bubble.
	painter->setPen(Qt::NoPen);
	painter->setBrush(bubbleColor);
	painter->drawRoundedRect(bubbleRect, BubbleRadius, BubbleRadius);

	// Message body (rich text + inline emoji), drawn via the document layout.
	painter->save();
	painter->translate(bubbleRect.left() + BubblePadding, bubbleRect.top() + BubblePadding);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette.setColor(QPalette::Text, textColor);
	QRect clip(0, 0, contentWidth, textH);
	ctx.clip = clip;
	doc.documentLayout()->draw(painter, ctx);

	painter->restore();

	// Timestamp beneath the text, aligned to the bubble's trailing edge.
	if(!timeText.isEmpty())
	{
		QRect timeRect(bubbleRect.left() + BubblePadding,
		               bubbleRect.top() + BubblePadding + textH + TimeGap,
		               contentWidth,
		               timeFm.height());
		painter->setPen(timeColor);
		painter->setFont(timeFont);
		painter->drawText(timeRect, (isSent ? Qt::AlignRight : Qt::AlignLeft), timeText);
	}

	painter->restore();
}

QSize ConversationBubbleDelegate::sizeHint(const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
	QString rawText = index.data(ConversationThreadModel::TextRole).toString();
	QDateTime time  = index.data(ConversationThreadModel::TimeRole).toDateTime();

	int viewportWidth = option.rect.width();

	// option.rect may be empty during initial layout; fall back to a sane default so the row
	// still gets a usable height.
	if(viewportWidth <= 0)
	{
		viewportWidth = 400;
	}

	int maxBubbleWidth = (viewportWidth * BubbleMaxWidthPct) / 100;
	int maxTextWidth = maxBubbleWidth - (2 * BubblePadding);

	if(maxTextWidth < 40)
	{
		maxTextWidth = 40;
	}

	QTextDocument doc;
	buildBubbleDoc(doc, rawText, option.font, QColor(0x11, 0x11, 0x11), maxTextWidth);
	int textH = (int)doc.size().height();

	QFont timeFont = option.font;
	QFontMetrics timeFm(timeFont);
	int timeHeight = time.isValid() ? (timeFm.height() + TimeGap) : 0;

	int height = textH + timeHeight + (2 * BubblePadding) + (2 * BubbleMargin);

	return QSize(viewportWidth, height);
}
