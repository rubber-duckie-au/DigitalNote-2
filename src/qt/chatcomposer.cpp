#include <QVBoxLayout>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QGridLayout>
#include <QScrollArea>
#include <QWidgetAction>
#include <QInputDialog>
#include <QColorDialog>
#include <QLineEdit>
#include <QApplication>
#include <QPalette>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextBlock>
#include <QKeyEvent>
#include <QRegExp>
#include <QFont>

#include <QIcon>
#include <QUrl>
#include <QImage>
#include <QTextImageFormat>

#include "chatcomposer.h"
#include "xdnemoji.h"

// SMSG two-pane redesign, step 3. See chatcomposer.h for rationale.

// A small, self-contained set of common emoji for the picker. Rendered with the platform
// emoji font (e.g. Segoe UI Emoji on Windows). Kept short and general-purpose.
struct EmojiDef
{
	const char *utf8;
	const char *shortcode;
};

static const EmojiDef kEmojiSet[] = {
	{ "\xF0\x9F\x98\x80", ":grinning:" },
	{ "\xF0\x9F\x98\x83", ":smiley:" },
	{ "\xF0\x9F\x98\x84", ":smile:" },
	{ "\xF0\x9F\x98\x81", ":grin:" },
	{ "\xF0\x9F\x98\x86", ":laughing:" },
	{ "\xF0\x9F\x98\x85", ":sweat_smile:" },
	{ "\xF0\x9F\xA4\xA3", ":rofl:" },
	{ "\xF0\x9F\x98\x82", ":joy:" },
	{ "\xF0\x9F\x99\x82", ":slight_smile:" },
	{ "\xF0\x9F\x99\x83", ":upside_down:" },
	{ "\xF0\x9F\x98\x89", ":wink:" },
	{ "\xF0\x9F\x98\x8A", ":blush:" },
	{ "\xF0\x9F\x98\x87", ":innocent:" },
	{ "\xF0\x9F\xA5\xB0", ":smiling_face_with_three_hearts:" },
	{ "\xF0\x9F\x98\x8D", ":heart_eyes:" },
	{ "\xF0\x9F\xA4\xA9", ":star_struck:" },
	{ "\xF0\x9F\x98\x98", ":kissing_heart:" },
	{ "\xF0\x9F\x98\x97", ":kissing:" },
	{ "\xF0\x9F\x98\x9A", ":kissing_closed_eyes:" },
	{ "\xF0\x9F\x98\x99", ":kissing_smiling_eyes:" },
	{ "\xF0\x9F\x98\x8B", ":yum:" },
	{ "\xF0\x9F\x98\x9B", ":stuck_out_tongue:" },
	{ "\xF0\x9F\x98\x9C", ":stuck_out_tongue_winking_eye:" },
	{ "\xF0\x9F\xA4\xAA", ":zany_face:" },
	{ "\xF0\x9F\x98\x9D", ":stuck_out_tongue_closed_eyes:" },
	{ "\xF0\x9F\xA4\x91", ":money_mouth:" },
	{ "\xF0\x9F\xA4\x97", ":hugging:" },
	{ "\xF0\x9F\xA4\xAD", ":hand_over_mouth:" },
	{ "\xF0\x9F\xA4\xAB", ":shushing:" },
	{ "\xF0\x9F\xA4\x94", ":thinking:" },
	{ "\xF0\x9F\xA4\x90", ":zipper_mouth:" },
	{ "\xF0\x9F\xA4\xA8", ":raised_eyebrow:" },
	{ "\xF0\x9F\x98\x90", ":neutral_face:" },
	{ "\xF0\x9F\x98\x91", ":expressionless:" },
	{ "\xF0\x9F\x98\xB6", ":no_mouth:" },
	{ "\xF0\x9F\x98\x8F", ":smirk:" },
	{ "\xF0\x9F\x98\x92", ":unamused:" },
	{ "\xF0\x9F\x99\x84", ":roll_eyes:" },
	{ "\xF0\x9F\x98\xAC", ":grimacing:" },
	{ "\xF0\x9F\x98\xAE\xE2\x80\x8D\xF0\x9F\x92\xA8", ":exhaling:" },
	{ "\xF0\x9F\x98\x8C", ":relieved:" },
	{ "\xF0\x9F\x98\x94", ":pensive:" },
	{ "\xF0\x9F\x98\xAA", ":sleepy:" },
	{ "\xF0\x9F\xA4\xA4", ":drooling:" },
	{ "\xF0\x9F\x98\xB4", ":sleeping:" },
	{ "\xF0\x9F\x98\xB7", ":mask:" },
	{ "\xF0\x9F\xA4\x92", ":thermometer_face:" },
	{ "\xF0\x9F\xA4\x95", ":head_bandage:" },
	{ "\xF0\x9F\xA5\xB5", ":hot_face:" },
	{ "\xF0\x9F\xA5\xB6", ":cold_face:" },
	{ "\xF0\x9F\xA5\xB4", ":woozy:" },
	{ "\xF0\x9F\x98\xB5", ":dizzy_face:" },
	{ "\xF0\x9F\xA4\xAF", ":exploding_head:" },
	{ "\xF0\x9F\xA4\xA0", ":cowboy:" },
	{ "\xF0\x9F\xA5\xB3", ":partying_face:" },
	{ "\xF0\x9F\x98\x8E", ":sunglasses:" },
	{ "\xF0\x9F\xA4\x93", ":nerd:" },
	{ "\xF0\x9F\xA7\x90", ":monocle:" },
	{ "\xF0\x9F\x98\x95", ":confused:" },
	{ "\xF0\x9F\x98\x9F", ":worried:" },
	{ "\xF0\x9F\x99\x81", ":slight_frown:" },
	{ "\xF0\x9F\x98\xAE", ":open_mouth:" },
	{ "\xF0\x9F\x98\xAF", ":hushed:" },
	{ "\xF0\x9F\x98\xB2", ":astonished:" },
	{ "\xF0\x9F\x98\xB3", ":flushed:" },
	{ "\xF0\x9F\xA5\xBA", ":pleading:" },
	{ "\xF0\x9F\x98\xA6", ":frowning:" },
	{ "\xF0\x9F\x98\xA7", ":anguished:" },
	{ "\xF0\x9F\x98\xA8", ":fearful:" },
	{ "\xF0\x9F\x98\xB0", ":cold_sweat:" },
	{ "\xF0\x9F\x98\xA5", ":disappointed_relieved:" },
	{ "\xF0\x9F\x98\xA2", ":cry:" },
	{ "\xF0\x9F\x98\xAD", ":sob:" },
	{ "\xF0\x9F\x98\xB1", ":scream:" },
	{ "\xF0\x9F\x98\x96", ":confounded:" },
	{ "\xF0\x9F\x98\xA3", ":persevere:" },
	{ "\xF0\x9F\x98\x9E", ":disappointed:" },
	{ "\xF0\x9F\x98\x93", ":sweat:" },
	{ "\xF0\x9F\x98\xA9", ":weary:" },
	{ "\xF0\x9F\x98\xAB", ":tired_face:" },
	{ "\xF0\x9F\xA5\xB1", ":yawning:" },
	{ "\xF0\x9F\x98\xA4", ":triumph:" },
	{ "\xF0\x9F\x98\xA1", ":rage:" },
	{ "\xF0\x9F\x98\xA0", ":angry:" },
	{ "\xF0\x9F\xA4\xAC", ":cursing:" },
	{ "\xF0\x9F\x98\x88", ":smiling_imp:" },
	{ "\xF0\x9F\x91\xBF", ":imp:" },
	{ "\xF0\x9F\x92\x80", ":skull:" },
	{ "\xF0\x9F\x92\xA9", ":poop:" },
	{ "\xF0\x9F\xA4\xA1", ":clown:" },
	{ "\xF0\x9F\x91\xBB", ":ghost:" },
	{ "\xF0\x9F\x91\xBD", ":alien:" },
	{ "\xF0\x9F\xA4\x96", ":robot:" },
	{ "\xF0\x9F\x91\x8B", ":wave:" },
	{ "\xF0\x9F\xA4\x9A", ":raised_back_of_hand:" },
	{ "\xE2\x9C\x8B", ":raised_hand:" },
	{ "\xF0\x9F\x96\x90\xEF\xB8\x8F", ":hand_splayed:" },
	{ "\xF0\x9F\x91\x8C", ":ok_hand:" },
	{ "\xF0\x9F\xA4\x8C", ":pinched_fingers:" },
	{ "\xF0\x9F\xA4\x8F", ":pinching_hand:" },
	{ "\xE2\x9C\x8C\xEF\xB8\x8F", ":victory:" },
	{ "\xF0\x9F\xA4\x9E", ":fingers_crossed:" },
	{ "\xF0\x9F\xA4\x9F", ":love_you_gesture:" },
	{ "\xF0\x9F\xA4\x98", ":metal:" },
	{ "\xF0\x9F\xA4\x99", ":call_me:" },
	{ "\xF0\x9F\x91\x88", ":point_left:" },
	{ "\xF0\x9F\x91\x89", ":point_right:" },
	{ "\xF0\x9F\x91\x86", ":point_up_2:" },
	{ "\xF0\x9F\x91\x87", ":point_down:" },
	{ "\xF0\x9F\x91\x8D", ":thumbsup:" },
	{ "\xF0\x9F\x91\x8E", ":thumbsdown:" },
	{ "\xE2\x9C\x8A", ":fist:" },
	{ "\xF0\x9F\x91\x8A", ":punch:" },
	{ "\xF0\x9F\xA4\x9B", ":fist_left:" },
	{ "\xF0\x9F\xA4\x9C", ":fist_right:" },
	{ "\xF0\x9F\x91\x8F", ":clap:" },
	{ "\xF0\x9F\x99\x8C", ":raised_hands:" },
	{ "\xF0\x9F\x91\x90", ":open_hands:" },
	{ "\xF0\x9F\xA4\xB2", ":palms_up:" },
	{ "\xF0\x9F\x99\x8F", ":pray:" },
	{ "\xF0\x9F\x92\xAA", ":muscle:" },
	{ "\xF0\x9F\xA6\xBE", ":mechanical_arm:" },
	{ "\xE2\x9D\xA4\xEF\xB8\x8F", ":heart:" },
	{ "\xF0\x9F\xA7\xA1", ":orange_heart:" },
	{ "\xF0\x9F\x92\x9B", ":yellow_heart:" },
	{ "\xF0\x9F\x92\x9A", ":green_heart:" },
	{ "\xF0\x9F\x92\x99", ":blue_heart:" },
	{ "\xF0\x9F\x92\x9C", ":purple_heart:" },
	{ "\xF0\x9F\x96\xA4", ":black_heart:" },
	{ "\xF0\x9F\xA4\x8D", ":white_heart:" },
	{ "\xF0\x9F\xA4\x8E", ":brown_heart:" },
	{ "\xF0\x9F\x92\x94", ":broken_heart:" },
	{ "\xE2\x9D\xA3\xEF\xB8\x8F", ":heart_exclamation:" },
	{ "\xF0\x9F\x92\x95", ":two_hearts:" },
	{ "\xF0\x9F\x92\x9E", ":revolving_hearts:" },
	{ "\xF0\x9F\x92\x93", ":heartbeat:" },
	{ "\xF0\x9F\x92\x97", ":heartpulse:" },
	{ "\xF0\x9F\x92\x96", ":sparkling_heart:" },
	{ "\xF0\x9F\x92\x98", ":cupid:" },
	{ "\xF0\x9F\x92\x9D", ":gift_heart:" },
	{ "\xF0\x9F\x92\xAF", ":100:" },
	{ "\xF0\x9F\x92\xA2", ":anger:" },
	{ "\xF0\x9F\x92\xA5", ":boom:" },
	{ "\xF0\x9F\x92\xAB", ":dizzy:" },
	{ "\xF0\x9F\x92\xA6", ":sweat_drops:" },
	{ "\xF0\x9F\x92\xA8", ":dash:" },
	{ "\xF0\x9F\x95\xB3\xEF\xB8\x8F", ":hole:" },
	{ "\xF0\x9F\x92\xAC", ":speech_balloon:" },
	{ "\xF0\x9F\x92\xAD", ":thought_balloon:" },
	{ "\xF0\x9F\x94\xA5", ":fire:" },
	{ "\xE2\xAD\x90", ":star:" },
	{ "\xF0\x9F\x8C\x9F", ":star2:" },
	{ "\xE2\x9C\xA8", ":sparkles:" },
	{ "\xE2\x9A\xA1", ":zap:" },
	{ "\xE2\x98\x80\xEF\xB8\x8F", ":sunny:" },
	{ "\xF0\x9F\x8C\x99", ":crescent_moon:" },
	{ "\xE2\x98\x81\xEF\xB8\x8F", ":cloud:" },
	{ "\xF0\x9F\x8C\x88", ":rainbow:" },
	{ "\xE2\x98\x82\xEF\xB8\x8F", ":umbrella:" },
	{ "\xE2\x9D\x84\xEF\xB8\x8F", ":snowflake:" },
	{ "\xE2\x9C\x85", ":white_check_mark:" },
	{ "\xE2\x9D\x8C", ":x:" },
	{ "\xE2\x9D\x93", ":question:" },
	{ "\xE2\x9D\x97", ":exclamation:" },
	{ "\xE2\x9A\xA0\xEF\xB8\x8F", ":warning:" },
	{ "\xE2\x99\xBB\xEF\xB8\x8F", ":recycle:" },
	{ "\xF0\x9F\x94\x94", ":bell:" },
	{ "\xF0\x9F\x8E\xB5", ":musical_note:" },
	{ "\xF0\x9F\x90\xB6", ":dog:" },
	{ "\xF0\x9F\x90\xB1", ":cat:" },
	{ "\xF0\x9F\x90\xAD", ":mouse:" },
	{ "\xF0\x9F\x90\xB9", ":hamster:" },
	{ "\xF0\x9F\x90\xB0", ":rabbit:" },
	{ "\xF0\x9F\xA6\x8A", ":fox:" },
	{ "\xF0\x9F\x90\xBB", ":bear:" },
	{ "\xF0\x9F\x90\xBC", ":panda:" },
	{ "\xF0\x9F\x90\xA8", ":koala:" },
	{ "\xF0\x9F\x90\xAF", ":tiger:" },
	{ "\xF0\x9F\xA6\x81", ":lion:" },
	{ "\xF0\x9F\x90\xAE", ":cow:" },
	{ "\xF0\x9F\x90\xB7", ":pig:" },
	{ "\xF0\x9F\x90\xB8", ":frog:" },
	{ "\xF0\x9F\x90\xB5", ":monkey_face:" },
	{ "\xF0\x9F\x90\x94", ":chicken:" },
	{ "\xF0\x9F\xA6\x84", ":unicorn:" },
	{ "\xF0\x9F\x90\x9D", ":bee:" },
	{ "\xF0\x9F\xA6\x8B", ":butterfly:" },
	{ "\xF0\x9F\x90\xA2", ":turtle:" },
	{ "\xF0\x9F\x90\x99", ":octopus:" },
	{ "\xF0\x9F\xA6\x80", ":crab:" },
	{ "\xF0\x9F\x90\xAC", ":dolphin:" },
	{ "\xF0\x9F\x90\xB3", ":whale:" },
	{ "\xF0\x9F\x90\xA0", ":tropical_fish:" },
	{ "\xF0\x9F\xA6\x95", ":sauropod:" },
	{ "\xF0\x9F\x8D\x8E", ":apple:" },
	{ "\xF0\x9F\x8D\x8C", ":banana:" },
	{ "\xF0\x9F\x8D\x87", ":grapes:" },
	{ "\xF0\x9F\x8D\x93", ":strawberry:" },
	{ "\xF0\x9F\x8D\x89", ":watermelon:" },
	{ "\xF0\x9F\x8D\x92", ":cherries:" },
	{ "\xF0\x9F\x8D\x91", ":peach:" },
	{ "\xF0\x9F\xA5\xAD", ":mango:" },
	{ "\xF0\x9F\x8D\x8D", ":pineapple:" },
	{ "\xF0\x9F\xA5\xA5", ":coconut:" },
	{ "\xF0\x9F\xA5\x9D", ":kiwi:" },
	{ "\xF0\x9F\x8D\x85", ":tomato:" },
	{ "\xF0\x9F\xA5\x91", ":avocado:" },
	{ "\xF0\x9F\x8C\xBD", ":corn:" },
	{ "\xF0\x9F\x8C\xB6\xEF\xB8\x8F", ":hot_pepper:" },
	{ "\xF0\x9F\x8D\x84", ":mushroom:" },
	{ "\xF0\x9F\x8D\x9E", ":bread:" },
	{ "\xF0\x9F\xA7\x80", ":cheese:" },
	{ "\xF0\x9F\x8D\x94", ":hamburger:" },
	{ "\xF0\x9F\x8D\x9F", ":fries:" },
	{ "\xF0\x9F\x8D\x95", ":pizza:" },
	{ "\xF0\x9F\x8C\xAD", ":hotdog:" },
	{ "\xF0\x9F\x8C\xAE", ":taco:" },
	{ "\xF0\x9F\x8D\x9C", ":ramen:" },
	{ "\xF0\x9F\x8D\xA3", ":sushi:" },
	{ "\xF0\x9F\x8D\xA9", ":doughnut:" },
	{ "\xF0\x9F\x8D\xAA", ":cookie:" },
	{ "\xF0\x9F\x8E\x82", ":birthday:" },
	{ "\xF0\x9F\x8D\xB0", ":cake:" },
	{ "\xF0\x9F\x8D\xAB", ":chocolate_bar:" },
	{ "\xF0\x9F\x8D\xBF", ":popcorn:" },
	{ "\xE2\x98\x95", ":coffee:" },
	{ "\xF0\x9F\x8D\xBA", ":beer:" },
	{ "\xF0\x9F\x8D\xBB", ":beers:" },
	{ "\xF0\x9F\xA5\x82", ":champagne_glass:" },
	{ "\xF0\x9F\x8D\xB7", ":wine_glass:" },
	{ "\xF0\x9F\xA5\x83", ":tumbler_glass:" },
	{ "\xE2\x9A\xBD", ":soccer:" },
	{ "\xF0\x9F\x8F\x80", ":basketball:" },
	{ "\xF0\x9F\x8F\x88", ":football:" },
	{ "\xE2\x9A\xBE", ":baseball:" },
	{ "\xF0\x9F\x8E\xBE", ":tennis:" },
	{ "\xF0\x9F\x8E\xB1", ":8ball:" },
	{ "\xF0\x9F\x8F\x86", ":trophy:" },
	{ "\xF0\x9F\xA5\x87", ":first_place:" },
	{ "\xF0\x9F\x8E\xAE", ":video_game:" },
	{ "\xF0\x9F\x8E\xB2", ":game_die:" },
	{ "\xF0\x9F\x8E\xAF", ":dart:" },
	{ "\xF0\x9F\x8E\xB8", ":guitar:" },
	{ "\xF0\x9F\x8E\xB9", ":musical_keyboard:" },
	{ "\xF0\x9F\x8E\xA4", ":microphone:" },
	{ "\xF0\x9F\x8E\xA7", ":headphones:" },
	{ "\xF0\x9F\x8E\x81", ":gift:" },
	{ "\xF0\x9F\x8E\x89", ":tada:" },
	{ "\xF0\x9F\x8E\x8A", ":confetti_ball:" },
	{ "\xF0\x9F\x9A\x80", ":rocket:" },
	{ "\xF0\x9F\x92\xB0", ":moneybag:" },
	{ "\xF0\x9F\x92\xB5", ":dollar:" },
	{ "\xF0\x9F\x92\x8E", ":gem:" },
	{ "\xF0\x9F\x94\x91", ":key:" },
	{ "\xF0\x9F\x94\x92", ":lock:" },
	{ "\xF0\x9F\x93\x88", ":chart_increasing:" },
	{ "\xF0\x9F\x93\x89", ":chart_decreasing:" },
	{ "\xF0\x9F\x92\xA1", ":bulb:" },
	{ "\xF0\x9F\x94\xA7", ":wrench:" },
	{ "\xE2\x9A\x99\xEF\xB8\x8F", ":gear:" },
	{ "\xF0\x9F\x9B\xA0\xEF\xB8\x8F", ":hammer_and_wrench:" }
};

