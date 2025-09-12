#include "src/cli/argument_parser.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ArgumentParser parser;
    
    // Test the failing case
    QStringList args = {"--test-hotkey", "Meta+V"};
    qDebug() << "Testing args:" << args;
    
    bool success = parser.parse(args);
    qDebug() << "Parse success:" << success;
    qDebug() << "Test mode:" << parser.isTestMode();
    qDebug() << "Test hotkey:" << parser.getTestHotkey();
    qDebug() << "Error string:" << parser.getErrorString();
    
    // Test another case
    args = {"--test-tray"};
    qDebug() << "\nTesting args:" << args;
    
    success = parser.parse(args);
    qDebug() << "Parse success:" << success;
    qDebug() << "Test mode:" << parser.isTestMode();
    qDebug() << "Error string:" << parser.getErrorString();
    
    return 0;
}
