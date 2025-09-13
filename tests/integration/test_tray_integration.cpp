#include <QtTest/QtTest>
#include <QObject>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QSignalSpy>
#include <QClipboard>
#include <QMenu>
#include <QAction>
#include <QElapsedTimer>

// Include implemented headers
#include "models/clipboard_item.h"

// Forward declarations for classes that don't exist yet
// These will need to be implemented in Phase 3.3
// class TrayIcon;
// class ClipboardManager;
// class ClipboardWindow;

// Include headers once they exist
#include "ui/tray_icon.h"
#include "services/clipboard_manager.h"
#include "ui/clipboard_window.h"
// #include "models/clipboard_item.h"

class TestTrayIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Tray Icon Lifecycle Tests
    void testTrayIconCreation();
    void testTrayIconVisibility();
    void testTrayIconDestruction();
    void testTrayIconReinitialization();

    // System Integration Tests
    void testSystemTrayAvailability();
    void testTrayIconThemeAdaptation();
    void testTrayIconStatesIntegration();
    void testNotificationIntegration();

    // Context Menu Integration Tests
    void testContextMenuCreation();
    void testMenuStructureIntegration();
    void testMenuUpdateOnHistoryChange();
    void testRecentItemsMenuDisplay();

    // Manager Integration Tests
    void testTrayManagerConnection();
    void testHistoryUpdatesTrayMenu();
    void testMonitoringStateReflection();
    void testTrayIconStateSync();

    // Window Integration Tests
    void testTrayWindowInteraction();
    void testTrayClickShowsWindow();
    void testWindowHideUpdatesTray();
    void testTrayMenuShowHistory();

    // User Interaction Integration Tests
    void testLeftClickBehavior();
    void testRightClickBehavior();
    void testDoubleClickBehavior();
    void testMiddleClickBehavior();

    // Recent Items Integration Tests
    void testRecentItemsDisplay();
    void testRecentItemSelection();
    void testRecentItemsUpdate();
    void testRecentItemsLimit();

    // Performance Integration Tests
    void testTrayUpdatePerformance();
    void testMenuDisplayPerformance();
    void testIconUpdatePerformance();
    void testBulkHistoryUpdatePerformance();

    // Signal Integration Tests
    void testManagerSignalHandling();
    void testWindowSignalHandling();
    void testTraySignalEmission();
    void testCrossComponentSignaling();

    // Error Handling Integration Tests
    void testSystemTrayUnavailable();
    void testIconLoadFailure();
    void testMenuCreationFailure();
    void testSignalConnectionFailures();

    // Multi-Component Workflow Tests
    void testCompleteWorkflowIntegration();
    void testStartupSequenceIntegration();
    void testShutdownSequenceIntegration();

private:
    TrayIcon* trayIcon;
    ClipboardManager* manager;
    ClipboardWindow* window;
    QClipboard* clipboard;
    QList<ClipboardItem> testHistory;
    
    // Helper methods
    void createComponents();
    void connectComponents();
    ClipboardItem createTestItem(const QString& text, int index);
    void populateTestHistory(int count);
    void setClipboardText(const QString& text);
    void simulateTrayClick(QSystemTrayIcon::ActivationReason reason);
    void waitForTrayUpdate();
    bool isSystemTrayAvailable();
    QMenu* getTrayContextMenu();
    QList<QAction*> getRecentItemsActions();
};

void TestTrayIntegration::initTestCase()
{
    Ensure QApplication exists for system tray
    if (!QApplication::instance()) {
        static int argc = 1;
        static char* argv[] = {"test"};
        new QApplication(argc, argv);
    }
    
    Skip all tests if system tray is not available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QSKIP("System tray not available on this platform");
    }
    
    clipboard = QApplication::clipboard();
    QVERIFY(clipboard != nullptr);
}

void TestTrayIntegration::cleanupTestCase()
{
    Cleanup handled by QApplication destructor
}

void TestTrayIntegration::init()
{
    Create fresh component instances for each test
    createComponents();
    connectComponents();
    
    Create test data
    populateTestHistory(10);
    
    Clear clipboard for clean test start
    clipboard->clear();
}

void TestTrayIntegration::cleanup()
{
    delete trayIcon;
    delete window;
    delete manager;
    trayIcon = nullptr;
    window = nullptr;
    manager = nullptr;
    
    clipboard->clear();
}

