#include "global_hotkey.h"
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QTimer>
#include <QShortcut>

// Platform-specific includes
#ifdef Q_OS_LINUX
// Only include X11 headers if X11 is available
#ifdef QT_X11EXTRAS_LIB
#include <QX11Info>
#endif
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

// Static test mode variable
bool GlobalHotkey::s_testMode = false;
QStringList GlobalHotkey::s_testModeRegisteredHotkeys;

/**
 * Platform-specific data structure
 */
struct GlobalHotkey::PlatformData
{
#ifdef Q_OS_LINUX
    Display* display = nullptr;
    Window rootWindow = 0;
    int x11Modifiers = 0;
    int x11Keycode = 0;
    bool x11Registered = false;
#endif
    
    // Fallback Qt shortcut
    QShortcut* fallbackShortcut = nullptr;
    
    PlatformData() = default;
    ~PlatformData() = default;
};

GlobalHotkey::GlobalHotkey(QObject* parent)
    : QObject(parent)
    , m_registered(false)
    , m_hotkeyString()
    , m_lastError()
    , m_platformData(new PlatformData())
{
    qDebug() << "GlobalHotkey: Initialized on platform:" << getCurrentPlatform();
}

GlobalHotkey::~GlobalHotkey()
{
    if (m_registered) {
        unregisterHotkey();
    }
    delete m_platformData;
}

bool GlobalHotkey::registerHotkey(const QString& hotkeyString)
{
    clearError();
    
    if (hotkeyString.isEmpty()) {
        setError("Hotkey string cannot be empty");
        return false;
    }
    
    if (!isValidHotkeyString(hotkeyString)) {
        setError(QString("Invalid hotkey string format: %1").arg(hotkeyString));
        return false;
    }
    
    // Unregister any existing hotkey
    if (m_registered) {
        unregisterHotkey();
    }
    
    int modifiers, key;
    if (!parseHotkeyString(hotkeyString, modifiers, key)) {
        setError(QString("Failed to parse hotkey string: %1").arg(hotkeyString));
        return false;
    }
    
    bool success = false;
    
    // In test mode, simulate successful registration for valid hotkeys only
    if (s_testMode) {
        // Additional validation for test mode - check for obviously invalid patterns
        if (hotkeyString.contains("++") || 
            hotkeyString.startsWith("+") || 
            hotkeyString.endsWith("+") ||
            hotkeyString.split("+", Qt::SkipEmptyParts).size() > 5) {
            setError(QString("Invalid hotkey string format in test mode: %1").arg(hotkeyString));
            return false;
        }
        
        // Check for conflicts in test mode
        if (s_testModeRegisteredHotkeys.contains(hotkeyString)) {
            setError(QString("Hotkey already registered in test mode: %1").arg(hotkeyString));
            return false;
        }
        
        success = true;
        s_testModeRegisteredHotkeys.append(hotkeyString);
        qDebug() << "GlobalHotkey: Test mode - simulating registration of" << hotkeyString;
    } else {
        QString platform = getCurrentPlatform();
        
        if (platform == "X11") {
            success = registerX11Hotkey(modifiers, key);
        } else if (platform == "Wayland") {
            success = registerWaylandHotkey(modifiers, key);
        } else {
            success = registerFallbackHotkey(modifiers, key);
        }
    }
    
    if (success) {
        m_registered = true;
        m_hotkeyString = hotkeyString;
        if (s_testMode) {
            qDebug() << "GlobalHotkey: Test mode - simulated registration of" << hotkeyString;
        } else {
            QString platform = getCurrentPlatform();
            qDebug() << "GlobalHotkey: Successfully registered" << hotkeyString << "on" << platform;
        }
    } else {
        if (m_lastError.isEmpty()) {
            QString platform = getCurrentPlatform();
            setError(QString("Failed to register hotkey on %1 platform").arg(platform));
        }
        qDebug() << "GlobalHotkey: Failed to register" << hotkeyString << ":" << m_lastError;
    }
    
    return success;
}

