#pragma once

#include <QQueue>
#include <QSet>
#include <QString>

namespace cliplink {

class RecentIds {
public:
    explicit RecentIds(int capacity);

    bool contains(const QString &id) const;
    bool containsOrInsert(const QString &id);

private:
    int m_capacity;
    QQueue<QString> m_order;
    QSet<QString> m_ids;
};

} // namespace cliplink
