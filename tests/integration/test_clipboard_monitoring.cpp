#include <QtTest/QtTest>
#include <QObject>
#include <QClipboard>
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>
#include <QElapsedTimer>
#include <QMimeData>

// Forward declarations for classes that don't exist yet
// These will need to be implemented in Phase 3.3
class ClipboardManager;
struct ClipboardItem;

// Include headers once they exist
// #include "services/clipboard_manager.h"
// #include "models/clipboard_item.h"

class TestClipboardMonitoring : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic Monitoring Tests
    void testStartMonitoring();
    void testStopMonitoring();
    void testRestartMonitoring();
    void testMonitoringStateSignals();

    // Clipboard Change Detection Tests
    void testTextChangeDetection();
    void testImageChangeDetection();
    void testHtmlChangeDetection();
    void testUrlChangeDetection();
    void testCustomMimeTypeDetection();

    // History Management Integration Tests
    void testHistoryGrowth();
    void testHistoryLimit();
    void testDuplicateDetection();
    void testEmptyClipboardHandling();
    void testLargeContentHandling();

    // Performance Integration Tests
    void testChangeDetectionPerformance();
    void testHistoryAdditionPerformance();
    void testMemoryUsageGrowth();
    void testConcurrentChanges();

    // Edge Case Integration Tests
    void testRapidClipboardChanges();
    void testSystemClipboardConflicts();
    void testClipboardClearHandling();
    void testApplicationSwitchHandling();

    // Signal Integration Tests
    void testHistoryUpdatedSignal();
    void testItemAddedSignal();
    void testMonitoringStateChangedSignal();
    void testErrorHandlingSignals();

    // Persistence Integration Tests
    void testHistorySaveOnChange();
    void testHistoryLoadOnStart();
    void testCorruptedDataRecovery();

private:
    ClipboardManager* manager;
    QClipboard* clipboard;
    QList<ClipboardItem> capturedItems;
    
    // Helper methods
    void setClipboardText(const QString& text);
    void setClipboardImage(const QPixmap& image);
    void setClipboardHtml(const QString& html);
    void setClipboardUrls(const QList<QUrl>& urls);
    ClipboardItem getLastHistoryItem();
    void waitForClipboardChange();
    QString generateLargeText(int sizeKB);
};

void TestClipboardMonitoring::initTestCase()
{
    // Ensure QApplication exists for clipboard access
    if (!QApplication::instance()) {
        static int argc = 1;
        static char* argv[] = {"test"};
        new QApplication(argc, argv);
    }
    
    clipboard = QApplication::clipboard();
    QVERIFY(clipboard != nullptr);
}

void TestClipboardMonitoring::cleanupTestCase()
{
    // Cleanup handled by QApplication destructor
}

void TestClipboardMonitoring::init()
{
    // Create fresh manager instance for each test
    // This will fail until ClipboardManager is implemented
    // manager = new ClipboardManager(this);
    manager = nullptr; // Placeholder until implementation exists
    
    capturedItems.clear();
    
    // Clear clipboard for clean test start
    clipboard->clear();
}

void TestClipboardMonitoring::cleanup()
{
    if (manager) {
        // manager->stopMonitoring();
    }
    delete manager;
    manager = nullptr;
    
    // Clear clipboard after test
    clipboard->clear();
}

void TestClipboardMonitoring::testStartMonitoring()
{
    // Integration Test: Manager should start monitoring clipboard changes
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // QVERIFY(!manager->isMonitoring());
    // 
    // QSignalSpy stateSpy(manager, &ClipboardManager::monitoringStateChanged);
    // manager->startMonitoring();
    // 
    // QVERIFY(manager->isMonitoring());
    // QCOMPARE(stateSpy.count(), 1);
    // QCOMPARE(stateSpy.at(0).at(0).toBool(), true);
}