ChatComposer::ChatComposer(QWidget *parent) :
	QWidget(parent),
	m_editor(new QTextEdit(this)),
	m_toolbar(new QToolBar(this)),
	m_boldAction(0),
	m_italicAction(0),
	m_underlineAction(0),
	m_strikeoutAction(0),
	m_linkAction(0),
	m_colorAction(0),
	m_emojiButton(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);

	buildToolbar();

	// Compact editor: about two lines tall, grows modestly. Chat, not a document. Kept
	// deliberately short so the conversation above gets the most vertical space.
	m_editor->setAcceptRichText(true);
	m_editor->setMinimumHeight(36);
	m_editor->setMaximumHeight(90);
	m_editor->setPlaceholderText(tr("Write a message..."));
	m_editor->installEventFilter(this);

	layout->addWidget(m_toolbar);
	layout->addWidget(m_editor);

	setLayout(layout);
}

void ChatComposer::buildToolbar()
{
	m_toolbar->setIconSize(QSize(16, 16));
	m_toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);

	// Bold / Italic / Underline / Strikeout are checkable toggles; Link is a one-shot.
	m_boldAction = m_toolbar->addAction(tr("B"));
	m_boldAction->setToolTip(tr("Bold"));
	m_boldAction->setCheckable(true);
	QFont boldFont = m_boldAction->font();
	boldFont.setBold(true);
	m_boldAction->setFont(boldFont);
	connect(m_boldAction, SIGNAL(triggered()), this, SLOT(toggleBold()));

	m_italicAction = m_toolbar->addAction(tr("I"));
	m_italicAction->setToolTip(tr("Italic"));
	m_italicAction->setCheckable(true);
	QFont italicFont = m_italicAction->font();
	italicFont.setItalic(true);
	m_italicAction->setFont(italicFont);
	connect(m_italicAction, SIGNAL(triggered()), this, SLOT(toggleItalic()));

	m_underlineAction = m_toolbar->addAction(tr("U"));
	m_underlineAction->setToolTip(tr("Underline"));
	m_underlineAction->setCheckable(true);
	QFont underlineFont = m_underlineAction->font();
	underlineFont.setUnderline(true);
	m_underlineAction->setFont(underlineFont);
	connect(m_underlineAction, SIGNAL(triggered()), this, SLOT(toggleUnderline()));

	m_strikeoutAction = m_toolbar->addAction(tr("S"));
	m_strikeoutAction->setToolTip(tr("Strikethrough"));
	m_strikeoutAction->setCheckable(true);
	QFont strikeFont = m_strikeoutAction->font();
	strikeFont.setStrikeOut(true);
	m_strikeoutAction->setFont(strikeFont);
	connect(m_strikeoutAction, SIGNAL(triggered()), this, SLOT(toggleStrikeout()));

	m_toolbar->addSeparator();

	m_linkAction = m_toolbar->addAction(tr("Link"));
	m_linkAction->setToolTip(tr("Insert link"));
	connect(m_linkAction, SIGNAL(triggered()), this, SLOT(insertLink()));

	m_colorAction = m_toolbar->addAction(tr("A"));
	m_colorAction->setToolTip(tr("Text colour"));
	connect(m_colorAction, SIGNAL(triggered()), this, SLOT(pickTextColor()));

	m_toolbar->addSeparator();

	// Emoji picker button (popup grid).
	m_emojiButton = new QToolButton(this);
	m_emojiButton->setText(QString::fromUtf8("\xF0\x9F\x98\x8A"));
	m_emojiButton->setToolTip(tr("Insert emoji"));
	m_emojiButton->setPopupMode(QToolButton::InstantPopup);
	m_emojiButton->setAutoRaise(true);
	connect(m_emojiButton, SIGNAL(clicked()), this, SLOT(showEmojiPicker()));
	m_toolbar->addWidget(m_emojiButton);
}

