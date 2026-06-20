#include "client/clipboard_watcher.h"

#include <QClipboard>
#include <QGuiApplication>

namespace cliplink {

ClipboardWatcher::ClipboardWatcher(QString deviceId, QObject *parent)
    : QObject(parent)
    , m_deviceId(std::move(deviceId))
{
    m_lastObservedText = QGuiApplication::clipboard()->text();
    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &ClipboardWatcher::clipboardChanged);
    m_pollTimer.setInterval(500);
    connect(&m_pollTimer, &QTimer::timeout, this, &ClipboardWatcher::observeClipboard);
    m_pollTimer.start();
}

void ClipboardWatcher::applyRemoteText(const QString &messageId, const QString &text)
{
    m_appliedIds.containsOrInsert(messageId);
    m_lastRemoteText = text;
    m_lastObservedText = text;
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
    observeClipboard();
}

void ClipboardWatcher::observeClipboard()
{
    const QString text = QGuiApplication::clipboard()->text();
    if (text == m_lastObservedText) {
        return;
    }

    m_lastObservedText = text;
    if (shouldSendCurrentClipboard()) {
        emit localTextCopied(text);
    }
}

} // namespace cliplink