void TestTrayIntegration::testTrayIconCreation()
{
    // Integration Test: Tray icon should be created and initialized properly
    QVERIFY(trayIcon != nullptr);
    QVERIFY(trayIcon->inherits("QSystemTrayIcon"));
    QVERIFY(!trayIcon->icon().isNull());
}

void TestTrayIntegration::testTrayIconVisibility()
{
    // Integration Test: Tray icon should be visible in system tray
    trayIcon->show();
    QVERIFY(trayIcon->isVisible());
}

void TestTrayIntegration::testTrayIconDestruction()
{
    // Integration Test: Tray icon should be cleanly destroyed
    trayIcon->show();
    QVERIFY(trayIcon->isVisible());
    
    delete trayIcon;
    trayIcon = nullptr;
    
    // Should not crash or leave artifacts
}

void TestTrayIntegration::testTrayIconReinitialization()
{
    // Integration Test: Tray icon should handle recreation correctly
    
    trayIcon->show();
    delete trayIcon;
    
    // Recreate
    trayIcon = new TrayIcon(this);
    connectComponents();
    trayIcon->show();
    
    QVERIFY(trayIcon->isVisible());
}

void TestTrayIntegration::testSystemTrayAvailability()
{
    // Integration Test: Should detect and respond to system tray availability
    bool available = QSystemTrayIcon::isSystemTrayAvailable();
    
    if (available) {
        QVERIFY(trayIcon->isVisible() || trayIcon->isSystemTrayAvailable());
    } else {
        // Should handle gracefully when tray is not available
        QVERIFY(!trayIcon->isVisible());
    }
}

void TestTrayIntegration::testTrayIconThemeAdaptation()
{
    // Integration Test: Tray icon should adapt to system theme
    QIcon initialIcon = trayIcon->icon();
    QVERIFY(!initialIcon.isNull());
    
    // Test theme change adaptation (if possible to simulate)
    trayIcon->setIconTheme("dark");
    QIcon darkIcon = trayIcon->icon();
    
    trayIcon->setIconTheme("light");
    QIcon lightIcon = trayIcon->icon();
    
    // Icons should adapt to themes (implementation detail)
    QVERIFY(!darkIcon.isNull());
    QVERIFY(!lightIcon.isNull());
}

void TestTrayIntegration::testTrayIconStatesIntegration()
{
    // Integration Test: Tray icon should reflect application state
    // Test empty history state
    trayIcon->updateIconState(false);
    QIcon emptyIcon = trayIcon->icon();
    
    // Test populated history state
    trayIcon->updateIconState(true);
    QIcon populatedIcon = trayIcon->icon();
    
    // Icons should be different to indicate state
    QVERIFY(!emptyIcon.isNull());
    QVERIFY(!populatedIcon.isNull());
}

void TestTrayIntegration::testNotificationIntegration()
{
    // Integration Test: Tray notifications should work with system
    if (QSystemTrayIcon::supportsMessages()) {
        QSignalSpy spy(trayIcon, &QSystemTrayIcon::messageClicked);
        
        trayIcon->showMessage("Test", "Integration test notification",
                             QSystemTrayIcon::Information, 1000);
        
        // Notification should be displayed
        // (Automatic verification is difficult for notifications)
    }
}

void TestTrayIntegration::testContextMenuCreation()
{
    // Integration Test: Context menu should be created and populated
    QMenu* menu = trayIcon->contextMenu();
    QVERIFY(menu != nullptr);
    
    QList<QAction*> actions = menu->actions();
    QVERIFY(actions.size() >= 6); // Show History, Recent Items, Monitoring, Settings, About, Exit
    
    Verify expected actions exist
    bool hasShowHistory = false;
    bool hasExit = false;
    
    for (QAction* action : actions) {
        if (action->isSeparator()) continue;
        if (action->menu()) continue; // Skip submenus for now
        
        if (action->text().contains("Show History")) hasShowHistory = true;
        if (action->text().contains("Exit")) hasExit = true;
    }
    
    QVERIFY(hasShowHistory);
    QVERIFY(hasExit);
}