void ChatComposer::mergeFormat(const QTextCharFormat &format)
{
	// Standard Qt pattern (salvaged from the previous editor): apply to the selection, or to
	// the word under the cursor if there is no selection.
	QTextCursor cursor = m_editor->textCursor();

	if(!cursor.hasSelection())
	{
		cursor.select(QTextCursor::WordUnderCursor);
	}

	cursor.mergeCharFormat(format);
	m_editor->mergeCurrentCharFormat(format);
}

void ChatComposer::toggleBold()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(m_boldAction->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormat(fmt);
}

void ChatComposer::toggleItalic()
{
	QTextCharFormat fmt;
	fmt.setFontItalic(m_italicAction->isChecked());
	mergeFormat(fmt);
}

void ChatComposer::toggleUnderline()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(m_underlineAction->isChecked());
	mergeFormat(fmt);
}

void ChatComposer::toggleStrikeout()
{
	QTextCharFormat fmt;
	fmt.setFontStrikeOut(m_strikeoutAction->isChecked());
	mergeFormat(fmt);
}

void ChatComposer::pickTextColor()
{
	QColor col = QColorDialog::getColor(Qt::black, this, tr("Text colour"));

	if(col.isValid())
	{
		QTextCharFormat fmt;
		fmt.setForeground(col);
		mergeFormat(fmt);
	}
}

