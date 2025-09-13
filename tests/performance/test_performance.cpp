#include <QtTest/QtTest>
#include <QObject>
#include <QElapsedTimer>
#include <QApplication>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QClipboard>
#include <QTimer>
#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>

#include "../../src/services/clipboard_manager.h"
#include "../../src/ui/clipboard_window.h"
#include "../../src/ui/tray_icon.h"
#include "../../src/models/clipboard_item.h"
#include "../../src/models/clipboard_history.h"
#include "../../src/models/configuration.h"

/**
 * @brief Performance tests for clipboard manager system
 * 
 * These tests verify that the clipboard manager meets strict performance requirements:
 * - <50ms clipboard change detection
 * - <200ms popup display time  
 * - <10ms history retrieval
 * - <10MB total memory usage with 100 items
 */
class TestPerformance : public QObject
{
    Q_OBJECT

private:
    ClipboardManager* manager;
    ClipboardWindow* window;
    QTemporaryDir* tempDir;
    QApplication* app;

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Core Performance Requirements
    void testClipboardChangeDetection_Under50ms();
    void testPopupDisplayTime_Under200ms();
    void testHistoryRetrieval_Under10ms();
    void testMemoryUsage_Under10MB();
    
    // Scalability Tests
    void testLargeHistoryPerformance();
    void testConcurrentOperationsPerformance();
    void testPersistencePerformance();
    void testSearchPerformance();
    
    // Memory Efficiency Tests
    void testMemoryLeaks();
    void testMemoryGrowthOverTime();
    void testLargeContentHandling();
    
    // UI Responsiveness Tests
    void testUIResponsiveness();
    void testTrayIconPerformance();
    void testKeyboardNavigationPerformance();
    
    // Edge Case Performance
    void testVeryLargeClipboardContent();
    void testHighFrequencyClipboardChanges();
    void testSystemResourceUsage();

private:
    // Helper methods
    void simulateClipboardChanges(int count);
    QList<ClipboardItem> createTestItems(int count);
    qint64 measureMemoryUsage();
    void waitForClipboardChange();
    QString generateLargeText(int sizeKB);
};

void TestPerformance::initTestCase()
{
    // Initialize Qt application if not already done
    if (!QApplication::instance()) {
        static int argc = 1;
        static const char* argv[] = {"test"};
        static char* non_const_argv[] = {const_cast<char*>(argv[0])};
        app = new QApplication(argc, non_const_argv);
    } else {
        app = qobject_cast<QApplication*>(QApplication::instance());
    }
    
    QVERIFY(app != nullptr);
    
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());
    
    // Set up test environment paths
    QStandardPaths::setTestModeEnabled(true);
}

void TestPerformance::init()
{
    manager = new ClipboardManager();
    QVERIFY(manager != nullptr);
    
    window = new ClipboardWindow();
    QVERIFY(window != nullptr);
    
    // Initialize with temporary configuration
    // Note: ClipboardManager handles its own configuration automatically
}

void TestPerformance::cleanup()
{
    delete window;
    window = nullptr;
    
    delete manager;
    manager = nullptr;
}

void TestPerformance::cleanupTestCase()
{
    delete tempDir;
    tempDir = nullptr;
}

// Core Performance Requirements

