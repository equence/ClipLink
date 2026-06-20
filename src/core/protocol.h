#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace cliplink {

struct Message {
    QString type;
    QString id;
    QString deviceId;
    QString deviceName;
    QString text;

    friend bool operator==(const Message &left, const Message &right)
    {
        return left.type == right.type && left.id == right.id && left.deviceId == right.deviceId
            && left.deviceName == right.deviceName && left.text == right.text;
    }
};

QByteArray encodeFrame(const Message &message);
std::optional<Message> decodeFrame(const QByteArray &frame);

} // namespace cliplink
