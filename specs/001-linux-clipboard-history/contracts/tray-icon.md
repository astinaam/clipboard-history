# TrayIcon Library Contract

## Public Interface

### TrayIcon Class

```cpp
class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit TrayIcon(QObject *parent = nullptr);
    ~TrayIcon();

    // Icon management
    void setIconTheme(const QString &theme);
    void setCustomIcon(const QIcon &icon);
    void updateIconState(bool hasHistory);
    
    // Menu management
    void setHistoryCount(int count);
    void setMonitoringState(bool enabled);
    void updateRecentItems(const QList<ClipboardItem> &recentItems);

public slots:
    void showHistoryWindow();
    void toggleMonitoring();
    void showSettings();
    void showAbout();
    void exitApplication();

signals:
    void historyWindowRequested();
    void monitoringToggleRequested();
    void settingsRequested();
    void aboutRequested();
    void exitRequested();
    void recentItemSelected(const QString &id);

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onMenuActionTriggered();

private:
    // Internal implementation details omitted
};
```

## Contract Guarantees

### Icon Behavior
- `setIconTheme()`: Must automatically select appropriate icon for system theme
- `updateIconState()`: Must visually indicate when clipboard history is empty/populated
- Icon: Must be visible in system tray and respond to user interaction

### Menu Structure
```
┌─────────────────────────────┐
│ Show History        Ctrl+V  │  ← Primary action
├─────────────────────────────┤
│ Recent Items:               │  ← Submenu with last 5 items
│   ► Item 1 preview...       │
│   ► Item 2 preview...       │
│   ► Item 3 preview...       │
├─────────────────────────────┤
│ ☑ Monitoring Enabled        │  ← Toggle checkbox
├─────────────────────────────┤
│ Settings...                 │
│ About                       │
├─────────────────────────────┤
│ Exit                        │
└─────────────────────────────┘
```

### Interaction Behavior
- Left click: Must emit `historyWindowRequested()` signal
- Right click: Must show context menu
- Double click: Must emit `historyWindowRequested()` signal
- Middle click: Must toggle monitoring (emit `monitoringToggleRequested()`)

### Menu Actions
- "Show History": Must emit `historyWindowRequested()`
- Recent item selection: Must emit `recentItemSelected(id)`
- "Monitoring Enabled": Must emit `monitoringToggleRequested()`
- "Settings": Must emit `settingsRequested()`
- "About": Must emit `aboutRequested()`
- "Exit": Must emit `exitRequested()`

### Performance Contracts
- Menu updates: Must complete in <100ms when history changes
- Icon updates: Must be instant (<50ms) for state changes
- Recent items: Must show last 5 items with truncated previews (max 50 chars)

### System Integration
- Tray availability: Must detect system tray availability before showing
- Icon themes: Must adapt to system icon theme changes automatically
- Notifications: Must show balloon notifications for important events

## Visual Specification

### Icon States
- **Normal**: Standard clipboard icon in system theme colors
- **Empty**: Grayed out clipboard icon when no history
- **Active**: Slightly highlighted icon when monitoring clipboard
- **Error**: Warning overlay when system errors occur

### Menu Item Format
- Recent items: `"Preview text..." (timestamp)`
- Truncation: Show first 50 characters with "..." if longer
- Special characters: Replace newlines with "↵" symbol
- Empty items: Show "(empty)" placeholder

### Notification Messages
```cpp
// Notification types and messages
enum NotificationType {
    Info,       // "Clipboard monitoring started"
    Warning,    // "History limit reached, removing old items"
    Error       // "Failed to save clipboard history"
};
```

## Error Conditions

### System Tray Errors
- Tray unavailable: Hide tray icon, provide alternative access method
- Icon loading failure: Use fallback system icon
- Menu creation failure: Provide minimal menu with essential actions

### Integration Errors
- Theme detection failure: Use default system icon theme
- Notification failure: Continue operation without notifications
- Menu update failure: Keep previous menu state, log error

### State Synchronization
- History count mismatch: Refresh from authoritative source
- Monitoring state desync: Query actual monitoring state
- Recent items out of date: Request fresh data from clipboard manager

## Platform-Specific Behavior

### Linux Desktop Environments
- **GNOME**: Integration with GNOME Shell notification area
- **KDE**: Native KDE system tray integration
- **XFCE**: Standard freedesktop.org system tray protocol
- **Others**: Fallback to basic system tray functionality

### Icon Theme Support
- Follow freedesktop.org icon naming specification
- Support for custom icon themes
- Automatic high-DPI icon scaling
- Fallback to bundled icons if system icons unavailable