void ChatComposer::insertLink()
{
	// Salvaged link behaviour: prompt for a URL and anchor the selection/word.
	QTextCharFormat fmt;
	QString existing = m_editor->currentCharFormat().anchorHref();

	bool ok = false;
	QString url = QInputDialog::getText(this, tr("Create a link"), tr("Link URL:"),
	                                    QLineEdit::Normal, existing, &ok);

	if(ok && !url.isEmpty())
	{
		fmt.setAnchor(true);
		fmt.setAnchorHref(url);
		fmt.setForeground(QApplication::palette().color(QPalette::Link));
		fmt.setFontUnderline(true);
	}
	else
	{
		fmt.setAnchor(false);
		fmt.setForeground(QApplication::palette().color(QPalette::Text));
		fmt.setFontUnderline(false);
	}

	mergeFormat(fmt);
}

void ChatComposer::showEmojiPicker()
{
	// Grid of emoji as QActions carrying their character in data(). A single slot
	// (emojiActionTriggered) reads the sender action's data and inserts it. This avoids both
	// lambda-connect and the deprecated QSignalMapper::mapped(QString), so it compiles cleanly
	// across Qt 5 versions.
	QMenu menu(this);

	QWidget *grid = new QWidget(&menu);
	QGridLayout *gl = new QGridLayout(grid);
	gl->setContentsMargins(6, 6, 6, 6);
	gl->setSpacing(2);

	int cols = 7;
	int row = 0;

	// XDN custom emoji first: show the bundled image, insert the shortcode (:xdn:) on click.
	const QList<XdnEmoji::Entry> &xdn = XdnEmoji::all();
	for(int i = 0; i < xdn.size(); i++)
	{
		const XdnEmoji::Entry &e = xdn.at(i);

		QToolButton *b = new QToolButton(grid);
		b->setAutoRaise(true);
		b->setFixedSize(30, 30);
		b->setIconSize(QSize(22, 22));
		b->setToolTip(e.shortcode);

		// The icon must go on the ACTION: setDefaultAction() makes the button mirror the
		// action's icon/text, overriding any icon set directly on the button. Data carries
		// "img|<resourcePath>|<shortcode>" so the trigger handler inserts an inline image.
		QAction *a = new QAction(b);
		a->setIcon(QIcon(e.resourcePath));
		a->setToolTip(e.shortcode);
		a->setData(QString("img|%1|%2").arg(e.resourcePath).arg(e.shortcode));
		b->setDefaultAction(a);
		connect(a, SIGNAL(triggered()), this, SLOT(emojiActionTriggered()));
		connect(a, SIGNAL(triggered()), &menu, SLOT(close()));

		gl->addWidget(b, row + (i / cols), i % cols);
	}

	if(!xdn.isEmpty())
	{
		row += ((xdn.size() - 1) / cols) + 1;
	}

	int count = (int)(sizeof(kEmojiSet) / sizeof(kEmojiSet[0]));

	for(int i = 0; i < count; i++)
	{
		QString emoji     = QString::fromUtf8(kEmojiSet[i].utf8);
		QString shortcode = QString::fromUtf8(kEmojiSet[i].shortcode);

		QToolButton *b = new QToolButton(grid);
		b->setText(emoji);
		b->setAutoRaise(true);
		b->setFixedSize(30, 30);
		b->setToolTip(shortcode);
		QFont ef = b->font();
		ef.setPointSize(ef.pointSize() + 4);
		b->setFont(ef);

		QAction *a = new QAction(emoji, b);
		a->setData(emoji);
		a->setToolTip(shortcode);
		b->setDefaultAction(a);
		connect(a, SIGNAL(triggered()), this, SLOT(emojiActionTriggered()));

		// Close the popup after a pick.
		connect(a, SIGNAL(triggered()), &menu, SLOT(close()));

		gl->addWidget(b, row + (i / cols), i % cols);
	}

	// The full emoji set is large, so present the grid inside a fixed-size scroll area rather
	// than a menu that would run off-screen.
	QScrollArea *scroll = new QScrollArea(&menu);
	scroll->setWidget(grid);
	scroll->setWidgetResizable(true);
	scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scroll->setFixedSize(7 * 34 + 24, 260);

	QWidgetAction *wa = new QWidgetAction(&menu);
	wa->setDefaultWidget(scroll);
	menu.addAction(wa);

	menu.exec(m_emojiButton->mapToGlobal(QPoint(0, m_emojiButton->height())));
}

