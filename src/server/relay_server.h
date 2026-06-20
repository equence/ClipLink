#pragma once

#include <QHash>
#include <QTcpServer>

namespace cliplink {

class RelayServer final : public QObject {
    Q_OBJECT

public:
    explicit RelayServer(QObject *parent = nullptr);
    ~RelayServer() override;

    bool listen(const QHostAddress &address, quint16 port);
    quint16 port() const;

private:
    void acceptConnections();
    void readFromClient(QTcpSocket *socket);
    void removeClient(QTcpSocket *socket);
    void broadcast(const QByteArray &frame, QTcpSocket *sender);

    QTcpServer m_server;
    QHash<QTcpSocket *, QByteArray> m_buffers;
};

} // namespace cliplink
