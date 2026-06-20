#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

namespace cliplink {

class NetworkClient final : public QObject {
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    bool sendText(const QString &text, const QString &deviceId, const QString &deviceName);
signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &message);
    void clipboardReceived(const QString &id, const QString &text);
private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
    QTimer m_reconnectTimer;
    QString m_host;
    quint16 m_port{};
    int m_reconnectAttempts{};
};

} // namespace cliplink
