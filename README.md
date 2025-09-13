# Linux Clipboard History Manager

A lightweight, modern clipboard history manager for Linux with global hotkey support, system tray integration, and pinning functionality.

## Features

- **Cross-platform clipboard monitoring** (X11 and Wayland support)
- **Global hotkey** (Meta+V by default) to open clipboard history
- **System tray integration** with context menu
- **Pin items** to keep them at the top of the history
- **Configurable history limit** (default: 50 items)
- **Modern Qt6 interface** with frameless popup window
- **Minimal memory footprint** and fast performance
- **JSON-based configuration** and persistence
- **Command-line interface** for automation and scripting

## Requirements

### System Requirements
- Linux (X11 or Wayland)
- Qt6 (Core, Widgets, GUI)
- CMake 3.16 or later
- C++20 compatible compiler (GCC 10+ or Clang 12+)

### Dependencies
- **Qt6**: Core, Widgets, GUI, Test modules
- **X11 development libraries** (for X11 support):
  ```bash
  # Ubuntu/Debian
  sudo apt install libx11-dev libxtst-dev
  
  # Fedora/RHEL
  sudo dnf install libX11-devel libXtst-devel
  
  # Arch Linux
  sudo pacman -S libx11 libxtst
  ```

## Building from Source

### Quick Build
```bash
# Clone the repository
git clone <repository-url>
cd clipboard-history

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the application
make -j$(nproc)

# Install (optional)
sudo make install
```

### Development Build
For development with debug symbols and testing:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
make test
```

### CMake Options
- `-DCMAKE_BUILD_TYPE=Release|Debug` - Build type (default: Release)
- `-DCMAKE_INSTALL_PREFIX=/usr/local` - Installation prefix
- `-DENABLE_TESTING=ON|OFF` - Enable/disable tests (default: ON)

## Installation

### From Build Directory
```bash
cd build
sudo make install
```

This installs:
- Binary: `/usr/local/bin/clipboard-manager`
- Desktop file: `/usr/local/share/applications/clipboard-manager.desktop`

### Manual Installation
Copy the built binary to your preferred location:
```bash
cp build/clipboard-manager ~/.local/bin/
# Or system-wide:
sudo cp build/clipboard-manager /usr/local/bin/
```

## Usage

### Starting the Application
```bash
# Start normally (with system tray)
clipboard-manager

# Start minimized to system tray
clipboard-manager --start-minimized

# Show help
clipboard-manager --help
```

### Keyboard Shortcuts
- **Meta+V** (Windows/Super+V): Open clipboard history popup
- **Escape**: Close clipboard history popup
- **Up/Down arrows**: Navigate through history items
- **Enter**: Select and paste current item
- **Delete**: Remove selected item from history
- **Ctrl+P**: Pin/unpin selected item

### System Tray
Right-click the system tray icon for options:
- **Show Clipboard History**: Open the popup window
- **Clear History**: Remove all clipboard items
- **Settings**: Open configuration dialog
- **Quit**: Exit the application

### Command Line Interface
```bash
# Start with custom configuration
clipboard-manager --config-path ~/.config/my-clipboard/

# Start with custom hotkey
clipboard-manager --hotkey "Ctrl+Shift+V"

# Start without system tray
clipboard-manager --no-tray

# Test hotkey registration
clipboard-manager --test-hotkey "Meta+V"

# List available hotkey combinations
clipboard-manager --list-hotkeys

# Test system tray support
clipboard-manager --test-tray

# Test clipboard access
clipboard-manager --verify-clipboard

# Enable verbose logging
clipboard-manager --verbose
```

## Configuration

Configuration is stored in JSON format at:
- **Linux**: `~/.config/clipboard-manager/config.json`

### Default Configuration
```json
{
    "maxHistorySize": 50,
    "globalHotkey": "Meta+V",
    "startMinimized": false,
    "showTrayIcon": true,
    "enablePersistence": true,
    "theme": "system"
}
```

### Configuration Options
- `maxHistorySize`: Maximum number of items to keep in history (1-1000)
- `globalHotkey`: Global hotkey combination (e.g., "Meta+V", "Ctrl+Alt+V")
- `startMinimized`: Start application minimized to system tray
- `showTrayIcon`: Show system tray icon
- `enablePersistence`: Save/restore history between sessions
- `theme`: UI theme ("system", "light", "dark")

## Performance

The application is designed for minimal resource usage:
- **Memory**: < 10MB with 100 clipboard items
- **CPU**: < 1% during normal operation
- **Clipboard detection**: < 50ms response time
- **Popup display**: < 200ms from hotkey press

Performance is validated through automated tests in `tests/performance/`.

## Testing

The project includes comprehensive test coverage:

```bash
# Run all tests
cd build
make test