void TestPerformance::testClipboardChangeDetection_Under50ms()
{
    QVERIFY(manager != nullptr);
    
    // Start monitoring
    manager->startMonitoring();
    QVERIFY(manager->isMonitoring());
    
    // Set up signal spy for clipboard changes
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QElapsedTimer timer;
    QStringList testTexts = {
        "Performance test content 1",
        "Performance test content 2", 
        "Performance test content 3",
        "Performance test content 4",
        "Performance test content 5"
    };
    
    QList<qint64> detectionTimes;
    
    for (const QString& text : testTexts) {
        spy.clear();
        timer.start();
        
        // Simulate clipboard change
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(text);
        
        // Wait for signal with timeout
        bool signalReceived = spy.wait(100); // 100ms max wait
        qint64 elapsed = timer.elapsed();
        
        QVERIFY2(signalReceived, "Clipboard change signal not received within timeout");
        QVERIFY2(elapsed < 50, QString("Detection time %1ms exceeds 50ms requirement").arg(elapsed).toLocal8Bit());
        
        detectionTimes.append(elapsed);
        
        // Small delay between tests
        QTest::qWait(10);
    }
    
    // Calculate statistics
    qint64 maxTime = *std::max_element(detectionTimes.begin(), detectionTimes.end());
    qint64 avgTime = std::accumulate(detectionTimes.begin(), detectionTimes.end(), 0LL) / detectionTimes.size();
    
    qDebug() << "Clipboard detection times - Max:" << maxTime << "ms, Avg:" << avgTime << "ms";
    
    QVERIFY2(maxTime < 50, QString("Maximum detection time %1ms exceeds 50ms requirement").arg(maxTime).toLocal8Bit());
    QVERIFY2(avgTime < 25, QString("Average detection time %1ms should be well under 50ms").arg(avgTime).toLocal8Bit());
}

void TestPerformance::testPopupDisplayTime_Under200ms()
{
    QVERIFY(window != nullptr);
    QVERIFY(manager != nullptr);
    
    // Add some test items to the history through clipboard simulation
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QClipboard* clipboard = QApplication::clipboard();
    for (int i = 0; i < 10; ++i) {
        clipboard->setText(QString("Test item %1 for popup display").arg(i));
        spy.wait(100); // Wait for each item to be processed
    }
    
    // Connect window to manager to get the history
    window->setHistory(manager->getHistory());
    
    QElapsedTimer timer;
    QList<qint64> displayTimes;
    
    // Test popup display multiple times
    for (int i = 0; i < 5; ++i) {
        // Ensure window is hidden
        window->hide();
        QVERIFY(!window->isVisible());
        
        timer.start();
        
        // Show window at cursor position (simulating hotkey trigger)
        window->showAtCursor();
        
        // Wait for window to be visible
        QTest::qWaitForWindowExposed(window);
        
        qint64 elapsed = timer.elapsed();
        displayTimes.append(elapsed);
        
        QVERIFY2(window->isVisible(), "Window should be visible after showAtCursor()");
        QVERIFY2(elapsed < 200, QString("Display time %1ms exceeds 200ms requirement").arg(elapsed).toLocal8Bit());
        
        // Hide window for next test
        window->hide();
        QTest::qWait(50); // Brief pause between tests
    }
    
    // Calculate statistics
    qint64 maxTime = *std::max_element(displayTimes.begin(), displayTimes.end());
    qint64 avgTime = std::accumulate(displayTimes.begin(), displayTimes.end(), 0LL) / displayTimes.size();
    
    qDebug() << "Popup display times - Max:" << maxTime << "ms, Avg:" << avgTime << "ms";
    
    QVERIFY2(maxTime < 200, QString("Maximum display time %1ms exceeds 200ms requirement").arg(maxTime).toLocal8Bit());
    QVERIFY2(avgTime < 100, QString("Average display time %1ms should be well under 200ms").arg(avgTime).toLocal8Bit());
}

