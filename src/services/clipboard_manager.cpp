#include "clipboard_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QMimeData>
#include <QElapsedTimer>

ClipboardManager::ClipboardManager(QObject* parent)
    : QObject(parent)
    , m_clipboard(QApplication::clipboard())
    , m_monitoring(false)
    , m_saveTimer(new QTimer(this))
    , m_lastProcessTime(0)
{
    // Initialize configuration
    initializeConfiguration();
    
    // Connect internal signals
    connect(&m_history, &ClipboardHistory::itemAdded,
            this, &ClipboardManager::itemAdded);
    connect(&m_history, &ClipboardHistory::itemRemoved,
            this, &ClipboardManager::itemRemoved);
    connect(&m_history, &ClipboardHistory::itemPinned,
            this, &ClipboardManager::itemPinned);
    connect(&m_history, &ClipboardHistory::itemUnpinned,
            this, &ClipboardManager::itemUnpinned);
    connect(&m_history, &ClipboardHistory::orderChanged,
            this, &ClipboardManager::historyChanged);
    connect(&m_config, &Configuration::maxHistoryItemsChanged,
            this, &ClipboardManager::onConfigurationChanged);
    
    // Setup save timer for deferred persistence
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(1000); // 1 second delay for batching
    connect(m_saveTimer, &QTimer::timeout,
            this, &ClipboardManager::saveHistory);
    
    // Load existing history
    loadHistory();
    
    // Apply configuration to history
    m_history.setMaxItems(m_config.maxHistoryItems());
}

ClipboardManager::~ClipboardManager()
{
    stopMonitoring();
    saveHistory();
}

QList<ClipboardItem> ClipboardManager::getHistory() const
{
    return m_history.items();
}

ClipboardItem ClipboardManager::getItem(const QString& id) const
{
    return m_history.getItem(id);
}

bool ClipboardManager::pinItem(const QString& id)
{
    ClipboardItem item = m_history.getItem(id);
    if (!item.isValid()) {
        return false;
    }
    
    if (m_history.pinItem(id)) {
        emit itemPinned(id);
        scheduleSave();
        return true;
    }
    
    return false;
}

bool ClipboardManager::unpinItem(const QString& id)
{
    ClipboardItem item = m_history.getItem(id);
    if (!item.isValid()) {
        return false;
    }
    
    if (m_history.unpinItem(id)) {
        emit itemUnpinned(id);
        scheduleSave();
        return true;
    }
    
    return false;
}

bool ClipboardManager::removeItem(const QString& id)
{
    ClipboardItem item = m_history.getItem(id);
    if (!item.isValid()) {
        return false;
    }
    
    if (m_history.removeItem(id)) {
        emit itemRemoved(id);
        scheduleSave();
        return true;
    }
    
    return false;
}

int ClipboardManager::maxHistoryItems() const
{
    return m_config.maxHistoryItems();
}

void ClipboardManager::setMaxHistoryItems(int max)
{
    // Validate range as per contract tests
    if (max < 10 || max > 100) {
        emit error(QString("Invalid max history items: %1. Must be 10-100.").arg(max));
        return;
    }
    
    m_config.setMaxHistoryItems(max);
    m_history.setMaxItems(max);
    scheduleSave();
}

bool ClipboardManager::loadHistory()
{
    try {
        QString historyPath = m_config.configDirectory() + "/clipboard-history.json";
        if (m_history.loadFromFile(historyPath)) {
            emit historyChanged();
            return true;
        }
    } catch (const std::exception& e) {
        emit error(QString("Failed to load history: %1").arg(e.what()));
    }
    
    return false;
}

bool ClipboardManager::saveHistory()
{
    try {
        QString historyPath = m_config.configDirectory() + "/clipboard-history.json";
        if (m_history.saveToFile(historyPath)) {
            return true;
        }
    } catch (const std::exception& e) {
        emit error(QString("Failed to save history: %1").arg(e.what()));
    }
    
    return false;
}

void ClipboardManager::startMonitoring()
{
    if (m_monitoring || !m_clipboard) {
        return;
    }
    
    connect(m_clipboard, &QClipboard::dataChanged,
            this, &ClipboardManager::onClipboardChanged);
    
    m_monitoring = true;
}

void ClipboardManager::stopMonitoring()
{
    if (!m_monitoring || !m_clipboard) {
        return;
    }
    
    disconnect(m_clipboard, &QClipboard::dataChanged,
               this, &ClipboardManager::onClipboardChanged);
    
    m_monitoring = false;
}

bool ClipboardManager::isMonitoring() const
{
    return m_monitoring;
}

void ClipboardManager::onClipboardChanged()
{
    if (!m_clipboard) {
        return;
    }
    
    QElapsedTimer timer;
    timer.start();
    
    const QMimeData* mimeData = m_clipboard->mimeData();
    if (!mimeData || !mimeData->hasText()) {
        return;
    }
    
    QString content = mimeData->text();
    
    if (processClipboardContent(content)) {
        // Check performance contract: <50ms processing time
        qint64 elapsed = timer.elapsed();
        if (elapsed > 50) {
            qWarning() << "ClipboardManager: Processing time" << elapsed 
                      << "ms exceeds 50ms performance contract";
        }
        
        m_lastProcessTime = elapsed;
    }
}

void ClipboardManager::onConfigurationChanged()
{
    // Update history size if configuration changed
    m_history.setMaxItems(m_config.maxHistoryItems());
    scheduleSave();
}

bool ClipboardManager::processClipboardContent(const QString& content)
{
    if (!shouldAddContent(content)) {
        return false;
    }
    
    ClipboardItem item(content);
    if (!item.isValid()) {
        return false;
    }
    
    // Check for duplicates using hasDuplicate method
    if (m_history.hasDuplicate(content)) {
        // Move existing item to top by removing and re-adding
        // Find the item by hash (this is inefficient but works for now)
        auto items = m_history.items();
        for (const auto& existingItem : items) {
            if (existingItem.hash() == item.hash()) {
                m_history.removeItem(existingItem.id());
                m_history.addItem(existingItem);
                return true;
            }
        }
    }
    
    // Add new item - addItem returns the ID as QString
    QString id = m_history.addItem(item);
    if (!id.isEmpty()) {
        return true;
    }
    
    return false;
}

void ClipboardManager::initializeConfiguration()
{
    // Configuration will use its default path
    // Try to load existing configuration or create with defaults
    if (!m_config.load()) {
        // Set defaults
        m_config.setMaxHistoryItems(50);
        m_config.setHotkey("Meta+V");
        m_config.save();
    }
}

void ClipboardManager::scheduleSave()
{
    // Defer save operations to batch rapid changes
    m_saveTimer->start();
}

bool ClipboardManager::shouldAddContent(const QString& content) const
{
    // Filter out empty or very short content
    if (content.trimmed().isEmpty() || content.length() < 2) {
        return false;
    }
    
    // Filter out very long content that might be binary data
    if (content.length() > 10000) {
        return false;
    }
    
    // Filter out content that looks like passwords (all asterisks, etc.)
    if (content.trimmed() == QString("*").repeated(content.trimmed().length())) {
        return false;
    }
    
    return true;
}
