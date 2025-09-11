#pragma once

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QCloseEvent>
#include <QScreen>
#include <QCursor>
#include <QApplication>
#include <QList>

#include "../models/clipboard_item.h"

/**
 * ClipboardWindow - Frameless popup window for clipboard history display
 * 
 * This UI component provides a keyboard-navigable popup window that displays
 * clipboard history items with pinned items appearing first. Designed for
 * fast <200ms display time and intuitive keyboard/mouse interaction.
 * 
 * Design principles:
 * - Frameless window that appears at cursor or specified position
 * - Keyboard navigation with arrow keys, Enter, and Escape
 * - Mouse interaction with single/double click selection
 * - Auto-hide on focus loss or explicit close
 * - Performance target: <200ms display time
 */
class ClipboardWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor - Creates frameless popup window
     * @param parent QWidget parent for memory management
     */
    explicit ClipboardWindow(QWidget* parent = nullptr);
    
    /**
     * Destructor - Ensures proper cleanup
     */
    ~ClipboardWindow();

    // Window Management Methods
    /**
     * Show window at current cursor position
     * Window will be positioned to stay on screen
     */
    void showAtCursor();
    
    /**
     * Show window at specified position
     * @param position Screen coordinates for window placement
     */
    void showAtPosition(const QPoint& position);
    
    /**
     * Hide the window
     * Can be called explicitly or triggered by events
     */
    void hideWindow();

    // Content Management Methods
    /**
     * Set clipboard history items to display
     * @param items List of clipboard items (pinned items will appear first)
     */
    void setHistory(const QList<ClipboardItem>& items);
    
    /**
     * Update a specific item in the display
     * @param item Updated clipboard item
     */
    void updateItem(const ClipboardItem& item);
    
    /**
     * Remove an item from the display
     * @param id UUID of item to remove
     */
    void removeItem(const QString& id);

    // Configuration Methods
    /**
     * Set maximum number of items to display
     * @param maxItems Maximum items to show (default 10)
     */
    void setMaxDisplayItems(int maxItems);
    
    /**
     * Get current maximum display items
     * @return Current maximum items setting
     */
    int maxDisplayItems() const { return m_maxDisplayItems; }
    
    /**
     * Set height for each item row
     * @param height Height in pixels for each item
     */
    void setItemHeight(int height);
    
    /**
     * Get current item height
     * @return Current item height in pixels
     */
    int itemHeight() const { return m_itemHeight; }

    // State Query Methods
    /**
     * Check if window is currently visible
     * @return true if window is visible
     */
    bool isVisible() const;
    
    /**
     * Get currently selected item index
     * @return Index of selected item, -1 if none
     */
    int selectedIndex() const;
    
    /**
     * Get currently selected item
     * @return Selected clipboard item, invalid item if none selected
     */
    ClipboardItem selectedItem() const;

signals:
    /**
     * Emitted when user selects an item (Enter key, click, etc.)
     * @param item The selected clipboard item
     */
    void itemSelected(const ClipboardItem& item);
    
    /**
     * Emitted when window is closed/hidden
     */
    void windowClosed();

protected:
    // Qt Event Handlers
    /**
     * Handle key press events for navigation and selection
     * @param event Key press event
     */
    void keyPressEvent(QKeyEvent* event) override;
    
    /**
     * Handle mouse press events for item selection
     * @param event Mouse press event
     */
    void mousePressEvent(QMouseEvent* event) override;
    
    /**
     * Handle focus loss to auto-hide window
     * @param event Focus event
     */
    void focusOutEvent(QFocusEvent* event) override;
    
    /**
     * Handle close events
     * @param event Close event
     */
    void closeEvent(QCloseEvent* event) override;
    
    /**
     * Handle show events to ensure proper focus
     * @param event Show event
     */
    void showEvent(QShowEvent* event) override;

private slots:
    /**
     * Handle item activation from list widget
     * @param item Activated list widget item
     */
    void onItemActivated(QListWidgetItem* item);
    
    /**
     * Handle item selection change
     * @param current Currently selected item
     * @param previous Previously selected item
     */
    void onSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);

private:
    // UI Components
    QVBoxLayout* m_layout;           ///< Main window layout
    QListWidget* m_listWidget;       ///< List widget for displaying items
    
    // Data
    QList<ClipboardItem> m_items;    ///< Current clipboard items
    
    // Configuration
    int m_maxDisplayItems;           ///< Maximum items to display
    int m_itemHeight;                ///< Height of each item row
    
    // State
    bool m_ignoreNextFocusOut;       ///< Flag to ignore focus out during positioning
    
    // Helper Methods
    /**
     * Setup window properties (frameless, always on top, etc.)
     */
    void setupWindow();
    
    /**
     * Setup the list widget properties and connections
     */
    void setupListWidget();
    
    /**
     * Update the list widget with current items
     */
    void updateListWidget();
    
    /**
     * Position window to stay within screen bounds
     * @param preferredPosition Desired position
     * @return Actual position after screen boundary adjustment
     */
    QPoint adjustPositionForScreen(const QPoint& preferredPosition);
    
    /**
     * Calculate optimal window size based on content
     * @return Calculated window size
     */
    QSize calculateWindowSize() const;
    
    /**
     * Format item text for display
     * @param item Clipboard item to format
     * @return Formatted display text
     */
    QString formatItemText(const ClipboardItem& item) const;
    
    /**
     * Find list widget item by clipboard item ID
     * @param id Clipboard item UUID
     * @return List widget item or nullptr if not found
     */
    QListWidgetItem* findListWidgetItem(const QString& id) const;
    
    /**
     * Get clipboard item from list widget item
     * @param listItem List widget item
     * @return Associated clipboard item
     */
    ClipboardItem getClipboardItem(QListWidgetItem* listItem) const;
};