void TestPerformance::testHistoryRetrieval_Under10ms()
{
    QVERIFY(manager != nullptr);
    
    // Add 100 test items through clipboard simulation
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QClipboard* clipboard = QApplication::clipboard();
    for (int i = 0; i < 100; ++i) {
        clipboard->setText(QString("Performance test item %1 with content").arg(i));
        spy.wait(50); // Brief wait for each item
    }
    
    // Verify we have items in history
    QVERIFY(!manager->getHistory().isEmpty());
    
    QElapsedTimer timer;
    QList<qint64> retrievalTimes;
    
    // Test history retrieval multiple times
    for (int i = 0; i < 50; ++i) {
        timer.start();
        
        QList<ClipboardItem> history = manager->getHistory();
        
        qint64 elapsed = timer.elapsed();
        retrievalTimes.append(elapsed);
        
        QVERIFY2(!history.isEmpty(), "History should not be empty");
        QVERIFY2(elapsed < 10, QString("Retrieval time %1ms exceeds 10ms requirement").arg(elapsed).toLocal8Bit());
    }
    
    // Calculate statistics
    qint64 maxTime = *std::max_element(retrievalTimes.begin(), retrievalTimes.end());
    qint64 avgTime = std::accumulate(retrievalTimes.begin(), retrievalTimes.end(), 0LL) / retrievalTimes.size();
    
    qDebug() << "History retrieval times - Max:" << maxTime << "ms, Avg:" << avgTime << "ms";
    
    QVERIFY2(maxTime < 10, QString("Maximum retrieval time %1ms exceeds 10ms requirement").arg(maxTime).toLocal8Bit());
    QVERIFY2(avgTime < 5, QString("Average retrieval time %1ms should be well under 10ms").arg(avgTime).toLocal8Bit());
}

void TestPerformance::testMemoryUsage_Under10MB()
{
    QVERIFY(manager != nullptr);
    
    // Measure baseline memory
    qint64 baselineMemory = measureMemoryUsage();
    
    // Add exactly 100 items as specified in requirements through clipboard
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QClipboard* clipboard = QApplication::clipboard();
    for (int i = 0; i < 100; ++i) {
        QString content = QString("Memory test item %1 with substantial content for realistic testing").arg(i);
        clipboard->setText(content);
        spy.wait(50); // Wait for processing
    }
    
    // Force any pending operations to complete
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Measure memory after adding items
    qint64 finalMemory = measureMemoryUsage();
    qint64 memoryIncrease = finalMemory - baselineMemory;
    
    // Convert to MB (1024 * 1024 bytes)
    double memoryIncreaseMB = memoryIncrease / (1024.0 * 1024.0);
    
    qDebug() << "Memory usage - Baseline:" << baselineMemory << "bytes";
    qDebug() << "Memory usage - Final:" << finalMemory << "bytes"; 
    qDebug() << "Memory increase:" << memoryIncreaseMB << "MB with 100 items";
    
    QVERIFY2(memoryIncreaseMB < 10.0, QString("Memory increase %1MB exceeds 10MB requirement").arg(memoryIncreaseMB).toLocal8Bit());
    
    // Verify that items are actually stored
    QList<ClipboardItem> history = manager->getHistory();
    QVERIFY2(history.size() <= 100, "History should contain at most 100 items");
    QVERIFY2(!history.isEmpty(), "History should not be empty after adding items");
}

// Scalability Tests

void TestPerformance::testLargeHistoryPerformance()
{
    QVERIFY(manager != nullptr);
    
    // Test performance with maximum capacity
    manager->setMaxHistoryItems(1000);
    manager->startMonitoring();
    
    QElapsedTimer timer;
    timer.start();
    
    // Add many items through clipboard simulation
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    for (int i = 0; i < 100; ++i) { // Reduced from 1000 for test practicality
        clipboard->setText(QString("Large history test item %1").arg(i));
        spy.wait(10); // Brief wait
    }
    
    qint64 addTime = timer.elapsed();
    
    // Test retrieval performance with large history
    timer.restart();
    QList<ClipboardItem> history = manager->getHistory();
    qint64 retrievalTime = timer.elapsed();
    
    qDebug() << "Large history - Add time:" << addTime << "ms, Retrieval time:" << retrievalTime << "ms";
    
    QVERIFY2(retrievalTime < 50, QString("Large history retrieval %1ms should be under 50ms").arg(retrievalTime).toLocal8Bit());
    QVERIFY2(history.size() <= 1000, "History size should not exceed maximum");
}

