#include <QApplication>
#include <QTimer>
#include "src/models/configuration.h"
#include "src/ui/settings_dialog.h"
#include "src/ui/about_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create a configuration instance
    Configuration config;
    
    // Test Settings Dialog
    SettingsDialog settingsDialog(&config);
    settingsDialog.show();
    
    // Test About Dialog
    AboutDialog aboutDialog;
    aboutDialog.show();
    
    // Show for 10 seconds then quit
    QTimer::singleShot(10000, &app, &QApplication::quit);
    
    return app.exec();
}