#include "core/recent_ids.h"

namespace cliplink {

RecentIds::RecentIds(int capacity)
    : m_capacity(qMax(1, capacity))
{
}

bool RecentIds::containsOrInsert(const QString &id)
{
    if (m_ids.contains(id)) {
        m_order.removeOne(id);
        m_order.enqueue(id);
        return true;
    }

    m_ids.insert(id);
    m_order.enqueue(id);
    while (m_order.size() > m_capacity) {
        m_ids.remove(m_order.dequeue());
    }
    return false;
}

} // namespace cliplink
