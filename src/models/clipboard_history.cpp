#include "clipboard_history.h"
#include <QJsonDocument>
#include <QDebug>
#include <algorithm>

ClipboardHistory::ClipboardHistory(QObject* parent)
    : QObject(parent)
    , m_maxItems(DEFAULT_MAX_ITEMS)
    , m_pinnedCount(0)
{
}

ClipboardHistory::ClipboardHistory(int maxItems, QObject* parent)
    : QObject(parent)
    , m_maxItems(qBound(MIN_MAX_ITEMS, maxItems, MAX_MAX_ITEMS))
    , m_pinnedCount(0)
{
}

ClipboardHistory::ClipboardHistory(const QJsonObject& json, QObject* parent)
    : QObject(parent)
    , m_maxItems(DEFAULT_MAX_ITEMS)
    , m_pinnedCount(0)
{
    fromJson(json);
}

QList<ClipboardItem> ClipboardHistory::pinnedItems() const
{
    QList<ClipboardItem> pinned;
    for (const auto& item : m_items) {
        if (item.pinned()) {
            pinned.append(item);
        } else {
            break; // Pinned items are always at the beginning
        }
    }
    return pinned;
}

QList<ClipboardItem> ClipboardHistory::unpinnedItems() const
{
    QList<ClipboardItem> unpinned;
    for (const auto& item : m_items) {
        if (!item.pinned()) {
            unpinned.append(item);
        }
    }
    return unpinned;
}

void ClipboardHistory::setMaxItems(int maxItems)
{
    int newMaxItems = qBound(MIN_MAX_ITEMS, maxItems, MAX_MAX_ITEMS);
    if (newMaxItems != m_maxItems) {
        m_maxItems = newMaxItems;
        enforceSizeLimit();
    }
}

QString ClipboardHistory::addItem(const QString& text)
{
    ClipboardItem item(text);
    return addItem(item);
}

QString ClipboardHistory::addItem(const ClipboardItem& item)
{
    if (!item.isValid()) {
        return QString();
    }

    // Check for duplicate by hash
    auto existingIt = findItemByHash(item.hash());
    if (existingIt != m_items.end()) {
        // Update existing item timestamp and move to appropriate position
        ClipboardItem updatedItem = *existingIt;
        updatedItem = ClipboardItem(item.text()); // Create new with current timestamp
        if (existingIt->pinned()) {
            updatedItem.pin(); // Preserve pin state
        }
        
        m_items.erase(existingIt);
        m_items.append(updatedItem);
        reorderItems();
        
        emit itemUpdated(updatedItem);
        emit orderChanged();
        return updatedItem.id();
    }

    // Add new item
    m_items.append(item);
    reorderItems();
    enforceSizeLimit();
    updatePinnedCount();

    emit itemAdded(item);
    emit orderChanged();
    return item.id();
}

bool ClipboardHistory::pinItem(const QString& id)
{
    auto it = findItem(id);
    if (it != m_items.end() && !it->pinned()) {
        it->pin();
        reorderItems();
        updatePinnedCount();
        
        emit itemPinned(id);
        emit orderChanged();
        return true;
    }
    return false;
}

bool ClipboardHistory::unpinItem(const QString& id)
{
    auto it = findItem(id);
    if (it != m_items.end() && it->pinned()) {
        it->unpin();
        reorderItems();
        updatePinnedCount();
        
        emit itemUnpinned(id);
        emit orderChanged();
        return true;
    }
    return false;
}

bool ClipboardHistory::togglePinItem(const QString& id)
{
    auto it = findItem(id);
    if (it != m_items.end()) {
        if (it->pinned()) {
            return unpinItem(id);
        } else {
            return pinItem(id);
        }
    }
    return false;
}

bool ClipboardHistory::removeItem(const QString& id)
{
    auto it = findItem(id);
    if (it != m_items.end() && !it->pinned()) {
        m_items.erase(it);
        updatePinnedCount();
        
        emit itemRemoved(id);
        emit orderChanged();
        return true;
    }
    return false;
}

void ClipboardHistory::clear()
{
    bool changed = false;
    auto it = m_items.begin();
    while (it != m_items.end()) {
        if (!it->pinned()) {
            QString removedId = it->id();
            it = m_items.erase(it);
            emit itemRemoved(removedId);
            changed = true;
        } else {
            ++it;
        }
    }
    
    if (changed) {
        updatePinnedCount();
        emit historyCleared();
        emit orderChanged();
    }
}

