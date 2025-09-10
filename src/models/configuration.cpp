#include "configuration.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QDebug>
#include <QRegularExpression>
#include <QKeySequence>

Configuration::Configuration(QObject* parent)
    : QObject(parent)
    , m_configPath(defaultConfigPath())
{
    applyDefaults();
}

Configuration::Configuration(const QString& configPath, QObject* parent)
    : QObject(parent)
    , m_configPath(configPath)
{
    applyDefaults();
}

void Configuration::setMaxHistoryItems(int maxItems)
{
    int validatedMax = qBound(MIN_MAX_HISTORY_ITEMS, maxItems, MAX_MAX_HISTORY_ITEMS);
    if (validatedMax != m_maxHistoryItems) {
        m_maxHistoryItems = validatedMax;
        emit maxHistoryItemsChanged(m_maxHistoryItems);
    }
}

void Configuration::setHotkey(const QString& hotkey)
{
    if (isValidHotkey(hotkey) && hotkey != m_hotkey) {
        m_hotkey = hotkey;
        emit hotkeyChanged(m_hotkey);
    }
}

void Configuration::setAutostart(bool autostart)
{
    if (autostart != m_autostart) {
        m_autostart = autostart;
        emit autostartChanged(m_autostart);
    }
}

void Configuration::setShowNotifications(bool show)
{
    if (show != m_showNotifications) {
        m_showNotifications = show;
        emit showNotificationsChanged(m_showNotifications);
    }
}

void Configuration::setWindowPosition(const QPoint& position)
{
    if (position != m_windowPosition) {
        m_windowPosition = position;
        emit windowPositionChanged(m_windowPosition);
    }
}

void Configuration::setWindowSize(const QSize& size)
{
    if (size != m_windowSize && size.width() > 0 && size.height() > 0) {
        m_windowSize = size;
        emit windowSizeChanged(m_windowSize);
    }
}

QString Configuration::configDirectory() const
{
    return QFileInfo(m_configPath).absolutePath();
}

QString Configuration::defaultConfigPath()
{
    return defaultConfigDirectory() + "/config.json";
}

QString Configuration::defaultConfigDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/clipboard-manager";
}

bool Configuration::load()
{
    QFile file(m_configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Configuration file does not exist, using defaults:" << m_configPath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse configuration file:" << error.errorString();
        return false;
    }

    QJsonObject json = doc.object();
    
    // Check if migration is needed
    if (json.contains("version") && json["version"].toString() != currentVersion()) {
        json = migrateConfiguration(json);
    }

    bool success = fromJson(json);
    if (success) {
        emit configurationLoaded();
    }
    return success;
}

bool Configuration::save()
{
    if (!ensureConfigDirectory()) {
        qWarning() << "Failed to create configuration directory";
        return false;
    }

    QJsonDocument doc(toJson());
    
    // Use QSaveFile for atomic write operations
    QSaveFile file(m_configPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open configuration file for writing:" << m_configPath;
        return false;
    }

    qint64 bytesWritten = file.write(doc.toJson());
    if (bytesWritten == -1 || !file.commit()) {
        qWarning() << "Failed to write configuration file:" << file.errorString();
        return false;
    }

    emit configurationSaved();
    return true;
}

void Configuration::resetToDefaults()
{
    applyDefaults();
    
    // Emit all change signals
    emit maxHistoryItemsChanged(m_maxHistoryItems);
    emit hotkeyChanged(m_hotkey);
    emit autostartChanged(m_autostart);
    emit showNotificationsChanged(m_showNotifications);
    emit windowPositionChanged(m_windowPosition);
    emit windowSizeChanged(m_windowSize);
}

bool Configuration::exists() const
{
    return QFile::exists(m_configPath);
}

QJsonObject Configuration::toJson() const
{
    QJsonObject json;
    
    json["version"] = m_version;
    json["maxHistoryItems"] = m_maxHistoryItems;
    json["hotkey"] = m_hotkey;
    json["autostart"] = m_autostart;
    json["showNotifications"] = m_showNotifications;
    
    QJsonObject windowPos;
    windowPos["x"] = m_windowPosition.x();
    windowPos["y"] = m_windowPosition.y();
    json["windowPosition"] = windowPos;
    
    QJsonObject windowSize;
    windowSize["width"] = m_windowSize.width();
    windowSize["height"] = m_windowSize.height();
    json["windowSize"] = windowSize;
    
    return json;
}