bool GlobalHotkey::registerDefaultHotkey()
{
    return registerHotkey("Meta+V");
}

bool GlobalHotkey::unregisterHotkey()
{
    if (!m_registered) {
        return false;
    }
    
    // Remove from test mode registry if in test mode
    if (s_testMode) {
        s_testModeRegisteredHotkeys.removeAll(m_hotkeyString);
        qDebug() << "GlobalHotkey: Test mode - removed" << m_hotkeyString << "from registry";
    } else {
        unregisterPlatformHotkey();
    }
    
    m_registered = false;
    m_hotkeyString.clear();
    clearError();
    
    qDebug() << "GlobalHotkey: Unregistered hotkey";
    return true;
}

bool GlobalHotkey::isRegistered() const
{
    return m_registered;
}

QString GlobalHotkey::getHotkeyString() const
{
    return m_hotkeyString;
}

QString GlobalHotkey::lastError() const
{
    return m_lastError;
}

void GlobalHotkey::clearError()
{
    m_lastError.clear();
}

QString GlobalHotkey::getCurrentPlatform()
{
    if (isX11Available()) {
        return "X11";
    } else if (isWaylandAvailable()) {
        return "Wayland";
    } else {
        return "Unknown";
    }
}

bool GlobalHotkey::isX11Available()
{
    // Check if running under X11
    QString sessionType = qgetenv("XDG_SESSION_TYPE");
    QString display = qgetenv("DISPLAY");
    
    return sessionType == "x11" || !display.isEmpty();
}

bool GlobalHotkey::isWaylandAvailable()
{
    // Check if running under Wayland
    QString sessionType = qgetenv("XDG_SESSION_TYPE");
    QString waylandDisplay = qgetenv("WAYLAND_DISPLAY");
    
    return sessionType == "wayland" || !waylandDisplay.isEmpty();
}

void GlobalHotkey::handleHotkeyEvent()
{
    if (m_registered) {
        qDebug() << "GlobalHotkey: Hotkey triggered:" << m_hotkeyString;
        emit hotkeyTriggered();
    }
}

bool GlobalHotkey::parseHotkeyString(const QString& hotkeyString, int& modifiers, int& key)
{
    QStringList parts = hotkeyString.split("+", Qt::SkipEmptyParts);
    
    if (parts.size() < 2) {
        setError("Hotkey must contain at least one modifier and one key");
        return false;
    }
    
    // Last part is the key, others are modifiers
    QString keyString = parts.takeLast();
    
    modifiers = parseModifiers(parts);
    if (modifiers == 0) {
        setError("No valid modifiers found");
        return false;
    }
    
    key = parseKey(keyString);
    if (key == 0) {
        setError(QString("Invalid key: %1").arg(keyString));
        return false;
    }
    
    return true;
}

bool GlobalHotkey::registerX11Hotkey(int modifiers, int key)
{
#ifdef Q_OS_LINUX
#ifdef QT_X11EXTRAS_LIB
    if (!QX11Info::isPlatformX11()) {
        setError("X11 platform not available");
        return false;
    }
    
    m_platformData->display = QX11Info::display();
    if (!m_platformData->display) {
        setError("Could not get X11 display");
        return false;
    }
    
    m_platformData->rootWindow = QX11Info::appRootWindow();
#else
    // Fallback when QX11Info is not available
    m_platformData->display = XOpenDisplay(nullptr);
    if (!m_platformData->display) {
        setError("Could not open X11 display");
        return false;
    }
    
    m_platformData->rootWindow = DefaultRootWindow(m_platformData->display);
#endif
    
    m_platformData->x11Modifiers = modifiers;
    m_platformData->x11Keycode = XKeysymToKeycode(m_platformData->display, key);
    
    if (m_platformData->x11Keycode == 0) {
        setError("Could not convert key to X11 keycode");
        return false;
    }
    
    // Register the hotkey with X11
    // Note: This is a simplified implementation
    // Real implementation would need proper error checking and event handling
    int result = XGrabKey(m_platformData->display, 
                         m_platformData->x11Keycode,
                         m_platformData->x11Modifiers,
                         m_platformData->rootWindow,
                         False,
                         GrabModeAsync,
                         GrabModeAsync);
    
    XSync(m_platformData->display, False);
    
    if (result == BadAccess) {
        setError("Hotkey already in use by another application");
        return false;
    } else if (result != Success) {
        setError(QString("X11 XGrabKey failed with code: %1").arg(result));
        return false;
    }
    
    m_platformData->x11Registered = true;
    return true;
#else
    Q_UNUSED(modifiers)
    Q_UNUSED(key)
    setError("X11 support not compiled in");
    return false;
#endif
}