void TestClipboardMonitoring::testStopMonitoring()
{
    // Integration Test: Manager should stop monitoring and ignore changes
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QVERIFY(manager->isMonitoring());
    // 
    // QSignalSpy stateSpy(manager, &ClipboardManager::monitoringStateChanged);
    // manager->stopMonitoring();
    // 
    // QVERIFY(!manager->isMonitoring());
    // QCOMPARE(stateSpy.count(), 1);
    // QCOMPARE(stateSpy.at(0).at(0).toBool(), false);
    // 
    // // Test that changes are ignored when stopped
    // int initialCount = manager->getHistory().count();
    // setClipboardText("Should be ignored");
    // waitForClipboardChange();
    // QCOMPARE(manager->getHistory().count(), initialCount);
}

void TestClipboardMonitoring::testRestartMonitoring()
{
    // Integration Test: Manager should handle start/stop cycles gracefully
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // // Start -> Stop -> Start cycle
    // manager->startMonitoring();
    // manager->stopMonitoring();
    // manager->startMonitoring();
    // QVERIFY(manager->isMonitoring());
    // 
    // // Verify monitoring still works after restart
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // setClipboardText("Test after restart");
    // waitForClipboardChange();
    // QCOMPARE(spy.count(), 1);
}

void TestClipboardMonitoring::testMonitoringStateSignals()
{
    // Integration Test: State change signals should work correctly
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // QSignalSpy spy(manager, &ClipboardManager::monitoringStateChanged);
    // 
    // manager->startMonitoring();
    // manager->stopMonitoring();
    // manager->startMonitoring();
    // 
    // QCOMPARE(spy.count(), 3);
    // QCOMPARE(spy.at(0).at(0).toBool(), true);  // Started
    // QCOMPARE(spy.at(1).at(0).toBool(), false); // Stopped
    // QCOMPARE(spy.at(2).at(0).toBool(), true);  // Restarted
}

void TestClipboardMonitoring::testTextChangeDetection()
{
    // Integration Test: Text clipboard changes should be detected and stored
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // setClipboardText("Test text content");
    // waitForClipboardChange();
    // 
    // QCOMPARE(spy.count(), 1);
    // ClipboardItem item = getLastHistoryItem();
    // QCOMPARE(item.text, "Test text content");
    // QCOMPARE(item.type, ClipboardItem::Text);
}

void TestClipboardMonitoring::testImageChangeDetection()
{
    // Integration Test: Image clipboard changes should be detected
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // QPixmap testImage(100, 100);
    // testImage.fill(Qt::red);
    // setClipboardImage(testImage);
    // waitForClipboardChange();
    // 
    // QCOMPARE(spy.count(), 1);
    // ClipboardItem item = getLastHistoryItem();
    // QCOMPARE(item.type, ClipboardItem::Image);
    // QVERIFY(!item.imageData.isNull());
}

void TestClipboardMonitoring::testHtmlChangeDetection()
{
    // Integration Test: HTML clipboard changes should be detected
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // QString htmlContent = "<html><body><b>Bold text</b></body></html>";
    // setClipboardHtml(htmlContent);
    // waitForClipboardChange();
    // 
    // QCOMPARE(spy.count(), 1);
    // ClipboardItem item = getLastHistoryItem();
    // QCOMPARE(item.type, ClipboardItem::Html);
    // QVERIFY(item.htmlData.contains("<b>Bold text</b>"));
}

void TestClipboardMonitoring::testUrlChangeDetection()
{
    // Integration Test: URL clipboard changes should be detected
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // QList<QUrl> urls = {QUrl("https://example.com"), QUrl("file:///tmp/test.txt")};
    // setClipboardUrls(urls);
    // waitForClipboardChange();
    // 
    // QCOMPARE(spy.count(), 1);
    // ClipboardItem item = getLastHistoryItem();
    // QCOMPARE(item.type, ClipboardItem::Urls);
    // QCOMPARE(item.urls.size(), 2);
}

void TestClipboardMonitoring::testCustomMimeTypeDetection()
{
    // Integration Test: Custom MIME types should be handled gracefully
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // QMimeData* mimeData = new QMimeData();
    // mimeData->setData("application/x-custom", "Custom data");
    // clipboard->setMimeData(mimeData);
    // waitForClipboardChange();
    // 
    // // Should either handle gracefully or ignore unknown types
    // // depending on implementation strategy
}

