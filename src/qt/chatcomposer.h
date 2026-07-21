#ifndef CHATCOMPOSER_H
#define CHATCOMPOSER_H

#include <QWidget>
#include <QString>

class QTextEdit;
class QToolBar;
class QAction;
class QToolButton;
class QTextCharFormat;

// ChatComposer (SMSG two-pane redesign, step 3).
//
// A compact, modern message compose widget for the messaging page: a slim formatting toolbar
// (bold / italic / underline / strikeout / link) plus an emoji picker, over a height-reduced
// QTextEdit. Replaces the dated word-processor ribbon (MRichTextEdit) for composing, while
// keeping the same essential interface (toPlainText / toHtml / clear / setText) so the send
// path is unchanged.
//
// Format seam: messageHtml() is the single place that decides the stored wire format. SMSG
// currently stores HTML, so it returns the editor's HTML (with plain URLs/emails auto-linked,
// salvaged from the old plugin). If the project later moves to markdown or plain text, only
// messageHtml() changes -- the UI and send path stay the same. The core text-format
// operations reuse the standard Qt QTextCharFormat + merge-on-selection pattern.

class ChatComposer : public QWidget
{
	Q_OBJECT

	public:
		explicit ChatComposer(QWidget *parent = 0);

		// Interface parity with the widget it replaces.
		QString toPlainText() const;
		QString toHtml() const;			// the format seam (see messageHtml())
		void    clear();

	public slots:
		void setText(const QString &text);

	signals:
		// Emitted when the user presses Enter (without Shift) -- lets the page send on Enter.
		void sendRequested();

	protected:
		bool eventFilter(QObject *watched, QEvent *event);

	private slots:
		void toggleBold();
		void toggleItalic();
		void toggleUnderline();
		void toggleStrikeout();
		void insertLink();
		void pickTextColor();
		void showEmojiPicker();
		void insertText(const QString &text);
		void insertEmojiImage(const QString &resourcePath, const QString &shortcode);
		void maybeConvertShortcode();
		static QString standardEmojiForShortcode(const QString &shortcode);
		void emojiActionTriggered();

	private:
		void buildToolbar();
		void mergeFormat(const QTextCharFormat &format);
		QString messageHtml() const;

		QTextEdit   *m_editor;
		QToolBar    *m_toolbar;
		QAction     *m_boldAction;
		QAction     *m_italicAction;
		QAction     *m_underlineAction;
		QAction     *m_strikeoutAction;
		QAction     *m_linkAction;
		QAction     *m_colorAction;
		QToolButton *m_emojiButton;
};

#endif // CHATCOMPOSER_H