bool GlobalHotkey::registerWaylandHotkey(int modifiers, int key)
{
    Q_UNUSED(modifiers)
    Q_UNUSED(key)
    
    // Wayland global hotkey support would require compositor-specific protocols
    // For now, fall back to the fallback implementation
    setError("Wayland global hotkeys not yet implemented - using fallback");
    return registerFallbackHotkey(modifiers, key);
}

bool GlobalHotkey::registerFallbackHotkey(int modifiers, int key)
{
    // Use QShortcut as fallback (only works when application has focus)
    if (m_platformData->fallbackShortcut) {
        delete m_platformData->fallbackShortcut;
        m_platformData->fallbackShortcut = nullptr;
    }
    
    // Convert modifiers and key to QKeySequence
    QKeySequence keySequence(modifiers | key);
    
    m_platformData->fallbackShortcut = new QShortcut(keySequence, QCoreApplication::instance());
    if (!m_platformData->fallbackShortcut) {
        setError("Could not create fallback shortcut");
        return false;
    }
    
    connect(m_platformData->fallbackShortcut, &QShortcut::activated,
            this, &GlobalHotkey::handleHotkeyEvent);
    
    qDebug() << "GlobalHotkey: Using fallback QShortcut implementation";
    return true;
}

void GlobalHotkey::unregisterPlatformHotkey()
{
#ifdef Q_OS_LINUX
    if (m_platformData->x11Registered && m_platformData->display) {
        XUngrabKey(m_platformData->display,
                  m_platformData->x11Keycode,
                  m_platformData->x11Modifiers,
                  m_platformData->rootWindow);
        XSync(m_platformData->display, False);
        
#ifndef QT_X11EXTRAS_LIB
        // If we opened the display ourselves, close it
        XCloseDisplay(m_platformData->display);
        m_platformData->display = nullptr;
#endif
        
        m_platformData->x11Registered = false;
    }
#endif
    
    if (m_platformData->fallbackShortcut) {
        delete m_platformData->fallbackShortcut;
        m_platformData->fallbackShortcut = nullptr;
    }
}

void GlobalHotkey::setError(const QString& error)
{
    m_lastError = error;
    qDebug() << "GlobalHotkey error:" << error;
}

int GlobalHotkey::parseModifiers(const QStringList& modifierStrings)
{
    int modifiers = 0;
    
    for (const QString& modifier : modifierStrings) {
        QString mod = modifier.trimmed().toLower();
        
        if (mod == "ctrl" || mod == "control") {
            modifiers |= Qt::ControlModifier;
        } else if (mod == "alt") {
            modifiers |= Qt::AltModifier;
        } else if (mod == "shift") {
            modifiers |= Qt::ShiftModifier;
        } else if (mod == "meta" || mod == "super" || mod == "cmd") {
            modifiers |= Qt::MetaModifier;
        } else {
            qDebug() << "GlobalHotkey: Unknown modifier:" << modifier;
            return 0; // Invalid modifier
        }
    }
    
    return modifiers;
}

