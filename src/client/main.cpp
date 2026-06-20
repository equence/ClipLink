#include "client/clipboard_watcher.h"
#include "client/main_window.h"
#include "client/network_client.h"

#include <QApplication>
#include <QSettings>
#include <QStatusBar>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); app.setApplicationName("ClipLink");
    QSettings settings; const QString deviceName = settings.value("deviceName", QSysInfo::machineHostName()).toString();
    cliplink::MainWindow window(deviceName); cliplink::NetworkClient network; cliplink::ClipboardWatcher clipboard(deviceName);
    QObject::connect(&window,&cliplink::MainWindow::connectRequested,&network,&cliplink::NetworkClient::connectToServer);
    QObject::connect(&window,&cliplink::MainWindow::sendRequested,&network,[&](const QString &text){ network.sendText(text,deviceName,deviceName); window.appendHistory(deviceName,text); });
    QObject::connect(&clipboard,&cliplink::ClipboardWatcher::localTextCopied,&network,[&](const QString &text){ network.sendText(text,deviceName,deviceName); window.appendHistory(deviceName,text); });
    QObject::connect(&network,&cliplink::NetworkClient::clipboardReceived,&clipboard,&cliplink::ClipboardWatcher::applyRemoteText);
    QObject::connect(&network,&cliplink::NetworkClient::clipboardReceived,&window,[&](const QString &,const QString &text){ window.appendHistory("远端设备",text); });
    QObject::connect(&network,&cliplink::NetworkClient::connected,&window,[&]{window.setConnected(true);}); QObject::connect(&network,&cliplink::NetworkClient::disconnected,&window,[&]{window.setConnected(false);});
    QObject::connect(&network,&cliplink::NetworkClient::errorOccurred,&window,[&](const QString &message){ window.statusBar()->showMessage(message, 4'000); });
    window.show(); return app.exec();
}
