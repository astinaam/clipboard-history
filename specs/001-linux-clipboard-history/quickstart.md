# Quickstart Guide: Linux Clipboard History Manager

## Prerequisites

### System Requirements
- Linux system with X11 or Wayland display server
- Qt6 development libraries (Qt 6.5 or later)
- C++17 compatible compiler (GCC 9+ or Clang 10+)
- CMake 3.16 or later

### Development Dependencies
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-base-dev-tools cmake build-essential

# Fedora/RHEL
sudo dnf install qt6-qtbase-devel cmake gcc-c++

# Arch Linux
sudo pacman -S qt6-base cmake gcc
```

## Quick Setup (5 minutes)

### 1. Clone and Build
```bash
git clone <repository-url> clipboard-history
cd clipboard-history
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### 2. Run Tests
```bash
# Run all tests
make test

# Run specific test suites
./tests/test_clipboard_manager
./tests/test_clipboard_window
./tests/test_tray_icon
```

### 3. Start Application
```bash
# Run from build directory
./clipboard-manager

# Or install and run
sudo make install
clipboard-manager
```

## Verification Checklist

### ✅ Basic Functionality
1. **Clipboard Monitoring**:
   - Copy some text: `echo "test 1" | xclip -selection clipboard`
   - Copy more text: `echo "test 2" | xclip -selection clipboard`
   - Verify both items appear in history

2. **Global Hotkey**:
   - Press `Meta+V` (Windows/Super key + V)
   - Popup window should appear with clipboard history
   - Arrow keys should navigate between items
   - Enter should select item and close popup

3. **System Tray**:
   - System tray icon should be visible
   - Right-click should show context menu
   - Left-click should open clipboard history popup
   - Recent items should appear in tray menu

### ✅ Advanced Features
4. **Item Pinning**:
   - Open history popup (`Meta+V`)
   - Right-click on an item → select "Pin"
   - Pinned item should move to top of list
   - Pinned item should persist after copying new content

5. **Persistence**:
   - Copy several items to build history
   - Close application: `pkill clipboard-manager`
   - Restart application: `clipboard-manager`
   - History should be restored from previous session

6. **Configuration**:
   - Check config file exists: `ls ~/.config/clipboard-manager/`
   - Modify history limit in settings
   - Verify limit is enforced when copying new items

## Usage Examples

### Command Line Interface
```bash
# Show help
clipboard-manager --help

# Start with custom config
clipboard-manager --config-path ~/.config/my-clipboard/

# Set history limit
clipboard-manager --history-limit 30

# Enable debug logging
clipboard-manager --verbose
```

### Integration Testing
```bash
# Test clipboard integration
echo "Integration test content" | xclip -selection clipboard
clipboard-manager --verify-clipboard

# Test hotkey registration
clipboard-manager --test-hotkey Meta+V

# Test system tray availability
clipboard-manager --test-tray
```

## Common Issues & Solutions

### ❌ Hotkey Not Working
**Problem**: Meta+V doesn't open popup  
**Solution**: 
```bash
# Check if hotkey is registered
clipboard-manager --list-hotkeys

# Try alternative hotkey
clipboard-manager --hotkey "Ctrl+Alt+V"
```

### ❌ System Tray Missing
**Problem**: No tray icon visible  
**Solution**:
```bash
# Check tray support
clipboard-manager --test-tray

# Start without tray (keyboard-only mode)
clipboard-manager --no-tray
```

### ❌ Clipboard Not Monitored
**Problem**: New clipboard content not appearing in history  
**Solution**:
```bash
# Check clipboard access permissions
clipboard-manager --test-clipboard

# Verify clipboard content
xclip -o -selection clipboard
```

### ❌ Configuration Issues
**Problem**: Settings not persisting  
**Solution**:
```bash
# Check config directory permissions
ls -la ~/.config/clipboard-manager/

# Reset to defaults
clipboard-manager --reset-config
```

## Development Workflow

### Running Tests During Development
```bash
# Run tests in watch mode (requires inotify-tools)
while inotifywait -e modify src/ tests/; do make test; done

# Run specific test category
ctest -R "clipboard_manager"
ctest -R "integration"
ctest -R "ui"
```

### Debug Mode
```bash
# Start with debug output
QT_LOGGING_RULES="clipboard.*=true" clipboard-manager

# Start with GDB
gdb --args clipboard-manager --verbose
```

### Performance Testing
```bash
# Test with large clipboard content
dd if=/dev/urandom bs=1024 count=100 | base64 | xclip -selection clipboard

# Test with many history items
for i in {1..50}; do echo "Test item $i" | xclip -selection clipboard; sleep 0.1; done
```

## Next Steps

### For Users
- Configure autostart: Add to desktop environment startup applications
- Customize hotkey: Edit `~/.config/clipboard-manager/config.json`
- Set history limit: Adjust `maxHistoryItems` in configuration

### For Developers
- Read architecture documentation in `/docs/architecture.md`
- Review coding standards in `/docs/contributing.md`
- Check issue tracker for feature requests and bugs

### For System Integration
- Create desktop entry: `/usr/share/applications/clipboard-manager.desktop`
- Set up systemd user service for autostart
- Configure system-wide defaults in `/etc/clipboard-manager/`

## Troubleshooting

### Log Files
- Application logs: `~/.local/share/clipboard-manager/logs/`
- System logs: `journalctl -u clipboard-manager --user`
- Debug logs: Set `QT_LOGGING_RULES="clipboard.*=true"`

### Performance Monitoring
```bash
# Monitor memory usage
ps aux | grep clipboard-manager

# Monitor file descriptor usage
lsof -p $(pgrep clipboard-manager)

# Monitor system calls
strace -p $(pgrep clipboard-manager)
```

For additional support, check the project documentation or file an issue on the project repository.
