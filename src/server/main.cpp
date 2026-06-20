#include "server/relay_server.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    application.setApplicationName("cliplink_server");

    QCommandLineParser parser;
    parser.addHelpOption();
    const QCommandLineOption portOption({"p", "port"}, "Listening port.", "port", "45454");
    parser.addOption(portOption);
    parser.process(application);

    bool validPort = false;
    const quint16 port = parser.value(portOption).toUShort(&validPort);
    if (!validPort || port == 0) {
        qCritical() << "Port must be between 1 and 65535.";
        return 1;
    }

    cliplink::RelayServer server;
    if (!server.listen(QHostAddress::Any, port)) {
        qCritical() << "Unable to listen on port" << port;
        return 1;
    }

    qInfo() << "ClipLink relay server listening on port" << server.port();
    return application.exec();
}
