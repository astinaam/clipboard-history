#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <QObject>
#include <QCommandLineParser>
#include <QStringList>
#include <QApplication>

/**
 * @brief Command-line argument parser for clipboard history manager
 * 
 * This class encapsulates all command-line argument parsing logic,
 * providing a clean interface for accessing parsed options and values.
 */
class ArgumentParser : public QObject
{
    Q_OBJECT

public:
    explicit ArgumentParser(QObject* parent = nullptr);
    ~ArgumentParser();

    /**
     * @brief Parse command-line arguments
     * @param app QApplication instance for Qt's argument processing
     * @return true if parsing succeeded, false on error
     */
    bool parse(QApplication& app);

    /**
     * @brief Parse command-line arguments from a string list
     * @param args Command-line arguments as string list (for testing)
     * @return true if parsing succeeded, false on error
     */
    bool parse(const QStringList& args);

    // Configuration option getters
    QString getConfigPath() const;
    int getHistoryLimit() const;
    QString getHotkey() const;

    // Display option getters
    bool isVerbose() const;
    bool isNoTray() const;

    // Test mode getters
    bool isTestMode() const;
    QString getTestHotkey() const;

    // Error handling
    QString getErrorString() const;

private:
    void setupParser();
    bool validateArguments();

private:
    QCommandLineParser m_parser;
    
    // Parsed values
    QString m_configPath;
    int m_historyLimit;
    QString m_hotkey;
    bool m_verbose;
    bool m_noTray;
    bool m_testMode;
    QString m_testHotkey;
    QString m_errorString;

    // Command line options
    QCommandLineOption m_configPathOption;
    QCommandLineOption m_historyLimitOption;
    QCommandLineOption m_hotkeyOption;
    QCommandLineOption m_verboseOption;
    QCommandLineOption m_noTrayOption;
    QCommandLineOption m_verifyClipboardOption;
    QCommandLineOption m_testHotkeyOption;
    QCommandLineOption m_testTrayOption;
    QCommandLineOption m_listHotkeysOption;
};

#endif // ARGUMENT_PARSER_H
