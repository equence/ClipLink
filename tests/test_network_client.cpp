#include <QtTest>

#include "client/network_client.h"
#include "server/relay_server.h"

class NetworkClientTest final : public QObject {
    Q_OBJECT
private slots:
    void connectsToRelayServer();
};

void NetworkClientTest::connectsToRelayServer()
{
    cliplink::RelayServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    cliplink::NetworkClient client;
    QSignalSpy connected(&client, &cliplink::NetworkClient::connected);
    client.connectToServer("127.0.0.1", server.port());
    QTRY_VERIFY_WITH_TIMEOUT(connected.count() == 1, 1'000);
}

QTEST_MAIN(NetworkClientTest)
#include "test_network_client.moc"
