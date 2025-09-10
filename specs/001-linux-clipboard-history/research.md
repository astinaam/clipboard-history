# Research: Linux Clipboard History Manager

## Global Hotkey Implementation

**Decision**: Use platform-specific hotkey APIs with Qt integration
**Rationale**: 
- QxtGlobalShortcut is deprecated and not maintained for Qt6
- Platform-specific approach ensures X11 and Wayland compatibility
- Better control over hotkey conflicts and system integration

**Implementation Approach**:
- X11: Use XGrabKey with QX11Info for window manager integration
- Wayland: Use compositor-specific global shortcut protocols
- Fallback: QShortcut with application focus workarounds

**Alternatives considered**:
- QxtGlobalShortcut: Deprecated, Qt5 only
- libqxt: Unmaintained, compatibility issues
- Third-party libraries: Additional dependencies against project goals

## Qt6 Architecture Patterns

**Decision**: Qt6 Widgets with modern signal/slot patterns
**Rationale**:
- Mature, stable API for desktop applications
- Better performance than QML for simple list interfaces
- Direct integration with system clipboard and tray

**Component Design**:
- QApplication main event loop
- QSystemTrayIcon for system tray integration
- QClipboard with dataChanged() signal monitoring
- QWidget popup with QListWidget for history display
- QJsonDocument for configuration and persistence

**Alternatives considered**:
- QML: Overkill for simple list interface, larger memory footprint
- GTK: Would require different language bindings, larger dependency chain

## Cross-Platform Clipboard Monitoring

**Decision**: QClipboard::dataChanged() signal with content filtering
**Rationale**:
- Native Qt integration across X11 and Wayland
- Automatic handling of clipboard ownership changes
- Built-in MIME type detection and text extraction

**Implementation Strategy**:
- Connect to QClipboard::dataChanged() signal
- Filter duplicate content and empty/whitespace-only text
- Handle large text gracefully (truncation for display, full storage)
- Respect system clipboard ownership protocols

**Alternatives considered**:
- Platform-specific clipboard APIs: More complex, platform-dependent code
- Polling: Inefficient, potential race conditions
- External clipboard libraries: Additional dependencies

## Data Persistence Strategy

**Decision**: JSON file storage in XDG config directory
**Rationale**:
- Human-readable format for debugging and user inspection
- QJsonDocument provides robust parsing and generation
- XDG Base Directory compliance for Linux systems
- Atomic file operations prevent corruption

**File Structure**:
```json
{
  "version": "1.0.0",
  "settings": {
    "maxHistoryItems": 50,
    "hotkey": "Meta+V"
  },
  "history": [
    {
      "id": "uuid",
      "text": "content",
      "timestamp": "ISO8601",
      "pinned": false,
      "preview": "truncated content..."
    }
  ]
}
```

**Alternatives considered**:
- SQLite: Overkill for simple text storage, binary format harder to debug
- Binary formats: Not human-readable, versioning complexity
- System clipboard integration: Not persistent across reboots

## Memory and Performance Optimization

**Decision**: Lazy loading with content truncation and cleanup strategies
**Rationale**:
- Maintain responsive UI with large clipboard history
- Minimize memory footprint for background operation
- Efficient text preview generation

**Optimization Strategies**:
- Store full content, generate 100-char previews for display
- Implement LRU cleanup for unpinned items when limit reached
- Use QTimer for deferred clipboard processing (avoid UI blocking)
- Implement smart duplicate detection (content hash comparison)

**Performance Targets**:
- <50ms clipboard change detection and processing
- <200ms popup window display time
- <10MB total memory usage under normal operation
- <1MB configuration file size with 50 items

## Error Handling and Edge Cases

**Decision**: Graceful degradation with user notification
**Rationale**:
- Desktop applications should handle system integration failures gracefully
- Users need feedback about configuration or permission issues

**Error Scenarios**:
- Clipboard access denied: Continue with reduced functionality, notify user
- Hotkey registration failure: Provide alternative access via tray icon
- File system permissions: Fallback to temp directory, warn user
- Large clipboard content: Truncate for performance, store full content
- Malformed configuration: Reset to defaults, backup corrupted file

**Recovery Strategies**:
- Configuration validation with automatic repair
- Backup and restore for history data
- System tray notifications for critical errors
- Graceful startup even with missing dependencies
