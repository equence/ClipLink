#include <QSignalSpy>
#include <QTcpSocket>
#include <QtTest>

#include "core/protocol.h"
#include "server/relay_server.h"

class RelayServerTest final : public QObject {
    Q_OBJECT

private slots:
    void forwardsClipboardFrameToOtherClient();
};

void RelayServerTest::forwardsClipboardFrameToOtherClient()
{
    cliplink::RelayServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    QTcpSocket sender;
    QTcpSocket receiver;
    receiver.connectToHost(QHostAddress::LocalHost, server.port());
    sender.connectToHost(QHostAddress::LocalHost, server.port());
    QVERIFY(receiver.waitForConnected());
    QVERIFY(sender.waitForConnected());

    const cliplink::Message message{
        "clipboard", "message-1", "device-a", "Laptop", "hello"};
    QSignalSpy receiverReady(&receiver, &QTcpSocket::readyRead);
    sender.write(cliplink::encodeFrame(message));
    QVERIFY(sender.waitForBytesWritten());
    QTRY_VERIFY_WITH_TIMEOUT(receiverReady.count() > 0, 1'000);

    const auto decoded = cliplink::decodeFrame(receiver.readAll());
    QVERIFY(decoded.has_value());
    QCOMPARE(*decoded, message);
}

QTEST_MAIN(RelayServerTest)

#include "test_relay_server.moc"
