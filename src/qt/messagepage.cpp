#include <QBoxLayout>
#include <QSizePolicy>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QStyledItemDelegate>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QToolBar>
#include <QMenu>

#include "sendmessagesdialog.h"
#include "qt/plugins/mrichtexteditor/mrichtextedit.h"
#include "messagemodel.h"
#include "conversationmodel.h"
#include "conversationthreadmodel.h"
#include "conversationbubbledelegate.h"
#include "conversationlistdelegate.h"
#include "chatcomposer.h"
#include "bitcoingui.h"
#include "csvmodelwriter.h"
#include "guiutil.h"
#include "smsg.h"

#include "messagepage.h"
#include "ui_messagepage.h"

#define DECORATION_SIZE 64
#define NUM_ITEMS 3

class MessageViewDelegate : public QStyledItemDelegate
{
protected:
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

void MessageViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    QString align(index.data(MessageModel::TypeRole) == 1 ? "left" : "right");
    QString html = "<p align=\"" + align + "\" style=\"color:black;\">" + index.data(MessageModel::HTMLRole).toString() + "</p>";
    doc.setHtml(html);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    doc.setTextWidth( textRect.width() );
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize MessageViewDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(index.data(MessageModel::HTMLRole).toString());
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height() + 20);
}


MessagePage::MessagePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessagePage),
    model(0),
    conversationModel(0),
    threadModel(0),
    bubbleDelegate(0),
    listDelegate(0),
    chatComposer(0),
    msgdelegate(new MessageViewDelegate()),
    messageTextEdit(new MRichTextEdit())
{
    ui->setupUi(this);


#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->deleteButton->setIcon(QIcon());
#endif
    // Context menu actions
    replyAction           = new QAction(ui->sendButton->text(),            this);
    copyFromAddressAction = new QAction(ui->copyFromAddressButton->text(), this);
    copyToAddressAction   = new QAction(ui->copyToAddressButton->text(),   this);
    deleteAction          = new QAction(ui->deleteButton->text(),          this);

    // Build context menu
    contextMenu = new QMenu();

    contextMenu->addAction(replyAction);
    contextMenu->addAction(copyFromAddressAction);
    contextMenu->addAction(copyToAddressAction);
    contextMenu->addAction(deleteAction);

    connect(replyAction,           SIGNAL(triggered()), this, SLOT(on_sendButton_clicked()));
    connect(copyFromAddressAction, SIGNAL(triggered()), this, SLOT(on_copyFromAddressButton_clicked()));
    connect(copyToAddressAction,   SIGNAL(triggered()), this, SLOT(on_copyToAddressButton_clicked()));
    connect(deleteAction,          SIGNAL(triggered()), this, SLOT(on_deleteButton_clicked()));

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    // Show Messages
    ui->listConversation->setItemDelegate(msgdelegate);
    ui->listConversation->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listConversation->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listConversation->setAttribute(Qt::WA_MacShowFocusRect, false);
}

MessagePage::~MessagePage()
{
    delete ui;
}

