#include "core/protocol.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace {

constexpr int kMaxPayloadBytes = 16 * 1024;

quint32 readLength(const QByteArray &frame)
{
    return (static_cast<quint32>(static_cast<unsigned char>(frame[0])) << 24U)
        | (static_cast<quint32>(static_cast<unsigned char>(frame[1])) << 16U)
        | (static_cast<quint32>(static_cast<unsigned char>(frame[2])) << 8U)
        | static_cast<quint32>(static_cast<unsigned char>(frame[3]));
}

QByteArray lengthPrefix(quint32 length)
{
    QByteArray prefix(4, Qt::Uninitialized);
    prefix[0] = static_cast<char>((length >> 24U) & 0xFFU);
    prefix[1] = static_cast<char>((length >> 16U) & 0xFFU);
    prefix[2] = static_cast<char>((length >> 8U) & 0xFFU);
    prefix[3] = static_cast<char>(length & 0xFFU);
    return prefix;
}

} // namespace

namespace cliplink {

QByteArray encodeFrame(const Message &message)
{
    const QJsonObject object{
        {"type", message.type},
        {"id", message.id},
        {"deviceId", message.deviceId},
        {"deviceName", message.deviceName},
        {"text", message.text},
    };
    const QByteArray payload = QJsonDocument(object).toJson(QJsonDocument::Compact);
    return lengthPrefix(static_cast<quint32>(payload.size())) + payload;
}

std::optional<Message> decodeFrame(const QByteArray &frame)
{
    if (frame.size() < 4) {
        return std::nullopt;
    }

    const quint32 payloadLength = readLength(frame);
    if (payloadLength > kMaxPayloadBytes || frame.size() != static_cast<qsizetype>(payloadLength) + 4) {
        return std::nullopt;
    }

    const QJsonDocument document = QJsonDocument::fromJson(frame.sliced(4));
    if (!document.isObject()) {
        return std::nullopt;
    }

    const QJsonObject object = document.object();
    const QString type = object.value("type").toString();
    const QString id = object.value("id").toString();
    if (type.isEmpty() || id.isEmpty()) {
        return std::nullopt;
    }

    return Message{
        type,
        id,
        object.value("deviceId").toString(),
        object.value("deviceName").toString(),
        object.value("text").toString(),
    };
}

} // namespace cliplink
