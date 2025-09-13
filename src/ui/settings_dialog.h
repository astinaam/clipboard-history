#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QScrollArea>

class Configuration;

/**
 * @brief Modern settings dialog for clipboard manager configuration
 * 
 * Features:
 * - Modern, responsive UI design
 * - System color scheme adaptation
 * - Live configuration updates
 * - Input validation
 * - Organized sections
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param config Configuration object to modify
     * @param parent Parent widget for proper lifecycle management
     */
    explicit SettingsDialog(Configuration* config, QWidget* parent = nullptr);
    
    /**
     * Destructor
     */
    ~SettingsDialog();

public slots:
    /**
     * Apply current settings without closing dialog
     */
    void applySettings();
    
    /**
     * Reset settings to defaults
     */
    void resetToDefaults();

signals:
    /**
     * Emitted when settings are successfully applied
     */
    void settingsApplied();

private slots:
    /**
     * Handle OK button click - apply and close
     */
    void onOkClicked();
    
    /**
     * Handle Cancel button click - close without saving
     */
    void onCancelClicked();
    
    /**
     * Handle Apply button click - save without closing
     */
    void onApplyClicked();
    
    /**
     * Handle hotkey test button click
     */
    void onTestHotkeyClicked();

private:
    /**
     * Initialize UI components and layout
     */
    void setupUI();
    
    /**
     * Create general settings section
     */
    QGroupBox* createGeneralSection();
    
    /**
     * Create hotkey settings section
     */
    QGroupBox* createHotkeySection();
    
    /**
     * Create button bar at bottom
     */
    QHBoxLayout* createButtonBar();
    
    /**
     * Load current configuration into UI
     */
    void loadConfiguration();
    
    /**
     * Save UI values to configuration
     */
    void saveConfiguration();
    
    /**
     * Apply modern styling to dialog
     */
    void applyModernStyling();
    
    /**
     * Validate current input values
     * @return true if all inputs are valid
     */
    bool validateInputs();

private:
    Configuration* m_config;
    
    // General settings
    QSpinBox* m_historyLimitSpinBox;
    QCheckBox* m_startMinimizedCheckBox;  // Uses autostart
    QCheckBox* m_showTrayIconCheckBox;    // Uses showNotifications
    
    // Hotkey settings
    QLineEdit* m_hotkeyLineEdit;
    QPushButton* m_testHotkeyButton;
    
    // Button bar
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
    QPushButton* m_resetButton;
    
    // Layout components
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
};