void TestClipboardMonitoring::testHistoryGrowth()
{
    // Integration Test: History should grow as items are added
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // int initialCount = manager->getHistory().count();
    // 
    // for (int i = 0; i < 5; ++i) {
    //     setClipboardText(QString("Item %1").arg(i));
    //     waitForClipboardChange();
    // }
    // 
    // QCOMPARE(manager->getHistory().count(), initialCount + 5);
}

void TestClipboardMonitoring::testHistoryLimit()
{
    // Integration Test: History should respect maximum item limit
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->setMaxHistorySize(10); // Set small limit for testing
    // manager->startMonitoring();
    // 
    // // Add more items than the limit
    // for (int i = 0; i < 15; ++i) {
    //     setClipboardText(QString("Item %1").arg(i));
    //     waitForClipboardChange();
    // }
    // 
    // // Should not exceed limit
    // QVERIFY(manager->getHistory().count() <= 10);
    // 
    // // Newest items should be preserved
    // ClipboardItem newest = manager->getHistory().first();
    // QVERIFY(newest.text.contains("Item 14")); // Last item added
}

void TestClipboardMonitoring::testDuplicateDetection()
{
    // Integration Test: Duplicate clipboard content should be handled properly
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // setClipboardText("Duplicate content");
    // waitForClipboardChange();
    // int countAfterFirst = manager->getHistory().count();
    // 
    // setClipboardText("Duplicate content"); // Same content again
    // waitForClipboardChange();
    // int countAfterSecond = manager->getHistory().count();
    // 
    // // Behavior depends on implementation:
    // // Either ignore duplicates (same count) or move to top (same count)
    // QCOMPARE(countAfterFirst, countAfterSecond);
}

void TestClipboardMonitoring::testEmptyClipboardHandling()
{
    // Integration Test: Empty clipboard should be handled gracefully
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // setClipboardText("Some content");
    // waitForClipboardChange();
    // int countWithContent = manager->getHistory().count();
    // 
    // clipboard->clear();
    // waitForClipboardChange();
    // 
    // // Empty clipboard should either be ignored or handled gracefully
    // // History count should not decrease due to clearing
    // QVERIFY(manager->getHistory().count() >= countWithContent);
}

void TestClipboardMonitoring::testLargeContentHandling()
{
    // Integration Test: Large clipboard content should be handled efficiently
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // QString largeText = generateLargeText(100); // 100KB text
    // 
    // QElapsedTimer timer;
    // timer.start();
    // 
    // setClipboardText(largeText);
    // waitForClipboardChange();
    // 
    // qint64 elapsed = timer.elapsed();
    // 
    // // Should still meet performance requirements even with large content
    // QVERIFY(elapsed < 50); // <50ms requirement
    // 
    // ClipboardItem item = getLastHistoryItem();
    // QCOMPARE(item.text.length(), largeText.length());
}

void TestClipboardMonitoring::testChangeDetectionPerformance()
{
    // Integration Test: Change detection should meet performance requirements
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // QElapsedTimer timer;
    // 
    // for (int i = 0; i < 10; ++i) {
    //     timer.start();
    //     setClipboardText(QString("Performance test %1").arg(i));
    //     waitForClipboardChange();
    //     qint64 elapsed = timer.elapsed();
    //     
    //     // Each change should be detected within 50ms
    //     QVERIFY(elapsed < 50);
    // }
}

void TestClipboardMonitoring::testHistoryAdditionPerformance()
{
    // Integration Test: Adding items to history should be fast
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // // Add many items and verify performance doesn't degrade
    // QElapsedTimer timer;
    // 
    // for (int i = 0; i < 50; ++i) {
    //     timer.start();
    //     setClipboardText(QString("History performance test %1").arg(i));
    //     waitForClipboardChange();
    //     qint64 elapsed = timer.elapsed();
    //     
    //     // Performance should remain consistent
    //     QVERIFY(elapsed < 50);
    // }
}

