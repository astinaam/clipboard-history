#include <QApplication>
#include <QSystemTrayIcon>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <memory>

// Include our components
#include "services/clipboard_manager.h"
#include "ui/clipboard_window.h"
#include "ui/tray_icon.h"
#include "models/configuration.h"

/**
 * @brief Main application class for Clipboard History Manager
 * 
 * Handles application lifecycle, component initialization, and coordination
 * between ClipboardManager, ClipboardWindow, and TrayIcon.
 */
class ClipboardHistoryApp
{
public:
    ClipboardHistoryApp() = default;
    ~ClipboardHistoryApp() = default;
    
    /**
     * @brief Initialize and start the application
     * @param argc Command line argument count
     * @param argv Command line arguments
     * @return Exit code (0 = success)
     */
    int run(int argc, char* argv[]);
    
private:
    /**
     * @brief Parse command line arguments
     * @param app QApplication instance
     * @return true if parsing succeeded and app should continue
     */
    bool parseCommandLine(QApplication& app);
    
    /**
     * @brief Initialize configuration and data directories
     * @return true if initialization succeeded
     */
    bool initializeConfiguration();
    
    /**
     * @brief Create and initialize core components
     * @return true if all components created successfully
     */
    bool initializeComponents();
    
    /**
     * @brief Connect signals between components
     */
    void connectComponents();
    
    /**
     * @brief Check system requirements and capabilities
     * @return true if system is compatible
     */
    bool checkSystemRequirements();
    
    /**
     * @brief Show help message and exit
     */
    void showHelp();
    
    /**
     * @brief Run system tests based on command line options
     * @return true if tests passed
     */
    bool runSystemTests();

private:
    // Core components
    std::unique_ptr<ClipboardManager> m_clipboardManager;
    std::unique_ptr<ClipboardWindow> m_clipboardWindow;
    std::unique_ptr<TrayIcon> m_trayIcon;
    std::unique_ptr<Configuration> m_configuration;
    
    // Command line options
    QCommandLineParser m_parser;
    QString m_configPath;
    int m_historyLimit = -1;
    bool m_verbose = false;
    bool m_noTray = false;
    QString m_customHotkey;
    bool m_testMode = false;
    
    // Application state
    bool m_componentsInitialized = false;
};

int ClipboardHistoryApp::run(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Clipboard History Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ClipboardHistory");
    app.setApplicationDisplayName("Clipboard History Manager");
    
    // Parse command line arguments
    if (!parseCommandLine(app)) {
        return 1;
    }
    
    // Check system requirements
    if (!checkSystemRequirements()) {
        return 1;
    }
    
    // Initialize configuration
    if (!initializeConfiguration()) {
        qCritical() << "Failed to initialize configuration";
        return 1;
    }
    
    // Run tests if requested
    if (m_testMode) {
        return runSystemTests() ? 0 : 1;
    }
    
    // Initialize core components
    if (!initializeComponents()) {
        qCritical() << "Failed to initialize components";
        return 1;
    }
    
    // Connect component signals
    connectComponents();
    
    if (m_verbose) {
        qDebug() << "Clipboard History Manager started successfully";
        qDebug() << "Configuration path:" << m_configPath;
        qDebug() << "History limit:" << m_configuration->maxHistoryItems();
        qDebug() << "System tray available:" << QSystemTrayIcon::isSystemTrayAvailable();
    }
    
    // Start the event loop
    return app.exec();
}