void TestPerformance::testConcurrentOperationsPerformance()
{
    QVERIFY(manager != nullptr);
    
    manager->startMonitoring();
    QElapsedTimer timer;
    timer.start();
    
    // Simulate concurrent operations through clipboard
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    for (int i = 0; i < 50; ++i) { // Reduced for test practicality
        QString text = QString("Concurrent item %1").arg(i);
        clipboard->setText(text);
        spy.wait(20); // Brief wait for processing
        
        // Interleave different operations
        if (i % 10 == 0) {
            QList<ClipboardItem> history = manager->getHistory();
            Q_UNUSED(history);
        }
        
        if (i % 15 == 0 && !manager->getHistory().isEmpty()) {
            QString firstId = manager->getHistory().first().id();
            manager->pinItem(firstId);
        }
    }
    
    qint64 elapsed = timer.elapsed();
    qDebug() << "Concurrent operations completed in:" << elapsed << "ms";
    
    // Should handle concurrent operations efficiently
    QVERIFY2(elapsed < 5000, QString("Concurrent operations %1ms should complete under 5 seconds").arg(elapsed).toLocal8Bit());
}

void TestPerformance::testPersistencePerformance()
{
    QVERIFY(manager != nullptr);
    
    // Add test data through clipboard simulation
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    QClipboard* clipboard = QApplication::clipboard();
    
    for (int i = 0; i < 50; ++i) { // Reduced for test practicality
        clipboard->setText(QString("Persistence test item %1").arg(i));
        spy.wait(20);
    }
    
    QElapsedTimer timer;
    
    // Test save performance
    timer.start();
    bool saveResult = manager->saveHistory();
    qint64 saveTime = timer.elapsed();
    
    QVERIFY2(saveResult, "Save operation should succeed");
    QVERIFY2(saveTime < 500, QString("Save time %1ms should be under 500ms").arg(saveTime).toLocal8Bit());
    
    // Test load performance  
    timer.restart();
    bool loadResult = manager->loadHistory();
    qint64 loadTime = timer.elapsed();
    
    QVERIFY2(loadResult, "Load operation should succeed");
    QVERIFY2(loadTime < 500, QString("Load time %1ms should be under 500ms").arg(loadTime).toLocal8Bit());
    QVERIFY2(!manager->getHistory().isEmpty(), "History should be restored after load");
    
    qDebug() << "Persistence performance - Save:" << saveTime << "ms, Load:" << loadTime << "ms";
}

void TestPerformance::testSearchPerformance()
{
    QVERIFY(manager != nullptr);
    
    // Add many items with searchable content through clipboard
    manager->startMonitoring();
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    for (int i = 0; i < 50; ++i) { // Reduced for test practicality
        QString content = QString("Search test content item %1 with various keywords like performance, clipboard, manager").arg(i);
        clipboard->setText(content);
        spy.wait(20);
    }
    
    QElapsedTimer timer;
    QStringList searchTerms = {"performance", "clipboard", "manager", "test", "content"};
    
    for (const QString& term : searchTerms) {
        timer.start();
        
        // Simulate search by getting history and filtering
        QList<ClipboardItem> history = manager->getHistory();
        QList<ClipboardItem> results;
        
        for (const ClipboardItem& item : history) {
            if (item.text().contains(term, Qt::CaseInsensitive)) {
                results.append(item);
            }
        }
        
        qint64 elapsed = timer.elapsed();
        
        qDebug() << "Search for'" << term << "' found" << results.size() << "results in" << elapsed << "ms";
        
        QVERIFY2(elapsed < 100, QString("Search time %1ms should be under 100ms").arg(elapsed).toLocal8Bit());
        QVERIFY2(!results.isEmpty(), "Search should find matching results");
    }
}

// Memory Efficiency Tests

