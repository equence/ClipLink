#include "client/network_client.h"

#include "core/protocol.h"

#include <QUuid>

namespace cliplink {

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::connected, this, &NetworkClient::connected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &NetworkClient::disconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, [this] {
        m_buffer += m_socket.readAll();
        while (m_buffer.size() >= 4) {
            const quint32 size = (quint32(uchar(m_buffer[0])) << 24U) | (quint32(uchar(m_buffer[1])) << 16U)
                | (quint32(uchar(m_buffer[2])) << 8U) | quint32(uchar(m_buffer[3]));
            if (size > 16 * 1024 || m_buffer.size() < qsizetype(size) + 4) return;
            const auto message = decodeFrame(m_buffer.first(qsizetype(size) + 4));
            m_buffer.remove(0, qsizetype(size) + 4);
            if (message && message->type == "clipboard") emit clipboardReceived(message->id, message->text);
        }
    });
}

void NetworkClient::connectToServer(const QString &host, quint16 port) { m_socket.connectToHost(host, port); }

void NetworkClient::sendText(const QString &text, const QString &deviceId, const QString &deviceName)
{
    m_socket.write(encodeFrame({"clipboard", QUuid::createUuid().toString(QUuid::WithoutBraces), deviceId, deviceName, text}));
}

} // namespace cliplink
