// v2.0.0.9 Qt6: QRegExp removed from qtbase.
#include <QRegularExpression>
#include <QImage>

#include "xdnemoji.h"

// SMSG two-pane redesign, step 3 - custom emoji. See xdnemoji.h for rationale.

QList<XdnEmoji::Entry>   XdnEmoji::s_entries;
QMap<QString, QString>   XdnEmoji::s_byShortcode;
bool                     XdnEmoji::s_init = false;

void XdnEmoji::ensureInit()
{
	if(s_init)
	{
		return;
	}

	// The registered XDN custom emoji. Shortcode -> bundled resource (:/emoji/<name>).
	// Images are provided in src/qt/res/emoji/<name>.png and aliased in bitcoin.qrc under
	// the /emoji prefix. To add more: drop the PNG in res/emoji/, add a <file> line to the
	// .qrc /emoji section, and add an entry below.
	s_entries.append(Entry(":DN:",       "DN",       ":/emoji/DN"));
	s_entries.append(Entry(":moon:",     "moon",     ":/emoji/moon"));
	s_entries.append(Entry(":xdn:",      "xdn",      ":/emoji/xdn"));
	s_entries.append(Entry(":xdnwhite:", "xdnwhite", ":/emoji/xdnwhite"));

	for(int i = 0; i < s_entries.size(); i++)
	{
		s_byShortcode.insert(s_entries.at(i).shortcode, s_entries.at(i).resourcePath);
	}

	s_init = true;
}

const QList<XdnEmoji::Entry>& XdnEmoji::all()
{
	ensureInit();

	return s_entries;
}

QString XdnEmoji::resourceForShortcode(const QString &shortcode)
{
	ensureInit();

	return s_byShortcode.value(shortcode, QString());
}

bool XdnEmoji::isShortcode(const QString &shortcode)
{
	ensureInit();

	return s_byShortcode.contains(shortcode);
}

QString XdnEmoji::substituteShortcodesToHtml(const QString &text, int emojiPx)
{
	ensureInit();

	QString out = text;

	// Replace each known shortcode with an <img> tag pointing at the bundled resource. Simple
	// literal replacement per registered shortcode -- the set is small and the shortcodes are
	// distinctive (colon-delimited), so this is safe and cheap.
	for(int i = 0; i < s_entries.size(); i++)
	{
		const Entry &e = s_entries.at(i);

		// Preserve the image's aspect ratio: fix the height to emojiPx and derive the width
		// from the actual image dimensions (some XDN emoji are non-square, e.g. the wordmark).
		int w = emojiPx;
		int h = emojiPx;

		QImage img(e.resourcePath);
		if(!img.isNull() && img.height() > 0)
		{
			w = (img.width() * emojiPx) / img.height();
		}

		QString imgTag = QString("<img src=\"%1\" width=\"%2\" height=\"%3\" />")
		                     .arg(e.resourcePath)
		                     .arg(w)
		                     .arg(h);

		out.replace(e.shortcode, imgTag);
	}

	return out;
}

QString XdnEmoji::substituteImagesToShortcodes(const QString &html)
{
	ensureInit();

	QString out = html;

	// Replace any <img ... src=":/emoji/name" ...> with its shortcode. Match on the resource
	// path so it works regardless of attribute order or sizing.
	for(int i = 0; i < s_entries.size(); i++)
	{
		const Entry &e = s_entries.at(i);

		QRegularExpression rx(QString("<img[^>]*src=\"%1\"[^>]*/?>").arg(QRegularExpression::escape(e.resourcePath)));
		out.replace(rx, e.shortcode);
	}

	return out;
}
