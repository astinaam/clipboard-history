#include "tray_icon.h"

#include <QApplication>
#include <QStyle>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QRegularExpression>

TrayIcon::TrayIcon(QObject* parent)
    : QSystemTrayIcon(parent)
    , m_contextMenu(nullptr)
    , m_showHistoryAction(nullptr)
    , m_monitoringAction(nullptr)
    , m_settingsAction(nullptr)
    , m_aboutAction(nullptr)
    , m_exitAction(nullptr)
    , m_recentItemsMenu(nullptr)
    , m_currentTheme("system")
    , m_hasCustomIcon(false)
    , m_hasHistory(false)
    , m_monitoringEnabled(true)
    , m_historyCount(0)
{
    // Initialize context menu
    initializeMenu();
    
    // Set initial icon
    updateIconState(false);
    
    // Connect activation signal
    connect(this, &QSystemTrayIcon::activated,
            this, &TrayIcon::handleActivation);
    
    // Show the tray icon
    show();
}

TrayIcon::~TrayIcon()
{
    // Qt handles cleanup automatically due to parent-child relationships
}

void TrayIcon::setIconTheme(const QString& theme)
{
    m_currentTheme = theme;
    m_hasCustomIcon = false;
    m_customIcon = QIcon();
    
    // Update icon with current state
    updateIconState(m_hasHistory);
}

void TrayIcon::setCustomIcon(const QIcon& icon)
{
    m_customIcon = icon;
    m_hasCustomIcon = true;
    
    // Apply custom icon immediately
    setIcon(icon);
}

void TrayIcon::updateIconState(bool hasHistory)
{
    m_hasHistory = hasHistory;
    
    if (m_hasCustomIcon) {
        // Don't change custom icon
        return;
    }
    
    QIcon icon = getStateIcon(hasHistory);
    setIcon(icon);
}

void TrayIcon::setHistoryCount(int count)
{
    m_historyCount = count;
    
    // Update tooltip to show count
    QString tooltip = QString("Clipboard History Manager (%1 items)").arg(count);
    setToolTip(tooltip);
}

void TrayIcon::setMonitoringState(bool enabled)
{
    m_monitoringEnabled = enabled;
    
    if (m_monitoringAction) {
        m_monitoringAction->setChecked(enabled);
        m_monitoringAction->setText(enabled ? "Monitoring Enabled" : "Monitoring Disabled");
    }
    
    // Update tooltip
    QString status = enabled ? "Active" : "Paused";
    QString tooltip = QString("Clipboard History Manager - %1").arg(status);
    setToolTip(tooltip);
}

void TrayIcon::updateRecentItems(const QList<ClipboardItem>& items)
{
    m_recentItems = items;
    updateRecentItemsMenu();
}

void TrayIcon::showHistoryWindow()
{
    emit historyWindowRequested();
}

void TrayIcon::toggleMonitoring()
{
    emit monitoringToggleRequested();
}

void TrayIcon::showSettings()
{
    emit settingsRequested();
}

void TrayIcon::showAbout()
{
    emit aboutRequested();
}

void TrayIcon::exitApplication()
{
    emit exitRequested();
}

void TrayIcon::handleActivation(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        // Left click - show history window
        emit historyWindowRequested();
        break;
        
    case QSystemTrayIcon::DoubleClick:
        // Double click - show history window
        emit historyWindowRequested();
        break;
        
    case QSystemTrayIcon::MiddleClick:
        // Middle click - toggle monitoring
        emit monitoringToggleRequested();
        break;
        
    case QSystemTrayIcon::Context:
        // Right click - context menu is shown automatically
        break;
        
    default:
        break;
    }
}

void TrayIcon::handleRecentItemTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    
    // Find the corresponding clipboard item
    int index = m_recentItemActions.indexOf(action);
    if (index >= 0 && index < m_recentItems.size()) {
        emit recentItemSelected(m_recentItems.at(index));
    }
}

