#ifndef TRAY_ICON_H
#define TRAY_ICON_H

#include <QSystemTrayIcon>
#include <QObject>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QList>

#include "models/clipboard_item.h"

/**
 * @brief System tray icon implementation for clipboard history manager
 * 
 * Provides system tray integration with context menu showing recent clipboard items,
 * monitoring controls, and application actions. Supports automatic icon theme 
 * adaptation and efficient menu updates.
 * 
 * Performance Requirements:
 * - Menu updates must complete in <100ms
 * - Icon updates must complete in <50ms
 * - Memory efficient operation with history changes
 */
class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    /**
     * @brief Construct TrayIcon with parent
     * @param parent QObject parent (optional)
     */
    explicit TrayIcon(QObject* parent = nullptr);
    
    /**
     * @brief Destructor - cleans up menu and actions
     */
    ~TrayIcon();

public slots:
    /**
     * @brief Set icon theme for system integration
     * @param theme Theme name ("dark", "light", or system default)
     */
    void setIconTheme(const QString& theme);
    
    /**
     * @brief Set custom icon (overrides theme)
     * @param icon Custom QIcon to use
     */
    void setCustomIcon(const QIcon& icon);
    
    /**
     * @brief Update icon to reflect clipboard history state
     * @param hasHistory True if clipboard history contains items
     */
    void updateIconState(bool hasHistory);
    
    /**
     * @brief Update menu to show current history count
     * @param count Number of items in clipboard history
     */
    void setHistoryCount(int count);
    
    /**
     * @brief Update monitoring state in menu
     * @param enabled True if clipboard monitoring is active
     */
    void setMonitoringState(bool enabled);
    
    /**
     * @brief Update recent items submenu
     * @param items List of recent clipboard items (will show last 5)
     */
    void updateRecentItems(const QList<ClipboardItem>& items);
    
    /**
     * @brief Show history window (emits signal)
     */
    void showHistoryWindow();
    
    /**
     * @brief Toggle monitoring state (emits signal)
     */
    void toggleMonitoring();
    
    /**
     * @brief Show settings dialog (emits signal)
     */
    void showSettings();
    
    /**
     * @brief Show about dialog (emits signal)
     */
    void showAbout();
    
    /**
     * @brief Exit application (emits signal)
     */
    void exitApplication();

signals:
    /**
     * @brief Emitted when user requests to show history window
     */
    void historyWindowRequested();
    
    /**
     * @brief Emitted when user requests to toggle monitoring
     */
    void monitoringToggleRequested();
    
    /**
     * @brief Emitted when user requests settings dialog
     */
    void settingsRequested();
    
    /**
     * @brief Emitted when user requests about dialog
     */
    void aboutRequested();
    
    /**
     * @brief Emitted when user requests application exit
     */
    void exitRequested();
    
    /**
     * @brief Emitted when user selects a recent item
     * @param item Selected clipboard item
     */
    void recentItemSelected(const ClipboardItem& item);

private slots:
    /**
     * @brief Handle tray icon activation (clicks)
     * @param reason Activation reason (single click, double click, etc.)
     */
    void handleActivation(QSystemTrayIcon::ActivationReason reason);
    
    /**
     * @brief Handle recent item menu action triggered
     */
    void handleRecentItemTriggered();

private:
    /**
     * @brief Initialize context menu structure
     */
    void initializeMenu();
    
    /**
     * @brief Update recent items submenu efficiently
     */
    void updateRecentItemsMenu();
    
    /**
     * @brief Truncate text to specified length with ellipsis
     * @param text Text to truncate
     * @param maxLength Maximum character count (default 50)
     * @return Truncated text with "..." if needed
     */
    QString truncateText(const QString& text, int maxLength = 50) const;
    
    /**
     * @brief Get appropriate icon for current theme and state
     * @param hasHistory Whether clipboard has items
     * @return QIcon for current state
     */
    QIcon getStateIcon(bool hasHistory) const;

private:
    // Context menu and actions
    QMenu* m_contextMenu;
    QAction* m_showHistoryAction;
    QAction* m_monitoringAction;
    QAction* m_settingsAction;
    QAction* m_aboutAction;
    QAction* m_exitAction;
    
    // Recent items submenu
    QMenu* m_recentItemsMenu;
    QList<QAction*> m_recentItemActions;
    
    // State tracking
    QString m_currentTheme;
    QIcon m_customIcon;
    bool m_hasCustomIcon;
    bool m_hasHistory;
    bool m_monitoringEnabled;
    int m_historyCount;
    
    // Recent items cache
    QList<ClipboardItem> m_recentItems;
    
    // Constants
    static constexpr int MAX_RECENT_ITEMS = 5;
    static constexpr int MAX_PREVIEW_LENGTH = 50;
};

#endif // TRAY_ICON_H