bool ClipboardHistoryApp::parseCommandLine(QApplication& app)
{
    m_parser.setApplicationDescription("Linux Clipboard History Manager");
    m_parser.addHelpOption();
    m_parser.addVersionOption();
    
    // Configuration options
    QCommandLineOption configPathOption({"c", "config-path"}, 
        "Set custom configuration directory path", "path");
    m_parser.addOption(configPathOption);
    
    QCommandLineOption historyLimitOption({"l", "history-limit"}, 
        "Set maximum number of history items (10-100)", "count");
    m_parser.addOption(historyLimitOption);
    
    QCommandLineOption hotkeyOption({"k", "hotkey"}, 
        "Set custom global hotkey (e.g., 'Ctrl+Alt+V')", "key");
    m_parser.addOption(hotkeyOption);
    
    // Display options
    QCommandLineOption verboseOption("verbose", 
        "Enable verbose output");
    m_parser.addOption(verboseOption);
    
    QCommandLineOption noTrayOption("no-tray", 
        "Disable system tray icon");
    m_parser.addOption(noTrayOption);
    
    // Test options
    QCommandLineOption verifyClipboardOption("verify-clipboard", 
        "Test clipboard access and exit");
    m_parser.addOption(verifyClipboardOption);
    
    QCommandLineOption testHotkeyOption("test-hotkey", 
        "Test hotkey registration", "key");
    m_parser.addOption(testHotkeyOption);
    
    QCommandLineOption testTrayOption("test-tray", 
        "Test system tray availability and exit");
    m_parser.addOption(testTrayOption);
    
    QCommandLineOption listHotkeysOption("list-hotkeys", 
        "List available hotkey combinations and exit");
    m_parser.addOption(listHotkeysOption);
    
    // Process arguments
    m_parser.process(app);
    
    // Extract values
    if (m_parser.isSet(configPathOption)) {
        m_configPath = m_parser.value(configPathOption);
    }
    
    if (m_parser.isSet(historyLimitOption)) {
        bool ok;
        m_historyLimit = m_parser.value(historyLimitOption).toInt(&ok);
        if (!ok || m_historyLimit < 10 || m_historyLimit > 100) {
            qCritical() << "Invalid history limit. Must be between 10 and 100.";
            return false;
        }
    }
    
    if (m_parser.isSet(hotkeyOption)) {
        m_customHotkey = m_parser.value(hotkeyOption);
    }
    
    m_verbose = m_parser.isSet(verboseOption);
    m_noTray = m_parser.isSet(noTrayOption);
    
    // Check for test modes
    m_testMode = m_parser.isSet(verifyClipboardOption) || 
                m_parser.isSet(testHotkeyOption) || 
                m_parser.isSet(testTrayOption) || 
                m_parser.isSet(listHotkeysOption);
    
    return true;
}

bool ClipboardHistoryApp::initializeConfiguration()
{
    // Determine configuration path
    if (m_configPath.isEmpty()) {
        m_configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) 
                      + "/clipboard-manager";
    }
    
    // Create configuration directory if it doesn't exist
    QDir configDir(m_configPath);
    if (!configDir.exists() && !configDir.mkpath(".")) {
        qCritical() << "Failed to create configuration directory:" << m_configPath;
        return false;
    }
    
    // Load configuration
    QString configFile = m_configPath + "/config.json";
    m_configuration = std::make_unique<Configuration>(configFile);
    
    if (QFile::exists(configFile)) {
        if (!m_configuration->load()) {
            qWarning() << "Failed to load configuration from" << configFile;
            qWarning() << "Using default configuration";
        }
    }
    
    // Apply command line overrides
    if (m_historyLimit > 0) {
        m_configuration->setMaxHistoryItems(m_historyLimit);
    }
    
    if (!m_customHotkey.isEmpty()) {
        m_configuration->setHotkey(m_customHotkey);
    }
    
    // Save configuration with any updates
    if (!m_configuration->save()) {
        qWarning() << "Failed to save configuration to" << configFile;
    }
    
    return true;
}

bool ClipboardHistoryApp::initializeComponents()
{
    try {
        // Create ClipboardManager
        m_clipboardManager = std::make_unique<ClipboardManager>();
        m_clipboardManager->setMaxHistoryItems(m_configuration->maxHistoryItems());
        
        // Load history (ClipboardManager handles the file path internally)
        m_clipboardManager->loadHistory();
        
        // Create ClipboardWindow
        m_clipboardWindow = std::make_unique<ClipboardWindow>();
        
        // Create TrayIcon if system tray is available and not disabled
        if (!m_noTray && QSystemTrayIcon::isSystemTrayAvailable()) {
            m_trayIcon = std::make_unique<TrayIcon>();
            m_trayIcon->show();
        } else if (!m_noTray) {
            qWarning() << "System tray not available, running without tray icon";
        }
        
        m_componentsInitialized = true;
        return true;
        
    } catch (const std::exception& e) {
        qCritical() << "Exception during component initialization:" << e.what();
        return false;
    }
}

