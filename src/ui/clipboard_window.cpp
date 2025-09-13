#include "clipboard_window.h"
#include <QScreen>
#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QStyleOption>
#include <QStyle>
#include <QDebug>

ClipboardWindow::ClipboardWindow(QWidget* parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_listWidget(new QListWidget(this))
    , m_maxDisplayItems(10)
    , m_itemHeight(30)
    , m_ignoreNextFocusOut(false)
{
    setupWindow();
    setupListWidget();
    
    // Setup layout
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_listWidget);
    
    setLayout(m_layout);
}

ClipboardWindow::~ClipboardWindow()
{
    // QWidget handles cleanup of child widgets
}

void ClipboardWindow::showAtCursor()
{
    QPoint cursorPos = QCursor::pos();
    showAtPosition(cursorPos);
}

void ClipboardWindow::showAtPosition(const QPoint& position)
{
    // Update window size based on content
    QSize windowSize = calculateWindowSize();
    resize(windowSize);
    
    // Adjust position to stay on screen
    QPoint adjustedPos = adjustPositionForScreen(position);
    move(adjustedPos);
    
    // Show window and ensure it gets focus
    m_ignoreNextFocusOut = true;
    show();
    raise();
    activateWindow();
    setFocus();
    
    // Reset focus ignore flag after a short delay
    QTimer::singleShot(100, [this]() {
        m_ignoreNextFocusOut = false;
    });
}

void ClipboardWindow::showAtCenter()
{
    // Update window size based on content
    QSize windowSize = calculateWindowSize();
    resize(windowSize);
    
    // Get primary screen center
    QScreen* primaryScreen = QApplication::primaryScreen();
    if (!primaryScreen) {
        // Fallback to first available screen
        const QList<QScreen*> screens = QApplication::screens();
        if (!screens.isEmpty()) {
            primaryScreen = screens.first();
        }
    }
    
    if (primaryScreen) {
        QRect screenGeometry = primaryScreen->availableGeometry();
        QPoint centerPos = screenGeometry.center() - QPoint(windowSize.width() / 2, windowSize.height() / 2);
        
        // Ensure window stays on screen
        QPoint adjustedPos = adjustPositionForScreen(centerPos);
        move(adjustedPos);
    } else {
        // Ultimate fallback - use fixed position
        move(100, 100);
    }
    
    // Show window and ensure it gets focus
    m_ignoreNextFocusOut = true;
    show();
    raise();
    activateWindow();
    setFocus();
    
    // Reset focus ignore flag after a short delay
    QTimer::singleShot(100, [this]() {
        m_ignoreNextFocusOut = false;
    });
}

void ClipboardWindow::hideWindow()
{
    hide();
    emit windowClosed();
}

void ClipboardWindow::setHistory(const QList<ClipboardItem>& items)
{
    m_items = items;
    updateListWidget();
}

void ClipboardWindow::updateItem(const ClipboardItem& item)
{
    // Find and update the item in our list
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id() == item.id()) {
            m_items[i] = item;
            updateListWidget();
            break;
        }
    }
}

void ClipboardWindow::removeItem(const QString& id)
{
    // Remove item from our list
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id() == id) {
            m_items.removeAt(i);
            updateListWidget();
            break;
        }
    }
}

void ClipboardWindow::setMaxDisplayItems(int maxItems)
{
    if (maxItems > 0) {
        m_maxDisplayItems = maxItems;
        updateListWidget();
    }
}

void ClipboardWindow::setItemHeight(int height)
{
    if (height > 0) {
        m_itemHeight = height;
        updateListWidget();
    }
}

bool ClipboardWindow::isVisible() const
{
    return QWidget::isVisible();
}

int ClipboardWindow::selectedIndex() const
{
    return m_listWidget->currentRow();
}

ClipboardItem ClipboardWindow::selectedItem() const
{
    int index = selectedIndex();
    if (index >= 0 && index < m_items.size()) {
        return m_items[index];
    }
    return ClipboardItem(); // Invalid item
}

void ClipboardWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            hideWindow();
            break;
            
        case Qt::Key_Return:
        case Qt::Key_Enter:
            {
                ClipboardItem item = selectedItem();
                if (item.isValid()) {
                    emit itemSelected(item);
                    hideWindow();
                }
            }
            break;
            
        case Qt::Key_Up:
            {
                int currentRow = m_listWidget->currentRow();
                if (currentRow > 0) {
                    m_listWidget->setCurrentRow(currentRow - 1);
                } else if (m_listWidget->count() > 0) {
                    // Wrap to bottom
                    m_listWidget->setCurrentRow(m_listWidget->count() - 1);
                }
            }
            break;
            
        case Qt::Key_Down:
            {
                int currentRow = m_listWidget->currentRow();
                if (currentRow < m_listWidget->count() - 1) {
                    m_listWidget->setCurrentRow(currentRow + 1);
                } else if (m_listWidget->count() > 0) {
                    // Wrap to top
                    m_listWidget->setCurrentRow(0);
                }
            }
            break;
            
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void ClipboardWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Let the list widget handle the click
        QWidget::mousePressEvent(event);
    } else {
        QWidget::mousePressEvent(event);
    }
}

void ClipboardWindow::focusOutEvent(QFocusEvent* event)
{
    if (!m_ignoreNextFocusOut) {
        hideWindow();
    }
    QWidget::focusOutEvent(event);
}

void ClipboardWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}

void ClipboardWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    
    // Select first item if any items exist
    if (m_listWidget->count() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

void ClipboardWindow::onItemActivated(QListWidgetItem* item)
{
    if (item) {
        ClipboardItem clipboardItem = getClipboardItem(item);
        if (clipboardItem.isValid()) {
            emit itemSelected(clipboardItem);
            hideWindow();
        }
    }
}

void ClipboardWindow::onSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    Q_UNUSED(current)
    Q_UNUSED(previous)
    // Currently no special handling needed for selection changes
}

void ClipboardWindow::setupWindow()
{
    // Make window frameless and always on top
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_ShowWithoutActivating, false);
    
    // Set window properties
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    
    // Basic styling
    setStyleSheet(
        "ClipboardWindow {"
        "    background-color: white;"
        "    border: 2px solid #333333;"
        "    border-radius: 4px;"
        "}"
    );
}

void ClipboardWindow::setupListWidget()
{
    // Configure list widget
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setAlternatingRowColors(true);
    m_listWidget->setWordWrap(true);
    m_listWidget->setTextElideMode(Qt::ElideRight);
    
    // Connect signals
    connect(m_listWidget, &QListWidget::itemActivated,
            this, &ClipboardWindow::onItemActivated);
    connect(m_listWidget, &QListWidget::currentItemChanged,
            this, &ClipboardWindow::onSelectionChanged);
    
    // Style the list widget
    m_listWidget->setStyleSheet(
        "QListWidget {"
        "    background-color: white;"
        "    border: none;"
        "    outline: none;"
        "    selection-background-color: #3875d7;"
        "    selection-color: white;"
        "}"
        "QListWidget::item {"
        "    padding: 4px 8px;"
        "    border-bottom: 1px solid #e0e0e0;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #3875d7;"
        "    color: white;"
        "}"
    );
}

void ClipboardWindow::updateListWidget()
{
    m_listWidget->clear();
    
    // Limit items to maximum display count
    int itemCount = qMin(m_items.size(), m_maxDisplayItems);
    
    for (int i = 0; i < itemCount; ++i) {
        const ClipboardItem& item = m_items[i];
        
        QListWidgetItem* listItem = new QListWidgetItem();
        listItem->setText(formatItemText(item));
        listItem->setData(Qt::UserRole, item.id());
        listItem->setSizeHint(QSize(0, m_itemHeight));
        
        // Style pinned items differently
        if (item.pinned()) {
            QFont font = listItem->font();
            font.setBold(true);
            listItem->setFont(font);
            listItem->setIcon(QIcon("📌")); // Pin emoji as icon
        }
        
        m_listWidget->addItem(listItem);
    }
    
    // Update window size
    QSize newSize = calculateWindowSize();
    resize(newSize);
}

QPoint ClipboardWindow::adjustPositionForScreen(const QPoint& preferredPosition)
{
    QScreen* screen = QApplication::screenAt(preferredPosition);
    if (!screen) {
        screen = QApplication::primaryScreen();
    }
    
    QRect screenGeometry = screen->availableGeometry();
    QSize windowSize = size();
    
    QPoint adjustedPos = preferredPosition;
    
    // Adjust horizontal position
    if (adjustedPos.x() + windowSize.width() > screenGeometry.right()) {
        adjustedPos.setX(screenGeometry.right() - windowSize.width());
    }
    if (adjustedPos.x() < screenGeometry.left()) {
        adjustedPos.setX(screenGeometry.left());
    }
    
    // Adjust vertical position
    if (adjustedPos.y() + windowSize.height() > screenGeometry.bottom()) {
        adjustedPos.setY(preferredPosition.y() - windowSize.height());
    }
    if (adjustedPos.y() < screenGeometry.top()) {
        adjustedPos.setY(screenGeometry.top());
    }
    
    return adjustedPos;
}

QSize ClipboardWindow::calculateWindowSize() const
{
    int itemCount = qMin(m_items.size(), m_maxDisplayItems);
    if (itemCount == 0) {
        return QSize(300, 50); // Minimum size for empty list
    }
    
    // Calculate based on content
    int contentHeight = itemCount * m_itemHeight;
    int contentWidth = 400; // Base width
    
    // Add margins and scrollbar space
    int totalHeight = contentHeight + 20; // Margins and spacing
    int totalWidth = contentWidth + 20;   // Margins
    
    // Limit maximum size
    totalHeight = qMin(totalHeight, 600);
    totalWidth = qMin(totalWidth, 800);
    
    return QSize(totalWidth, totalHeight);
}

QString ClipboardWindow::formatItemText(const ClipboardItem& item) const
{
    QString text = item.text();
    
    // Limit length and clean up whitespace
    const int maxLength = 100;
    if (text.length() > maxLength) {
        text = text.left(maxLength - 3) + "...";
    }
    
    // Replace newlines with spaces and compress whitespace
    text = text.replace('\n', ' ').replace('\r', ' ');
    text = text.simplified();
    
    // Add pin indicator prefix if pinned
    if (item.pinned()) {
        text = "📌 " + text;
    }
    
    return text;
}

QListWidgetItem* ClipboardWindow::findListWidgetItem(const QString& id) const
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item && item->data(Qt::UserRole).toString() == id) {
            return item;
        }
    }
    return nullptr;
}

ClipboardItem ClipboardWindow::getClipboardItem(QListWidgetItem* listItem) const
{
    if (!listItem) {
        return ClipboardItem(); // Invalid item
    }
    
    QString id = listItem->data(Qt::UserRole).toString();
    for (const ClipboardItem& item : m_items) {
        if (item.id() == id) {
            return item;
        }
    }
    
    return ClipboardItem(); // Invalid item
}
