#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextOption>
#include <QImage>
#include <QUrl>
#include <QApplication>
#include <QFontMetrics>
#include <QDateTime>

#include "conversationlistdelegate.h"
#include "conversationmodel.h"
#include "xdnemoji.h"

// SMSG two-pane redesign, step 2 polish. See conversationlistdelegate.h for rationale.

ConversationListDelegate::ConversationListDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

void ConversationListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	QString title   = index.data(ConversationModel::CounterpartyDisplayRole).toString();
	QString preview = index.data(ConversationModel::LastPreviewRole).toString().replace('\n', ' ');
	QString viaStr  = index.data(ConversationModel::MyAddressDisplayRole).toString();
	QDateTime time  = index.data(ConversationModel::LastTimeRole).toDateTime();
	QString timeStr = time.isValid() ? time.toString("MMM d, h:mm ap") : QString();

	QRect r = option.rect.adjusted(RowPadding, RowPadding, -RowPadding, -RowPadding);

	// Selected / hover background.
	if(option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, QColor(0x2f, 0x6f, 0xed, 0x22));
	}
	else if(option.state & QStyle::State_MouseOver)
	{
		painter->fillRect(option.rect, QColor(0x00, 0x00, 0x00, 0x0c));
	}

	// Bottom separator line.
	painter->setPen(QColor(0x00, 0x00, 0x00, 0x14));
	painter->drawLine(option.rect.left() + RowPadding, option.rect.bottom(),
	                  option.rect.right() - RowPadding, option.rect.bottom());

	QFont titleFont = option.font;
	titleFont.setBold(true);

	QFont previewFont = option.font;

	QFont smallFont = option.font;
	smallFont.setPointSizeF(smallFont.pointSizeF() > 1.0 ? smallFont.pointSizeF() - 1.5 : smallFont.pointSizeF());

	QFontMetrics titleFm(titleFont);
	QFontMetrics previewFm(previewFont);
	QFontMetrics smallFm(smallFont);

	int timeWidth = timeStr.isEmpty() ? 0 : smallFm.horizontalAdvance(timeStr);

	int y = r.top();

	// Line 1: counterparty "LABEL (address)" (bold, elided) + last time (right).
	QRect titleRect(r.left(), y, r.width() - timeWidth - 6, titleFm.height());
	painter->setPen(QColor(0x11, 0x11, 0x11));
	painter->setFont(titleFont);
	painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter,
	                  titleFm.elidedText(title, Qt::ElideMiddle, titleRect.width()));

	if(!timeStr.isEmpty())
	{
		QRect timeRect(r.right() - timeWidth, y, timeWidth, titleFm.height());
		painter->setPen(QColor(0x88, 0x88, 0x88));
		painter->setFont(smallFont);
		painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, timeStr);
	}

	y += titleFm.height() + 3;

	// Line 2: preview (muted). Rendered via a small QTextDocument so XDN custom-emoji
	// shortcodes (:xdn:) show as their bundled images inline, matching the message bubbles.
	// Standard Unicode emoji render either way. Clipped to a single line height.
	{
		QString previewHtml = XdnEmoji::substituteShortcodesToHtml(preview.toHtmlEscaped(),
		                                                            previewFm.height());

		QTextDocument doc;
		doc.setDefaultFont(previewFont);

		// Keep the preview on a single line: disable wrapping so long text clips at the right
		// edge (like elision) instead of wrapping onto a second, clipped line.
		QTextOption previewOpt;
		previewOpt.setWrapMode(QTextOption::NoWrap);
		doc.setDefaultTextOption(previewOpt);

		const QList<XdnEmoji::Entry> &entries = XdnEmoji::all();
		for(int i = 0; i < entries.size(); i++)
		{
			QImage img(entries.at(i).resourcePath);
			if(!img.isNull())
			{
				doc.addResource(QTextDocument::ImageResource, QUrl(entries.at(i).resourcePath), img);
			}
		}

		doc.setHtml(QString("<span style=\"color:#555555;\">%1</span>").arg(previewHtml));
		doc.setTextWidth(r.width());

		painter->save();
		painter->translate(r.left(), y);
		QRect clip(0, 0, r.width(), previewFm.height() + 2);
		QAbstractTextDocumentLayout::PaintContext ctx;
		ctx.clip = clip;
		doc.documentLayout()->draw(painter, ctx);
		painter->restore();
	}

	y += previewFm.height() + 3;

	// Line 3: "via <my LABEL (address) or address>" -- full, always shown. Distinguishes
	// channels that share the same counterparty but use different addresses of mine.
	QRect viaRect(r.left(), y, r.width(), smallFm.height());
	painter->setPen(QColor(0x99, 0x99, 0x99));
	painter->setFont(smallFont);
	painter->drawText(viaRect, Qt::AlignLeft | Qt::AlignVCenter,
	                  smallFm.elidedText(QString("via ") + viaStr, Qt::ElideMiddle, viaRect.width()));

	painter->restore();
}

QSize ConversationListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(0, RowHeight);
}