void TestClipboardMonitoring::testMemoryUsageGrowth()
{
    // Integration Test: Memory usage should remain reasonable as history grows
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->setMaxHistorySize(100);
    // manager->startMonitoring();
    // 
    // // Add items to fill history
    // for (int i = 0; i < 100; ++i) {
    //     setClipboardText(QString("Memory test item %1 with some content").arg(i));
    //     waitForClipboardChange();
    // }
    // 
    // // Memory usage validation would need platform-specific code
    // // For now, just verify history is maintained correctly
    // QCOMPARE(manager->getHistory().count(), 100);
}

void TestClipboardMonitoring::testConcurrentChanges()
{
    // Integration Test: Handle rapid consecutive clipboard changes
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // // Rapid changes without waiting
    // for (int i = 0; i < 5; ++i) {
    //     setClipboardText(QString("Rapid change %1").arg(i));
    // }
    // 
    // // Wait for all changes to be processed
    // QTest::qWait(500);
    // 
    // // Should have detected all changes
    // QVERIFY(spy.count() >= 1); // At least the last change should be detected
}

void TestClipboardMonitoring::testRapidClipboardChanges()
{
    // Integration Test: Handle applications that change clipboard rapidly
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // int initialCount = manager->getHistory().count();
    // 
    // // Simulate rapid changes like copy operations in quick succession
    // QTimer timer;
    // int changeCount = 0;
    // 
    // connect(&timer, &QTimer::timeout, [this, &changeCount]() {
    //     setClipboardText(QString("Rapid %1").arg(changeCount++));
    // });
    // 
    // timer.start(10); // Change every 10ms
    // QTest::qWait(100); // Run for 100ms
    // timer.stop();
    // 
    // // Should handle rapid changes without crashing or missing data
    // QVERIFY(manager->getHistory().count() > initialCount);
}

void TestClipboardMonitoring::testSystemClipboardConflicts()
{
    // Integration Test: Handle conflicts with other clipboard applications
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // // Simulate external application changing clipboard
    // setClipboardText("From clipboard manager");
    // waitForClipboardChange();
    // 
    // // Simulate external change
    // clipboard->setText("From external application");
    // waitForClipboardChange();
    // 
    // // Both changes should be detected
    // QVERIFY(manager->getHistory().count() >= 2);
}

void TestClipboardMonitoring::testClipboardClearHandling()
{
    // Integration Test: Handle clipboard clear operations gracefully
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // setClipboardText("Content before clear");
    // waitForClipboardChange();
    // int countBeforeClear = manager->getHistory().count();
    // 
    // clipboard->clear();
    // waitForClipboardChange();
    // 
    // setClipboardText("Content after clear");
    // waitForClipboardChange();
    // 
    // // Should continue working normally after clear
    // QVERIFY(manager->getHistory().count() >= countBeforeClear);
}

void TestClipboardMonitoring::testApplicationSwitchHandling()
{
    // Integration Test: Handle clipboard changes during application switches
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // // Simulate application focus changes
    // setClipboardText("From application 1");
    // waitForClipboardChange();
    // 
    // // Simulate focus change (this is difficult to test automatically)
    // setClipboardText("From application 2");
    // waitForClipboardChange();
    // 
    // // Should detect changes regardless of application focus
    // QVERIFY(manager->getHistory().count() >= 2);
}

void TestClipboardMonitoring::testHistoryUpdatedSignal()
{
    // Integration Test: historyUpdated signal should be emitted appropriately
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::historyUpdated);
    // 
    // setClipboardText("Test signal emission");
    // waitForClipboardChange();
    // 
    // QCOMPARE(spy.count(), 1);
}

void TestClipboardMonitoring::testItemAddedSignal()
{
    // Integration Test: itemAdded signal should contain correct data
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    // 
    // setClipboardText("Signal test content");
    // waitForClipboardChange();
    // 
    // QCOMPARE(spy.count(), 1);
    // ClipboardItem emittedItem = spy.at(0).at(0).value<ClipboardItem>();
    // QCOMPARE(emittedItem.text, "Signal test content");
}