void ChatComposer::emojiActionTriggered()
{
	QAction *a = qobject_cast<QAction *>(sender());

	if(!a)
	{
		return;
	}

	QString data = a->data().toString();

	// XDN custom emoji encode "img|<resourcePath>|<shortcode>" -> insert as inline image.
	// Standard emoji carry their character directly -> insert as text.
	if(data.startsWith("img|"))
	{
		QStringList parts = data.split('|');
		if(parts.size() == 3)
		{
			insertEmojiImage(parts.at(1), parts.at(2));
		}
	}
	else
	{
		insertText(data);
	}
}

void ChatComposer::insertText(const QString &text)
{
	m_editor->textCursor().insertText(text);
	m_editor->setFocus();
}

void ChatComposer::insertEmojiImage(const QString &resourcePath, const QString &shortcode)
{
	// Insert a custom emoji as an inline image so the user SEES it while composing (WYSIWYG).
	// The image resource is registered on the editor's document, and the image format carries
	// the shortcode as its name so messageHtml() can convert it back to :shortcode: on send
	// (keeping the wire payload tiny). Aspect ratio is preserved from the source image.
	QImage img(resourcePath);
	if(img.isNull())
	{
		// Fall back to the shortcode text if the image can't be loaded.
		insertText(shortcode);
		return;
	}

	m_editor->document()->addResource(QTextDocument::ImageResource, QUrl(resourcePath), img);

	QFontMetrics fm(m_editor->font());
	int h = fm.height() + 2;
	int w = (img.height() > 0) ? (img.width() * h) / img.height() : h;

	QTextImageFormat fmt;
	fmt.setName(resourcePath);		// the src the editor's toHtml() will emit
	fmt.setWidth(w);
	fmt.setHeight(h);

	m_editor->textCursor().insertImage(fmt);
	m_editor->setFocus();
}