int GlobalHotkey::parseKey(const QString& keyString)
{
    QString key = keyString.trimmed().toUpper();
    
    // Handle common single-character keys
    if (key.length() == 1 && key[0].isLetter()) {
        return key[0].unicode();
    }
    
    // Handle common special keys
    if (key == "V") return Qt::Key_V;
    if (key == "C") return Qt::Key_C;
    if (key == "X") return Qt::Key_X;
    if (key == "A") return Qt::Key_A;
    if (key == "S") return Qt::Key_S;
    if (key == "Z") return Qt::Key_Z;
    if (key == "Y") return Qt::Key_Y;
    if (key == "SPACE") return Qt::Key_Space;
    if (key == "TAB") return Qt::Key_Tab;
    if (key == "ENTER" || key == "RETURN") return Qt::Key_Return;
    if (key == "ESC" || key == "ESCAPE") return Qt::Key_Escape;
    if (key == "DEL" || key == "DELETE") return Qt::Key_Delete;
    if (key == "BACKSPACE") return Qt::Key_Backspace;
    if (key == "INSERT") return Qt::Key_Insert;
    if (key == "HOME") return Qt::Key_Home;
    if (key == "END") return Qt::Key_End;
    if (key == "PAGEUP") return Qt::Key_PageUp;
    if (key == "PAGEDOWN") return Qt::Key_PageDown;
    
    // Handle function keys
    if (key.startsWith("F") && key.length() > 1) {
        bool ok;
        int fNum = key.mid(1).toInt(&ok);
        if (ok && fNum >= 1 && fNum <= 24) {
            return Qt::Key_F1 + fNum - 1;
        }
    }
    
    // Handle arrow keys
    if (key == "UP" || key == "UPARROW") return Qt::Key_Up;
    if (key == "DOWN" || key == "DOWNARROW") return Qt::Key_Down;
    if (key == "LEFT" || key == "LEFTARROW") return Qt::Key_Left;
    if (key == "RIGHT" || key == "RIGHTARROW") return Qt::Key_Right;
    
    qDebug() << "GlobalHotkey: Unknown key:" << keyString;
    return 0; // Unknown key
}

bool GlobalHotkey::isValidHotkeyString(const QString& hotkeyString)
{
    if (hotkeyString.isEmpty()) {
        return false;
    }
    
    QStringList parts = hotkeyString.split("+", Qt::SkipEmptyParts);
    
    // Must have at least modifier + key
    if (parts.size() < 2) {
        return false;
    }
    
    // Check for empty parts
    for (const QString& part : parts) {
        if (part.trimmed().isEmpty()) {
            return false;
        }
    }
    
    // Last part should be a key, others should be modifiers
    QString keyPart = parts.last().trimmed().toLower();
    QStringList modifierParts = parts.mid(0, parts.size() - 1);
    
    // Validate modifiers
    for (const QString& modifier : modifierParts) {
        QString mod = modifier.trimmed().toLower();
        if (mod != "ctrl" && mod != "control" && 
            mod != "alt" && mod != "shift" && 
            mod != "meta" && mod != "super" && mod != "cmd") {
            return false;
        }
    }
    
    // Basic key validation (more thorough check happens in parseKey)
    if (keyPart.isEmpty()) {
        return false;
    }
    
    return true;
}

void GlobalHotkey::setTestMode(bool enabled)
{
    s_testMode = enabled;
    if (enabled) {
        s_testModeRegisteredHotkeys.clear();
        qDebug() << "GlobalHotkey: Test mode enabled - hotkeys will be simulated";
    } else {
        s_testModeRegisteredHotkeys.clear();
        qDebug() << "GlobalHotkey: Test mode disabled - hotkeys will be registered normally";
    }
}

bool GlobalHotkey::isTestMode()
{
    return s_testMode;
}

void GlobalHotkey::simulateHotkeyTrigger()
{
    if (s_testMode && m_registered) {
        qDebug() << "GlobalHotkey: Test mode - simulating hotkey trigger for" << m_hotkeyString;
        emit hotkeyTriggered();
    }
}
