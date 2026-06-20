#pragma once

#include "core/recent_ids.h"

#include <QObject>
#include <QString>
#include <QTimer>

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
    void observeClipboard();

    QString m_deviceId;
    QString m_lastRemoteText;
    QString m_lastObservedText;
    RecentIds m_appliedIds{20};
    QTimer m_pollTimer;
};

} // namespace cliplink