QString ChatComposer::standardEmojiForShortcode(const QString &shortcode)
{
	// Reverse lookup in the standard emoji table: :name: -> Unicode character.
	int count = (int)(sizeof(kEmojiSet) / sizeof(kEmojiSet[0]));

	for(int i = 0; i < count; i++)
	{
		if(shortcode == QString::fromUtf8(kEmojiSet[i].shortcode))
		{
			return QString::fromUtf8(kEmojiSet[i].utf8);
		}
	}

	return QString();
}

bool ChatComposer::eventFilter(QObject *watched, QEvent *event)
{
	if(watched == m_editor && event->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);

		// Enter sends; Shift+Enter inserts a newline.
		if((ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
		   && !(ke->modifiers() & Qt::ShiftModifier))
		{
			emit sendRequested();
			return true;
		}

		// Typing the closing ':' of a shortcode -> try to convert it to the emoji. Let the ':'
		// be inserted first (so the text reads ":name:"), then run the check.
		if(ke->text() == ":")
		{
			m_editor->textCursor().insertText(":");
			maybeConvertShortcode();
			return true;
		}
	}

	return QWidget::eventFilter(watched, event);
}

void ChatComposer::maybeConvertShortcode()
{
	// Look at the text on the current line up to the cursor, find a trailing :shortcode:, and
	// if it maps to an emoji, replace just those characters. Standard emoji become Unicode
	// text; XDN emoji become an inline image (same as picking from the picker).
	QTextCursor cursor = m_editor->textCursor();
	int posInBlock = cursor.positionInBlock();

	QString block = cursor.block().text();
	if(posInBlock < 2 || posInBlock > block.length())
	{
		return;
	}

	QString upToCursor = block.left(posInBlock);

	// Find the last ':' before the closing one -- i.e. the start of a :name: token.
	int closeColon = upToCursor.length() - 1;			// the ':' just typed
	int openColon = upToCursor.lastIndexOf(':', closeColon - 1);
	if(openColon < 0)
	{
		return;
	}

	QString shortcode = upToCursor.mid(openColon, closeColon - openColon + 1);	// ":name:"

	// Reject if it contains whitespace (not a real shortcode).
	if(shortcode.contains(' ') || shortcode.length() < 3)
	{
		return;
	}

	QString xdnResource = XdnEmoji::resourceForShortcode(shortcode);
	QString stdUnicode  = standardEmojiForShortcode(shortcode);

	if(xdnResource.isEmpty() && stdUnicode.isEmpty())
	{
		return;		// unknown shortcode -- leave the typed text as-is
	}

	// Select the shortcode text (from openColon to cursor) and replace it.
	int blockStart = cursor.position() - posInBlock;
	QTextCursor sel = m_editor->textCursor();
	sel.setPosition(blockStart + openColon);
	sel.setPosition(blockStart + closeColon + 1, QTextCursor::KeepAnchor);
	sel.removeSelectedText();
	m_editor->setTextCursor(sel);

	if(!xdnResource.isEmpty())
	{
		insertEmojiImage(xdnResource, shortcode);
	}
	else
	{
		insertText(stdUnicode);
	}
}

