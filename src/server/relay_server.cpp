#include "server/relay_server.h"

#include "core/protocol.h"

#include <QTcpSocket>

namespace {

quint32 frameLength(const QByteArray &buffer)
{
    return (static_cast<quint32>(static_cast<unsigned char>(buffer[0])) << 24U)
        | (static_cast<quint32>(static_cast<unsigned char>(buffer[1])) << 16U)
        | (static_cast<quint32>(static_cast<unsigned char>(buffer[2])) << 8U)
        | static_cast<quint32>(static_cast<unsigned char>(buffer[3]));
}

} // namespace

namespace cliplink {

RelayServer::RelayServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, &RelayServer::acceptConnections);
}

RelayServer::~RelayServer()
{
    for (QTcpSocket *socket : m_buffers.keys()) {
        socket->disconnect(this);
    }
}

bool RelayServer::listen(const QHostAddress &address, quint16 port)
{
    return m_server.listen(address, port);
}

quint16 RelayServer::port() const
{
    return m_server.serverPort();
}

void RelayServer::acceptConnections()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket *socket = m_server.nextPendingConnection();
        m_buffers.insert(socket, {});
        connect(socket, &QTcpSocket::readyRead, this, [this, socket] { readFromClient(socket); });
        connect(socket, &QTcpSocket::disconnected, this, [this, socket] { removeClient(socket); });
    }
}

void RelayServer::readFromClient(QTcpSocket *socket)
{
    QByteArray &buffer = m_buffers[socket];
    buffer.append(socket->readAll());

    while (buffer.size() >= 4) {
        const quint32 payloadLength = frameLength(buffer);
        if (payloadLength > 16 * 1024) {
            socket->disconnectFromHost();
            return;
        }

        const qsizetype frameSize = static_cast<qsizetype>(payloadLength) + 4;
        if (buffer.size() < frameSize) {
            return;
        }

        const QByteArray frame = buffer.first(frameSize);
        buffer.remove(0, frameSize);
        const auto message = decodeFrame(frame);
        if (message && message->type == "clipboard") {
            broadcast(frame, socket);
        }
    }
}

void RelayServer::removeClient(QTcpSocket *socket)
{
    m_buffers.remove(socket);
    socket->deleteLater();
}

void RelayServer::broadcast(const QByteArray &frame, QTcpSocket *sender)
{
    for (QTcpSocket *socket : m_buffers.keys()) {
        if (socket != sender && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(frame);
        }
    }
}

} // namespace cliplink
