#include "adrenalinenodeconfigdialog.h"
#include "ui_adrenalinenodeconfigdialog.h"

#include <QModelIndex>

AdrenalineNodeConfigDialog::AdrenalineNodeConfigDialog(QWidget *parent, QString nodeAddress, QString privkey) :
    QDialog(parent),
    ui(new Ui::AdrenalineNodeConfigDialog)
{
    ui->setupUi(this);
    // v2.0.0.9 W-16: rpcallowip=127.0.0.1 removed from this template.  Loopback
    // is always allowed, and the line used to switch RPC to listen on every
    // interface.  Leaving it out keeps a node set up from this dialog on
    // loopback-only RPC.
    QString desc = "rpcuser=REPLACEME<br>rpcpassword=REPLACEME<br>server=1<br>listen=1<br>port=REPLACEMEWITHYOURPORT<br>masternode=1<br>masternodeaddr=" + nodeAddress + "<br>masternodeprivkey=" + privkey + "<br>";
    ui->detailText->setHtml(desc);
}

AdrenalineNodeConfigDialog::~AdrenalineNodeConfigDialog()
{
    delete ui;
}