QString ChatComposer::messageHtml() const
{
	// THE FORMAT SEAM. SMSG currently stores HTML, so emit the editor's HTML with plain
	// URLs/emails auto-linked (salvaged from the previous editor). To move to markdown or
	// plain text later, change only this method (and add a format flag) -- nothing else.
	QString s = m_editor->toHtml();

	// Convert any inline custom-emoji images back to their :shortcode: so the wire payload
	// stays tiny (a few bytes) instead of carrying image markup. The recipient re-renders the
	// shortcode to the bundled image on display.
	s = XdnEmoji::substituteImagesToShortcodes(s);

	// Auto-link emails and URLs that were typed as plain text.
	s = s.replace(QRegExp("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)([a-zA-Z\\d]+@[a-zA-Z\\d]+\\.[a-zA-Z]+)"),
	              "\\1<a href=\"mailto:\\2\">\\2</a>");
	s = s.replace(QRegExp("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)((?:https?|ftp|file)://[^\\s'\"<>]+)"),
	              "\\1<a href=\"\\2\">\\2</a>");

	return s;
}

QString ChatComposer::toHtml() const
{
	return messageHtml();
}

QString ChatComposer::toPlainText() const
{
	return m_editor->toPlainText();
}

void ChatComposer::clear()
{
	m_editor->clear();
}

void ChatComposer::setText(const QString &text)
{
	m_editor->setHtml(text);
}
