#pragma once

#include <QMainWindow>

class QLabel;
class QListWidget;
class QPlainTextEdit;

namespace cliplink {

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QString deviceName, QWidget *parent = nullptr);
    void appendHistory(const QString &source, const QString &text);
    QString historyPreviewAt(int index) const;
    void setConnected(bool connected);

signals:
    void sendRequested(const QString &text);
    void connectRequested(const QString &host, quint16 port);
    void settingsChanged(const QString &deviceName, const QString &host, quint16 port);

private:
    QString m_deviceName;
    QLabel *m_statusLabel{};
    QListWidget *m_history{};
    QPlainTextEdit *m_detail{};
    QPlainTextEdit *m_composer{};
};

} // namespace cliplink
