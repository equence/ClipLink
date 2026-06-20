#include <QtTest>

#include "core/protocol.h"

class ProtocolTest final : public QObject {
    Q_OBJECT

private slots:
    void clipboardMessageRoundTrips();
    void rejectsInvalidFrames();
};

void ProtocolTest::clipboardMessageRoundTrips()
{
    const cliplink::Message message{
        "clipboard", "message-1", "device-a", "Laptop", "hello"};

    const auto decoded = cliplink::decodeFrame(cliplink::encodeFrame(message));

    QVERIFY(decoded.has_value());
    QCOMPARE(*decoded, message);
}

void ProtocolTest::rejectsInvalidFrames()
{
    const QByteArray invalidLength{"\0\0\0\x41", 4};
    QVERIFY(!cliplink::decodeFrame(invalidLength).has_value());
}

QTEST_MAIN(ProtocolTest)

#include "test_protocol.moc"
