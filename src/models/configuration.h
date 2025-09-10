#pragma once

#include <QObject>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

/**
 * @brief Manages user preferences and application settings
 * 
 * Configuration handles storage and retrieval of user preferences with:
 * - Validation of all settings values
 * - Automatic migration between configuration versions
 * - Atomic write operations to prevent corruption
 * - Default values for all settings
 */
class Configuration : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor with standard settings
     */
    explicit Configuration(QObject* parent = nullptr);
    
    /**
     * @brief Constructor loading from custom config file path
     * @param configPath Path to configuration file
     * @param parent Parent QObject
     */
    explicit Configuration(const QString& configPath, QObject* parent = nullptr);

    // Version information
    static QString currentVersion() { return "1.0.0"; }
    QString version() const { return m_version; }
    
    // History settings
    int maxHistoryItems() const { return m_maxHistoryItems; }
    void setMaxHistoryItems(int maxItems);
    
    // Hotkey settings
    QString hotkey() const { return m_hotkey; }
    void setHotkey(const QString& hotkey);
    
    // System integration settings
    bool autostart() const { return m_autostart; }
    void setAutostart(bool autostart);
    
    bool showNotifications() const { return m_showNotifications; }
    void setShowNotifications(bool show);
    
    // Window settings
    QPoint windowPosition() const { return m_windowPosition; }
    void setWindowPosition(const QPoint& position);
    
    QSize windowSize() const { return m_windowSize; }
    void setWindowSize(const QSize& size);
    
    // File paths
    QString configPath() const { return m_configPath; }
    QString configDirectory() const;
    static QString defaultConfigPath();
    static QString defaultConfigDirectory();

    // Persistence
    /**
     * @brief Load configuration from file
     * @return true if loading was successful
     */
    bool load();
    
    /**
     * @brief Save configuration to file
     * @return true if saving was successful
     */
    bool save();
    
    /**
     * @brief Reset all settings to defaults
     */
    void resetToDefaults();
    
    /**
     * @brief Check if configuration file exists
     * @return true if config file exists
     */
    bool exists() const;

    // Serialization
    /**
     * @brief Serialize to JSON object
     * @return JSON representation of the configuration
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Load data from JSON object
     * @param json JSON object to load from
     * @return true if loading was successful
     */
    bool fromJson(const QJsonObject& json);

    // Validation
    /**
     * @brief Validate hotkey string format
     * @param hotkey Hotkey string to validate
     * @return true if hotkey is valid
     */
    static bool isValidHotkey(const QString& hotkey);
    
    /**
     * @brief Validate max history items value
     * @param maxItems Value to validate
     * @return true if value is valid
     */
    static bool isValidMaxHistoryItems(int maxItems);

signals:
    /**
     * @brief Emitted when max history items setting changes
     * @param maxItems New maximum items value
     */
    void maxHistoryItemsChanged(int maxItems);
    
    /**
     * @brief Emitted when hotkey setting changes
     * @param hotkey New hotkey string
     */
    void hotkeyChanged(const QString& hotkey);
    
    /**
     * @brief Emitted when autostart setting changes
     * @param autostart New autostart value
     */
    void autostartChanged(bool autostart);
    
    /**
     * @brief Emitted when show notifications setting changes
     * @param show New show notifications value
     */
    void showNotificationsChanged(bool show);
    
    /**
     * @brief Emitted when window position changes
     * @param position New window position
     */
    void windowPositionChanged(const QPoint& position);
    
    /**
     * @brief Emitted when window size changes
     * @param size New window size
     */
    void windowSizeChanged(const QSize& size);
    
    /**
     * @brief Emitted when configuration is loaded
     */
    void configurationLoaded();
    
    /**
     * @brief Emitted when configuration is saved
     */
    void configurationSaved();

private:
    /**
     * @brief Ensure configuration directory exists
     * @return true if directory exists or was created
     */
    bool ensureConfigDirectory();
    
    /**
     * @brief Migrate configuration from older version
     * @param json JSON object to migrate
     * @return Migrated JSON object
     */
    QJsonObject migrateConfiguration(const QJsonObject& json);
    
    /**
     * @brief Apply default values for all settings
     */
    void applyDefaults();
    
    /**
     * @brief Validate and sanitize all current settings
     */
    void validateSettings();

    // Default values
    static constexpr int DEFAULT_MAX_HISTORY_ITEMS = 50;
    static constexpr int MIN_MAX_HISTORY_ITEMS = 10;
    static constexpr int MAX_MAX_HISTORY_ITEMS = 100;
    static constexpr const char* DEFAULT_HOTKEY = "Meta+V";
    static constexpr bool DEFAULT_AUTOSTART = false;
    static constexpr bool DEFAULT_SHOW_NOTIFICATIONS = true;
    static constexpr int DEFAULT_WINDOW_WIDTH = 400;
    static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    static constexpr int DEFAULT_WINDOW_X = 100;
    static constexpr int DEFAULT_WINDOW_Y = 100;

    // Configuration values
    QString m_version;                 ///< Configuration format version
    int m_maxHistoryItems;             ///< Maximum items in history
    QString m_hotkey;                  ///< Global hotkey combination
    bool m_autostart;                  ///< Start with system
    bool m_showNotifications;          ///< Show tray notifications
    QPoint m_windowPosition;           ///< Last popup window position
    QSize m_windowSize;                ///< Last popup window size

    // File system
    QString m_configPath;              ///< Path to configuration file
};
