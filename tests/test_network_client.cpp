#include <QtTest>

#include "client/network_client.h"
#include "server/relay_server.h"

class NetworkClientTest final : public QObject {
    Q_OBJECT
private slots:
    void connectsToRelayServer();
    void relaysTextBetweenClients();
    void refusesToSendBeforeConnection();
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

void NetworkClientTest::refusesToSendBeforeConnection()
{
    cliplink::NetworkClient client;
    QVERIFY(!client.sendText("not connected", "device-a", "Desktop"));
}

void NetworkClientTest::relaysTextBetweenClients()
{
    cliplink::RelayServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    cliplink::NetworkClient sender;
    cliplink::NetworkClient receiver;
    QSignalSpy senderConnected(&sender, &cliplink::NetworkClient::connected);
    QSignalSpy receiverConnected(&receiver, &cliplink::NetworkClient::connected);
    QSignalSpy received(&receiver, &cliplink::NetworkClient::clipboardReceived);
    sender.connectToServer("127.0.0.1", server.port());
    receiver.connectToServer("127.0.0.1", server.port());
    QTRY_VERIFY_WITH_TIMEOUT(senderConnected.count() == 1 && receiverConnected.count() == 1, 1'000);

    sender.sendText("end-to-end text", "device-a", "Desktop");
    QTRY_VERIFY_WITH_TIMEOUT(received.count() == 1, 1'000);
    QCOMPARE(received.at(0).at(1).toString(), QString("end-to-end text"));
}

QTEST_MAIN(NetworkClientTest)
#include "test_network_client.moc"