void MessagePage::setModel(MessageModel *model)
{
    this->model = model;
    if(!model)
        return;

    // The proxy model is still created (other code/paths may reference model->proxyModel),
    // but it no longer drives the inbox view -- the Back-flow uses ConversationModel /
    // ConversationThreadModel directly instead of the fragile flat-table + proxy plumbing.
    model->proxyModel = new QSortFilterProxyModel(this);
    model->proxyModel->setSourceModel(model);
    model->proxyModel->setDynamicSortFilter(true);
    model->proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    model->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // SMSG redesign (Back-flow): the top-level "inbox" is now the conversation list
    // (ConversationModel), not the flat message table. Selecting a conversation hides this
    // list and shows the thread (bubbles) + compose; Back returns here.
    if(!bubbleDelegate)
    {
        bubbleDelegate = new ConversationBubbleDelegate(this);
    }

    // Build the conversation-grouped model and the thread model BEFORE binding them to the
    // views (otherwise setModel() would receive a null pointer and the list would stay empty).
    conversationModel = new ConversationModel(model, this);
    threadModel = new ConversationThreadModel(model, this);

    ui->tableView->setModel(conversationModel);
    // ConversationModel is a single-column list model; make the QTableView present it as a
    // clean list: no visible header, the one column stretched to full width, whole-row
    // selection, and a comfortable row height for the label + preview text.
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->verticalHeader()->setDefaultSectionSize(68);
    ui->tableView->setShowGrid(false);
    if(!listDelegate)
    {
        listDelegate = new ConversationListDelegate(this);
    }
    ui->tableView->setItemDelegate(listDelegate);
    ui->tableView->setMouseTracking(true);

    ui->listConversation->setModel(threadModel);
    ui->listConversation->setModelColumn(0);
    ui->listConversation->setItemDelegate(bubbleDelegate);

    // Replace the dated MRichTextEdit compose widget with the modern ChatComposer, at runtime
    // (avoids editing the .ui). Insert it into the same layout slot as the old messageEdit,
    // then hide messageEdit. All compose interactions go through chatComposer from here;
    // messageEdit is kept (hidden) so nothing else that references it breaks.
    if(!chatComposer)
    {
        chatComposer = new ChatComposer(this);

        // Insert the composer directly into the known compose layout (ui->verticalLayout, the
        // vertical layout that holds messageDetails, messageEdit and the button row), right
        // after the old messageEdit. Using the named layout is robust -- parentWidget()->
        // layout() is not, because a widget's parent is the containing WIDGET, skipping
        // layouts, so it would return the wrong (top-level) layout.
        int idx = ui->verticalLayout->indexOf(ui->messageEdit);
        if(idx >= 0)
        {
            ui->verticalLayout->insertWidget(idx + 1, chatComposer);
        }
        else
        {
            ui->verticalLayout->addWidget(chatComposer);
        }

        chatComposer->hide();

        // Permanently hide the old MRichTextEdit (its full word-processor ribbon is replaced
        // by the ChatComposer). It stays in the tree so nothing that references it breaks.
        ui->messageEdit->hide();

        // Reorder so the action buttons (Send / Copy / Delete) sit BELOW the composer: remove
        // the button row from its slot and re-add it at the end (after the composer). Natural
        // chat flow: thread -> compose box -> Send underneath.
        ui->verticalLayout->removeItem(ui->horizontalLayout);
        ui->verticalLayout->addLayout(ui->horizontalLayout);

        // Distribute vertical space: the conversation (messageDetails) should absorb all the
        // extra height when the window grows; the composer and button row stay compact at the
        // bottom. Give messageDetails a stretch factor and everything else zero so the thread
        // fills the available real estate instead of leaving a gap.
        int detailsIdx = ui->verticalLayout->indexOf(ui->messageDetails);
        if(detailsIdx >= 0)
        {
            ui->verticalLayout->setStretch(detailsIdx, 1);
        }
        int composerIdx = ui->verticalLayout->indexOf(chatComposer);
        if(composerIdx >= 0)
        {
            ui->verticalLayout->setStretch(composerIdx, 0);
        }

        // Keep the composer from expanding vertically -- it should hug its content height.
        chatComposer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        // Ensure the conversation groupbox is allowed to grow vertically to fill the space.
        ui->messageDetails->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // The compose section lives in ui->verticalLayout, which is itself an item inside the
        // outer ui->verticalLayout_2 (alongside the label and the conversation-list tableView).
        // Give that inner layout a stretch factor within the outer one, so the whole
        // thread+compose section expands to fill the window height instead of sitting at its
        // hint height and leaving a gap. The tableView (list view) also gets stretch so the
        // list fills the height when it is the visible view.
        int innerIdx = ui->verticalLayout_2->indexOf(ui->verticalLayout);
        if(innerIdx >= 0)
        {
            ui->verticalLayout_2->setStretch(innerIdx, 1);
        }
        int tableIdx = ui->verticalLayout_2->indexOf(ui->tableView);
        if(tableIdx >= 0)
        {
            ui->verticalLayout_2->setStretch(tableIdx, 1);
        }

        // Send when the composer signals Enter.
        connect(chatComposer, SIGNAL(sendRequested()), this, SLOT(on_sendButton_clicked()));
    }

    connect(ui->tableView->selectionModel(),        SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectionChanged()));
    connect(ui->tableView,                          SIGNAL(doubleClicked(QModelIndex)),                       this, SLOT(selectionChanged()));
    connect(ui->listConversation->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),  this, SLOT(itemSelectionChanged()));
    connect(ui->listConversation,                   SIGNAL(doubleClicked(QModelIndex)),                       this, SLOT(itemSelectionChanged()));
    //connect(ui->messageEdit,                        SIGNAL(textChanged()),                                    this, SLOT(messageTextChanged()));

    // Scroll to bottom
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(incomingMessage()));

    // Refresh the conversation list when the model resets (e.g. after unlock or after a
    // send/receive rebuilds it). Use a dedicated handler that preserves the current view:
    // if a thread is open, stay in it; do not bounce back to the list on every rebuild.
    connect(conversationModel, SIGNAL(modelReset()), this, SLOT(conversationsRebuilt()));

    // Initial state: show the conversation list, hide the thread/compose until a
    // conversation is selected.
    ui->messageDetails->hide();
    ui->tableView->show();

    selectionChanged();
}

