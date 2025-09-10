# ClipboardManager Library Contract

## Public Interface

### ClipboardManager Class

```cpp
class ClipboardManager : public QObject
{
    Q_OBJECT

public:
    explicit ClipboardManager(QObject *parent = nullptr);
    ~ClipboardManager();

    // Configuration
    void setMaxHistoryItems(int maxItems);
    int maxHistoryItems() const;
    
    // History management
    QList<ClipboardItem> getHistory() const;
    ClipboardItem getItem(const QString &id) const;
    bool pinItem(const QString &id);
    bool unpinItem(const QString &id);
    bool removeItem(const QString &id);
    void clearHistory();
    
    // Persistence
    bool loadHistory();
    bool saveHistory();
    QString configPath() const;

public slots:
    void startMonitoring();
    void stopMonitoring();

signals:
    void historyChanged();
    void itemAdded(const ClipboardItem &item);
    void itemPinned(const QString &id);
    void itemUnpinned(const QString &id);
    void itemRemoved(const QString &id);
    void error(const QString &message);

private slots:
    void onClipboardChanged();

private:
    // Internal implementation details omitted
};
```

### ClipboardItem Structure

```cpp
struct ClipboardItem
{
    QString id;
    QString text;
    QString preview;
    QDateTime timestamp;
    bool pinned;
    QString hash;
    
    // Serialization
    QJsonObject toJson() const;
    static ClipboardItem fromJson(const QJsonObject &json);
    
    // Operators
    bool operator==(const ClipboardItem &other) const;
    bool operator!=(const ClipboardItem &other) const;
};
Q_DECLARE_METATYPE(ClipboardItem)
```

## Contract Guarantees

### Input Validation
- `setMaxHistoryItems(maxItems)`: Must accept values 10-100, reject others with error signal
- `pinItem(id)`: Must return false for non-existent IDs, true for successful pins
- `removeItem(id)`: Must return false for pinned items or non-existent IDs

### Output Guarantees
- `getHistory()`: Always returns items ordered by: pinned items first, then timestamp descending
- `historyChanged()`: Emitted within 50ms of any history modification
- `itemAdded(item)`: Emitted exactly once per new clipboard content

### Performance Contracts
- `getHistory()`: Must complete in <10ms for up to 100 items
- `onClipboardChanged()`: Must process clipboard changes in <50ms
- Memory usage: Must not exceed 10MB for 100 items with 1KB average content

### Persistence Contracts
- `saveHistory()`: Must use atomic write operations to prevent corruption
- `loadHistory()`: Must handle corrupted files gracefully, returning to defaults
- Configuration: Must be stored in XDG-compliant config directory

## Error Conditions

### Input Errors
- Invalid clipboard content (empty, whitespace-only): Log warning, ignore
- Invalid ID format: Return false, emit error signal with description
- Invalid configuration values: Use defaults, emit error signal

### System Errors  
- File system permission denied: Emit error signal, continue with memory-only operation
- Clipboard access denied: Emit error signal, disable monitoring until retry
- JSON parsing errors: Emit error signal, reset to defaults with backup

### Error Signal Format
```cpp
// Error signal payload format
void error(const QString &message);

// Standard error message format:
// "ERROR_TYPE: Description - Suggested action"
// Examples:
// "PERMISSION_DENIED: Cannot write to config directory - Check file permissions"
// "CLIPBOARD_ACCESS: Cannot monitor clipboard - Application may need elevated permissions"
```