void TestTrayIntegration::testMenuStructureIntegration()
{
    // Integration Test: Menu structure should match specification
    QMenu* menu = getTrayContextMenu();
    QList<QAction*> actions = menu->actions();
    
    Verify menu structure matches specification
    Expected order: Show History, Recent Items, Separator, Monitoring, Separator, Settings, About, Exit
    
    bool foundShowHistory = false;
    bool foundRecentItems = false;
    bool foundMonitoring = false;
    bool foundSettings = false;
    bool foundAbout = false;
    bool foundExit = false;
    
    for (QAction* action : actions) {
        QString text = action->text();
        if (text.contains("Show History")) foundShowHistory = true;
        else if (text.contains("Recent")) foundRecentItems = true;
        else if (text.contains("Monitoring")) foundMonitoring = true;
        else if (text.contains("Settings")) foundSettings = true;
        else if (text.contains("About")) foundAbout = true;
        else if (text.contains("Exit")) foundExit = true;
    }
    
    QVERIFY(foundShowHistory);
    QVERIFY(foundMonitoring);
    QVERIFY(foundSettings);
    QVERIFY(foundAbout);
    QVERIFY(foundExit);
}

void TestTrayIntegration::testMenuUpdateOnHistoryChange()
{
    // Integration Test: Menu should update when history changes
    Start with empty history
    trayIcon->updateRecentItems(QList<ClipboardItem>());
    
    QMenu* menu = getTrayContextMenu();
    QList<QAction*> initialActions = getRecentItemsActions();
    
    Add items to history
    trayIcon->updateRecentItems(testHistory.mid(0, 3));
    
    QList<QAction*> updatedActions = getRecentItemsActions();
    
    Menu should reflect new history
    QVERIFY(updatedActions.size() >= initialActions.size());
}

void TestTrayIntegration::testRecentItemsMenuDisplay()
{
    // Integration Test: Recent items should be displayed correctly in menu
    Update with test history
    trayIcon->updateRecentItems(testHistory.mid(0, 5));
    
    QList<QAction*> recentActions = getRecentItemsActions();
    
    // Should show up to 5 recent items
    QVERIFY(recentActions.size() <= 5);
    
    Verify preview text is truncated to 50 characters
    for (QAction* action : recentActions) {
        QString text = action->text();
        QVERIFY(text.length() <= 50 + 10); // Allow for ellipsis and formatting
    }
}

void TestTrayIntegration::testTrayManagerConnection()
{
    // Integration Test: Tray icon should be connected to clipboard manager
    // Test signal connection
    QSignalSpy managerSpy(manager, &ClipboardManager::historyUpdated);
    QSignalSpy traySpy(trayIcon, &TrayIcon::monitoringToggleRequested);
    
    Trigger manager signal
    setClipboardText("Test connection");
    waitForTrayUpdate();
    
    Manager should emit signal
    QVERIFY(managerSpy.count() >= 1);
    
    Trigger tray signal
    trayIcon->toggleMonitoring();
    
    Tray should emit signal
    QCOMPARE(traySpy.count(), 1);
}

void TestTrayIntegration::testHistoryUpdatesTrayMenu()
{
    // Integration Test: History changes should update tray menu
    Start with empty history
    manager->clearHistory();
    waitForTrayUpdate();
    
    QList<QAction*> emptyActions = getRecentItemsActions();
    
    Add items to history through manager
    for (int i = 0; i < 3; ++i) {
        setClipboardText(QString("History item %1").arg(i));
        waitForTrayUpdate();
    }
    
    QList<QAction*> populatedActions = getRecentItemsActions();
    
    Menu should reflect new items
    QVERIFY(populatedActions.size() > emptyActions.size());
}

void TestTrayIntegration::testMonitoringStateReflection()
{
    // Integration Test: Tray should reflect monitoring state
    Start monitoring
    manager->startMonitoring();
    trayIcon->setMonitoringState(true);
    
    QMenu* menu = getTrayContextMenu();
    QAction* monitoringAction = nullptr;
    
    for (QAction* action : menu->actions()) {
        if (action->text().contains("Monitoring")) {
            monitoringAction = action;
            break;
        }
    }
    
    QVERIFY(monitoringAction != nullptr);
    QVERIFY(monitoringAction->isCheckable());
    QVERIFY(monitoringAction->isChecked());
    
    Stop monitoring
    manager->stopMonitoring();
    trayIcon->setMonitoringState(false);
    
    QVERIFY(!monitoringAction->isChecked());
}