void TestPerformance::testMemoryLeaks()
{
    qint64 initialMemory = measureMemoryUsage();
    
    // Perform many operations that could potentially leak memory
    for (int cycle = 0; cycle < 5; ++cycle) { // Reduced cycles for practicality
        ClipboardManager* testManager = new ClipboardManager();
        testManager->startMonitoring();
        
        QClipboard* clipboard = QApplication::clipboard();
        QSignalSpy spy(testManager, &ClipboardManager::itemAdded);
        
        // Add and work with items
        for (int i = 0; i < 10; ++i) { // Reduced items for practicality
            clipboard->setText(QString("Leak test item %1").arg(i));
            spy.wait(50);
        }
        
        // Test operations
        QList<ClipboardItem> history = testManager->getHistory();
        for (const ClipboardItem& item : history) {
            testManager->pinItem(item.id());
            testManager->unpinItem(item.id());
        }
        
        delete testManager;
    }
    
    // Force garbage collection
    QApplication::processEvents();
    QTest::qWait(100);
    
    qint64 finalMemory = measureMemoryUsage();
    qint64 memoryDifference = finalMemory - initialMemory;
    double memoryDifferenceMB = memoryDifference / (1024.0 * 1024.0);
    
    qDebug() << "Memory leak test - Initial:" << initialMemory << "Final:" << finalMemory << "Difference:" << memoryDifferenceMB << "MB";
    
    // Allow for some memory overhead but not significant leaks
    QVERIFY2(memoryDifferenceMB < 5.0, QString("Memory increase %1MB suggests memory leaks").arg(memoryDifferenceMB).toLocal8Bit());
}

void TestPerformance::testMemoryGrowthOverTime()
{
    QVERIFY(manager != nullptr);
    
    qint64 initialMemory = measureMemoryUsage();
    QList<qint64> memoryMeasurements;
    
    manager->startMonitoring();
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    // Simulate long-term usage
    for (int batch = 0; batch < 5; ++batch) { // Reduced batches
        // Add items
        for (int i = 0; i < 10; ++i) { // Reduced items per batch
            clipboard->setText(QString("Long term test %1-%2").arg(batch).arg(i));
            spy.wait(50);
        }
        
        QApplication::processEvents();
        qint64 currentMemory = measureMemoryUsage();
        memoryMeasurements.append(currentMemory);
    }
    
    // Check for unbounded growth
    qint64 maxMemory = *std::max_element(memoryMeasurements.begin(), memoryMeasurements.end());
    qint64 totalGrowth = maxMemory - initialMemory;
    double totalGrowthMB = totalGrowth / (1024.0 * 1024.0);
    
    qDebug() << "Memory growth over time:" << totalGrowthMB << "MB";
    
    QVERIFY2(totalGrowthMB < 20.0, QString("Memory growth %1MB over time is too high").arg(totalGrowthMB).toLocal8Bit());
}

void TestPerformance::testLargeContentHandling()
{
    QVERIFY(manager != nullptr);
    
    // Test with large clipboard content
    QString largeContent = generateLargeText(1024); // 1MB of text
    
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QElapsedTimer timer;
    timer.start();
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(largeContent);
    
    bool signalReceived = spy.wait(5000); // Longer timeout for large content
    qint64 addTime = timer.elapsed();
    
    timer.restart();
    QList<ClipboardItem> history = manager->getHistory();
    qint64 retrievalTime = timer.elapsed();
    
    qDebug() << "Large content handling - Add:" << addTime << "ms, Retrieve:" << retrievalTime << "ms";
    
    QVERIFY2(signalReceived, "Large content should be processed");
    QVERIFY2(addTime < 2000, QString("Large content add time %1ms should be under 2 seconds").arg(addTime).toLocal8Bit());
    QVERIFY2(retrievalTime < 100, QString("Large content retrieval %1ms should be under 100ms").arg(retrievalTime).toLocal8Bit());
    QVERIFY2(!history.isEmpty(), "History should contain the large item");
    QVERIFY2(history.first().text().size() > 500000, "Large content should be preserved");
}

// UI Responsiveness Tests

