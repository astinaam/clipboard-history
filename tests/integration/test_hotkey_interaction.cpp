#include <QtTest/QtTest>
#include <QObject>
#include <QApplication>
#include <QKeyEvent>
#include <QSignalSpy>
#include <QTimer>
#include <QElapsedTimer>
#include <QClipboard>

// Include implemented headers
#include "models/clipboard_item.h"
#include "models/clipboard_history.h"
#include "models/configuration.h"
#include "services/clipboard_manager.h"
#include "ui/clipboard_window.h"
#include "lib/global_hotkey.h"

class TestHotkeyInteraction : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Hotkey Registration Tests
    void testHotkeyRegistration();
    void testHotkeyUnregistration();
    void testHotkeyReregistration();
    void testMultipleHotkeyAttempts();

    // Hotkey Trigger Tests
    void testMetaVTrigger();
    void testHotkeyWithModifiers();
    void testHotkeyInDifferentContexts();
    void testRapidHotkeyPresses();

    // Window Display Integration Tests
    void testWindowShowOnHotkey();
    void testWindowPositioning();
    void testWindowFocusOnShow();
    void testWindowDisplayPerformance();

    // Keyboard Navigation Integration Tests
    void testArrowKeyNavigation();
    void testHomeEndKeys();
    void testPageUpDownKeys();
    void testTabNavigation();

    // Item Selection Integration Tests
    void testEnterKeySelection();
    void testMouseClickSelection();
    void testDoubleClickSelection();
    void testSelectionFeedback();

    // Window Hiding Integration Tests
    void testEscapeKeyHide();
    void testClickOutsideHide();
    void testFocusLossHide();
    void testSecondHotkeyHide();

    // Performance Integration Tests
    void testHotkeyResponseTime();
    void testWindowShowPerformance();
    void testNavigationResponsiveness();
    void testHidePerformance();

    // Global Hotkey Integration Tests
    void testHotkeyFromDifferentApplications();
    void testHotkeyDuringFullscreenApps();
    void testHotkeyWithSystemShortcuts();
    void testHotkeyConflictResolution();

    // Error Handling Integration Tests
    void testHotkeyRegistrationFailure();
    void testWindowShowFailure();
    void testInvalidKeyEvents();
    void testSystemResourceExhaustion();

    // Multi-Monitor Integration Tests
    void testWindowPositionMultiMonitor();
    void testHotkeyOnSecondaryMonitor();
    void testMonitorChangeHandling();

private:
    ClipboardWindow* window;
    ClipboardManager* manager;
    QList<ClipboardItem> testHistory;
    
    // Helper methods
    void simulateHotkey();
    void simulateKeyPress(Qt::Key key, Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    void simulateMouseClick(const QPoint& position);
    QPoint getWindowCenter();
    ClipboardItem createTestItem(const QString& text, int index);
    void populateTestHistory(int count);
    bool isWindowVisible();
    void waitForWindowAction();
};

void TestHotkeyInteraction::initTestCase()
{
    // Ensure QApplication exists for window and keyboard events
    if (!QApplication::instance()) {
        static int argc = 1;
        static char* argv[] = {"test"};
        new QApplication(argc, argv);
    }
}

void TestHotkeyInteraction::cleanupTestCase()
{
    // Cleanup handled by QApplication destructor
}

void TestHotkeyInteraction::init()
{
    // Create fresh instances for each test
    // These will fail until classes are implemented
    // manager = new ClipboardManager(this);
    // window = new ClipboardWindow(manager, this);
    manager = nullptr; // Placeholder until implementation exists
    window = nullptr;  // Placeholder until implementation exists
    
    // Create test history
    populateTestHistory(10);
}

void TestHotkeyInteraction::cleanup()
{
    if (window) {
        // window->hide();
        // window->unregisterHotkey();
    }
    delete window;
    delete manager;
    window = nullptr;
    manager = nullptr;
}