void MessagePage::on_sendButton_clicked()
{
    if(!model)
        return;

    std::string sError;
    std::string sendTo  = replyToAddress.toStdString();
    std::string message = (chatComposer ? chatComposer->toHtml() : ui->messageEdit->toHtml()).toStdString();
    std::string addFrom = replyFromAddress.toStdString();

    if (DigitalNote::SMSG::Send(addFrom, sendTo, message, sError) != 0)
    {
        QMessageBox::warning(NULL, tr("Send Secure Message"),
            tr("Send failed: %1.").arg(sError.c_str()),
            QMessageBox::Ok, QMessageBox::Ok);

        return;
    };

    //ui->messageEdit->setMaximumHeight(30);
    if(chatComposer) chatComposer->clear();
    ui->listConversation->scrollToBottom();
}

void MessagePage::on_newButton_clicked()
{
    if(!model)
        return;

    SendMessagesDialog dlg(SendMessagesDialog::Encrypted, SendMessagesDialog::Dialog, this);

    dlg.setModel(model);
    dlg.exec();
}

void MessagePage::on_copyFromAddressButton_clicked()
{
    GUIUtil::copyEntryData(ui->tableView, MessageModel::FromAddress, Qt::DisplayRole);
}

void MessagePage::on_copyToAddressButton_clicked()
{
    GUIUtil::copyEntryData(ui->tableView, MessageModel::ToAddress, Qt::DisplayRole);
}

void MessagePage::on_deleteButton_clicked()
{
    QListView *list = ui->listConversation;

    if(!list->selectionModel())
        return;

    QModelIndexList indexes = list->selectionModel()->selectedIndexes();

    if(!indexes.isEmpty())
    {
        list->model()->removeRow(indexes.at(0).row());
        indexes = list->selectionModel()->selectedIndexes();

        if(indexes.isEmpty())
            on_backButton_clicked();
    }
}

void MessagePage::conversationsRebuilt()
{
    // The conversation model just rebuilt (unlock, or a send/receive changed the messages).
    // The tableView (bound to the model) refreshes its rows automatically, and the thread
    // model auto-rebuilds on the same message-model change (it listens to rowsInserted). So
    // there is nothing to reload here -- crucially, we must simply NOT navigate back to the
    // list. If a thread is open, keep it open and scroll to the newest message.
    if(ui->messageDetails->isVisible())
    {
        ui->listConversation->scrollToBottom();
    }
}

void MessagePage::on_backButton_clicked()
{
    // Back-flow: return from a thread to the conversation list.
    ui->tableView->clearSelection();
    ui->listConversation->clearSelection();

    ui->messageDetails->hide();
    ui->tableView->show();

    ui->newButton->setEnabled(true);
    ui->newButton->setVisible(true);
    ui->sendButton->setEnabled(false);
    ui->sendButton->setVisible(false);
    if(chatComposer) { chatComposer->setVisible(false); chatComposer->clear(); }
    ui->contactLabel->clear();
}

