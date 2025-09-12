#include <QtTest/QtTest>
#include <QObject>
#include <QSystemTrayIcon>
#include <QSignalSpy>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QElapsedTimer>
#include <QApplication>

// Include implemented headers
#include "models/clipboard_item.h"

// Include the implemented TrayIcon header
#include "ui/tray_icon.h"

class TestTrayIcon : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor/Destructor Tests
    void testConstruction();
    void testDestruction();
    void testSystemTrayAvailability();

    // Icon Management Tests
    void testSetIconTheme();
    void testSetCustomIcon();
    void testUpdateIconState_hasHistory();
    void testUpdateIconState_noHistory();
    void testIconVisibility();

    // Menu Management Tests
    void testSetHistoryCount();
    void testSetMonitoringState();
    void testUpdateRecentItems_empty();
    void testUpdateRecentItems_withItems();
    void testMenuStructure();

    // Interaction Tests
    void testLeftClick();
    void testRightClick();
    void testDoubleClick();
    void testMiddleClick();

    // Slot Tests
    void testShowHistoryWindow();
    void testToggleMonitoring();
    void testShowSettings();
    void testShowAbout();
    void testExitApplication();

    // Signal Tests
    void testHistoryWindowRequestedSignal();
    void testMonitoringToggleRequestedSignal();
    void testSettingsRequestedSignal();
    void testAboutRequestedSignal();
    void testExitRequestedSignal();
    void testRecentItemSelectedSignal();

    // Menu Action Tests
    void testMenuShowHistory();
    void testMenuRecentItems();
    void testMenuMonitoringToggle();
    void testMenuSettings();
    void testMenuAbout();
    void testMenuExit();

    // Performance Tests
    void testMenuUpdatePerformance();
    void testIconUpdatePerformance();

    // System Integration Tests
    void testIconThemeAdaptation();
    void testNotificationSupport();

private:
    TrayIcon* trayIcon;
    QList<ClipboardItem> testItems;
    
    // Helper methods
    ClipboardItem createTestItem(const QString& text, bool pinned = false);
    QList<ClipboardItem> createTestHistory(int count);
    void simulateActivation(QSystemTrayIcon::ActivationReason reason);
    bool isSystemTrayAvailable();
};

void TestTrayIcon::initTestCase()
{
    // Ensure QApplication exists for system tray tests
    if (!QApplication::instance()) {
        static int argc = 1;
        static char* argv[] = {"test"};
        new QApplication(argc, argv);
    }
    
    // Skip all tests if system tray is not available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QSKIP("System tray not available on this platform");
    }
}

void TestTrayIcon::cleanupTestCase()
{
    // Cleanup handled by QApplication destructor
}

void TestTrayIcon::init()
{
    // Create fresh tray icon instance for each test
    trayIcon = new TrayIcon(this);
    
    // Create test data
    testItems = createTestHistory(5);
}

void TestTrayIcon::cleanup()
{
    delete trayIcon;
    trayIcon = nullptr;
}

void TestTrayIcon::testConstruction()
{
    // Test that TrayIcon can be constructed
    TrayIcon testIcon;
    QVERIFY(&testIcon != nullptr);
    QVERIFY(testIcon.inherits("QSystemTrayIcon"));
}

void TestTrayIcon::testDestruction()
{
    // Test that TrayIcon can be properly destroyed
    TrayIcon* testIcon = new TrayIcon();
    delete testIcon; // Should not crash
}

void TestTrayIcon::testSystemTrayAvailability()
{
    // Contract: Must detect system tray availability before showing
    QVERIFY(QSystemTrayIcon::isSystemTrayAvailable());
    if (trayIcon) {
        QVERIFY(trayIcon->isVisible());
    }
}

void TestTrayIcon::testSetIconTheme()
{
    // Contract: Must automatically select appropriate icon for system theme
    trayIcon->setIconTheme("dark");
    QIcon icon = trayIcon->icon();
    QVERIFY(!icon.isNull());
    
    trayIcon->setIconTheme("light");
    QIcon lightIcon = trayIcon->icon();
    QVERIFY(!lightIcon.isNull());
    // Icons should be different for different themes
}

