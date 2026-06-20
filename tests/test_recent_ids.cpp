#include <QtTest>

#include "core/recent_ids.h"

class RecentIdsTest final : public QObject {
    Q_OBJECT

private slots:
    void recognizesSeenIdsAndEvictsOldest();
};

void RecentIdsTest::recognizesSeenIdsAndEvictsOldest()
{
    cliplink::RecentIds ids(2);

    QVERIFY(!ids.containsOrInsert("one"));
    QVERIFY(!ids.containsOrInsert("two"));
    QVERIFY(ids.containsOrInsert("one"));
    QVERIFY(!ids.containsOrInsert("three"));
    QVERIFY(!ids.containsOrInsert("two"));
}

QTEST_MAIN(RecentIdsTest)

#include "test_recent_ids.moc"
