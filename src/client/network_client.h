#pragma once

#include <QObject>
#include <QTcpSocket>

namespace cliplink {

class NetworkClient final : public QObject {
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void sendText(const QString &text, const QString &deviceId, const QString &deviceName);
signals:
    void connected();
    void disconnected();
    void clipboardReceived(const QString &id, const QString &text);
private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
};

} // namespace cliplink
