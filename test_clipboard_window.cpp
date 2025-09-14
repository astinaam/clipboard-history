#include <QApplication>
#include <QTimer>
#include "src/models/clipboard_item.h"
#include "src/ui/clipboard_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create a clipboard window instance
    ClipboardWindow window;
    
    // Create some test clipboard items
    QList<ClipboardItem> testItems;
    
    ClipboardItem item1("First test item for glass design window", QDateTime::currentDateTime().addSecs(-30));
    testItems.append(item1);
    
    ClipboardItem item2("Second item with some longer text to test the glass design and wrapping behavior in the modernized clipboard window", QDateTime::currentDateTime().addSecs(-20));
    testItems.append(item2);
    
    ClipboardItem item3("Third item: URL test https://example.com/some/path", QDateTime::currentDateTime().addSecs(-10));
    testItems.append(item3);
    
    ClipboardItem item4("Fourth item: Code snippet\nif (condition) {\n    doSomething();\n}", QDateTime::currentDateTime().addSecs(-5));
    testItems.append(item4);
    
    // Set the test items in the window
    window.setHistory(testItems);
    
    // Show the window at center
    window.showAtCenter();
    
    // Keep window open for 15 seconds to test the design
    QTimer::singleShot(15000, &app, &QApplication::quit);
    
    return app.exec();
}