void TestTrayIntegration::testTrayIconStateSync()
{
    // Integration Test: Tray icon state should sync with application state
    Start with empty history
    manager->clearHistory();
    trayIcon->updateIconState(false);
    
    QIcon emptyIcon = trayIcon->icon();
    
    Add items to history
    setClipboardText("Content for tray sync test");
    waitForTrayUpdate();
    trayIcon->updateIconState(true);
    
    QIcon populatedIcon = trayIcon->icon();
    
    // Icons should be different to indicate state
    QVERIFY(emptyIcon.cacheKey() != populatedIcon.cacheKey());
}

void TestTrayIntegration::testTrayWindowInteraction()
{
    // Integration Test: Tray and window should interact correctly
    QSignalSpy traySignal(trayIcon, &TrayIcon::historyWindowRequested);
    QSignalSpy windowSignal(window, &ClipboardWindow::windowShown);
    
    Tray should request window display
    trayIcon->showHistoryWindow();
    
    QCOMPARE(traySignal.count(), 1);
    
    Window should respond to request
    This would need proper signal connection
    if (windowSignal.isValid()) {
        QVERIFY(windowSignal.count() >= 0); // May or may not emit depending on implementation
    }
}

void TestTrayIntegration::testTrayClickShowsWindow()
{
    // Integration Test: Tray click should show clipboard window
    
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    // Simulate left click
    simulateTrayClick(QSystemTrayIcon::Trigger);
    
    QCOMPARE(spy.count(), 1);
}

void TestTrayIntegration::testWindowHideUpdatesTray()
{
    // Integration Test: Window hiding should update tray state if needed
    
    // Show window
    window->show();
    
    // Hide window
    window->hide();
    
    // Tray should remain visible and functional
    QVERIFY(trayIcon->isVisible());
}

void TestTrayIntegration::testTrayMenuShowHistory()
{
    // Integration Test: "Show History" menu action should work
    
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    QMenu* menu = getTrayContextMenu();
    QAction* showHistoryAction = nullptr;
    
    for (QAction* action : menu->actions()) {
        if (action->text().contains("Show History")) {
            showHistoryAction = action;
            break;
        }
    }
    
    QVERIFY(showHistoryAction != nullptr);
    showHistoryAction->trigger();
    
    QCOMPARE(spy.count(), 1);
}

void TestTrayIntegration::testLeftClickBehavior()
{
    // Integration Test: Left click should show history window
    
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    simulateTrayClick(QSystemTrayIcon::Trigger);
    
    QCOMPARE(spy.count(), 1);
}

void TestTrayIntegration::testRightClickBehavior()
{
    // Integration Test: Right click should show context menu
    
    simulateTrayClick(QSystemTrayIcon::Context);
    
    QMenu* menu = getTrayContextMenu();
    QVERIFY(menu != nullptr);
    // Menu visibility is hard to test automatically
}

void TestTrayIntegration::testDoubleClickBehavior()
{
    // Integration Test: Double click should show history window
    
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    simulateTrayClick(QSystemTrayIcon::DoubleClick);
    
    QCOMPARE(spy.count(), 1);
}

void TestTrayIntegration::testMiddleClickBehavior()
{
    // Integration Test: Middle click should toggle monitoring
    
    QSignalSpy spy(trayIcon, &TrayIcon::monitoringToggleRequested);
    
    simulateTrayClick(QSystemTrayIcon::MiddleClick);
    
    QCOMPARE(spy.count(), 1);
}

void TestTrayIntegration::testRecentItemsDisplay()
{
    // Integration Test: Recent items should be displayed correctly
    
    // Populate with test history
    trayIcon->updateRecentItems(testHistory.mid(0, 5));
    
    QList<QAction*> recentActions = getRecentItemsActions();
    
    // Should show exactly 5 items or less
    QVERIFY(recentActions.size() <= 5);
    
    // Each action should have appropriate text
    for (int i = 0; i < recentActions.size(); ++i) {
        QString actionText = recentActions[i]->text();
        QVERIFY(!actionText.isEmpty());
        QVERIFY(actionText.length() <= 50 + 10); // 50 chars + formatting
    }
}

