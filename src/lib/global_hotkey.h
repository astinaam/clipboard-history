#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QElapsedTimer>

/**
 * GlobalHotkey - Cross-platform global hotkey registration for Linux
 * 
 * This library provides global hotkey registration and management for Linux
 * systems, supporting both X11 and Wayland environments. It handles hotkey
 * string parsing, platform detection, and signal emission when hotkeys are
 * triggered.
 * 
 * Supported platforms:
 * - X11: Uses XGrabKey with Qt integration
 * - Wayland: Uses compositor-specific protocols (future enhancement)
 * - Fallback: Basic QShortcut implementation
 * 
 * Performance targets:
 * - Registration: <100ms
 * - Trigger latency: <50ms
 * - Memory usage: <1KB per instance
 */
class GlobalHotkey : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor - Creates global hotkey manager
     * @param parent QObject parent for memory management
     */
    explicit GlobalHotkey(QObject* parent = nullptr);
    
    /**
     * Destructor - Automatically unregisters all hotkeys
     */
    ~GlobalHotkey();

    // Hotkey Registration Methods
    /**
     * Register a global hotkey from string specification
     * @param hotkeyString String like "Meta+V", "Ctrl+Alt+V"
     * @return true if registration successful, false otherwise
     */
    bool registerHotkey(const QString& hotkeyString);
    
    /**
     * Register the default hotkey (Meta+V)
     * @return true if registration successful, false otherwise
     */
    bool registerDefaultHotkey();
    
    /**
     * Unregister currently active hotkey
     * @return true if something was unregistered, false if nothing was registered
     */
    bool unregisterHotkey();

    // State Query Methods
    /**
     * Check if a hotkey is currently registered
     * @return true if hotkey is active, false otherwise
     */
    bool isRegistered() const;
    
    /**
     * Get the current hotkey string
     * @return String representation of registered hotkey, empty if none
     */
    QString getHotkeyString() const;

    // Error Handling Methods
    /**
     * Get the last error message
     * @return Error description, empty if no error
     */
    QString lastError() const;
    
    /**
     * Clear the last error message
     */
    void clearError();

    // Platform Detection Methods
    /**
     * Get the current platform name
     * @return "X11", "Wayland", or "Unknown"
     */
    static QString getCurrentPlatform();
    
    /**
     * Check if X11 is available
     * @return true if running under X11
     */
    static bool isX11Available();
    
    /**
     * Check if Wayland is available
     * @return true if running under Wayland
     */
    static bool isWaylandAvailable();

    // Test Mode Methods (for automated testing)
    /**
     * Enable or disable test mode
     * In test mode, hotkeys are simulated instead of actually registered
     * @param enabled true to enable test mode, false to disable
     */
    static void setTestMode(bool enabled);
    
    /**
     * Check if test mode is enabled
     * @return true if in test mode, false if in normal mode
     */
    static bool isTestMode();
    
    /**
     * Simulate hotkey trigger (test mode only)
     * Emits hotkeyTriggered signal if a hotkey is registered
     */
    void simulateHotkeyTrigger();

signals:
    /**
     * Emitted when the registered hotkey is triggered
     */
    void hotkeyTriggered();

private slots:
    /**
     * Handle platform-specific hotkey events
     */
    void handleHotkeyEvent();

private:
    // Internal state
    bool m_registered;
    QString m_hotkeyString;
    QString m_lastError;
    
    // Platform-specific data
    struct PlatformData;
    PlatformData* m_platformData;
    
    // Static test mode control
    static bool s_testMode;
    static QStringList s_testModeRegisteredHotkeys;
    
    // Internal methods
    bool parseHotkeyString(const QString& hotkeyString, int& modifiers, int& key);
    bool registerX11Hotkey(int modifiers, int key);
    bool registerWaylandHotkey(int modifiers, int key);
    bool registerFallbackHotkey(int modifiers, int key);
    void unregisterPlatformHotkey();
    void setError(const QString& error);
    
    // Helper methods for parsing
    static int parseModifiers(const QStringList& modifierStrings);
    static int parseKey(const QString& keyString);
    static bool isValidHotkeyString(const QString& hotkeyString);
};