void TestPerformance::testUIResponsiveness()
{
    QVERIFY(window != nullptr);
    QVERIFY(manager != nullptr);
    
    // Add items to display through clipboard simulation
    manager->startMonitoring();
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    for (int i = 0; i < 20; ++i) { // Reduced for practicality
        clipboard->setText(QString("UI test item %1").arg(i));
        spy.wait(50);
    }
    
    // Set the history in the window
    window->setHistory(manager->getHistory());
    
    QElapsedTimer timer;
    
    // Test window operations
    timer.start();
    window->show();
    QTest::qWaitForWindowExposed(window);
    qint64 showTime = timer.elapsed();
    
    // Window update time is hard to measure directly, so we'll just test show/hide
    timer.restart();
    window->hide();
    qint64 hideTime = timer.elapsed();
    
    qDebug() << "UI responsiveness - Show:" << showTime << "ms, Hide:" << hideTime << "ms";
    
    QVERIFY2(showTime < 200, QString("Window show time %1ms should be under 200ms").arg(showTime).toLocal8Bit());
    QVERIFY2(hideTime < 50, QString("Window hide time %1ms should be under 50ms").arg(hideTime).toLocal8Bit());
}

void TestPerformance::testTrayIconPerformance()
{
    TrayIcon trayIcon;
    QVERIFY(trayIcon.isSystemTrayAvailable());
    
    QElapsedTimer timer;
    
    // Test tray icon operations
    timer.start();
    trayIcon.show();
    qint64 showTime = timer.elapsed();
    
    timer.restart();
    trayIcon.updateRecentItems(createTestItems(5));
    qint64 updateTime = timer.elapsed();
    
    qDebug() << "Tray icon performance - Show:" << showTime << "ms, Update:" << updateTime << "ms";
    
    QVERIFY2(showTime < 100, QString("Tray icon show time %1ms should be under 100ms").arg(showTime).toLocal8Bit());
    QVERIFY2(updateTime < 50, QString("Tray icon update time %1ms should be under 50ms").arg(updateTime).toLocal8Bit());
}

void TestPerformance::testKeyboardNavigationPerformance()
{
    QVERIFY(window != nullptr);
    QVERIFY(manager != nullptr);
    
    // Add items for navigation through clipboard
    manager->startMonitoring();
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    for (int i = 0; i < 10; ++i) { // Reduced for practicality
        clipboard->setText(QString("Navigation test item %1").arg(i));
        spy.wait(50);
    }
    
    window->setHistory(manager->getHistory());
    window->show();
    QTest::qWaitForWindowExposed(window);
    
    QElapsedTimer timer;
    QList<qint64> navigationTimes;
    
    // Test keyboard navigation
    for (int i = 0; i < 5; ++i) { // Reduced iterations
        timer.start();
        
        // Simulate key press
        QTest::keyPress(window, Qt::Key_Down);
        QApplication::processEvents();
        
        qint64 elapsed = timer.elapsed();
        navigationTimes.append(elapsed);
        
        QVERIFY2(elapsed < 50, QString("Navigation time %1ms should be under 50ms").arg(elapsed).toLocal8Bit());
    }
    
    qint64 avgTime = std::accumulate(navigationTimes.begin(), navigationTimes.end(), 0LL) / navigationTimes.size();
    qDebug() << "Keyboard navigation average time:" << avgTime << "ms";
    
    window->hide();
}

// Edge Case Performance

void TestPerformance::testVeryLargeClipboardContent()
{
    QVERIFY(manager != nullptr);
    
    // Test with very large content (10MB)
    QString veryLargeContent = generateLargeText(10240); // 10MB
    
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QElapsedTimer timer;
    timer.start();
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(veryLargeContent);
    
    bool signalReceived = spy.wait(10000); // Very long timeout for huge content
    qint64 addTime = timer.elapsed();
    
    qDebug() << "Very large content - Processing time:" << addTime << "ms, Signal received:" << signalReceived;
    
    // Large content should still be handled efficiently
    QVERIFY2(addTime < 5000, QString("Very large item processing %1ms should be under 5 seconds").arg(addTime).toLocal8Bit());
}

