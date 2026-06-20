#include "client/network_client.h"

#include "core/protocol.h"

#include <QUuid>

namespace cliplink {

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
{
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this] { m_socket.connectToHost(m_host, m_port); });
    connect(&m_socket, &QTcpSocket::connected, this, [this] { m_reconnectAttempts = 0; emit connected(); });
    connect(&m_socket, &QTcpSocket::disconnected, this, [this] {
        emit disconnected();
        if (m_reconnectAttempts++ < 3) m_reconnectTimer.start(1'000);
    });
    connect(&m_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) { emit errorOccurred(m_socket.errorString()); });
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

void NetworkClient::connectToServer(const QString &host, quint16 port)
{
    m_host = host; m_port = port; m_reconnectAttempts = 0; m_reconnectTimer.stop();
    m_socket.abort(); m_socket.connectToHost(m_host, m_port);
}

void NetworkClient::sendText(const QString &text, const QString &deviceId, const QString &deviceName)
{
    m_socket.write(encodeFrame({"clipboard", QUuid::createUuid().toString(QUuid::WithoutBraces), deviceId, deviceName, text}));
}

} // namespace cliplink
