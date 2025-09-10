# Clipboard History Manager Development Guidelines

Auto-generated from feature specifications. Last updated: 2025-09-11

## Active Technologies
- **Qt6 Widgets + Qt6 Core**: Modern C++ UI framework for cross-platform desktop apps
- **C++17/20**: Core language with modern features
- **QClipboard**: Native clipboard monitoring and integration
- **JSON**: Configuration and data persistence format
- **CMake**: Build system and project configuration
- **Qt Test**: Unit and integration testing framework

## Project Structure
```
src/
├── models/          # ClipboardItem, ClipboardHistory data structures
├── services/        # ClipboardManager core logic
├── ui/             # ClipboardWindow, TrayIcon UI components
├── cli/            # Command-line interface
└── lib/            # Shared utilities and helpers

tests/
├── contract/       # API contract validation tests
├── integration/    # System integration tests  
└── unit/          # Individual component tests
```

## Architecture Principles
- **Library-First**: Every feature implemented as standalone library
- **Signal/Slot**: Qt's event-driven communication between components
- **TDD Mandatory**: Tests written first, must fail, then implement
- **Modular Design**: ClipboardManager, ClipboardWindow, TrayIcon as separate libraries
- **Cross-Platform**: X11 and Wayland compatibility required

## Key Components

### ClipboardManager Library
- Monitors QClipboard::dataChanged() signals
- Manages history with configurable limits (10-100 items)
- Handles item pinning and persistence to JSON
- Emits signals for UI synchronization

### ClipboardWindow Library  
- Frameless popup window with keyboard navigation
- Displays history in vertical list (pinned items first)
- <200ms display time requirement
- Global hotkey integration (Meta+V)

### TrayIcon Library
- QSystemTrayIcon integration with context menu
- Recent items display (last 5 with 50-char previews)
- System theme icon adaptation
- Notification system for status updates

## Performance Requirements
- <50ms clipboard change detection and processing
- <200ms popup window display time  
- <10MB total memory usage
- <10ms history retrieval for 100 items

## Testing Strategy
- **Contract Tests**: Validate API boundaries and error conditions
- **Integration Tests**: Real clipboard, file system, Qt widgets
- **Performance Tests**: Memory usage, response times
- **TDD Order**: Contract → Integration → E2E → Unit

## Code Conventions
- Qt naming: CamelCase for classes, camelCase for methods
- C++ Core Guidelines compliance
- RAII for resource management
- const-correctness enforced
- Modern C++ features preferred (auto, range-for, smart pointers)

## Build Commands
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
make test
```

## Common Patterns
- Use QObject signals/slots for component communication
- QJsonDocument for configuration serialization
- QTimer for deferred processing (avoid UI blocking)
- Qt's implicit sharing for efficient string operations

## Recent Changes
- 001-linux-clipboard-history: Added Qt6 Widgets, ClipboardManager, ClipboardWindow, TrayIcon libraries

<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->