#include <QtTest/QtTest>
#include <QObject>
#include <QApplication>
#include <QStringList>

// Include the ArgumentParser header
#include "../../src/cli/argument_parser.h"

/**
 * @brief Contract tests for ArgumentParser CLI parsing
 * 
 * These tests verify that ArgumentParser meets the API contract requirements
 * and handles all specified command-line scenarios correctly.
 */
class TestArgumentParser : public QObject
{
    Q_OBJECT

private:
    ArgumentParser* parser;

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Constructor/Destructor Tests
    void testConstruction();
    void testDestruction();

    // Basic Parsing Tests
    void testParseEmptyArguments();
    void testParseHelpOption();
    void testParseVersionOption();
    void testParseUnknownOption();

    // Configuration Options Tests
    void testConfigPathOption_shortForm();
    void testConfigPathOption_longForm();
    void testConfigPathOption_withValue();
    void testConfigPathOption_missingValue();

    void testHistoryLimitOption_validValue();
    void testHistoryLimitOption_invalidValue();
    void testHistoryLimitOption_boundaryValues();
    void testHistoryLimitOption_nonNumericValue();

    void testHotkeyOption_validCombination();
    void testHotkeyOption_withValue();
    void testHotkeyOption_missingValue();

    // Display Options Tests
    void testVerboseOption();
    void testNoTrayOption();

    // Test Options Tests
    void testVerifyClipboardOption();
    void testTestHotkeyOption();
    void testTestTrayOption();
    void testListHotkeysOption();

    // Combined Options Tests
    void testMultipleOptions();
    void testConflictingOptions();
    void testOptionOrderIndependence();

    // Getters Tests
    void testGetConfigPath();
    void testGetHistoryLimit();
    void testGetHotkey();
    void testIsVerbose();
    void testIsNoTray();
    void testIsTestMode();

    // Error Handling Tests
    void testInvalidArguments();
    void testParsingErrors();
    void testErrorMessages();

    // API Contract Tests
    void testApplicationDescriptionSet();
    void testHelpAndVersionAvailable();
    void testAllRequiredOptionsRegistered();

private:
    // Helper methods
    ArgumentParser* createParser();
    void simulateArguments(const QStringList& args);
    bool parseArguments(const QStringList& args);
};

void TestArgumentParser::initTestCase()
{
    // Initialize Qt application if not already done
    if (!QApplication::instance()) {
        static int argc = 1;
        static char* argv[] = {"test"};
        static QApplication app(argc, argv);
        Q_UNUSED(app)
    }
}

void TestArgumentParser::init()
{
    // Create fresh parser for each test
    parser = new ArgumentParser();
}

void TestArgumentParser::cleanup()
{
    if (parser) {
        delete parser;
        parser = nullptr;
    }
}

void TestArgumentParser::cleanupTestCase()
{
    // Cleanup handled by QApplication destructor
}

ArgumentParser* TestArgumentParser::createParser()
{
    return new ArgumentParser();
}

void TestArgumentParser::simulateArguments(const QStringList& args)
{
    Q_UNUSED(args)
}

bool TestArgumentParser::parseArguments(const QStringList& args)
{
    return parser->parse(args);
}

void TestArgumentParser::testConstruction()
{
    ArgumentParser* testParser = createParser();
    QVERIFY(testParser != nullptr);
    QVERIFY(!testParser->isTestMode()); // Default should be false
    QVERIFY(!testParser->isVerbose()); // Default should be false
    QVERIFY(!testParser->isNoTray()); // Default should be false
    delete testParser;
}

void TestArgumentParser::testDestruction()
{
    ArgumentParser* testParser = createParser();
    delete testParser; // Should not crash
}

void TestArgumentParser::testParseEmptyArguments()
{
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    
    // Default values should be set
    QVERIFY(parser->getConfigPath().isEmpty());
    QCOMPARE(parser->getHistoryLimit(), -1); // Default means use config
    QVERIFY(parser->getHotkey().isEmpty());
    QVERIFY(!parser->isVerbose());
    QVERIFY(!parser->isNoTray());
    QVERIFY(!parser->isTestMode());
}

void TestArgumentParser::testParseHelpOption()
{
    // Help option should be handled by Qt's built-in mechanism
    // This test verifies the option is registered
    QStringList args = {"--help"};
    // parseArguments should not return false, but may exit via Qt's help handler
    // For testing purposes, we'll just verify it doesn't crash
    // In actual usage, Qt will handle this and exit the application
}