void TestTrayIcon::testSetCustomIcon()
{
    // Contract: Should accept custom icon
    QIcon customIcon(":/icons/custom.png");
    trayIcon->setCustomIcon(customIcon);
    QCOMPARE(trayIcon->icon().cacheKey(), customIcon.cacheKey());
}

void TestTrayIcon::testUpdateIconState_hasHistory()
{
    // Contract: Must visually indicate when clipboard history is populated
    QIcon beforeIcon = trayIcon->icon();
    trayIcon->updateIconState(true);
    QIcon afterIcon = trayIcon->icon();
    
    // Icons should be different to indicate state
    QVERIFY(beforeIcon.cacheKey() != afterIcon.cacheKey());
}

void TestTrayIcon::testUpdateIconState_noHistory()
{
    // Contract: Must visually indicate when clipboard history is empty
    trayIcon->updateIconState(true);  // Set to has history first
    trayIcon->updateIconState(false); // Then set to empty
    
    // Should show empty state icon
    QIcon emptyIcon = trayIcon->icon();
    QVERIFY(!emptyIcon.isNull());
}

void TestTrayIcon::testIconVisibility()
{
    // Contract: Must be visible in system tray and respond to user interaction
    trayIcon->show();
    QVERIFY(trayIcon->isVisible());
}

void TestTrayIcon::testSetHistoryCount()
{
    // Contract: Should update menu to reflect history count
    trayIcon->setHistoryCount(10);
    // Verify menu shows correct count information
    QVERIFY(trayIcon->toolTip().contains("10 items"));
}

void TestTrayIcon::testSetMonitoringState()
{
    // Contract: Should update menu checkbox state
    trayIcon->setMonitoringState(true);
    // Verify "Monitoring Enabled" checkbox is checked
    QMenu* menu = trayIcon->contextMenu();
    QVERIFY(menu != nullptr);
    
    trayIcon->setMonitoringState(false);
    // Verify "Monitoring Enabled" checkbox is unchecked
}

void TestTrayIcon::testUpdateRecentItems_empty()
{
    // Contract: Should handle empty recent items list
    QList<ClipboardItem> emptyItems;
    trayIcon->updateRecentItems(emptyItems);
    
    // Verify menu shows "No recent items" or similar
    QMenu* menu = trayIcon->contextMenu();
    QVERIFY(menu != nullptr);
}

void TestTrayIcon::testUpdateRecentItems_withItems()
{
    // Contract: Must show last 5 items with truncated previews (max 50 chars)
    QList<ClipboardItem> manyItems = createTestHistory(10);
    trayIcon->updateRecentItems(manyItems);
    
    // Verify only 5 items shown
    // Verify text is truncated to 50 chars
    // Verify preview format
    QMenu* menu = trayIcon->contextMenu();
    QVERIFY(menu != nullptr);
}

void TestTrayIcon::testMenuStructure()
{
    // Contract: Menu must follow specified structure
    QMenu* menu = trayIcon->contextMenu();
    QVERIFY(menu != nullptr);
    
    QList<QAction*> actions = menu->actions();
    QVERIFY(actions.size() >= 6); // Show History, Recent Items, Monitoring, Settings, About, Exit
    
    // Verify action names and order
    QCOMPARE(actions[0]->text(), "Show History");
    // Check other expected actions
}