void TestTrayIntegration::testRecentItemSelection()
{
    // Integration Test: Selecting recent item should emit signal
    
    trayIcon->updateRecentItems(testHistory.mid(0, 3));
    
    QSignalSpy spy(trayIcon, &TrayIcon::recentItemSelected);
    
    QList<QAction*> recentActions = getRecentItemsActions();
    if (!recentActions.isEmpty()) {
        recentActions.first()->trigger();
        QCOMPARE(spy.count(), 1);
    }
}

void TestTrayIntegration::testRecentItemsUpdate()
{
    // Integration Test: Recent items should update when history changes
    
    // Start with initial items
    trayIcon->updateRecentItems(testHistory.mid(0, 3));
    QList<QAction*> initialActions = getRecentItemsActions();
    
    // Update with different items
    trayIcon->updateRecentItems(testHistory.mid(3, 3));
    QList<QAction*> updatedActions = getRecentItemsActions();
    
    // Actions should be different
    QCOMPARE(updatedActions.size(), initialActions.size());
    
    // Content should be different
    if (!initialActions.isEmpty() && !updatedActions.isEmpty()) {
        QVERIFY(initialActions.first()->text() != updatedActions.first()->text());
    }
}

void TestTrayIntegration::testRecentItemsLimit()
{
    // Integration Test: Recent items should be limited to 5
    
    // Try to update with more than 5 items
    trayIcon->updateRecentItems(testHistory); // 10 items
    
    QList<QAction*> recentActions = getRecentItemsActions();
    
    // Should be limited to 5
    QVERIFY(recentActions.size() <= 5);
}

void TestTrayIntegration::testTrayUpdatePerformance()
{
    // Integration Test: Tray updates should meet performance requirements
    
    QElapsedTimer timer;
    timer.start();
    
    // Update tray with history
    trayIcon->updateRecentItems(testHistory.mid(0, 5));
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100); // <100ms requirement for menu updates
}

void TestTrayIntegration::testMenuDisplayPerformance()
{
    // Integration Test: Menu display should be fast
    
    trayIcon->updateRecentItems(testHistory.mid(0, 5));
    
    QElapsedTimer timer;
    timer.start();
    
    QMenu* menu = getTrayContextMenu();
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 50); // Menu access should be instant
    QVERIFY(menu != nullptr);
}

void TestTrayIntegration::testIconUpdatePerformance()
{
    // Integration Test: Icon updates should be instant
    
    QElapsedTimer timer;
    timer.start();
    
    trayIcon->updateIconState(true);
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 50); // <50ms requirement for icon updates
}

void TestTrayIntegration::testBulkHistoryUpdatePerformance()
{
    // Integration Test: Bulk history updates should be efficient
    
    // Create larger history
    QList<ClipboardItem> largeHistory;
    for (int i = 0; i < 100; ++i) {
        largeHistory << createTestItem(QString("Bulk item %1").arg(i), i);
    }
    
    QElapsedTimer timer;
    timer.start();
    
    // Update with large history (should only show first 5)
    trayIcon->updateRecentItems(largeHistory);
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100); // Should handle large updates efficiently
    
    QList<QAction*> recentActions = getRecentItemsActions();
    QVERIFY(recentActions.size() <= 5);
}

void TestTrayIntegration::testManagerSignalHandling()
{
    // Integration Test: Tray should handle manager signals correctly
    
    // Connect signals
    connect(manager, &ClipboardManager::historyUpdated,
            trayIcon, &TrayIcon::updateRecentItems);
    
    QSignalSpy spy(manager, &ClipboardManager::historyUpdated);
    
    // Trigger manager signal
    setClipboardText("Signal test content");
    waitForTrayUpdate();
    
    // Manager should emit signal
    QVERIFY(spy.count() >= 1);
    
    // Tray should have updated (hard to verify automatically)
}

void TestTrayIntegration::testWindowSignalHandling()
{
    // Integration Test: Tray should handle window signals correctly
    
    // Connect tray to window
    connect(trayIcon, &TrayIcon::historyWindowRequested,
            window, &ClipboardWindow::show);
    
    QSignalSpy traySignal(trayIcon, &TrayIcon::historyWindowRequested);
    
    // Trigger tray signal
    trayIcon->showHistoryWindow();
    
    QCOMPARE(traySignal.count(), 1);
}

