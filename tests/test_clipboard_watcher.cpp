#include <QApplication>
#include <QtTest>

#include "client/clipboard_watcher.h"

class ClipboardWatcherTest final : public QObject {
    Q_OBJECT

private slots:
    void remoteMessageDoesNotCreateOutboundLoop();
};

void ClipboardWatcherTest::remoteMessageDoesNotCreateOutboundLoop()
{
    cliplink::ClipboardWatcher watcher("device-a");
    watcher.applyRemoteText("message-1", "from another device");

    QVERIFY(watcher.wasApplied("message-1"));
    QVERIFY(!watcher.shouldSendCurrentClipboard());
}

QTEST_MAIN(ClipboardWatcherTest)

#include "test_clipboard_watcher.moc"