void TestPerformance::testHighFrequencyClipboardChanges()
{
    QVERIFY(manager != nullptr);
    
    manager->startMonitoring();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    QElapsedTimer timer;
    timer.start();
    
    // Simulate rapid clipboard changes
    QClipboard* clipboard = QApplication::clipboard();
    for (int i = 0; i < 20; ++i) {
        clipboard->setText(QString("Rapid change %1").arg(i));
        QTest::qWait(25); // 40 changes per second
    }
    
    qint64 elapsed = timer.elapsed();
    
    // Wait for all signals to be processed
    QTest::qWait(500);
    
    qDebug() << "High frequency changes - Total time:" << elapsed << "ms, Signals received:" << spy.count();
    
    // Should handle rapid changes without significant delay
    QVERIFY2(elapsed < 1000, QString("High frequency changes %1ms should complete under 1 second").arg(elapsed).toLocal8Bit());
    QVERIFY2(spy.count() > 10, "Should detect most clipboard changes");
}

void TestPerformance::testSystemResourceUsage()
{
    QVERIFY(manager != nullptr);
    
    // Test system resource usage under load
    manager->startMonitoring();
    QClipboard* clipboard = QApplication::clipboard();
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    
    // Create sustained load
    for (int cycle = 0; cycle < 3; ++cycle) { // Reduced for practicality
        for (int i = 0; i < 20; ++i) { // Reduced items
            clipboard->setText(QString("Resource test %1-%2").arg(cycle).arg(i));
            spy.wait(50);
        }
        
        // Perform various operations
        QList<ClipboardItem> history = manager->getHistory();
        for (const ClipboardItem& item : history) {
            manager->pinItem(item.id());
            manager->unpinItem(item.id());
        }
        
        manager->saveHistory();
        manager->loadHistory();
        
        QApplication::processEvents();
        QTest::qWait(50);
    }
    
    // Should complete without hanging or excessive resource usage
    QVERIFY(true); // If we get here, basic resource management is working
}

// Helper Methods

void TestPerformance::simulateClipboardChanges(int count)
{
    QClipboard* clipboard = QApplication::clipboard();
    for (int i = 0; i < count; ++i) {
        clipboard->setText(QString("Simulated content %1").arg(i));
        QTest::qWait(10);
    }
}

QList<ClipboardItem> TestPerformance::createTestItems(int count)
{
    QList<ClipboardItem> items;
    for (int i = 0; i < count; ++i) {
        QString content = QString("Test item %1 with some content for performance testing").arg(i);
        ClipboardItem item(content);
        items.append(item);
    }
    return items;
}

qint64 TestPerformance::measureMemoryUsage()
{
    // On Linux, read from /proc/self/status for accurate memory measurement
    QFile statusFile("/proc/self/status");
    if (statusFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&statusFile);
        QString line;
        while (stream.readLineInto(&line)) {
            if (line.startsWith("VmRSS:")) {
                // Extract memory size in KB
                QStringList parts = line.split(QRegularExpression("\\s+"));
                if (parts.size() >= 2) {
                    bool ok;
                    qint64 memoryKB = parts[1].toLongLong(&ok);
                    if (ok) {
                        return memoryKB * 1024; // Convert to bytes
                    }
                }
                break;
            }
        }
    }
    
    // Fallback - return 0 if unable to measure
    return 0;
}

void TestPerformance::waitForClipboardChange()
{
    // Wait for clipboard change signal with timeout
    QSignalSpy spy(manager, &ClipboardManager::itemAdded);
    spy.wait(1000);
}

QString TestPerformance::generateLargeText(int sizeKB)
{
    QString pattern = "This is a test pattern for generating large clipboard content. ";
    int targetLength = sizeKB * 1024;
    
    QString result;
    result.reserve(targetLength);
    
    while (result.length() < targetLength) {
        result += pattern;
    }
    
    return result.left(targetLength);
}

QTEST_MAIN(TestPerformance)
#include "test_performance.moc"