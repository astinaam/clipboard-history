# ClipboardWindow Library Contract

## Public Interface

### ClipboardWindow Class

```cpp
class ClipboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClipboardWindow(QWidget *parent = nullptr);
    ~ClipboardWindow();

    // Window management
    void showAtCursor();
    void showAtPosition(const QPoint &position);
    void hideWindow();
    
    // Content management
    void setHistory(const QList<ClipboardItem> &items);
    void updateItem(const ClipboardItem &item);
    void removeItem(const QString &id);
    
    // Configuration
    void setMaxDisplayItems(int maxItems);
    void setItemHeight(int height);
    void setWindowSize(const QSize &size);

public slots:
    void refreshHistory();
    void selectFirstItem();
    void selectNextItem();
    void selectPreviousItem();

signals:
    void itemSelected(const QString &id);
    void itemPinRequested(const QString &id);
    void itemRemoveRequested(const QString &id);
    void windowClosed();
    void focusLost();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onItemDoubleClicked(QListWidgetItem *item);
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    // Internal implementation details omitted
};
```

### ClipboardItemWidget Class

```cpp
class ClipboardItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClipboardItemWidget(const ClipboardItem &item, QWidget *parent = nullptr);
    
    // Content management
    void setItem(const ClipboardItem &item);
    ClipboardItem item() const;
    
    // Visual state
    void setSelected(bool selected);
    void setPinned(bool pinned);
    void setHighlighted(bool highlighted);

signals:
    void clicked();
    void doubleClicked();
    void pinToggleRequested();
    void removeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    // Internal implementation details omitted
};
```

## Contract Guarantees

### Display Behavior
- `showAtCursor()`: Must appear within 200ms at current mouse position
- `setHistory(items)`: Must display items in provided order (pinned first, timestamp desc)
- `selectFirstItem()`: Must visually highlight first item and scroll to top

### Keyboard Navigation
- Arrow keys: Must navigate between items with visual feedback
- Enter/Return: Must emit `itemSelected()` for highlighted item
- Escape: Must hide window and emit `windowClosed()`
- Meta+V: Must hide window (handled by global hotkey system)

### Mouse Interaction
- Single click: Must emit `itemSelected()` and close window
- Right click: Must show context menu with "Pin/Unpin" and "Remove" options
- Double click: Must emit `itemSelected()` and close window immediately

### Visual Requirements
- Window: Must be frameless, with subtle shadow and rounded corners
- Items: Must show preview text, timestamp, and pin indicator
- Selection: Must have clear visual feedback for keyboard/mouse selection
- Pinned items: Must have distinct visual indicator (pin icon)

### Performance Contracts
- `showAtCursor()`: Must appear in <200ms from signal to visible
- `setHistory()`: Must handle up to 100 items without lag
- Scrolling: Must be smooth for large lists (virtual scrolling if needed)
- Memory: Must not exceed 5MB for UI rendering with 100 items

### Window Management
- Focus: Must capture keyboard focus when shown
- Z-order: Must appear above all other windows
- Screen bounds: Must adjust position if cursor near screen edge
- Multi-monitor: Must appear on monitor containing cursor

## Layout Specification

### Window Structure
```
┌─────────────────────────────┐
│ ClipboardWindow             │
│ ┌─────────────────────────┐ │
│ │ QScrollArea             │ │
│ │ ┌─────────────────────┐ │ │
│ │ │ ClipboardItemWidget │ │ │  ← Pinned item (pin icon)
│ │ ├─────────────────────┤ │ │
│ │ │ ClipboardItemWidget │ │ │  ← Regular item
│ │ ├─────────────────────┤ │ │
│ │ │ ClipboardItemWidget │ │ │  ← Regular item
│ │ └─────────────────────┘ │ │
│ └─────────────────────────┘ │
└─────────────────────────────┘
```

### Item Widget Layout
```
┌─────────────────────────────────────┐
│ [📌] Preview text here...           │  ← Pin icon (if pinned)
│      2025-09-11 10:30:00           │  ← Timestamp
└─────────────────────────────────────┘
```

## Error Conditions

### Display Errors
- Empty history: Show "No clipboard history" message
- Invalid item data: Skip corrupted items, log warning
- Screen positioning: Fallback to center screen if cursor position invalid

### Interaction Errors
- Invalid item selection: Ignore action, maintain current state
- Focus loss during navigation: Emit `focusLost()` signal
- Window close during selection: Emit `windowClosed()` without item selection

### Performance Degradation
- Large item content: Truncate display text to prevent UI lag
- Many items: Implement virtual scrolling beyond 50 items
- Memory pressure: Clear cached widget data when window hidden