bool Configuration::fromJson(const QJsonObject& json)
{
    // Load version
    m_version = json.value("version").toString(currentVersion());
    
    // Load and validate maxHistoryItems
    int maxItems = json.value("maxHistoryItems").toInt(DEFAULT_MAX_HISTORY_ITEMS);
    m_maxHistoryItems = qBound(MIN_MAX_HISTORY_ITEMS, maxItems, MAX_MAX_HISTORY_ITEMS);
    
    // Load and validate hotkey
    QString hotkey = json.value("hotkey").toString(DEFAULT_HOTKEY);
    m_hotkey = isValidHotkey(hotkey) ? hotkey : DEFAULT_HOTKEY;
    
    // Load boolean settings
    m_autostart = json.value("autostart").toBool(DEFAULT_AUTOSTART);
    m_showNotifications = json.value("showNotifications").toBool(DEFAULT_SHOW_NOTIFICATIONS);
    
    // Load window position
    if (json.contains("windowPosition") && json["windowPosition"].isObject()) {
        QJsonObject pos = json["windowPosition"].toObject();
        int x = pos.value("x").toInt(DEFAULT_WINDOW_X);
        int y = pos.value("y").toInt(DEFAULT_WINDOW_Y);
        m_windowPosition = QPoint(x, y);
    } else {
        m_windowPosition = QPoint(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y);
    }
    
    // Load window size
    if (json.contains("windowSize") && json["windowSize"].isObject()) {
        QJsonObject size = json["windowSize"].toObject();
        int width = size.value("width").toInt(DEFAULT_WINDOW_WIDTH);
        int height = size.value("height").toInt(DEFAULT_WINDOW_HEIGHT);
        // Ensure minimum window size
        width = qMax(width, 200);
        height = qMax(height, 300);
        m_windowSize = QSize(width, height);
    } else {
        m_windowSize = QSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    }
    
    validateSettings();
    return true;
}

bool Configuration::isValidHotkey(const QString& hotkey)
{
    if (hotkey.isEmpty()) {
        return false;
    }
    
    // Try to parse as QKeySequence to validate format
    QKeySequence sequence(hotkey);
    if (sequence.isEmpty()) {
        return false;
    }
    
    // Basic validation for common modifiers
    QStringList validModifiers = {"Ctrl", "Alt", "Shift", "Meta", "Super", "Cmd"};
    QStringList parts = hotkey.split("+");
    
    if (parts.size() < 2) {
        return false; // Should have at least modifier + key
    }
    
    // Check if at least one modifier is present
    bool hasValidModifier = false;
    for (const QString& part : parts) {
        for (const QString& modifier : validModifiers) {
            if (part.trimmed().compare(modifier, Qt::CaseInsensitive) == 0) {
                hasValidModifier = true;
                break;
            }
        }
        if (hasValidModifier) break;
    }
    
    return hasValidModifier;
}

bool Configuration::isValidMaxHistoryItems(int maxItems)
{
    return maxItems >= MIN_MAX_HISTORY_ITEMS && maxItems <= MAX_MAX_HISTORY_ITEMS;
}

bool Configuration::ensureConfigDirectory()
{
    QDir dir;
    QString dirPath = QFileInfo(m_configPath).absolutePath();
    return dir.mkpath(dirPath);
}

QJsonObject Configuration::migrateConfiguration(const QJsonObject& json)
{
    QString version = json.value("version").toString();
    QJsonObject migrated = json;
    
    // Currently we only have version 1.0.0, but this is where
    // future migration logic would go
    if (version != currentVersion()) {
        qDebug() << "Migrating configuration from version" << version << "to" << currentVersion();
        migrated["version"] = currentVersion();
    }
    
    return migrated;
}

void Configuration::applyDefaults()
{
    m_version = currentVersion();
    m_maxHistoryItems = DEFAULT_MAX_HISTORY_ITEMS;
    m_hotkey = DEFAULT_HOTKEY;
    m_autostart = DEFAULT_AUTOSTART;
    m_showNotifications = DEFAULT_SHOW_NOTIFICATIONS;
    m_windowPosition = QPoint(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y);
    m_windowSize = QSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
}

void Configuration::validateSettings()
{
    // Ensure all settings are within valid ranges
    m_maxHistoryItems = qBound(MIN_MAX_HISTORY_ITEMS, m_maxHistoryItems, MAX_MAX_HISTORY_ITEMS);
    
    if (!isValidHotkey(m_hotkey)) {
        m_hotkey = DEFAULT_HOTKEY;
    }
    
    if (m_windowSize.width() <= 0 || m_windowSize.height() <= 0) {
        m_windowSize = QSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    }
    
    // Ensure minimum window size
    if (m_windowSize.width() < 200) {
        m_windowSize.setWidth(200);
    }
    if (m_windowSize.height() < 300) {
        m_windowSize.setHeight(300);
    }
}
