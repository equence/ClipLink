#include "client/main_window.h"

#include <QDateTime>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>

namespace cliplink {

MainWindow::MainWindow(QString deviceName, QWidget *parent)
    : QMainWindow(parent), m_deviceName(std::move(deviceName))
{
    setWindowTitle("ClipLink");
    resize(900, 600);
    setStyleSheet("QMainWindow{background:#F8FAFC;} QFrame#card{background:white;border:1px solid #E2E8F0;border-radius:12px;} "
                  "QPushButton{background:#312E81;color:white;border:0;border-radius:7px;padding:8px 14px;font-weight:600;} "
                  "QPlainTextEdit,QListWidget{border:1px solid #E2E8F0;border-radius:8px;background:#FFF;} QListWidget::item{padding:10px;border-bottom:1px solid #F1F5F9;}");
    auto *central = new QWidget(this); auto *outer = new QVBoxLayout(central); outer->setContentsMargins(28,24,28,24); outer->setSpacing(16);
    auto *header = new QHBoxLayout; auto *title = new QLabel("ClipLink"); title->setStyleSheet("font-size:26px;font-weight:700;color:#312E81;");
    m_statusLabel = new QLabel("● 未连接"); m_statusLabel->setStyleSheet("color:#F59E0B;font-weight:600;");
    auto *connectButton = new QPushButton("连接服务器"); header->addWidget(title); header->addStretch(); header->addWidget(m_statusLabel); header->addWidget(connectButton); outer->addLayout(header);
    auto *card = new QFrame; card->setObjectName("card"); auto *layout = new QVBoxLayout(card); layout->setContentsMargins(16,16,16,16);
    auto *splitter = new QSplitter; m_history = new QListWidget; m_history->setMinimumWidth(260); m_detail = new QPlainTextEdit; m_detail->setReadOnly(true); m_detail->setPlaceholderText("选择一条同步记录查看完整内容"); splitter->addWidget(m_history); splitter->addWidget(m_detail); splitter->setStretchFactor(1,1); layout->addWidget(splitter);
    m_composer = new QPlainTextEdit; m_composer->setPlaceholderText("输入文本并发送到已连接设备…"); m_composer->setFixedHeight(82); auto *send = new QPushButton("发送"); auto *bottom = new QHBoxLayout; bottom->addWidget(m_composer,1); bottom->addWidget(send); layout->addLayout(bottom); outer->addWidget(card,1); setCentralWidget(central);
    connect(m_history, &QListWidget::currentRowChanged, this, [this](int row){ if (auto *item=m_history->item(row)) m_detail->setPlainText(item->data(Qt::UserRole).toString()); });
    connect(send, &QPushButton::clicked, this, [this]{ const QString text=m_composer->toPlainText().trimmed(); if(!text.isEmpty()){ emit sendRequested(text); m_composer->clear(); }});
    connect(connectButton, &QPushButton::clicked, this, [this]{ bool ok=false; const QString host=QInputDialog::getText(this,"连接 ClipLink","服务器地址",QLineEdit::Normal,"127.0.0.1",&ok); if(ok) emit connectRequested(host,45454); });
}

void MainWindow::appendHistory(const QString &source, const QString &text)
{
    auto *item = new QListWidgetItem(QString("%1\n%2 · %3").arg(text.left(64), source, QDateTime::currentDateTime().toString("HH:mm")));
    item->setData(Qt::UserRole, text); m_history->insertItem(0,item); m_history->setCurrentRow(0);
}
QString MainWindow::historyPreviewAt(int index) const { return m_history->item(index) ? m_history->item(index)->text().section('\n',0,0) : QString{}; }
void MainWindow::setConnected(bool connected) { m_statusLabel->setText(connected ? "● 已连接" : "● 未连接"); m_statusLabel->setStyleSheet(connected ? "color:#14B8A6;font-weight:600;" : "color:#F59E0B;font-weight:600;"); }
} // namespace cliplink
