#include "client/clipboard_watcher.h"

#include <QClipboard>
#include <QGuiApplication>

namespace cliplink {

ClipboardWatcher::ClipboardWatcher(QString deviceId, QObject *parent)
    : QObject(parent)
    , m_deviceId(std::move(deviceId))
{
    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &ClipboardWatcher::clipboardChanged);
}

void ClipboardWatcher::applyRemoteText(const QString &messageId, const QString &text)
{
    m_appliedIds.containsOrInsert(messageId);
    m_lastRemoteText = text;
    QGuiApplication::clipboard()->setText(text);
}

bool ClipboardWatcher::wasApplied(const QString &messageId) const
{
    return m_appliedIds.contains(messageId);
}

bool ClipboardWatcher::shouldSendCurrentClipboard() const
{
    const QString text = QGuiApplication::clipboard()->text();
    return !text.isEmpty() && text != m_lastRemoteText;
}

void ClipboardWatcher::clipboardChanged()
{
    if (shouldSendCurrentClipboard()) {
        emit localTextCopied(QGuiApplication::clipboard()->text());
    }
}

} // namespace cliplink
