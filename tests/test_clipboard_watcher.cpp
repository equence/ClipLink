#include <QApplication>
#include <QClipboard>
#include <QtTest>

#include "client/clipboard_watcher.h"

class ClipboardWatcherTest final : public QObject {
    Q_OBJECT

private slots:
    void ignoresClipboardTextPresentAtStartup();
    void remoteMessageDoesNotCreateOutboundLoop();
    void emitsNewLocalClipboardText();
};

void ClipboardWatcherTest::ignoresClipboardTextPresentAtStartup()
{
    QGuiApplication::clipboard()->setText("clipboard text from before startup");

    cliplink::ClipboardWatcher watcher("device-a");
    QSignalSpy copied(&watcher, &cliplink::ClipboardWatcher::localTextCopied);

    QTest::qWait(700);
    QCOMPARE(copied.count(), 0);
}

void ClipboardWatcherTest::remoteMessageDoesNotCreateOutboundLoop()
{
    cliplink::ClipboardWatcher watcher("device-a");
    watcher.applyRemoteText("message-1", "from another device");

    QVERIFY(watcher.wasApplied("message-1"));
    QVERIFY(!watcher.shouldSendCurrentClipboard());
}

void ClipboardWatcherTest::emitsNewLocalClipboardText()
{
    cliplink::ClipboardWatcher watcher("device-a");
    QSignalSpy copied(&watcher, &cliplink::ClipboardWatcher::localTextCopied);
    QGuiApplication::clipboard()->setText("local clipboard text");
    QTRY_COMPARE_WITH_TIMEOUT(copied.count(), 1, 1'000);
    QCOMPARE(copied.at(0).at(0).toString(), QString("local clipboard text"));
}

QTEST_MAIN(ClipboardWatcherTest)

#include "test_clipboard_watcher.moc"