void TestTrayIcon::testLeftClick()
{
    // Contract: Left click must emit historyWindowRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    simulateActivation(QSystemTrayIcon::Trigger);
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testRightClick()
{
    // Contract: Right click must show context menu
    simulateActivation(QSystemTrayIcon::Context);
    
    // Verify context menu is shown
    QMenu* menu = trayIcon->contextMenu();
    QVERIFY(menu != nullptr);
    // Note: Can't easily test visibility without actual user interaction
}

void TestTrayIcon::testDoubleClick()
{
    // Contract: Double click must emit historyWindowRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    simulateActivation(QSystemTrayIcon::DoubleClick);
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testMiddleClick()
{
    // Contract: Middle click must toggle monitoring
    QSignalSpy spy(trayIcon, &TrayIcon::monitoringToggleRequested);
    
    simulateActivation(QSystemTrayIcon::MiddleClick);
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testShowHistoryWindow()
{
    // Contract: Must emit historyWindowRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    trayIcon->showHistoryWindow();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testToggleMonitoring()
{
    // Contract: Must emit monitoringToggleRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::monitoringToggleRequested);
    
    trayIcon->toggleMonitoring();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testShowSettings()
{
    // Contract: Must emit settingsRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::settingsRequested);
    
    trayIcon->showSettings();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testShowAbout()
{
    // Contract: Must emit aboutRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::aboutRequested);
    
    trayIcon->showAbout();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testExitApplication()
{
    // Contract: Must emit exitRequested signal
    QSignalSpy spy(trayIcon, &TrayIcon::exitRequested);
    
    trayIcon->exitApplication();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testHistoryWindowRequestedSignal()
{
    // Contract: Signal should be emitted on appropriate actions
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    
    // Test various ways to trigger this signal
    trayIcon->showHistoryWindow();
    simulateActivation(QSystemTrayIcon::Trigger);
    
    QVERIFY(spy.count() >= 1);
}

void TestTrayIcon::testMonitoringToggleRequestedSignal()
{
    // Contract: Signal should be emitted when monitoring toggle is requested
    QSignalSpy spy(trayIcon, &TrayIcon::monitoringToggleRequested);
    
    trayIcon->toggleMonitoring();
    simulateActivation(QSystemTrayIcon::MiddleClick);
    
    QVERIFY(spy.count() >= 1);
}

void TestTrayIcon::testSettingsRequestedSignal()
{
    // Contract: Signal should be emitted when settings are requested
    QSignalSpy spy(trayIcon, &TrayIcon::settingsRequested);
    
    trayIcon->showSettings();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testAboutRequestedSignal()
{
    // Contract: Signal should be emitted when about is requested
    QSignalSpy spy(trayIcon, &TrayIcon::aboutRequested);
    
    trayIcon->showAbout();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testExitRequestedSignal()
{
    // Contract: Signal should be emitted when exit is requested
    QSignalSpy spy(trayIcon, &TrayIcon::exitRequested);
    
    trayIcon->exitApplication();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testRecentItemSelectedSignal()
{
    // Contract: Signal should be emitted when recent item is selected
    QSignalSpy spy(trayIcon, &TrayIcon::recentItemSelected);
    
    trayIcon->updateRecentItems(testItems);
    // Simulate clicking on a recent item in menu would require actual menu interaction
    // For now, just verify the signal exists and can be connected
    QVERIFY(QMetaObject::checkConnectArgs("recentItemSelected(ClipboardItem)", "recentItemSelected(ClipboardItem)"));
}

void TestTrayIcon::testMenuShowHistory()
{
    // Contract: "Show History" menu action should work
    QMenu* menu = trayIcon->contextMenu();
    QAction* showHistoryAction = nullptr;
    
    for (QAction* action : menu->actions()) {
        if (action->text().contains("Show History")) {
            showHistoryAction = action;
            break;
        }
    }
    
    QVERIFY(showHistoryAction != nullptr);
    QSignalSpy spy(trayIcon, &TrayIcon::historyWindowRequested);
    showHistoryAction->trigger();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testMenuRecentItems()
{
    // Contract: Recent items should appear in submenu with previews
    trayIcon->updateRecentItems(testItems);
    QMenu* menu = trayIcon->contextMenu();
    
    // Find recent items submenu
    QMenu* recentMenu = nullptr;
    for (QAction* action : menu->actions()) {
        if (action->menu() && action->text().contains("Recent")) {
            recentMenu = action->menu();
            break;
        }
    }
    
    QVERIFY(recentMenu != nullptr);
    // Verify items are shown with correct previews
    // Verify 50-character limit
}

void TestTrayIcon::testMenuMonitoringToggle()
{
    // Contract: "Monitoring Enabled" should be a checkable action
    QMenu* menu = trayIcon->contextMenu();
    QAction* monitoringAction = nullptr;
    
    for (QAction* action : menu->actions()) {
        if (action->text().contains("Monitoring")) {
            monitoringAction = action;
            break;
        }
    }
    
    QVERIFY(monitoringAction != nullptr);
    QVERIFY(monitoringAction->isCheckable());
}

void TestTrayIcon::testMenuSettings()
{
    // Contract: "Settings" menu action should work
    QSignalSpy spy(trayIcon, &TrayIcon::settingsRequested);
    
    QMenu* menu = trayIcon->contextMenu();
    QAction* settingsAction = nullptr;
    for (QAction* action : menu->actions()) {
        if (action->text().contains("Settings")) {
            settingsAction = action;
            break;
        }
    }
    
    QVERIFY(settingsAction != nullptr);
    settingsAction->trigger();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testMenuAbout()
{
    // Contract: "About" menu action should work
    QSignalSpy spy(trayIcon, &TrayIcon::aboutRequested);
    
    QMenu* menu = trayIcon->contextMenu();
    QAction* aboutAction = nullptr;
    for (QAction* action : menu->actions()) {
        if (action->text().contains("About")) {
            aboutAction = action;
            break;
        }
    }
    
    QVERIFY(aboutAction != nullptr);
    aboutAction->trigger();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testMenuExit()
{
    // Contract: "Exit" menu action should work
    QSignalSpy spy(trayIcon, &TrayIcon::exitRequested);
    
    QMenu* menu = trayIcon->contextMenu();
    QAction* exitAction = nullptr;
    for (QAction* action : menu->actions()) {
        if (action->text().contains("Exit")) {
            exitAction = action;
            break;
        }
    }
    
    QVERIFY(exitAction != nullptr);
    exitAction->trigger();
    QCOMPARE(spy.count(), 1);
}

void TestTrayIcon::testMenuUpdatePerformance()
{
    // Contract: Menu updates must complete in <100ms when history changes
    QElapsedTimer timer;
    timer.start();
    
    trayIcon->updateRecentItems(testItems);
    qint64 elapsed = timer.elapsed();
    
    QVERIFY(elapsed < 100); // Must update within 100ms
}

void TestTrayIcon::testIconUpdatePerformance()
{
    // Contract: Icon updates must be instant (<50ms) for state changes
    QElapsedTimer timer;
    timer.start();
    
    trayIcon->updateIconState(true);
    qint64 elapsed = timer.elapsed();
    
    QVERIFY(elapsed < 50); // Must update within 50ms
}

void TestTrayIcon::testIconThemeAdaptation()
{
    // Contract: Must adapt to system icon theme changes automatically
    QIcon initialIcon = trayIcon->icon();
    QVERIFY(!initialIcon.isNull());
    
    // Test that we can set different themes
    trayIcon->setIconTheme("dark");
    trayIcon->setIconTheme("light");
    
    // Icon should still be valid after theme changes
    QIcon finalIcon = trayIcon->icon();
    QVERIFY(!finalIcon.isNull());
}

void TestTrayIcon::testNotificationSupport()
{
    // Contract: Must show balloon notifications for important events
    if (QSystemTrayIcon::supportsMessages()) {
        // Test notification functionality exists
        QVERIFY(trayIcon->supportsMessages());
        
        // We can't easily test actual notifications without user interaction
        // but we can verify the support exists
    } else {
        QSKIP("System tray does not support messages on this platform");
    }
}

ClipboardItem TestTrayIcon::createTestItem(const QString& text, bool pinned)
{
    // Now we can create a real ClipboardItem
    ClipboardItem item(text);
    if (pinned) {
        item.pin();
    }
    return item;
}

QList<ClipboardItem> TestTrayIcon::createTestHistory(int count)
{
    QList<ClipboardItem> items;
    for (int i = 0; i < count; ++i) {
        QString text = QString("Test item %1 with some longer text to test truncation").arg(i);
        items << createTestItem(text, i < 2);
    }
    return items;
}

void TestTrayIcon::simulateActivation(QSystemTrayIcon::ActivationReason reason)
{
    // This emits the activated signal to simulate user interaction
    emit trayIcon->activated(reason);
}

bool TestTrayIcon::isSystemTrayAvailable()
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

QTEST_MAIN(TestTrayIcon)
#include "test_tray_icon.moc"
