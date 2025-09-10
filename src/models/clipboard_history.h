#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "clipboard_item.h"

/**
 * @brief Manages a collection of clipboard items with ordering and size limits
 * 
 * ClipboardHistory maintains an ordered list of clipboard items where:
 * - Pinned items always appear at the top
 * - Regular items are ordered by timestamp (newest first)
 * - The total number of items never exceeds maxItems
 * - Duplicate items are handled by updating existing items
 */
class ClipboardHistory : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor with standard settings
     */
    explicit ClipboardHistory(QObject* parent = nullptr);
    
    /**
     * @brief Constructor with custom maximum items
     * @param maxItems Maximum number of items to store (10-100)
     * @param parent Parent QObject
     */
    explicit ClipboardHistory(int maxItems, QObject* parent = nullptr);
    
    /**
     * @brief Load history from JSON object
     * @param json JSON object containing history data
     * @param parent Parent QObject
     */
    explicit ClipboardHistory(const QJsonObject& json, QObject* parent = nullptr);

    // Getters
    int maxItems() const { return m_maxItems; }
    int count() const { return m_items.count(); }
    int pinnedCount() const { return m_pinnedCount; }
    bool isEmpty() const { return m_items.isEmpty(); }
    bool isFull() const { return m_items.count() >= m_maxItems; }
    
    /**
     * @brief Get all items in display order (pinned first, then by timestamp)
     * @return Ordered list of all items
     */
    QList<ClipboardItem> items() const { return m_items; }
    
    /**
     * @brief Get only pinned items
     * @return List of pinned items
     */
    QList<ClipboardItem> pinnedItems() const;
    
    /**
     * @brief Get only unpinned items
     * @return List of unpinned items ordered by timestamp
     */
    QList<ClipboardItem> unpinnedItems() const;

    // Configuration
    void setMaxItems(int maxItems);

    // Item Operations
    /**
     * @brief Add new clipboard item
     * @param text Text content to add
     * @return ID of the added/updated item
     * 
     * Handles duplicates by updating existing items and moving to top.
     * Enforces size limits by removing oldest unpinned items if needed.
     */
    QString addItem(const QString& text);
    
    /**
     * @brief Add an existing clipboard item
     * @param item Item to add
     * @return ID of the added/updated item
     */
    QString addItem(const ClipboardItem& item);
    
    /**
     * @brief Pin item by ID
     * @param id Item ID to pin
     * @return true if item was found and pinned
     */
    bool pinItem(const QString& id);
    
    /**
     * @brief Unpin item by ID
     * @param id Item ID to unpin
     * @return true if item was found and unpinned
     */
    bool unpinItem(const QString& id);
    
    /**
     * @brief Toggle pin state of item by ID
     * @param id Item ID to toggle
     * @return true if item was found and toggled
     */
    bool togglePinItem(const QString& id);
    
    /**
     * @brief Remove item by ID (only unpinned items can be removed)
     * @param id Item ID to remove
     * @return true if item was found and removed
     */
    bool removeItem(const QString& id);
    
    /**
     * @brief Remove all unpinned items
     */
    void clear();
    
    /**
     * @brief Remove all items (including pinned)
     */
    void clearAll();
    
    /**
     * @brief Get item by ID
     * @param id Item ID to find
     * @return ClipboardItem if found, invalid item otherwise
     */
    ClipboardItem getItem(const QString& id) const;
    
    /**
     * @brief Get item by index in display order
     * @param index Index in the ordered list
     * @return ClipboardItem if index is valid, invalid item otherwise
     */
    ClipboardItem getItemAt(int index) const;
    
    /**
     * @brief Find index of item by ID
     * @param id Item ID to find
     * @return Index in display order, or -1 if not found
     */
    int findItemIndex(const QString& id) const;
    
    /**
     * @brief Check if item with ID exists
     * @param id Item ID to check
     * @return true if item exists
     */
    bool hasItem(const QString& id) const;
    
    /**
     * @brief Check if text content already exists in history
     * @param text Text content to check
     * @return true if duplicate content exists
     */
    bool hasDuplicate(const QString& text) const;

    // Serialization
    /**
     * @brief Serialize to JSON object
     * @return JSON representation of the history
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Load data from JSON object
     * @param json JSON object to load from
     * @return true if loading was successful
     */
    bool fromJson(const QJsonObject& json);

signals:
    /**
     * @brief Emitted when an item is added to the history
     * @param item The added item
     */
    void itemAdded(const ClipboardItem& item);
    
    /**
     * @brief Emitted when an existing item is updated
     * @param item The updated item
     */
    void itemUpdated(const ClipboardItem& item);
    
    /**
     * @brief Emitted when an item is removed from the history
     * @param id ID of the removed item
     */
    void itemRemoved(const QString& id);
    
    /**
     * @brief Emitted when an item is pinned
     * @param id ID of the pinned item
     */
    void itemPinned(const QString& id);
    
    /**
     * @brief Emitted when an item is unpinned
     * @param id ID of the unpinned item
     */
    void itemUnpinned(const QString& id);
    
    /**
     * @brief Emitted when the history is cleared
     */
    void historyCleared();
    
    /**
     * @brief Emitted when the order of items changes
     */
    void orderChanged();

private:
    /**
     * @brief Enforce size limits by removing oldest unpinned items
     */
    void enforceSizeLimit();
    
    /**
     * @brief Reorder items to maintain display order (pinned first, then by timestamp)
     */
    void reorderItems();
    
    /**
     * @brief Update pinned count based on current items
     */
    void updatePinnedCount();
    
    /**
     * @brief Find item by ID
     * @param id Item ID to find
     * @return Iterator to the item, or end() if not found
     */
    QList<ClipboardItem>::iterator findItem(const QString& id);
    
    /**
     * @brief Find item by ID (const version)
     * @param id Item ID to find
     * @return Const iterator to the item, or end() if not found
     */
    QList<ClipboardItem>::const_iterator findItem(const QString& id) const;
    
    /**
     * @brief Find item by content hash
     * @param hash Content hash to find
     * @return Iterator to the item, or end() if not found
     */
    QList<ClipboardItem>::iterator findItemByHash(const QString& hash);

    static constexpr int DEFAULT_MAX_ITEMS = 50;
    static constexpr int MIN_MAX_ITEMS = 10;
    static constexpr int MAX_MAX_ITEMS = 100;

    QList<ClipboardItem> m_items;      ///< Ordered list of clipboard items
    int m_maxItems;                    ///< Maximum number of items to store
    int m_pinnedCount;                 ///< Current number of pinned items
};