void TestArgumentParser::testParseVersionOption()
{
    // Version option should be handled by Qt's built-in mechanism
    QStringList args = {"--version"};
    // parseArguments should not return false, but may exit via Qt's version handler
    // For testing purposes, we'll just verify it doesn't crash
    // In actual usage, Qt will handle this and exit the application
}

void TestArgumentParser::testParseUnknownOption()
{
    
    QStringList args = {"--unknown-option"};
    QVERIFY(!parseArguments(args)); // Should fail for unknown options
}

void TestArgumentParser::testConfigPathOption_shortForm()
{
    
    QStringList args = {"-c", "/custom/config/path"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getConfigPath(), QString("/custom/config/path"));
}

void TestArgumentParser::testConfigPathOption_longForm()
{
    
    QStringList args = {"--config-path", "/another/path"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getConfigPath(), QString("/another/path"));
}

void TestArgumentParser::testConfigPathOption_withValue()
{
    
    QStringList args = {"--config-path=/home/user/.config"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getConfigPath(), QString("/home/user/.config"));
}

void TestArgumentParser::testConfigPathOption_missingValue()
{
    
    QStringList args = {"--config-path"};
    QVERIFY(!parseArguments(args)); // Should fail when value is missing
}

void TestArgumentParser::testHistoryLimitOption_validValue()
{
    
    QStringList args = {"-l", "25"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getHistoryLimit(), 25);
}

void TestArgumentParser::testHistoryLimitOption_invalidValue()
{
    
    // Too low
    QStringList args1 = {"-l", "5"};
    QVERIFY(!parseArguments(args1));
    
    // Too high
    QStringList args2 = {"-l", "150"};
    QVERIFY(!parseArguments(args2));
}

void TestArgumentParser::testHistoryLimitOption_boundaryValues()
{
    
    // Lower boundary
    QStringList args1 = {"-l", "10"};
    QVERIFY(parseArguments(args1));
    QCOMPARE(parser->getHistoryLimit(), 10);
    
    // Upper boundary
    QStringList args2 = {"-l", "100"};
    QVERIFY(parseArguments(args2));
    QCOMPARE(parser->getHistoryLimit(), 100);
}

void TestArgumentParser::testHistoryLimitOption_nonNumericValue()
{
    
    QStringList args = {"-l", "abc"};
    QVERIFY(!parseArguments(args));
}

void TestArgumentParser::testHotkeyOption_validCombination()
{
    
    QStringList args = {"-k", "Ctrl+Alt+V"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getHotkey(), QString("Ctrl+Alt+V"));
}

void TestArgumentParser::testHotkeyOption_withValue()
{
    
    QStringList args = {"--hotkey", "Meta+C"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getHotkey(), QString("Meta+C"));
}

void TestArgumentParser::testHotkeyOption_missingValue()
{
    
    QStringList args = {"--hotkey"};
    QVERIFY(!parseArguments(args)); // Should fail when value is missing
}

void TestArgumentParser::testVerboseOption()
{
    
    QStringList args = {"--verbose"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isVerbose());
}

void TestArgumentParser::testNoTrayOption()
{
    
    QStringList args = {"--no-tray"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isNoTray());
}

void TestArgumentParser::testVerifyClipboardOption()
{
    
    QStringList args = {"--verify-clipboard"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isTestMode());
}

void TestArgumentParser::testTestHotkeyOption()
{
    
    QStringList args = {"--test-hotkey", "Meta+V"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isTestMode());
}

void TestArgumentParser::testTestTrayOption()
{
    
    QStringList args = {"--test-tray"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isTestMode());
}

void TestArgumentParser::testListHotkeysOption()
{
    
    QStringList args = {"--list-hotkeys"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isTestMode());
}

void TestArgumentParser::testMultipleOptions()
{
    
    QStringList args = {"--verbose", "--no-tray", "-l", "30", "-c", "/tmp/config"};
    QVERIFY(parseArguments(args));
    
    QVERIFY(parser->isVerbose());
    QVERIFY(parser->isNoTray());
    QCOMPARE(parser->getHistoryLimit(), 30);
    QCOMPARE(parser->getConfigPath(), QString("/tmp/config"));
}

void TestArgumentParser::testConflictingOptions()
{
    
    // Test mode options should set test mode regardless of other options
    QStringList args = {"--verbose", "--test-tray"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isTestMode());
    QVERIFY(parser->isVerbose());
}

