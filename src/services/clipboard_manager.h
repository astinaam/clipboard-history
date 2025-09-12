#pragma once

#include <QObject>
#include <QClipboard>
#include <QTimer>
#include <QPointer>
#include <QApplication>
#include <QList>

#include "../models/clipboard_item.h"
#include "../models/clipboard_history.h"
#include "../models/configuration.h"

/**
 * ClipboardManager - Core service for monitoring clipboard and managing history
 * 
 * This service integrates Qt clipboard monitoring with the ClipboardHistory model,
 * providing persistence, configuration management, and signal-based notifications.
 * 
 * Design principles:
 * - Signal-driven architecture for loose coupling
 * - Configurable monitoring with start/stop controls
 * - Automatic persistence with atomic file operations
 * - Performance contracts: <50ms processing, <10MB memory
 */
class ClipboardManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor - Creates manager with default configuration
     * @param parent QObject parent for memory management
     */
    explicit ClipboardManager(QObject* parent = nullptr);
    
    /**
     * Destructor - Ensures monitoring stops and history is saved
     */
    ~ClipboardManager();

    // History Access Methods
    /**
     * Get current clipboard history items
     * @return List of clipboard items in display order (pinned first)
     */
    QList<ClipboardItem> getHistory() const;
    
    /**
     * Retrieve specific item by ID
     * @param id UUID string identifier
     * @return ClipboardItem with matching ID, or invalid item if not found
     */
    ClipboardItem getItem(const QString& id) const;

    // History Modification Methods
    /**
     * Pin an item to prevent automatic removal
     * @param id UUID string identifier
     * @return true if item was found and pinned successfully
     */
    bool pinItem(const QString& id);
    
    /**
     * Unpin an item to allow automatic removal
     * @param id UUID string identifier  
     * @return true if item was found and unpinned successfully
     */
    bool unpinItem(const QString& id);
    
    /**
     * Remove an item from history
     * @param id UUID string identifier
     * @return true if item was found and removed successfully
     */
    bool removeItem(const QString& id);

    // Configuration Methods
    /**
     * Get maximum number of history items
     * @return Current max history size (10-100)
     */
    int maxHistoryItems() const;
    
    /**
     * Set maximum number of history items
     * @param max New maximum size (must be 10-100)
     */
    void setMaxHistoryItems(int max);

    // Persistence Methods
    /**
     * Load history from persistent storage
     * @return true if history was loaded successfully
     */
    bool loadHistory();
    
    /**
     * Save current history to persistent storage
     * @return true if history was saved successfully
     */
    bool saveHistory();

    // Monitoring Control Methods
    /**
     * Start clipboard monitoring
     * Connects to QClipboard::dataChanged signal
     */
    void startMonitoring();
    
    /**
     * Stop clipboard monitoring
     * Disconnects from QClipboard signals
     */
    void stopMonitoring();
    
    /**
     * Check if monitoring is currently active
     * @return true if monitoring clipboard changes
     */
    bool isMonitoring() const;

signals:
    /**
     * Emitted when history contents change (add/remove/reorder)
     */
    void historyChanged();
    
    /**
     * Emitted when new item is added to history
     * @param item The newly added clipboard item
     */
    void itemAdded(const ClipboardItem& item);
    
    /**
     * Emitted when item is pinned
     * @param id UUID of pinned item
     */
    void itemPinned(const QString& id);
    
    /**
     * Emitted when item is unpinned
     * @param id UUID of unpinned item
     */
    void itemUnpinned(const QString& id);
    
    /**
     * Emitted when item is removed from history
     * @param id UUID of removed item
     */
    void itemRemoved(const QString& id);
    
    /**
     * Emitted when monitoring state changes
     * @param monitoring true if monitoring started, false if stopped
     */
    void monitoringStateChanged(bool monitoring);
    
    /**
     * Emitted when error occurs (invalid config, file I/O failure, etc.)
     * @param message Human-readable error description
     */
    void error(const QString& message);

private slots:
    /**
     * Handle clipboard data change events
     * Called when QClipboard::dataChanged is emitted
     */
    void onClipboardChanged();
    
    /**
     * Handle configuration change notifications
     * Updates history size limits and persistence settings
     */
    void onConfigurationChanged();

private:
    // Core components
    QPointer<QClipboard> m_clipboard;        ///< Qt clipboard interface
    ClipboardHistory m_history;              ///< History model instance
    Configuration m_config;                  ///< Configuration model instance
    
    // Monitoring state
    bool m_monitoring;                       ///< Current monitoring status
    QTimer* m_saveTimer;                     ///< Deferred save operations
    
    // Performance tracking
    qint64 m_lastProcessTime;                ///< Last clipboard processing timestamp
    
    // Helper methods
    /**
     * Process new clipboard content
     * @param content Text content from clipboard
     * @return true if content was added to history
     */
    bool processClipboardContent(const QString& content);
    
    /**
     * Initialize default configuration
     * Sets up config file path and default values
     */
    void initializeConfiguration();
    
    /**
     * Schedule deferred save operation
     * Uses timer to batch multiple rapid changes
     */
    void scheduleSave();
    
    /**
     * Validate content before adding to history
     * @param content Text content to validate
     * @return true if content should be added to history
     */
    bool shouldAddContent(const QString& content) const;
};