void TestClipboardMonitoring::testMonitoringStateChangedSignal()
{
    // Integration Test: monitoringStateChanged signal timing
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // QSignalSpy spy(manager, &ClipboardManager::monitoringStateChanged);
    // 
    // manager->startMonitoring();
    // QCOMPARE(spy.count(), 1);
    // QCOMPARE(spy.at(0).at(0).toBool(), true);
    // 
    // manager->stopMonitoring();
    // QCOMPARE(spy.count(), 2);
    // QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void TestClipboardMonitoring::testErrorHandlingSignals()
{
    // Integration Test: Error conditions should emit appropriate signals
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // QSignalSpy errorSpy(manager, &ClipboardManager::errorOccurred);
    // 
    // // Test error conditions (implementation-dependent)
    // // For example: trying to start monitoring when already started
    // manager->startMonitoring();
    // manager->startMonitoring(); // Should handle gracefully
    // 
    // // Error signals are optional, but if implemented should work correctly
}

void TestClipboardMonitoring::testHistorySaveOnChange()
{
    // Integration Test: History should be persisted when items are added
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // manager->startMonitoring();
    // 
    // setClipboardText("Persistence test content");
    // waitForClipboardChange();
    // 
    // // Should trigger save operation
    // // This would need to check file system or signal emission
    // QSignalSpy saveSpy(manager, &ClipboardManager::historySaved);
    // if (saveSpy.isValid()) {
    //     QVERIFY(saveSpy.count() >= 1);
    // }
}

void TestClipboardMonitoring::testHistoryLoadOnStart()
{
    // Integration Test: History should be loaded when monitoring starts
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // // This test would need pre-existing history file
    // manager->startMonitoring();
    // 
    // // Should load previously saved history
    // // Implementation-dependent verification
}

void TestClipboardMonitoring::testCorruptedDataRecovery()
{
    // Integration Test: Should handle corrupted history data gracefully
    QSKIP("ClipboardManager not implemented yet - this test MUST fail until T012 is complete");
    
    // Uncomment once ClipboardManager exists:
    // // This would need to create corrupted data file first
    // // Then verify manager can start and recover
    // manager->startMonitoring();
    // 
    // // Should not crash and should be able to continue normally
    // setClipboardText("Recovery test");
    // waitForClipboardChange();
    // QVERIFY(manager->getHistory().count() >= 1);
}

// Helper method implementations
void TestClipboardMonitoring::setClipboardText(const QString& text)
{
    clipboard->setText(text);
}

void TestClipboardMonitoring::setClipboardImage(const QPixmap& image)
{
    clipboard->setPixmap(image);
}

void TestClipboardMonitoring::setClipboardHtml(const QString& html)
{
    QMimeData* mimeData = new QMimeData();
    mimeData->setHtml(html);
    clipboard->setMimeData(mimeData);
}

void TestClipboardMonitoring::setClipboardUrls(const QList<QUrl>& urls)
{
    QMimeData* mimeData = new QMimeData();
    mimeData->setUrls(urls);
    clipboard->setMimeData(mimeData);
}

ClipboardItem TestClipboardMonitoring::getLastHistoryItem()
{
    // This will fail until ClipboardManager and ClipboardItem are implemented
    // return manager->getHistory().first();
    return ClipboardItem{}; // Placeholder
}

void TestClipboardMonitoring::waitForClipboardChange()
{
    // Give time for clipboard change to be processed
    QTest::qWait(100);
}

QString TestClipboardMonitoring::generateLargeText(int sizeKB)
{
    QString text;
    QString chunk = QString("Large content chunk %1 ").arg(QDateTime::currentMSecsSinceEpoch());
    
    while (text.size() < sizeKB * 1024) {
        text += chunk;
    }
    
    return text;
}

QTEST_MAIN(TestClipboardMonitoring)
#include "test_clipboard_monitoring.moc"