void ClipboardHistory::clearAll()
{
    if (!m_items.isEmpty()) {
        QStringList removedIds;
        for (const auto& item : m_items) {
            removedIds.append(item.id());
        }
        
        m_items.clear();
        m_pinnedCount = 0;
        
        for (const QString& id : removedIds) {
            emit itemRemoved(id);
        }
        emit historyCleared();
        emit orderChanged();
    }
}

ClipboardItem ClipboardHistory::getItem(const QString& id) const
{
    auto it = findItem(id);
    if (it != m_items.end()) {
        return *it;
    }
    return ClipboardItem(); // Invalid item
}

ClipboardItem ClipboardHistory::getItemAt(int index) const
{
    if (index >= 0 && index < m_items.count()) {
        return m_items.at(index);
    }
    return ClipboardItem(); // Invalid item
}

int ClipboardHistory::findItemIndex(const QString& id) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i).id() == id) {
            return i;
        }
    }
    return -1;
}

bool ClipboardHistory::hasItem(const QString& id) const
{
    return findItem(id) != m_items.end();
}

bool ClipboardHistory::hasDuplicate(const QString& text) const
{
    QString hash = ClipboardItem::generateHash(text);
    return findItemByHash(hash) != m_items.end();
}

QJsonObject ClipboardHistory::toJson() const
{
    QJsonObject json;
    json["maxItems"] = m_maxItems;
    
    QJsonArray itemsArray;
    for (const auto& item : m_items) {
        itemsArray.append(item.toJson());
    }
    json["items"] = itemsArray;
    
    return json;
}

bool ClipboardHistory::fromJson(const QJsonObject& json)
{
    m_items.clear();
    m_pinnedCount = 0;
    
    // Load maxItems
    if (json.contains("maxItems")) {
        setMaxItems(json["maxItems"].toInt(DEFAULT_MAX_ITEMS));
    }
    
    // Load items
    if (json.contains("items") && json["items"].isArray()) {
        QJsonArray itemsArray = json["items"].toArray();
        for (const auto& value : itemsArray) {
            if (value.isObject()) {
                ClipboardItem item(value.toObject());
                if (item.isValid()) {
                    m_items.append(item);
                }
            }
        }
    }
    
    reorderItems();
    updatePinnedCount();
    return true;
}

void ClipboardHistory::enforceSizeLimit()
{
    while (m_items.count() > m_maxItems) {
        // Remove oldest unpinned item
        bool removed = false;
        for (auto it = m_items.end() - 1; it >= m_items.begin(); --it) {
            if (!it->pinned()) {
                QString removedId = it->id();
                m_items.erase(it);
                emit itemRemoved(removedId);
                removed = true;
                break;
            }
        }
        
        if (!removed) {
            // All items are pinned, can't remove any more
            break;
        }
    }
}

void ClipboardHistory::reorderItems()
{
    // Sort items: pinned first (by timestamp desc), then unpinned by timestamp desc
    std::stable_sort(m_items.begin(), m_items.end(), [](const ClipboardItem& a, const ClipboardItem& b) {
        // Pinned items always come first
        if (a.pinned() && !b.pinned()) {
            return true;
        }
        if (!a.pinned() && b.pinned()) {
            return false;
        }
        
        // Within same pin state, sort by timestamp (newer first)
        return a.timestamp() > b.timestamp();
    });
}

void ClipboardHistory::updatePinnedCount()
{
    m_pinnedCount = 0;
    for (const auto& item : m_items) {
        if (item.pinned()) {
            ++m_pinnedCount;
        }
    }
}

QList<ClipboardItem>::iterator ClipboardHistory::findItem(const QString& id)
{
    return std::find_if(m_items.begin(), m_items.end(), 
                       [&id](const ClipboardItem& item) {
                           return item.id() == id;
                       });
}

QList<ClipboardItem>::const_iterator ClipboardHistory::findItem(const QString& id) const
{
    return std::find_if(m_items.begin(), m_items.end(), 
                       [&id](const ClipboardItem& item) {
                           return item.id() == id;
                       });
}

QList<ClipboardItem>::iterator ClipboardHistory::findItemByHash(const QString& hash)
{
    return std::find_if(m_items.begin(), m_items.end(), 
                       [&hash](const ClipboardItem& item) {
                           return item.hash() == hash;
                       });
}
