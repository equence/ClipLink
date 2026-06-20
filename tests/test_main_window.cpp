#include <QtTest>

#include "client/main_window.h"

class MainWindowTest final : public QObject {
    Q_OBJECT
private slots:
    void showsNewestClipboardEntryFirst();
};

void MainWindowTest::showsNewestClipboardEntryFirst()
{
    cliplink::MainWindow window("Device A");
    window.appendHistory("Desktop", "first");
    window.appendHistory("Laptop", "second");
    QCOMPARE(window.historyPreviewAt(0), QString("second"));
}

QTEST_MAIN(MainWindowTest)
#include "test_main_window.moc"