void MessagePage::selectionChanged()
{
    // Back-flow: the top-level list (tableView) now shows CONVERSATIONS (ConversationModel).
    // Selecting one opens its thread: set the thread model's channel by pair key, capture the
    // reply addresses for that channel (so a reply always uses the correct my-address), and
    // switch from the conversation list to the thread + compose view. Deselecting returns to
    // the list state.
    QAbstractItemView *list = ui->tableView;

    if(!list->selectionModel())
        return;

    if(list->selectionModel()->hasSelection() && conversationModel)
    {
        int row = list->selectionModel()->selectedRows().isEmpty()
                    ? list->currentIndex().row()
                    : list->selectionModel()->selectedRows().first().row();

        if(row < 0 || row >= conversationModel->rowCount())
            return;

        // Channel identity for this conversation.
        QString pairKey      = conversationModel->pairKeyAt(row);
        QString counterparty = conversationModel->counterpartyAt(row);
        QString myAddress    = conversationModel->myAddressAt(row);
        QString displayName  = conversationModel->data(conversationModel->index(row, 0),
                                                       ConversationModel::DisplayNameRole).toString();

        // Reply addressing: the counterparty is the recipient, my-address is the sender for
        // this channel. This is what guarantees a reply is never mis-addressed.
        replyToAddress   = counterparty;
        replyFromAddress = myAddress;

        ui->contactLabel->setText(displayName);

        // Load the thread for this channel (oldest -> newest) and show it.
        threadModel->setPairKey(pairKey);

        replyAction->setEnabled(true);
        copyFromAddressAction->setEnabled(true);
        copyToAddressAction->setEnabled(true);
        deleteAction->setEnabled(true);

        ui->copyFromAddressButton->setEnabled(true);
        ui->copyToAddressButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);

        ui->newButton->setEnabled(false);
        ui->newButton->setVisible(false);
        ui->sendButton->setEnabled(true);
        ui->sendButton->setVisible(true);
        if(chatComposer) chatComposer->setVisible(true);

        ui->tableView->hide();
        ui->messageDetails->show();
        ui->listConversation->scrollToBottom();
    }
    else
    {
        // No conversation selected: show the conversation list, hide the thread/compose.
        ui->messageDetails->hide();
        ui->tableView->show();

        ui->newButton->setEnabled(true);
        ui->newButton->setVisible(true);
        ui->sendButton->setEnabled(false);
        ui->sendButton->setVisible(false);
        ui->copyFromAddressButton->setEnabled(false);
        ui->copyToAddressButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        if(chatComposer) { chatComposer->hide(); chatComposer->clear(); }
    }
}

void MessagePage::itemSelectionChanged()
{
    // Back-flow: the thread list (listConversation) shows message bubbles for the open
    // conversation. Selecting an individual bubble does not change page state -- the
    // conversation is already open and the compose/buttons are already configured by
    // selectionChanged(). Intentionally a no-op to avoid the old proxy-era side effects.
}

void MessagePage::incomingMessage()
{
    ui->listConversation->scrollToBottom();
}



void MessagePage::messageTextChanged()
{
    /*
    if(ui->messageEdit->toPlainText().endsWith("\n"))
    {
        ui->messageEdit->setMaximumHeight(80);
        ui->messageEdit->resize(256, ui->messageEdit->document()->size().height() + 10);
    }*/

}

void MessagePage::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(
            this,
            tr("Export Messages"), QString(),
            tr("Comma separated file (*.csv)"));

    if (filename.isNull()) return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(model->proxyModel);
    writer.addColumn("Type",             MessageModel::Type,             Qt::DisplayRole);
    writer.addColumn("Label",            MessageModel::Label,            Qt::DisplayRole);
    writer.addColumn("FromAddress",      MessageModel::FromAddress,      Qt::DisplayRole);
    writer.addColumn("ToAddress",        MessageModel::ToAddress,        Qt::DisplayRole);
    writer.addColumn("SentDateTime",     MessageModel::SentDateTime,     Qt::DisplayRole);
    writer.addColumn("ReceivedDateTime", MessageModel::ReceivedDateTime, Qt::DisplayRole);
    writer.addColumn("Message",          MessageModel::Message,          Qt::DisplayRole);

    if(!writer.write())
    {
        QMessageBox::critical(this, tr("Error exporting"), tr("Could not write to file %1.").arg(filename),
                              QMessageBox::Abort, QMessageBox::Abort);
    }
}


void MessagePage::contextualMenu(const QPoint &point)
{
    QModelIndex index = ui->tableView->indexAt(point);
    if(index.isValid())
    {
        contextMenu->exec(QCursor::pos());
    }
}