# Run specific test categories
ctest -R "contract"     # Contract tests
ctest -R "integration"  # Integration tests
ctest -R "unit"         # Unit tests
ctest -R "performance"  # Performance tests
```

### Test Categories
- **Contract tests**: API boundary validation
- **Integration tests**: Real system interaction
- **Unit tests**: Individual component testing
- **Performance tests**: Memory and timing validation

## Architecture

The application follows a modular, signal-driven architecture:

```
clipboard-manager
├── services/
│   └── ClipboardManager    # Core clipboard monitoring and history
├── ui/
│   ├── ClipboardWindow     # Frameless popup with keyboard navigation
│   └── TrayIcon           # System tray integration
├── models/
│   ├── ClipboardItem      # Individual clipboard entry
│   ├── ClipboardHistory   # History container and management
│   └── Configuration     # Application settings
└── lib/
    └── GlobalHotkey       # Cross-platform hotkey registration
```

Components communicate exclusively via Qt signals/slots for loose coupling and testability.

## Troubleshooting

### Global Hotkey Not Working

If you see errors like `"X11 XGrabKey failed with code: 1"`, this indicates the hotkey registration failed. The application will continue to work, but you'll need to use the system tray or CLI to access clipboard history.

**Common causes and solutions:**

1. **Hotkey already in use**: Another application (like your desktop environment) may be using Meta+V
   ```bash
   # Test different hotkey combinations
   ./clipboard-manager --test-hotkey "Ctrl+Alt+V"
   ./clipboard-manager --test-hotkey "Super+C"
   ```

2. **Desktop environment conflicts**: Many modern DEs reserve Meta+V for their own clipboard managers
   - **GNOME**: Check Settings → Keyboard → View and Customize Shortcuts
   - **KDE**: Check System Settings → Shortcuts → Global Shortcuts
   - **Disable built-in clipboard manager** if you prefer this application

3. **Permission issues**: X11 may require specific permissions
   ```bash
   # Check X11 session
   echo $XDG_SESSION_TYPE
   # Should show "x11" for X11 sessions
   ```

4. **Try alternative hotkeys** that are less likely to conflict:
   ```bash
   # Start with custom hotkey
   ./clipboard-manager --hotkey "Ctrl+Shift+V"
   ./clipboard-manager --hotkey "Alt+Insert"
   ```

5. **Fallback options**: The application works without global hotkeys
   - Use system tray icon to open clipboard history
   - The application provides diagnostics for troubleshooting:
     ```bash
     # List available hotkey combinations
     ./clipboard-manager --list-hotkeys
     
     # Test different hotkey combinations
     ./clipboard-manager --test-hotkey "Ctrl+Alt+V"
     
     # Check system tray support
     ./clipboard-manager --test-tray
     ```

**Important**: The application will continue running even if the global hotkey fails to register. You can still access all features through the system tray icon.

6. **Ensure X11 libraries are installed** (for X11 sessions):
   ```bash
   # Ubuntu/Debian
   sudo apt install libx11-dev libxtst-dev
   ```

### System Tray Icon Missing
1. Ensure your desktop environment supports system tray
2. Check if system tray is enabled in your DE settings
3. Try restarting the application

### High Memory Usage
1. Reduce `maxHistorySize` in configuration
2. Clear history periodically
3. Check for clipboard items with large content

### Build Issues
1. Ensure all Qt6 development packages are installed
2. Check CMake version (minimum 3.16 required)
3. Verify C++20 compiler support

## Development

### Project Structure
```
src/
├── main.cpp              # Application entry point
├── services/             # Core business logic
├── ui/                   # User interface components
├── models/               # Data structures
├── lib/                  # Utility libraries
└── cli/                  # Command-line interface

tests/
├── contract/             # API contract tests
├── integration/          # System integration tests
├── unit/                 # Unit tests
└── performance/          # Performance benchmarks
```

### Contributing Guidelines
1. Follow Qt coding conventions
2. Maintain test coverage for new features
3. Use Qt signals/slots for component communication
4. Write contract tests before implementation
5. Ensure cross-platform compatibility

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Copyright (c) 2025 Abdullah Al Mahmud

## Version

Current version: **1.0.0**

## Support

For bug reports and feature requests, please use the project's issue tracker.