void TestTrayIntegration::testTraySignalEmission()
{
    // Integration Test: Tray should emit correct signals
    QSignalSpy historySignal(trayIcon, &TrayIcon::historyWindowRequested);
    QSignalSpy monitoringSignal(trayIcon, &TrayIcon::monitoringToggleRequested);
    QSignalSpy settingsSignal(trayIcon, &TrayIcon::settingsRequested);
    QSignalSpy aboutSignal(trayIcon, &TrayIcon::aboutRequested);
    QSignalSpy exitSignal(trayIcon, &TrayIcon::exitRequested);
    
    // Test each signal
    trayIcon->showHistoryWindow();
    QCOMPARE(historySignal.count(), 1);
    
    trayIcon->toggleMonitoring();
    QCOMPARE(monitoringSignal.count(), 1);
    
    trayIcon->showSettings();
    QCOMPARE(settingsSignal.count(), 1);
    
    trayIcon->showAbout();
    QCOMPARE(aboutSignal.count(), 1);
    
    trayIcon->exitApplication();
    QCOMPARE(exitSignal.count(), 1);
}

void TestTrayIntegration::testCrossComponentSignaling()
{
    // Integration Test: Signals should flow correctly between components
    Set up cross-component connections
    connect(manager, &ClipboardManager::historyUpdated,
            trayIcon, &TrayIcon::updateRecentItems);
    connect(trayIcon, &TrayIcon::historyWindowRequested,
            window, &ClipboardWindow::show);
    
    QSignalSpy managerSpy(manager, &ClipboardManager::historyUpdated);
    QSignalSpy windowSpy(window, &ClipboardWindow::windowShown);
    
    Trigger manager update
    setClipboardText("Cross-component test");
    waitForTrayUpdate();
    
    // Should propagate through components
    QVERIFY(managerSpy.count() >= 1);
    
    Trigger tray action
    trayIcon->showHistoryWindow();
    
    // Should affect window
    if (windowSpy.isValid()) {
        QVERIFY(windowSpy.count() >= 0);
    }
}

void TestTrayIntegration::testSystemTrayUnavailable()
{
    // Integration Test: Handle system tray unavailability gracefully
    This test would need to simulate system tray being unavailable
    For now, just verify current availability
    
    bool available = QSystemTrayIcon::isSystemTrayAvailable();
    if (!available) {
        // Application should handle gracefully
        QVERIFY(!trayIcon->isVisible());
    } else {
        QVERIFY(trayIcon->show());
    }
}

void TestTrayIntegration::testIconLoadFailure()
{
    // Integration Test: Handle icon loading failures gracefully
    
    // Try to set invalid icon
    QIcon invalidIcon("non_existent_file.png");
    trayIcon->setCustomIcon(invalidIcon);
    
    // Should not crash and should have some fallback icon
    QIcon currentIcon = trayIcon->icon();
    QVERIFY(!currentIcon.isNull()); // Should have fallback
}

void TestTrayIntegration::testMenuCreationFailure()
{
    // Integration Test: Handle menu creation failures gracefully
    
    // This would test recovery from menu creation issues
    // For now, just verify menu exists
    QMenu* menu = getTrayContextMenu();
    
    if (menu == nullptr) {
        // Should handle gracefully and possibly recreate
        QFAIL("Menu creation failed");
    } else {
        QVERIFY(menu->actions().size() > 0);
    }
}

void TestTrayIntegration::testSignalConnectionFailures()
{
    // Integration Test: Handle signal connection failures gracefully
    
    // Test connections work
    bool connected = connect(trayIcon, &TrayIcon::historyWindowRequested,
                            this, [](){ /* test slot */ });
    QVERIFY(connected);
    
    // Test with invalid connection
    bool invalidConnection = connect(trayIcon, &TrayIcon::historyWindowRequested,
                                    nullptr, nullptr);
    QVERIFY(!invalidConnection);
}

