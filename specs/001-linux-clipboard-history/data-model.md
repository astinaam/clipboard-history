# Data Model: Linux Clipboard History Manager

## Core Entities

### ClipboardItem
Represents a single clipboard entry with metadata and content.

**Fields**:
- `id`: QString (UUID) - Unique identifier for the item
- `text`: QString - Full clipboard text content
- `preview`: QString - Truncated display text (max 100 chars)
- `timestamp`: QDateTime - When the item was copied
- `pinned`: bool - Whether the item is pinned to top of list
- `hash`: QString - Content hash for duplicate detection

**Validation Rules**:
- `text` must not be empty or whitespace-only
- `preview` auto-generated from `text` (first 100 chars with ellipsis)
- `timestamp` must be valid QDateTime
- `id` must be valid UUID format
- `hash` computed using QCryptographicHash::Sha256

**State Transitions**:
- New → Active (when first added to history)
- Active → Pinned (when user pins item)
- Pinned → Active (when user unpins item)
- Active → Expired (when pushed out by history limit, if not pinned)

### ClipboardHistory
Manages the collection of clipboard items with ordering and size limits.

**Fields**:
- `items`: QList<ClipboardItem> - Ordered list of clipboard items
- `maxItems`: int - Maximum number of items to store (default 50)
- `pinnedCount`: int - Current number of pinned items

**Validation Rules**:
- `maxItems` must be between 10 and 100
- Pinned items always appear at the top of the list
- Regular items ordered by timestamp (newest first)
- Total items never exceed `maxItems`

**Operations**:
- `addItem(text)` - Add new clipboard item, handle duplicates and limits
- `pinItem(id)` - Pin item by ID, move to top section
- `unpinItem(id)` - Unpin item by ID, reorder by timestamp
- `removeItem(id)` - Remove item by ID (only unpinned items)
- `clear()` - Remove all unpinned items
- `getItem(id)` - Retrieve item by ID

### Configuration
Stores user preferences and application settings.

**Fields**:
- `version`: QString - Configuration format version
- `maxHistoryItems`: int - Maximum items in history (default 50)
- `hotkey`: QString - Global hotkey combination (default "Meta+V")
- `autostart`: bool - Start with system (default false)
- `showNotifications`: bool - Show tray notifications (default true)
- `windowPosition`: QPoint - Last popup window position
- `windowSize`: QSize - Last popup window size

**Validation Rules**:
- `maxHistoryItems` must be between 10 and 100
- `hotkey` must be valid key combination string
- `version` must match supported configuration versions

**Persistence**:
- Stored in `~/.config/clipboard-manager/config.json`
- Atomic write operations to prevent corruption
- Automatic migration between configuration versions

## Data Relationships

### History → Items (1:N)
- One ClipboardHistory contains multiple ClipboardItems
- Items are ordered: pinned items first, then by timestamp descending
- History enforces size limits by removing oldest unpinned items

### Item → Content (1:1)
- Each ClipboardItem has exactly one text content
- Content is immutable once created
- Preview is derived from content automatically

## Serialization Format

### JSON Schema for ClipboardItem
```json
{
  "id": "550e8400-e29b-41d4-a716-446655440000",
  "text": "Full clipboard content here",
  "preview": "Full clipboard content...",
  "timestamp": "2025-09-11T10:30:00.000Z",
  "pinned": false,
  "hash": "a1b2c3d4e5f6..."
}
```

### JSON Schema for ClipboardHistory
```json
{
  "version": "1.0.0",
  "settings": {
    "maxHistoryItems": 50,
    "hotkey": "Meta+V",
    "autostart": false,
    "showNotifications": true,
    "windowPosition": {"x": 100, "y": 100},
    "windowSize": {"width": 400, "height": 600}
  },
  "history": [
    // Array of ClipboardItem objects
  ]
}
```

## Memory Management

### Size Optimization
- Store only essential data in memory
- Generate previews on-demand for display
- Use Qt's implicit sharing for QString content
- Implement cleanup strategies for memory pressure

### Cleanup Strategies
- Remove expired items when history limit reached
- Compress large text content for storage
- Clear cached data when application is minimized
- Implement periodic cleanup of old temporary data

## Error Handling

### Data Validation
- Validate all input data before storage
- Handle malformed JSON gracefully with defaults
- Migrate older configuration formats automatically
- Backup corrupted data before attempting repair

### Storage Failures
- Handle file system permission errors
- Provide fallback to temporary storage
- Notify user of persistent storage issues
- Implement retry logic for transient failures