void ClipboardHistoryApp::connectComponents()
{
    if (!m_componentsInitialized) {
        return;
    }
    
    // Connect ClipboardManager to ClipboardWindow
    // When history changes, update the window display
    QObject::connect(m_clipboardManager.get(), &ClipboardManager::historyChanged,
                     [this]() {
                         m_clipboardWindow->setHistory(m_clipboardManager->getHistory());
                     });
    
    // When an item is added, update the window
    QObject::connect(m_clipboardManager.get(), &ClipboardManager::itemAdded,
                     [this](const ClipboardItem& item) {
                         m_clipboardWindow->updateItem(item);
                     });
    
    // When user selects an item in the window, it gets copied to clipboard automatically
    QObject::connect(m_clipboardWindow.get(), &ClipboardWindow::itemSelected,
                     [this](const ClipboardItem& item) {
                         // The ClipboardManager will detect this clipboard change and handle it
                         QClipboard* clipboard = QApplication::clipboard();
                         clipboard->setText(item.text());
                         m_clipboardWindow->hideWindow();
                     });
    
    // Connect TrayIcon if available
    if (m_trayIcon) {
        // Show window when tray icon requests it
        QObject::connect(m_trayIcon.get(), &TrayIcon::historyWindowRequested,
                         [this]() {
                             m_clipboardWindow->showAtCursor();
                         });
        
        // Toggle monitoring (for now, just emit a debug message)
        QObject::connect(m_trayIcon.get(), &TrayIcon::monitoringToggleRequested,
                         [this]() {
                             if (m_verbose) {
                                 qDebug() << "Monitoring toggle requested";
                             }
                         });
        
        // Exit application
        QObject::connect(m_trayIcon.get(), &TrayIcon::exitRequested,
                         QApplication::instance(), &QApplication::quit);
        
        // Update tray with current history
        QObject::connect(m_clipboardManager.get(), &ClipboardManager::historyChanged,
                         [this]() {
                             m_trayIcon->updateRecentItems(m_clipboardManager->getHistory());
                         });
        
        // Handle recent item selection from tray
        QObject::connect(m_trayIcon.get(), &TrayIcon::recentItemSelected,
                         [this](const ClipboardItem& item) {
                             QClipboard* clipboard = QApplication::clipboard();
                             clipboard->setText(item.text());
                         });
    }
    
    // Start clipboard monitoring
    m_clipboardManager->startMonitoring();
    
    // Initial setup: populate tray with current history
    if (m_trayIcon) {
        m_trayIcon->updateRecentItems(m_clipboardManager->getHistory());
        m_trayIcon->setHistoryCount(m_clipboardManager->getHistory().size());
    }
}

bool ClipboardHistoryApp::checkSystemRequirements()
{
    // Check if system tray is available (warning, not error)
    if (!QSystemTrayIcon::isSystemTrayAvailable() && !m_noTray) {
        qWarning() << "System tray is not available on this system";
        qWarning() << "Application will run without tray icon";
        qWarning() << "Use --no-tray to suppress this warning";
    }
    
    return true;
}

bool ClipboardHistoryApp::runSystemTests()
{
    bool allTestsPassed = true;
    
    if (m_parser.isSet("verify-clipboard")) {
        qDebug() << "Testing clipboard access...";
        // This is a basic test - just try to access the clipboard
        QClipboard* clipboard = QApplication::clipboard();
        if (clipboard) {
            qDebug() << "Clipboard access: OK";
        } else {
            qCritical() << "Clipboard access: FAILED";
            allTestsPassed = false;
        }
    }
    
    if (m_parser.isSet("test-tray")) {
        qDebug() << "Testing system tray availability...";
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            qDebug() << "System tray: AVAILABLE";
        } else {
            qDebug() << "System tray: NOT AVAILABLE";
            // This is not necessarily a failure
        }
    }
    
    if (m_parser.isSet("test-hotkey")) {
        QString testKey = m_parser.value("test-hotkey");
        qDebug() << "Testing hotkey:" << testKey;
        // For now, just validate the format
        if (!testKey.isEmpty() && testKey.contains("+")) {
            qDebug() << "Hotkey format: OK";
        } else {
            qCritical() << "Hotkey format: INVALID";
            allTestsPassed = false;
        }
    }
    
    if (m_parser.isSet("list-hotkeys")) {
        qDebug() << "Available hotkey combinations:";
        qDebug() << "  Meta+V (default)";
        qDebug() << "  Ctrl+Alt+V";
        qDebug() << "  Ctrl+Shift+V";
        qDebug() << "  Alt+Shift+V";
        qDebug() << "  Super+V";
        // This is always successful
    }
    
    return allTestsPassed;
}

/**
 * @brief Application entry point
 */
int main(int argc, char* argv[])
{
    ClipboardHistoryApp app;
    return app.run(argc, argv);
}
