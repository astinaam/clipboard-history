# Clipboard History Manager Development Guidelines

AI coding agent instructions for productive development in this Qt6/C++ clipboard manager codebase.

## Core Architecture

### Signal-Driven Component Communication
Components communicate exclusively via Qt signals/slots for loose coupling:
```cpp
// ClipboardManager emits signals that UI components connect to
connect(&m_history, &ClipboardHistory::itemAdded, this, &ClipboardManager::itemAdded);
connect(manager, &ClipboardManager::historyChanged, trayIcon, &TrayIcon::updateRecentItems);
```

### Library-First Design Pattern
Every feature is implemented as a standalone library with contract-tested APIs:
- `src/services/clipboard_manager.h` - Core clipboard monitoring and history management
- `src/ui/clipboard_window.h` - Frameless popup with keyboard navigation  
- `src/ui/tray_icon.h` - System tray integration with context menu
- `src/models/` - Data structures (ClipboardItem, ClipboardHistory, Configuration)

### Test-Driven Development (Mandatory)
All code follows strict TDD: contract tests → integration tests → implementation
```cpp
// Contract tests verify API boundaries in tests/contract/
class TestClipboardManager : public QObject {
    void testItemAddedSignal(); // Must use QSignalSpy for signal validation
};
```

## Build System Patterns

### CMake Dynamic Test Discovery
Tests are auto-discovered and built only if source files exist:
```cmake
function(create_test_executable test_file)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_file}")
        get_filename_component(test_name ${test_file} NAME_WE)
        add_executable(${test_name} ${test_file} ${LIB_SOURCES})
```

### Platform-Specific Integration
X11/Wayland support is conditionally compiled:
```cmake
if(X11_FOUND)
    target_link_libraries(clipboard-manager ${PLATFORM_LIBRARIES})
    target_compile_definitions(clipboard-manager PRIVATE HAVE_X11)
endif()
```

## Performance Contracts (Tested)
Critical timing requirements enforced by contract tests:
- `<50ms` clipboard change detection (`ClipboardManager::onClipboardChanged`)
- `<200ms` popup display time (`ClipboardWindow::showAtCursor`)  
- `<10MB` total memory usage with 100 items
- `<10ms` history retrieval (`ClipboardManager::getHistory`)

## Development Workflows

### Build and Test
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
make test  # Runs all contract, integration, and unit tests
```

### Test Categories by Directory
- `tests/contract/` - API contract validation (signal signatures, return values)
- `tests/integration/` - Real clipboard/filesystem/Qt widgets interaction
- `tests/unit/` - Individual component logic testing
- `tests/performance/` - Memory usage and timing validation

### Signal Testing Pattern
Always use QSignalSpy for Qt signal validation:
```cpp
QSignalSpy spy(manager, &ClipboardManager::itemAdded);
setClipboardText("test content");
waitForClipboardChange();
QVERIFY(spy.count() >= 1);
ClipboardItem emittedItem = spy.at(0).at(0).value<ClipboardItem>();
```

## Critical Implementation Patterns

### Clipboard Monitoring Loop
```cpp
void ClipboardManager::startMonitoring() {
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardManager::onClipboardChanged);
    m_monitoring = true;
    emit monitoringStateChanged(true);
}
```

### Deferred Persistence for Performance
```cpp
// Use QTimer for non-blocking saves to meet <50ms processing requirement
void ClipboardManager::scheduleSave() {
    m_saveTimer->setSingleShot(true);
    m_saveTimer->start(1000); // Defer saves by 1 second
}
```

### Frameless Window Positioning
```cpp
// ClipboardWindow appears at cursor with screen boundary detection
void ClipboardWindow::showAtCursor() {
    QPoint cursorPos = QCursor::pos();
    adjustPositionForScreenBounds(cursorPos);
    move(cursorPos);
    show();
}
```

## Project-Specific Conventions

### Qt Object Lifecycle Management
- Use parent-child relationships for automatic cleanup
- QPointer for optional references that may become invalid
- RAII with smart pointers only when Qt patterns insufficient

### Configuration and Persistence
- JSON serialization via QJsonDocument for all config/history data
- XDG-compliant config directory usage (`QStandardPaths::ConfigLocation`)
- Atomic file operations to prevent corruption during saves

### Error Handling via Signals
```cpp
// Errors propagated via signals, not exceptions
if (!m_history.addItem(item)) {
    emit error("Failed to add item to history");
    return false;
}
```

<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->