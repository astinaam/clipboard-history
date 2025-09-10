#include "clipboard_item.h"
#include <QRegularExpression>
#include <QJsonDocument>

ClipboardItem::ClipboardItem()
    : m_pinned(false)
{
    // Invalid item by default - no content
}

ClipboardItem::ClipboardItem(const QString& text, const QDateTime& timestamp)
    : m_text(text)
    , m_timestamp(timestamp.isValid() ? timestamp : QDateTime::currentDateTime())
    , m_pinned(false)
{
    if (validateText(text)) {
        m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        initializeDerivedFields();
    }
}

ClipboardItem::ClipboardItem(const QJsonObject& json)
    : m_pinned(false)
{
    fromJson(json);
}

ClipboardItem::ClipboardItem(const ClipboardItem& other)
    : m_id(other.m_id)
    , m_text(other.m_text)
    , m_preview(other.m_preview)
    , m_timestamp(other.m_timestamp)
    , m_pinned(other.m_pinned)
    , m_hash(other.m_hash)
{
}

ClipboardItem& ClipboardItem::operator=(const ClipboardItem& other)
{
    if (this != &other) {
        m_id = other.m_id;
        m_text = other.m_text;
        m_preview = other.m_preview;
        m_timestamp = other.m_timestamp;
        m_pinned = other.m_pinned;
        m_hash = other.m_hash;
    }
    return *this;
}

bool ClipboardItem::isValid() const
{
    return !m_id.isEmpty() && 
           validateText(m_text) && 
           m_timestamp.isValid() &&
           !m_hash.isEmpty();
}

void ClipboardItem::pin()
{
    m_pinned = true;
}

void ClipboardItem::unpin()
{
    m_pinned = false;
}

void ClipboardItem::togglePin()
{
    m_pinned = !m_pinned;
}

QJsonObject ClipboardItem::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["text"] = m_text;
    json["preview"] = m_preview;
    json["timestamp"] = m_timestamp.toString(Qt::ISODate);
    json["pinned"] = m_pinned;
    json["hash"] = m_hash;
    return json;
}

bool ClipboardItem::fromJson(const QJsonObject& json)
{
    // Reset to invalid state
    m_id.clear();
    m_text.clear();
    m_preview.clear();
    m_timestamp = QDateTime();
    m_pinned = false;
    m_hash.clear();
    
    // Validate required fields
    if (!json.contains("text") || !json.contains("timestamp")) {
        return false;
    }
    
    QString text = json["text"].toString();
    if (!validateText(text)) {
        return false;
    }
    
    QDateTime timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    if (!timestamp.isValid()) {
        return false;
    }
    
    // Load all fields
    m_id = json.value("id").toString();
    m_text = text;
    m_timestamp = timestamp;
    m_pinned = json.value("pinned").toBool();
    
    // Generate missing derived fields
    if (m_id.isEmpty()) {
        m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    
    // Load or generate preview
    if (json.contains("preview")) {
        m_preview = json["preview"].toString();
    } else {
        m_preview = generatePreview(m_text);
    }
    
    // Load or generate hash
    if (json.contains("hash")) {
        m_hash = json["hash"].toString();
    } else {
        m_hash = generateHash(m_text);
    }
    
    return isValid();
}

bool ClipboardItem::operator==(const ClipboardItem& other) const
{
    // Items are equal if they have the same content hash
    return m_hash == other.m_hash && !m_hash.isEmpty();
}

bool ClipboardItem::operator!=(const ClipboardItem& other) const
{
    return !(*this == other);
}

bool ClipboardItem::operator<(const ClipboardItem& other) const
{
    // For sorting: newer items are "less" (come first)
    return m_timestamp > other.m_timestamp;
}

QString ClipboardItem::generatePreview(const QString& text, int maxLength)
{
    if (text.isEmpty()) {
        return QString();
    }
    
    // Remove excessive whitespace and newlines for preview
    QString cleaned = text.trimmed();
    cleaned.replace(QRegularExpression("\\s+"), " ");
    
    if (cleaned.length() <= maxLength) {
        return cleaned;
    }
    
    // Truncate and add ellipsis
    return cleaned.left(maxLength - 3) + "...";
}

QString ClipboardItem::generateHash(const QString& text)
{
    if (text.isEmpty()) {
        return QString();
    }
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(text.toUtf8());
    return QString(hash.result().toHex());
}

bool ClipboardItem::validateText(const QString& text)
{
    if (text.isEmpty()) {
        return false;
    }
    
    // Check if text is only whitespace
    QString trimmed = text.trimmed();
    return !trimmed.isEmpty();
}

void ClipboardItem::initializeDerivedFields()
{
    m_preview = generatePreview(m_text);
    m_hash = generateHash(m_text);
}
