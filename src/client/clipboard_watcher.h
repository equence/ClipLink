#pragma once

#include "core/recent_ids.h"

#include <QObject>
#include <QString>

namespace cliplink {

class ClipboardWatcher final : public QObject {
    Q_OBJECT

public:
    explicit ClipboardWatcher(QString deviceId, QObject *parent = nullptr);

    void applyRemoteText(const QString &messageId, const QString &text);
    bool wasApplied(const QString &messageId) const;
    bool shouldSendCurrentClipboard() const;

signals:
    void localTextCopied(const QString &text);

private:
    void clipboardChanged();

    QString m_deviceId;
    QString m_lastRemoteText;
    RecentIds m_appliedIds{20};
};

} // namespace cliplink