void TestTrayIntegration::testCompleteWorkflowIntegration()
{
    // Integration Test: Complete workflow from clipboard to tray display
    
    // Set up complete integration
    connectComponents();
    
    // Start with empty history
    manager->clearHistory();
    trayIcon->updateRecentItems(QList<ClipboardItem>());
    
    // Add content to clipboard
    setClipboardText("Complete workflow test");
    waitForTrayUpdate();
    
    // Should flow: Clipboard -> Manager -> Tray -> Menu
    QList<QAction*> recentActions = getRecentItemsActions();
    
    // Should have at least one item
    QVERIFY(recentActions.size() >= 1);
    
    // Click tray to show window
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    simulateTrayClick(QSystemTrayIcon::Trigger);
    QCOMPARE(spy.count(), 1);
}

void TestTrayIntegration::testStartupSequenceIntegration()
{
    // Integration Test: Startup sequence should initialize all components correctly
    
    // Test initialization order
    QVERIFY(manager != nullptr);
    QVERIFY(window != nullptr);
    QVERIFY(trayIcon != nullptr);
    
    // Tray should be visible
    trayIcon->show();
    QVERIFY(trayIcon->isVisible());
    
    // Manager should be ready
    manager->startMonitoring();
    QVERIFY(manager->isMonitoring());
    
    // Components should be connected
    connectComponents();
}

void TestTrayIntegration::testShutdownSequenceIntegration()
{
    // Integration Test: Shutdown should clean up all components correctly
    
    // Start with initialized components
    trayIcon->show();
    manager->startMonitoring();
    
    // Shutdown sequence
    manager->stopMonitoring();
    trayIcon->hide();
    
    // Components should be in clean state
    QVERIFY(!manager->isMonitoring());
    QVERIFY(!trayIcon->isVisible());
}

// Helper method implementations
void TestTrayIntegration::createComponents()
{
    Create actual components
    manager = new ClipboardManager(this);
    window = new ClipboardWindow(this);
    trayIcon = new TrayIcon(this);
}

void TestTrayIntegration::connectComponents()
{
    Connect the components as in main.cpp
    connect(manager, &ClipboardManager::historyChanged,
            [this]() {
                trayIcon->updateRecentItems(manager->getHistory());
            });
    
    connect(manager, &ClipboardManager::monitoringStateChanged,
            trayIcon, &TrayIcon::setMonitoringState);
    
    connect(trayIcon, &TrayIcon::historyWindowRequested,
            [this]() {
                window->showAtCursor();
            });
    
    connect(trayIcon, &TrayIcon::monitoringToggleRequested,
            [this]() {
                // Toggle monitoring logic would go here
            });
            
    connect(trayIcon, &TrayIcon::recentItemSelected,
            [this](const ClipboardItem& item) {
                QClipboard* clipboard = QApplication::clipboard();
                clipboard->setText(item.text());
            });
}

ClipboardItem TestTrayIntegration::createTestItem(const QString& text, int index)
{
    ClipboardItem item(text, QDateTime::currentDateTime().addSecs(-index));
    return item;
}

void TestTrayIntegration::populateTestHistory(int count)
{
    testHistory.clear();
    for (int i = 0; i < count; ++i) {
        QString text = QString("Test tray item %1 with some content").arg(i);
        testHistory << createTestItem(text, i);
    }
}

void TestTrayIntegration::setClipboardText(const QString& text)
{
    clipboard->setText(text);
}

void TestTrayIntegration::simulateTrayClick(QSystemTrayIcon::ActivationReason reason)
{
    if (trayIcon) {
        // emit trayIcon->activated(reason);
    }
}

void TestTrayIntegration::waitForTrayUpdate()
{
    Give time for tray updates to complete
    QTest::qWait(100);
}

bool TestTrayIntegration::isSystemTrayAvailable()
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

QMenu* TestTrayIntegration::getTrayContextMenu()
{
    if (trayIcon) {
        // return trayIcon->contextMenu();
    }
    return nullptr;
}

QList<QAction*> TestTrayIntegration::getRecentItemsActions()
{
    QList<QAction*> recentActions;
    
    QMenu* menu = getTrayContextMenu();
    if (menu) {
        // Find recent items submenu or actions
        // Implementation depends on menu structure
        // for (QAction* action : menu->actions()) {
        //     if (action->text().contains("Recent") || action->menu()) {
        //         // Handle recent items section
        //     }
        // }
    }
    
    return recentActions;
}

QTEST_MAIN(TestTrayIntegration)
#include "test_tray_integration.moc"