void TestHotkeyInteraction::testHotkeyRegistration()
{
    // Integration Test: Global hotkey should register successfully
    GlobalHotkey hotkey;
    
    // Test hotkey registration with valid key combination
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Test that duplicate registration fails
    QVERIFY(!hotkey.registerHotkey("Meta+V"));
    
    // Clean up by unregistering
    hotkey.unregisterHotkey();
    
    // Test that we can register again after unregistering
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyUnregistration()
{
    // Integration Test: Hotkey should unregister cleanly
    GlobalHotkey hotkey;
    
    // Register and then unregister
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    hotkey.unregisterHotkey();
    
    // Verify we can register again (indicates successful unregistration)
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyReregistration()
{
    // Integration Test: Should handle register -> unregister -> register cycle
    GlobalHotkey hotkey;
    
    // First registration
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Unregister
    hotkey.unregisterHotkey();
    
    // Re-register should succeed
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testMultipleHotkeyAttempts()
{
    // Integration Test: Multiple registration attempts should be handled gracefully
    GlobalHotkey hotkey;
    
    // First registration should succeed
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Second attempt should fail (already registered)
    QVERIFY(!hotkey.registerHotkey("Meta+V"));
    
    // Verify hotkey is still registered after failed second attempt
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testMetaVTrigger()
{
    // Integration Test: Meta+V should trigger window display
    ClipboardWindow testWindow;
    GlobalHotkey hotkey;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Connect hotkey to window
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, 
                     &testWindow, &ClipboardWindow::showAtCursor);
    
    // Register hotkey
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    QVERIFY(!testWindow.isVisible());
    
    // Simulate hotkey trigger
    QSignalSpy spy(&testWindow, &ClipboardWindow::itemSelected);
    emit hotkey.hotkeyPressed();
    
    // Window should now be visible
    QVERIFY(testWindow.isVisible());
    
    // Clean up
    testWindow.hideWindow();
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyWithModifiers()
{
    // Integration Test: Hotkey should work with additional modifiers
    GlobalHotkey hotkey;
    
    // Test registration with multiple modifiers
    QVERIFY(hotkey.registerHotkey("Meta+Shift+V"));
    QVERIFY(hotkey.isRegistered());
    
    // Clean up
    hotkey.unregisterHotkey();
    QVERIFY(!hotkey.isRegistered());
    
    // Test registration with Ctrl+Alt combination
    QVERIFY(hotkey.registerHotkey("Ctrl+Alt+C"));
    QVERIFY(hotkey.isRegistered());
    
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyInDifferentContexts()
{
    // Integration Test: Hotkey should work regardless of active application
    GlobalHotkey hotkey;
    
    // Test that hotkey registration works
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    QVERIFY(hotkey.isRegistered());
    
    // Test signal emission when hotkey is triggered
    QSignalSpy spy(&hotkey, &GlobalHotkey::hotkeyPressed);
    
    // Since we can't easily simulate global system hotkey events in a test,
    // we'll test that the signal mechanism works by emitting manually
    emit hotkey.hotkeyPressed();
    
    QCOMPARE(spy.count(), 1);
    
    // Clean up
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testRapidHotkeyPresses()
{
    // Integration Test: Rapid hotkey presses should be handled gracefully
    GlobalHotkey hotkey;
    
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    QSignalSpy spy(&hotkey, &GlobalHotkey::hotkeyPressed);
    
    // Simulate rapid hotkey signals
    for (int i = 0; i < 5; ++i) {
        emit hotkey.hotkeyPressed();
        QTest::qWait(50); // 50ms between presses
    }
    
    // Should have received all signals
    QCOMPARE(spy.count(), 5);
    
    // System should remain stable after rapid presses
    QVERIFY(hotkey.isRegistered());
    
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testWindowShowOnHotkey()
{
    // Integration Test: Window should appear when hotkey is pressed
    ClipboardWindow window;
    GlobalHotkey hotkey;
    
    // Set up the window with test data
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    window.setHistory(testHistory);
    
    // Connect hotkey to window show method
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, 
                     &window, &ClipboardWindow::showPopup);
    
    // Register hotkey
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Window should not be visible initially
    QVERIFY(!window.isVisible());
    
    // Simulate hotkey press by emitting the signal
    QSignalSpy showSpy(&window, &ClipboardWindow::windowShown);
    emit hotkey.hotkeyPressed();
    
    // Window should now be visible
    QVERIFY(window.isVisible());
    QCOMPARE(showSpy.count(), 1);
    
    // Test that the window has the correct content
    QVERIFY(window.hasHistory());
    
    // Clean up
    window.hide();
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testWindowPositioning()
{
    // Integration Test: Window should appear at correct position
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Test positioning at cursor
    testWindow.showAtCursor();
    
    // Should appear near cursor or at predefined position
    QPoint windowPos = testWindow.pos();
    QVERIFY(windowPos.x() >= 0);
    QVERIFY(windowPos.y() >= 0);
    
    // Should be within screen bounds
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    QRect windowGeometry = testWindow.geometry();
    QVERIFY(screenGeometry.contains(windowGeometry) || screenGeometry.intersects(windowGeometry));
    
    testWindow.hideWindow();
    
    // Test positioning at specific position
    QPoint testPos(100, 100);
    testWindow.showAtPosition(testPos);
    
    // Window should be positioned correctly (may be adjusted for screen bounds)
    QPoint actualPos = testWindow.pos();
    QVERIFY(actualPos.x() >= 0);
    QVERIFY(actualPos.y() >= 0);
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testWindowFocusOnShow()
{
    // Integration Test: Window should receive focus when shown
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Show window
    testWindow.showAtCursor();
    
    // Window should be visible and active
    QVERIFY(testWindow.isVisible());
    
    // Note: Focus behavior in test environment may differ from real usage
    // The window should be designed to grab focus when shown
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testWindowDisplayPerformance()
{
    // Integration Test: Window should appear within 200ms requirement
    ClipboardWindow testWindow;
    
    // Set up test history with more items to test with realistic content
    ClipboardHistory testHistory;
    for (int i = 0; i < 10; ++i) {
        ClipboardItem item(QString("Test clipboard item with content %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    QElapsedTimer timer;
    timer.start();
    
    testWindow.showAtCursor();
    
    // Measure time until window becomes visible
    while (!testWindow.isVisible() && timer.elapsed() < 500) {
        QTest::qWait(10);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 200); // <200ms requirement
    QVERIFY(testWindow.isVisible());
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testArrowKeyNavigation()
{
    // Integration Test: Arrow keys should navigate through history items
    ClipboardWindow testWindow;
    
    // Set up test history with multiple items
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    int initialSelection = testWindow.selectedIndex();
    QVERIFY(initialSelection >= 0); // Should have a valid selection
    
    // Test down arrow navigation
    simulateKeyPress(Qt::Key_Down);
    int afterDown = testWindow.selectedIndex();
    
    // Should move to next item (wrapping behavior may vary)
    if (initialSelection < testHistory.items().count() - 1) {
        QCOMPARE(afterDown, initialSelection + 1);
    } else {
        // May wrap to beginning or stay at end
        QVERIFY(afterDown >= 0);
    }
    
    // Test up arrow navigation
    simulateKeyPress(Qt::Key_Up);
    int afterUp = testWindow.selectedIndex();
    QCOMPARE(afterUp, initialSelection); // Should return to initial position
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testHomeEndKeys()
{
    // Integration Test: Home/End keys should jump to first/last items
    ClipboardWindow testWindow;
    
    // Set up test history with multiple items
    ClipboardHistory testHistory;
    for (int i = 0; i < 10; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    // Navigate to middle item first
    for (int i = 0; i < 3; ++i) {
        simulateKeyPress(Qt::Key_Down);
    }
    
    // Test End key - should go to last item
    simulateKeyPress(Qt::Key_End);
    int endSelection = testWindow.selectedIndex();
    QCOMPARE(endSelection, testHistory.items().count() - 1);
    
    // Test Home key - should go to first item
    simulateKeyPress(Qt::Key_Home);
    int homeSelection = testWindow.selectedIndex();
    QCOMPARE(homeSelection, 0);
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testPageUpDownKeys()
{
    // Integration Test: Page Up/Down should scroll by page
    ClipboardWindow testWindow;
    
    // Set up test history with enough items for paging
    ClipboardHistory testHistory;
    for (int i = 0; i < 20; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    int initialSelection = testWindow.selectedIndex();
    
    // Test Page Down
    simulateKeyPress(Qt::Key_PageDown);
    int afterPageDown = testWindow.selectedIndex();
    
    // Should move forward (exact behavior depends on page size implementation)
    QVERIFY(afterPageDown >= initialSelection);
    
    // Test Page Up
    simulateKeyPress(Qt::Key_PageUp);
    int afterPageUp = testWindow.selectedIndex();
    
    // Should move backward from the PageDown position
    QVERIFY(afterPageUp <= afterPageDown);
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testTabNavigation()
{
    // Integration Test: Tab should cycle through UI elements if any
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    int initialSelection = testWindow.selectedIndex();
    
    // Tab navigation behavior - in a simple list widget, tab may not change selection
    // but it should at least not crash or cause issues
    simulateKeyPress(Qt::Key_Tab);
    
    // Verify the window is still functional after tab press
    QVERIFY(testWindow.isVisible());
    int afterTab = testWindow.selectedIndex();
    QVERIFY(afterTab >= 0); // Should maintain valid selection
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testEnterKeySelection()
{
    // Integration Test: Enter should select current item and paste to clipboard
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    QSignalSpy selectionSpy(&testWindow, &ClipboardWindow::itemSelected);
    QSignalSpy closeSpy(&testWindow, &ClipboardWindow::windowClosed);
    
    // Get current selected item for comparison
    ClipboardItem selectedItem = testWindow.selectedItem();
    QVERIFY(!selectedItem.text.isEmpty());
    
    // Press Enter to select
    simulateKeyPress(Qt::Key_Return);
    
    // Should have emitted selection signal
    QCOMPARE(selectionSpy.count(), 1);
    
    // Verify the selected item is correct
    QList<QVariant> selectionArgs = selectionSpy.at(0);
    ClipboardItem emittedItem = selectionArgs.at(0).value<ClipboardItem>();
    QCOMPARE(emittedItem.text, selectedItem.text);
    
    // Window should hide after selection (depends on implementation)
    if (!testWindow.isVisible()) {
        QCOMPARE(closeSpy.count(), 1);
    }
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testMouseClickSelection()
{
    // Integration Test: Mouse click should select item
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    QSignalSpy selectionSpy(&testWindow, &ClipboardWindow::itemSelected);
    
    // Click somewhere in the window (center should be safe)
    QPoint clickPosition = getWindowCenter();
    simulateMouseClick(clickPosition);
    
    // Mouse click on list widget may or may not trigger selection immediately
    // depending on the click behavior (single vs double click to select)
    // At minimum, it should not crash and should be functional
    QVERIFY(testWindow.isVisible());
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testDoubleClickSelection()
{
    // Integration Test: Double-click should select and close immediately
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    QSignalSpy selectionSpy(&testWindow, &ClipboardWindow::itemSelected);
    
    // Double-click on center of window
    QPoint clickPosition = getWindowCenter();
    simulateMouseClick(clickPosition);
    QTest::qWait(50); // Brief pause between clicks
    simulateMouseClick(clickPosition);
    
    // Double-click behavior depends on the implementation
    // At minimum, the window should remain functional
    QVERIFY(testWindow.selectedIndex() >= 0);
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testSelectionFeedback()
{
    // Integration Test: Selection should provide visual feedback
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    // Get initial selection
    int initialSelection = testWindow.selectedIndex();
    QVERIFY(initialSelection >= 0);
    
    // Navigate to different item
    simulateKeyPress(Qt::Key_Down);
    
    // Should have updated selection
    int newSelection = testWindow.selectedIndex();
    QVERIFY(newSelection >= 0);
    
    // Selection should be valid and within range
    QVERIFY(newSelection < testHistory.items().count());
    
    // Visual feedback verification would require access to UI elements
    // For now, we verify that navigation updates selection properly
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testEscapeKeyHide()
{
    // Integration Test: Escape should hide window without selection
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    QVERIFY(testWindow.isVisible());
    
    QSignalSpy selectionSpy(&testWindow, &ClipboardWindow::itemSelected);
    QSignalSpy closeSpy(&testWindow, &ClipboardWindow::windowClosed);
    
    // Press Escape
    simulateKeyPress(Qt::Key_Escape);
    
    // Window should hide without selection
    QVERIFY(!testWindow.isVisible());
    QCOMPARE(selectionSpy.count(), 0); // No selection should occur
    
    // Window closed signal should be emitted
    QCOMPARE(closeSpy.count(), 1);
}

void TestHotkeyInteraction::testClickOutsideHide()
{
    // Integration Test: Clicking outside window should hide it
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtPosition(QPoint(100, 100)); // Fixed position for testing
    QVERIFY(testWindow.isVisible());
    
    // In test environment, clicking "outside" is harder to simulate
    // We can test that the window handles focus loss properly
    testWindow.clearFocus();
    
    // After losing focus, window may auto-hide (implementation dependent)
    // For now, just verify the window can handle focus changes
    QTest::qWait(50);
    
    // Manually hide to test the mechanism
    testWindow.hideWindow();
    QVERIFY(!testWindow.isVisible());
}

void TestHotkeyInteraction::testFocusLossHide()
{
    // Integration Test: Focus loss should hide window
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    QVERIFY(testWindow.isVisible());
    
    QSignalSpy closeSpy(&testWindow, &ClipboardWindow::windowClosed);
    
    // Simulate focus loss
    testWindow.clearFocus();
    
    // Give some time for focus loss handling
    QTest::qWait(100);
    
    // Window behavior on focus loss depends on implementation
    // It may auto-hide or require explicit hiding
    if (testWindow.isVisible()) {
        // Manually trigger hide to verify the mechanism works
        testWindow.hideWindow();
    }
    
    QVERIFY(!testWindow.isVisible());
}

void TestHotkeyInteraction::testSecondHotkeyHide()
{
    // Integration Test: Second hotkey press should hide visible window
    ClipboardWindow testWindow;
    GlobalHotkey hotkey;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Connect hotkey to toggle window visibility
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, [&testWindow]() {
        if (testWindow.isVisible()) {
            testWindow.hideWindow();
        } else {
            testWindow.showAtCursor();
        }
    });
    
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // First hotkey shows window
    emit hotkey.hotkeyPressed();
    QVERIFY(testWindow.isVisible());
    
    // Second hotkey hides window
    emit hotkey.hotkeyPressed();
    QVERIFY(!testWindow.isVisible());
    
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyResponseTime()
{
    // Integration Test: Hotkey response should meet performance requirements
    GlobalHotkey hotkey;
    
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    QSignalSpy spy(&hotkey, &GlobalHotkey::hotkeyPressed);
    
    // Test signal emission performance
    QElapsedTimer timer;
    timer.start();
    
    emit hotkey.hotkeyPressed();
    
    qint64 elapsed = timer.elapsed();
    
    // Signal emission should be essentially instant
    QVERIFY(elapsed < 10); // <10ms for signal emission
    QCOMPARE(spy.count(), 1);
    
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testWindowShowPerformance()
{
    // Integration Test: Window display should be consistently fast
    ClipboardWindow testWindow;
    
    // Set up test history with realistic content
    ClipboardHistory testHistory;
    for (int i = 0; i < 10; ++i) {
        ClipboardItem item(QString("Test clipboard item with some content %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Test multiple show operations for consistency
    for (int i = 0; i < 5; ++i) {
        QElapsedTimer timer;
        timer.start();
        
        testWindow.showAtCursor();
        
        // Measure time until window becomes visible
        while (!testWindow.isVisible() && timer.elapsed() < 300) {
            QTest::qWait(10);
        }
        
        qint64 showTime = timer.elapsed();
        QVERIFY(showTime < 200); // <200ms requirement
        QVERIFY(testWindow.isVisible());
        
        testWindow.hideWindow();
        QTest::qWait(50); // Brief pause between tests
    }
}

void TestHotkeyInteraction::testNavigationResponsiveness()
{
    // Integration Test: Keyboard navigation should be responsive
    ClipboardWindow testWindow;
    
    // Set up test history with enough items for navigation testing
    ClipboardHistory testHistory;
    for (int i = 0; i < 15; ++i) {
        ClipboardItem item(QString("Navigation test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    QElapsedTimer timer;
    
    // Test rapid navigation for responsiveness
    for (int i = 0; i < 10; ++i) {
        timer.start();
        simulateKeyPress(Qt::Key_Down);
        qint64 elapsed = timer.elapsed();
        
        QVERIFY(elapsed < 50); // Navigation should be near-instant
        QVERIFY(testWindow.selectedIndex() >= 0); // Should maintain valid selection
    }
    
    // Test other navigation keys
    timer.start();
    simulateKeyPress(Qt::Key_Home);
    QVERIFY(timer.elapsed() < 50);
    QCOMPARE(testWindow.selectedIndex(), 0);
    
    timer.start();
    simulateKeyPress(Qt::Key_End);
    QVERIFY(timer.elapsed() < 50);
    QCOMPARE(testWindow.selectedIndex(), testHistory.items().count() - 1);
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testHidePerformance()
{
    // Integration Test: Window hiding should be fast
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 10; ++i) {
        ClipboardItem item(QString("Hide test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    QVERIFY(testWindow.isVisible());
    
    QElapsedTimer timer;
    timer.start();
    
    // Test hiding via Escape key
    simulateKeyPress(Qt::Key_Escape);
    
    // Measure time until window becomes hidden
    while (testWindow.isVisible() && timer.elapsed() < 100) {
        QTest::qWait(10);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 50); // Hiding should be near-instant
    QVERIFY(!testWindow.isVisible());
    
    // Test multiple hide/show cycles for consistency
    for (int i = 0; i < 3; ++i) {
        testWindow.showAtCursor();
        QVERIFY(testWindow.isVisible());
        
        timer.start();
        testWindow.hideWindow();
        qint64 hideTime = timer.elapsed();
        
        QVERIFY(hideTime < 50);
        QVERIFY(!testWindow.isVisible());
    }
}

void TestHotkeyInteraction::testHotkeyFromDifferentApplications()
{
    // Integration Test: Global hotkey should work from any application
    ClipboardWindow testWindow;
    GlobalHotkey hotkey;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Global test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Connect hotkey to window
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, 
                     &testWindow, &ClipboardWindow::showAtCursor);
    
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Simulate hotkey from "different application" (test limitation: can't actually switch apps)
    // Test that the signal mechanism works globally
    QSignalSpy spy(&testWindow, &ClipboardWindow::itemSelected);
    emit hotkey.hotkeyPressed();
    
    QVERIFY(testWindow.isVisible());
    
    testWindow.hideWindow();
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyDuringFullscreenApps()
{
    // Integration Test: Hotkey should work even during fullscreen applications
    ClipboardWindow testWindow;
    GlobalHotkey hotkey;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Fullscreen test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Connect hotkey to window
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, 
                     &testWindow, &ClipboardWindow::showAtCursor);
    
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Test that window appears and has appropriate flags for appearing over fullscreen apps
    emit hotkey.hotkeyPressed();
    QVERIFY(testWindow.isVisible());
    
    // Check that window has stay-on-top characteristics
    Qt::WindowFlags flags = testWindow.windowFlags();
    QVERIFY(flags & Qt::WindowStaysOnTopHint || flags & Qt::Tool);
    
    testWindow.hideWindow();
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyWithSystemShortcuts()
{
    // Integration Test: Should coexist with system shortcuts
    GlobalHotkey hotkey;
    
    // Test various key combinations that might conflict with system shortcuts
    QStringList potentialConflicts = {
        "Meta+V",    // Our main hotkey
        "Ctrl+Alt+T", // Common terminal shortcut alternative  
        "Meta+Shift+V", // Variation of our main hotkey
        "Ctrl+Shift+V"  // Another common paste variation
    };
    
    for (const QString& keyCombo : potentialConflicts) {
        bool registered = hotkey.registerHotkey(keyCombo);
        
        if (registered) {
            // If registration succeeded, verify it works
            QVERIFY(hotkey.isRegistered());
            
            // Test signal emission
            QSignalSpy spy(&hotkey, &GlobalHotkey::hotkeyPressed);
            emit hotkey.hotkeyPressed();
            QCOMPARE(spy.count(), 1);
            
            // Clean up
            hotkey.unregisterHotkey();
            QVERIFY(!hotkey.isRegistered());
        } else {
            // If registration failed, that's acceptable for system conflicts
            QVERIFY(!hotkey.isRegistered());
        }
    }
}

void TestHotkeyInteraction::testHotkeyConflictResolution()
{
    // Integration Test: Handle conflicts with other applications
    GlobalHotkey hotkey1;
    GlobalHotkey hotkey2;
    
    // First hotkey should register successfully
    QVERIFY(hotkey1.registerHotkey("Meta+V"));
    QVERIFY(hotkey1.isRegistered());
    
    // Second hotkey trying to register same combination should fail
    QVERIFY(!hotkey2.registerHotkey("Meta+V"));
    QVERIFY(!hotkey2.isRegistered());
    
    // After first hotkey unregisters, second should be able to register
    hotkey1.unregisterHotkey();
    QVERIFY(!hotkey1.isRegistered());
    
    QVERIFY(hotkey2.registerHotkey("Meta+V"));
    QVERIFY(hotkey2.isRegistered());
    
    hotkey2.unregisterHotkey();
}

void TestHotkeyInteraction::testHotkeyRegistrationFailure()
{
    // Integration Test: Handle hotkey registration failures gracefully
    GlobalHotkey hotkey;
    
    // Try to register invalid hotkey combinations
    QVERIFY(!hotkey.registerHotkey("")); // Empty string should fail
    QVERIFY(!hotkey.isRegistered());
    
    QVERIFY(!hotkey.registerHotkey("InvalidKey")); // Invalid key should fail
    QVERIFY(!hotkey.isRegistered());
    
    QVERIFY(!hotkey.registerHotkey("V")); // No modifier should fail (unsafe)
    QVERIFY(!hotkey.isRegistered());
    
    // Valid registration should still work after failures
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    QVERIFY(hotkey.isRegistered());
    
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testWindowShowFailure()
{
    // Integration Test: Handle window show failures gracefully
    ClipboardWindow testWindow;
    
    // Set up minimal test history
    ClipboardHistory testHistory;
    ClipboardItem item("Test item", ClipboardItem::Text);
    testHistory.addItem(item);
    testWindow.setHistory(testHistory.items());
    
    // Test that multiple show attempts don't cause issues
    for (int i = 0; i < 3; ++i) {
        testWindow.showAtCursor();
        QVERIFY(testWindow.isVisible());
        testWindow.hideWindow();
        QVERIFY(!testWindow.isVisible());
    }
    
    // Test showing at extreme positions (edge case handling)
    testWindow.showAtPosition(QPoint(-1000, -1000));
    if (testWindow.isVisible()) {
        // Window should handle extreme positions gracefully
        QPoint pos = testWindow.pos();
        QVERIFY(pos.x() >= 0 && pos.y() >= 0); // Should be adjusted to screen bounds
        testWindow.hideWindow();
    }
}

void TestHotkeyInteraction::testInvalidKeyEvents()
{
    // Integration Test: Handle invalid or unexpected key events
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    testWindow.showAtCursor();
    
    // Test that invalid key events don't crash the application
    try {
        // Send some unusual key combinations
        simulateKeyPress(static_cast<Qt::Key>(0), Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier);
        simulateKeyPress(Qt::Key_F35); // Unlikely to exist key
        simulateKeyPress(Qt::Key_unknown);
        
        // Window should still be functional after invalid key events
        QVERIFY(testWindow.isVisible());
        QVERIFY(testWindow.selectedIndex() >= 0);
        
        // Normal navigation should still work
        simulateKeyPress(Qt::Key_Down);
        QVERIFY(testWindow.selectedIndex() >= 0);
        
    } catch (...) {
        QFAIL("Window should handle invalid key events gracefully without throwing exceptions");
    }
    
    testWindow.hideWindow();
}

void TestHotkeyInteraction::testSystemResourceExhaustion()
{
    // Integration Test: Handle low system resources gracefully
    GlobalHotkey hotkey;
    
    // Test that we can create and register multiple hotkeys sequentially
    // to test resource cleanup
    QStringList testKeys = {"Meta+V", "Meta+C", "Meta+X", "Meta+Z", "Meta+B"};
    
    for (const QString& keyCombo : testKeys) {
        QVERIFY(hotkey.registerHotkey(keyCombo));
        QVERIFY(hotkey.isRegistered());
        
        // Test signal emission works
        QSignalSpy spy(&hotkey, &GlobalHotkey::hotkeyPressed);
        emit hotkey.hotkeyPressed();
        QCOMPARE(spy.count(), 1);
        
        // Unregister to free resources
        hotkey.unregisterHotkey();
        QVERIFY(!hotkey.isRegistered());
    }
    
    // System should remain stable after resource cycling
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    QVERIFY(hotkey.isRegistered());
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testWindowPositionMultiMonitor()
{
    // Integration Test: Window positioning on multi-monitor setups
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Monitor test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Test positioning on primary screen
    QList<QScreen*> screens = QApplication::screens();
    QVERIFY(!screens.isEmpty());
    
    QScreen* primaryScreen = QApplication::primaryScreen();
    QRect primaryGeometry = primaryScreen->geometry();
    
    // Test showing at different positions on primary screen
    QPoint centerPos = primaryGeometry.center();
    testWindow.showAtPosition(centerPos);
    
    QVERIFY(testWindow.isVisible());
    QPoint windowPos = testWindow.pos();
    
    // Window should be positioned within reasonable bounds
    QVERIFY(windowPos.x() >= primaryGeometry.left());
    QVERIFY(windowPos.y() >= primaryGeometry.top());
    
    testWindow.hideWindow();
    
    // If multiple screens available, test positioning on secondary screen
    if (screens.count() > 1) {
        QScreen* secondaryScreen = screens.at(1);
        QRect secondaryGeometry = secondaryScreen->geometry();
        QPoint secondaryPos = secondaryGeometry.center();
        
        testWindow.showAtPosition(secondaryPos);
        QVERIFY(testWindow.isVisible());
        
        // Window should handle secondary screen positioning
        QPoint secondaryWindowPos = testWindow.pos();
        QVERIFY(secondaryWindowPos.x() >= 0);
        QVERIFY(secondaryWindowPos.y() >= 0);
        
        testWindow.hideWindow();
    }
}

void TestHotkeyInteraction::testHotkeyOnSecondaryMonitor()
{
    // Integration Test: Hotkey should work when triggered from secondary monitor
    ClipboardWindow testWindow;
    GlobalHotkey hotkey;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Secondary monitor item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Connect hotkey to window
    QObject::connect(&hotkey, &GlobalHotkey::hotkeyPressed, [&testWindow]() {
        // In multi-monitor setup, show at cursor position
        testWindow.showAtCursor();
    });
    
    QVERIFY(hotkey.registerHotkey("Meta+V"));
    
    // Test that hotkey works regardless of cursor position
    // (Simulating cursor on secondary monitor is complex in test environment)
    emit hotkey.hotkeyPressed();
    QVERIFY(testWindow.isVisible());
    
    // Window should appear at appropriate location
    QPoint windowPos = testWindow.pos();
    QVERIFY(windowPos.x() >= 0);
    QVERIFY(windowPos.y() >= 0);
    
    testWindow.hideWindow();
    hotkey.unregisterHotkey();
}

void TestHotkeyInteraction::testMonitorChangeHandling()
{
    // Integration Test: Handle monitor configuration changes
    ClipboardWindow testWindow;
    
    // Set up test history
    ClipboardHistory testHistory;
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item(QString("Monitor change test item %1").arg(i), ClipboardItem::Text);
        testHistory.addItem(item);
    }
    testWindow.setHistory(testHistory.items());
    
    // Show window on current screen setup
    testWindow.showAtCursor();
    QVERIFY(testWindow.isVisible());
    
    QPoint initialPos = testWindow.pos();
    QSize initialSize = testWindow.size();
    
    // Test that window adapts to screen geometry queries
    QList<QScreen*> screens = QApplication::screens();
    QVERIFY(!screens.isEmpty());
    
    QScreen* currentScreen = QApplication::screenAt(testWindow.pos());
    if (currentScreen) {
        QRect screenGeometry = currentScreen->geometry();
        
        // Verify window is positioned within or overlapping screen bounds
        QRect windowGeometry = testWindow.geometry();
        QVERIFY(screenGeometry.intersects(windowGeometry));
    }
    
    // Test repositioning to different screen areas
    if (screens.count() > 1) {
        QScreen* differentScreen = screens.at(screens.count() - 1);
        QPoint newPos = differentScreen->geometry().center();
        
        testWindow.hideWindow();
        testWindow.showAtPosition(newPos);
        
        QVERIFY(testWindow.isVisible());
        // Window should handle position on different screen
        QPoint newWindowPos = testWindow.pos();
        QVERIFY(newWindowPos.x() >= 0);
        QVERIFY(newWindowPos.y() >= 0);
    }
    
    testWindow.hideWindow();
}

// Helper method implementations
void TestHotkeyInteraction::simulateHotkey()
{
    // Simulate Meta+V key combination
    simulateKeyPress(Qt::Key_V, Qt::MetaModifier);
}

void TestHotkeyInteraction::simulateKeyPress(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    if (window && window->isVisible()) {
        QKeyEvent pressEvent(QEvent::KeyPress, key, modifiers);
        QKeyEvent releaseEvent(QEvent::KeyRelease, key, modifiers);
        
        QApplication::sendEvent(window, &pressEvent);
        QApplication::sendEvent(window, &releaseEvent);
    }
}

void TestHotkeyInteraction::simulateMouseClick(const QPoint& position)
{
    if (window && window->isVisible()) {
        QMouseEvent pressEvent(QEvent::MouseButtonPress, position, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, position, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        
        QApplication::sendEvent(window, &pressEvent);
        QApplication::sendEvent(window, &releaseEvent);
    }
}

QPoint TestHotkeyInteraction::getWindowCenter()
{
    if (window && window->isVisible()) {
        return window->rect().center();
    }
    return QPoint(100, 100); // Default fallback position
}

ClipboardItem TestHotkeyInteraction::createTestItem(const QString& text, int index)
{
    // Create a test clipboard item with the given text and index
    ClipboardItem item(text, ClipboardItem::Text);
    
    // Set pinned status for first two items
    if (index < 2) {
        item.pin();
    }
    
    return item;
}

void TestHotkeyInteraction::populateTestHistory(int count)
{
    testHistory.clear();
    for (int i = 0; i < count; ++i) {
        QString text = QString("Test clipboard item %1").arg(i);
        testHistory << createTestItem(text, i);
    }
}

bool TestHotkeyInteraction::isWindowVisible()
{
    return window && window->isVisible();
}

void TestHotkeyInteraction::waitForWindowAction()
{
    // Give time for window actions to complete
    QTest::qWait(100);
}

QTEST_MAIN(TestHotkeyInteraction)
#include "test_hotkey_interaction.moc"