void TrayIcon::initializeMenu()
{
    m_contextMenu = new QMenu();
    
    // Show History action
    m_showHistoryAction = new QAction("Show History", this);
    m_showHistoryAction->setIcon(QIcon::fromTheme("view-list-details"));
    connect(m_showHistoryAction, &QAction::triggered,
            this, &TrayIcon::showHistoryWindow);
    m_contextMenu->addAction(m_showHistoryAction);
    
    m_contextMenu->addSeparator();
    
    // Recent Items submenu
    m_recentItemsMenu = new QMenu("Recent Items");
    m_recentItemsMenu->setIcon(QIcon::fromTheme("document-open-recent"));
    m_contextMenu->addMenu(m_recentItemsMenu);
    
    m_contextMenu->addSeparator();
    
    // Monitoring toggle action
    m_monitoringAction = new QAction("Monitoring Enabled", this);
    m_monitoringAction->setCheckable(true);
    m_monitoringAction->setChecked(true);
    m_monitoringAction->setIcon(QIcon::fromTheme("media-record"));
    connect(m_monitoringAction, &QAction::triggered,
            this, &TrayIcon::toggleMonitoring);
    m_contextMenu->addAction(m_monitoringAction);
    
    m_contextMenu->addSeparator();
    
    // Settings action
    m_settingsAction = new QAction("Settings", this);
    m_settingsAction->setIcon(QIcon::fromTheme("preferences-system"));
    connect(m_settingsAction, &QAction::triggered,
            this, &TrayIcon::showSettings);
    m_contextMenu->addAction(m_settingsAction);
    
    // About action
    m_aboutAction = new QAction("About", this);
    m_aboutAction->setIcon(QIcon::fromTheme("help-about"));
    connect(m_aboutAction, &QAction::triggered,
            this, &TrayIcon::showAbout);
    m_contextMenu->addAction(m_aboutAction);
    
    m_contextMenu->addSeparator();
    
    // Exit action
    m_exitAction = new QAction("Exit", this);
    m_exitAction->setIcon(QIcon::fromTheme("application-exit"));
    connect(m_exitAction, &QAction::triggered,
            this, &TrayIcon::exitApplication);
    m_contextMenu->addAction(m_exitAction);
    
    // Set context menu
    setContextMenu(m_contextMenu);
    
    // Initialize recent items menu
    updateRecentItemsMenu();
}

void TrayIcon::updateRecentItemsMenu()
{
    // Clear existing actions
    for (QAction* action : m_recentItemActions) {
        m_recentItemsMenu->removeAction(action);
        delete action;
    }
    m_recentItemActions.clear();
    
    if (m_recentItems.isEmpty()) {
        // Show "No recent items" placeholder
        QAction* noItemsAction = new QAction("No recent items", this);
        noItemsAction->setEnabled(false);
        m_recentItemsMenu->addAction(noItemsAction);
        m_recentItemActions.append(noItemsAction);
        return;
    }
    
    // Add actions for recent items (max 5)
    int itemCount = qMin(m_recentItems.size(), MAX_RECENT_ITEMS);
    for (int i = 0; i < itemCount; ++i) {
        const ClipboardItem& item = m_recentItems.at(i);
        
        // Create preview text
        QString preview = truncateText(item.text(), MAX_PREVIEW_LENGTH);
        
        // Add pinned indicator
        if (item.pinned()) {
            preview = "📌 " + preview;
        }
        
        QAction* itemAction = new QAction(preview, this);
        itemAction->setToolTip(item.text()); // Full text in tooltip
        
        // Set icon based on item type
        if (item.pinned()) {
            itemAction->setIcon(QIcon::fromTheme("bookmark-new"));
        } else {
            itemAction->setIcon(QIcon::fromTheme("edit-copy"));
        }
        
        connect(itemAction, &QAction::triggered,
                this, &TrayIcon::handleRecentItemTriggered);
        
        m_recentItemsMenu->addAction(itemAction);
        m_recentItemActions.append(itemAction);
    }
}

QString TrayIcon::truncateText(const QString& text, int maxLength) const
{
    if (text.length() <= maxLength) {
        return text;
    }
    
    // Replace any newlines/tabs with spaces for menu display
    QString cleanText = text;
    cleanText.replace('\n', ' ');
    cleanText.replace('\t', ' ');
    cleanText.replace(QRegularExpression("\\s+"), " "); // Replace multiple spaces with single space
    
    return cleanText.left(maxLength - 3) + "...";
}

QIcon TrayIcon::getStateIcon(bool hasHistory) const
{
    QString iconName;
    
    if (m_currentTheme == "dark") {
        iconName = hasHistory ? "edit-copy" : "edit-copy-symbolic";
    } else if (m_currentTheme == "light") {
        iconName = hasHistory ? "edit-copy" : "edit-copy-symbolic";
    } else {
        // System theme - use different icons for different states
        iconName = hasHistory ? "edit-copy" : "document-new";
    }
    
    QIcon icon = QIcon::fromTheme(iconName);
    
    // Fallback to application style icon if theme icon not available
    if (icon.isNull()) {
        if (QApplication::instance()) {
            if (hasHistory) {
                icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
            } else {
                icon = QApplication::style()->standardIcon(QStyle::SP_FileDialogNewFolder);
            }
        }
    }
    
    return icon;
}
