#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>

/**
 * @brief Modern About dialog for clipboard manager
 * 
 * Features:
 * - Modern, attractive design
 * - System color scheme adaptation
 * - Application information and statistics
 * - License and contact information
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param clipboardCount Current number of clipboard items
     * @param parent Parent widget for proper lifecycle management
     */
    explicit AboutDialog(int clipboardCount = 0, QWidget* parent = nullptr);
    
    /**
     * Destructor
     */
    ~AboutDialog();

private slots:
    /**
     * Handle close button click
     */
    void onCloseClicked();

private:
    /**
     * Initialize UI components and layout
     */
    void setupUI();
    
    /**
     * Create header section with logo and title
     */
    QWidget* createHeaderSection();
    
    /**
     * Create information section with app details
     */
    QWidget* createInfoSection();
    
    /**
     * Create statistics section
     */
    QWidget* createStatsSection();
    
    /**
     * Create footer with buttons
     */
    QHBoxLayout* createFooter();
    
    /**
     * Apply modern styling to dialog
     */
    void applyModernStyling();

private:
    int m_clipboardCount;
    QPushButton* m_closeButton;
    QVBoxLayout* m_mainLayout;
};