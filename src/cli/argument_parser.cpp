#include "argument_parser.h"
#include <QCoreApplication>
#include <QDebug>

#include "argument_parser.h"

ArgumentParser::ArgumentParser(QObject *parent)
    : QObject(parent)
    , m_historyLimit(-1)
    , m_verbose(false)
    , m_noTray(false)
    , m_testMode(false)
    , m_configPathOption({"c", "config-path"}, 
        "Set custom configuration directory path", "path")
    , m_historyLimitOption({"l", "history-limit"}, 
        "Set maximum number of history items (10-100)", "count")
    , m_hotkeyOption({"k", "hotkey"}, 
        "Set custom global hotkey (e.g., 'Ctrl+Alt+V')", "key")
    , m_verboseOption("verbose", 
        "Enable verbose output")
    , m_noTrayOption("no-tray", 
        "Disable system tray icon")
    , m_verifyClipboardOption("verify-clipboard", 
        "Test clipboard access and exit")
    , m_testHotkeyOption("test-hotkey", 
        "Test hotkey registration", "key")
    , m_testTrayOption("test-tray", 
        "Test system tray availability and exit")
    , m_listHotkeysOption("list-hotkeys", 
        "List available hotkey combinations and exit")
{
    setupParser();
}

ArgumentParser::~ArgumentParser()
{
}

void ArgumentParser::setupParser()
{
    m_parser.setApplicationDescription("Linux Clipboard History Manager");
    m_parser.addHelpOption();
    m_parser.addVersionOption();
    
    // Add all command line options
    m_parser.addOption(m_configPathOption);
    m_parser.addOption(m_historyLimitOption);
    m_parser.addOption(m_hotkeyOption);
    m_parser.addOption(m_verboseOption);
    m_parser.addOption(m_noTrayOption);
    m_parser.addOption(m_verifyClipboardOption);
    m_parser.addOption(m_testHotkeyOption);
    m_parser.addOption(m_testTrayOption);
    m_parser.addOption(m_listHotkeysOption);
}

bool ArgumentParser::parse(QApplication& app)
{
    try {
        m_parser.process(app);
        return validateArguments();
    } catch (const std::exception& e) {
        m_errorString = QString("Argument parsing failed: %1").arg(e.what());
        return false;
    }
}

bool ArgumentParser::parse(const QStringList& args)
{
    // Clear any previous state
    m_configPath.clear();
    m_historyLimit = -1;
    m_hotkey.clear();
    m_verbose = false;
    m_noTray = false;
    m_testMode = false;
    m_testHotkey.clear();
    m_errorString.clear();

    // QCommandLineParser expects the first argument to be the executable name
    QStringList argsWithExeName = args;
    if (argsWithExeName.isEmpty() || argsWithExeName.first().startsWith("-")) {
        argsWithExeName.prepend("test-app");
    }

    if (!m_parser.parse(argsWithExeName)) {
        m_errorString = m_parser.errorText();
        return false;
    }

    return validateArguments();
}

bool ArgumentParser::validateArguments()
{
    // Extract configuration values
    if (m_parser.isSet(m_configPathOption)) {
        m_configPath = m_parser.value(m_configPathOption);
    }

    if (m_parser.isSet(m_historyLimitOption)) {
        bool ok;
        m_historyLimit = m_parser.value(m_historyLimitOption).toInt(&ok);
        if (!ok) {
            m_errorString = "Invalid history limit: must be a number";
            return false;
        }
        if (m_historyLimit < 10 || m_historyLimit > 100) {
            m_errorString = "Invalid history limit: must be between 10 and 100";
            return false;
        }
    }

    if (m_parser.isSet(m_hotkeyOption)) {
        m_hotkey = m_parser.value(m_hotkeyOption);
    }

    // Extract boolean flags
    m_verbose = m_parser.isSet(m_verboseOption);
    m_noTray = m_parser.isSet(m_noTrayOption);

    // Check for test modes
    m_testMode = m_parser.isSet(m_verifyClipboardOption) || 
                 m_parser.isSet(m_testHotkeyOption) || 
                 m_parser.isSet(m_testTrayOption) || 
                 m_parser.isSet(m_listHotkeysOption);

    if (m_parser.isSet(m_testHotkeyOption)) {
        m_testHotkey = m_parser.value(m_testHotkeyOption);
    }

    return true;
}

QString ArgumentParser::getConfigPath() const
{
    return m_configPath;
}

int ArgumentParser::getHistoryLimit() const
{
    return m_historyLimit;
}

QString ArgumentParser::getHotkey() const
{
    return m_hotkey;
}

bool ArgumentParser::isVerbose() const
{
    return m_verbose;
}

bool ArgumentParser::isNoTray() const
{
    return m_noTray;
}

bool ArgumentParser::isTestMode() const
{
    return m_testMode;
}

QString ArgumentParser::getTestHotkey() const
{
    return m_testHotkey;
}

QString ArgumentParser::getErrorString() const
{
    return m_errorString;
}
