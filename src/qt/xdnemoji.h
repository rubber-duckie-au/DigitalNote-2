#ifndef XDNEMOJI_H
#define XDNEMOJI_H

#include <QString>
#include <QList>
#include <QMap>

// XdnEmoji (SMSG two-pane redesign, step 3 - custom emoji).
//
// Registry of XDN-specific custom emoji. Each is referenced on the wire by a compact
// shortcode (e.g. ":xdn:") -- NOT by embedding the image -- so a message carries only a few
// bytes regardless of how many custom emoji it uses. This keeps SMSG payloads small (the
// proof-of-work hashes the whole payload, so bloat costs sender CPU + relay bandwidth).
//
// The images are bundled in the wallet as Qt resources (:/emoji/<name>), so both sides
// resolve the same shortcode to the same bundled image with no network dependency. Display
// code (compose editor, bubble delegate, list preview) resolves ":name:" -> resource path;
// plain-text extraction converts the image back to ":name:".

class XdnEmoji
{
	public:
		struct Entry
		{
			QString shortcode;		// e.g. ":xdn:"
			QString name;			// e.g. "xdn"
			QString resourcePath;	// e.g. ":/emoji/xdn"

			Entry() {}
			Entry(const QString &sc, const QString &nm, const QString &rp)
				: shortcode(sc), name(nm), resourcePath(rp) {}
		};

		// The full registered set (for the picker).
		static const QList<Entry>& all();

		// Resolve a shortcode (":xdn:") to its bundled resource path, or empty if unknown.
		static QString resourceForShortcode(const QString &shortcode);

		// True if the registry contains this shortcode.
		static bool isShortcode(const QString &shortcode);

		// Replace every known :shortcode: in an HTML/text string with an <img> tag pointing
		// at the bundled resource, sized to emojiPx. Unknown :tokens: are left untouched.
		// Used by display code (bubbles, previews) so a message stored with ":xdn:" renders
		// the bundled image inline.
		static QString substituteShortcodesToHtml(const QString &text, int emojiPx = 18);

		// Inverse: replace <img src=":/emoji/name"...> back to :shortcode: for plain-text
		// extraction (e.g. list previews that want text, or copy-to-clipboard).
		static QString substituteImagesToShortcodes(const QString &html);

	private:
		static void ensureInit();

		static QList<Entry>          s_entries;
		static QMap<QString, QString> s_byShortcode;	// shortcode -> resourcePath
		static bool                  s_init;
};

#endif // XDNEMOJI_H
