#ifndef CLIPBOARD_ITEM_H
#define CLIPBOARD_ITEM_H

#include <QString>
#include <QDateTime>
#include <QUuid>
#include <QJsonObject>
#include <QCryptographicHash>

/**
 * @brief Represents a single clipboard entry with metadata and content
 * 
 * ClipboardItem stores clipboard content along with metadata like timestamps,
 * pin status, and derived fields like preview text and content hash.
 * Items are immutable once created except for the pinned state.
 */
class ClipboardItem
{
public:
    /**
     * @brief Default constructor creates an invalid item
     */
    ClipboardItem();
    
    /**
     * @brief Creates a clipboard item from text content
     * @param text The clipboard text content
     * @param timestamp When the item was copied (default: current time)
     */
    explicit ClipboardItem(const QString& text, const QDateTime& timestamp = QDateTime::currentDateTime());
    
    /**
     * @brief Creates a clipboard item from JSON data
     * @param json JSON object containing item data
     */
    explicit ClipboardItem(const QJsonObject& json);
    
    /**
     * @brief Copy constructor
     */
    ClipboardItem(const ClipboardItem& other);
    
    /**
     * @brief Assignment operator
     */
    ClipboardItem& operator=(const ClipboardItem& other);
    
    /**
     * @brief Destructor
     */
    ~ClipboardItem() = default;
    
    // Accessors
    QString id() const { return m_id; }
    QString text() const { return m_text; }
    QString preview() const { return m_preview; }
    QDateTime timestamp() const { return m_timestamp; }
    bool pinned() const { return m_pinned; }
    QString hash() const { return m_hash; }
    
    /**
     * @brief Check if this item is valid
     * @return true if item has valid content and metadata
     */
    bool isValid() const;
    
    /**
     * @brief Pin this item to the top of the history
     */
    void pin();
    
    /**
     * @brief Unpin this item from the top of the history
     */
    void unpin();
    
    /**
     * @brief Toggle the pinned state
     */
    void togglePin();
    
    /**
     * @brief Serialize to JSON object
     * @return JSON representation of the item
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Load data from JSON object
     * @param json JSON object to load from
     * @return true if loading was successful
     */
    bool fromJson(const QJsonObject& json);
    
    /**
     * @brief Compare items for equality (based on content hash)
     * @param other Item to compare with
     * @return true if items have the same content
     */
    bool operator==(const ClipboardItem& other) const;
    
    /**
     * @brief Compare items for inequality
     * @param other Item to compare with
     * @return true if items have different content
     */
    bool operator!=(const ClipboardItem& other) const;
    
    /**
     * @brief Compare items for ordering (by timestamp)
     * @param other Item to compare with
     * @return true if this item is newer than other
     */
    bool operator<(const ClipboardItem& other) const;
    
    /**
     * @brief Generate preview text from full content
     * @param text Full text content
     * @param maxLength Maximum preview length (default: 100)
     * @return Truncated preview with ellipsis if needed
     */
    static QString generatePreview(const QString& text, int maxLength = 100);
    
    /**
     * @brief Generate content hash for duplicate detection
     * @param text Text content to hash
     * @return SHA256 hash of the content
     */
    static QString generateHash(const QString& text);
    
    /**
     * @brief Validate text content
     * @param text Text to validate
     * @return true if text is valid (not empty or whitespace-only)
     */
    static bool validateText(const QString& text);

private:
    QString m_id;           ///< Unique identifier (UUID)
    QString m_text;         ///< Full clipboard text content
    QString m_preview;      ///< Truncated display text
    QDateTime m_timestamp;  ///< When the item was copied
    bool m_pinned;          ///< Whether item is pinned
    QString m_hash;         ///< Content hash for duplicate detection
    
    /**
     * @brief Initialize derived fields from text content
     */
    void initializeDerivedFields();
};

#endif // CLIPBOARD_ITEM_H