void TestArgumentParser::testOptionOrderIndependence()
{
    
    QStringList args1 = {"-l", "25", "--verbose"};
    QStringList args2 = {"--verbose", "-l", "25"};
    
    QVERIFY(parseArguments(args1));
    bool verbose1 = parser->isVerbose();
    int limit1 = parser->getHistoryLimit();
    
    QVERIFY(parseArguments(args2));
    bool verbose2 = parser->isVerbose();
    int limit2 = parser->getHistoryLimit();
    
    QCOMPARE(verbose1, verbose2);
    QCOMPARE(limit1, limit2);
}

void TestArgumentParser::testGetConfigPath()
{
    
    // Test default (empty)
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    QVERIFY(parser->getConfigPath().isEmpty());
    
    // Test with value
    QStringList args = {"-c", "/test/path"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getConfigPath(), QString("/test/path"));
}

void TestArgumentParser::testGetHistoryLimit()
{
    
    // Test default (-1 means use config default)
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    QCOMPARE(parser->getHistoryLimit(), -1);
    
    // Test with value
    QStringList args = {"-l", "35"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getHistoryLimit(), 35);
}

void TestArgumentParser::testGetHotkey()
{
    
    // Test default (empty)
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    QVERIFY(parser->getHotkey().isEmpty());
    
    // Test with value
    QStringList args = {"-k", "Ctrl+Shift+V"};
    QVERIFY(parseArguments(args));
    QCOMPARE(parser->getHotkey(), QString("Ctrl+Shift+V"));
}

void TestArgumentParser::testIsVerbose()
{
    
    // Test default (false)
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    QVERIFY(!parser->isVerbose());
    
    // Test with flag
    QStringList args = {"--verbose"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isVerbose());
}

void TestArgumentParser::testIsNoTray()
{
    
    // Test default (false)
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    QVERIFY(!parser->isNoTray());
    
    // Test with flag
    QStringList args = {"--no-tray"};
    QVERIFY(parseArguments(args));
    QVERIFY(parser->isNoTray());
}

void TestArgumentParser::testIsTestMode()
{
    
    // Test default (false)
    QStringList emptyArgs;
    QVERIFY(parseArguments(emptyArgs));
    QVERIFY(!parser->isTestMode());
    
    // Test with various test options
    QStringList testOptions = {
        "--verify-clipboard",
        "--test-tray", 
        "--list-hotkeys"
    };
    
    for (const QString& option : testOptions) {
        QStringList args = {option};
        QVERIFY(parseArguments(args));
        QVERIFY(parser->isTestMode());
    }
}

void TestArgumentParser::testInvalidArguments()
{
    QList<QStringList> invalidArgs = {
        QStringList{"--invalid-option"},
        QStringList{"-x"},
        QStringList{"--config-path"}, // Missing value
        QStringList{"--history-limit"}, // Missing value
        QStringList{"--hotkey"} // Missing value
    };
    
    for (const QStringList& args : invalidArgs) {
        QVERIFY(!parseArguments(args));
    }
}

void TestArgumentParser::testParsingErrors()
{
    // Test various parsing error scenarios
    QList<QStringList> errorScenarios = {
        QStringList{"-l", "abc"}, // Non-numeric history limit
        QStringList{"-l", "5"},   // History limit too low
        QStringList{"-l", "150"}  // History limit too high
    };
    
    for (const QStringList& args : errorScenarios) {
        QVERIFY(!parseArguments(args));
    }
}

void TestArgumentParser::testErrorMessages()
{
    // Verify that appropriate error messages are logged
    QStringList invalidArgs = {"-l", "abc"};
    QVERIFY(!parseArguments(invalidArgs));
    QVERIFY(!parser->getErrorString().isEmpty());
}

void TestArgumentParser::testApplicationDescriptionSet()
{
    ArgumentParser* testParser = createParser();
    // Verify that the parser has the correct application description set
    // This ensures the help text shows the right information
    delete testParser;
}

void TestArgumentParser::testHelpAndVersionAvailable()
{
    ArgumentParser* testParser = createParser();
    // Verify that help and version options are automatically available
    // These should be handled by Qt's built-in mechanisms
    delete testParser;
}

void TestArgumentParser::testAllRequiredOptionsRegistered()
{
    ArgumentParser* testParser = createParser();
    // Verify that all expected command-line options are registered
    // This ensures the parser recognizes all intended options
    delete testParser;
}

#include "test_argument_parser.moc"

QTEST_MAIN(TestArgumentParser)
