#include "settings_dialog.h"
#include "../models/configuration.h"
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTimer>

SettingsDialog::SettingsDialog(Configuration* config, QWidget* parent)
    : QDialog(parent)
    , m_config(config)
    , m_historyLimitSpinBox(nullptr)
    , m_startMinimizedCheckBox(nullptr)
    , m_showTrayIconCheckBox(nullptr)
    , m_hotkeyLineEdit(nullptr)
    , m_testHotkeyButton(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_applyButton(nullptr)
    , m_resetButton(nullptr)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
{
    setupUI();
    loadConfiguration();
    applyModernStyling();
    
    // Set dialog properties
    setWindowTitle("Clipboard Manager Settings");
    setWindowIcon(QIcon::fromTheme("preferences-system"));
    setModal(true);
    
    // Size and position
    resize(500, 600);
    
    // Center on screen
    if (QScreen* screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.center() - rect().center());
    }
}

SettingsDialog::~SettingsDialog()
{
    // QDialog handles cleanup of child widgets
}

void SettingsDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Create title area
    QLabel* titleLabel = new QLabel("Settings");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(titleLabel);
    
    QLabel* subtitleLabel = new QLabel("Configure your clipboard manager preferences");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(subtitleLabel);
    
    // Add separator
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    m_mainLayout->addWidget(separator);
    
    // Create scroll area for content
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    
    m_contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setSpacing(15);
    
    // Add sections
    contentLayout->addWidget(createGeneralSection());
    contentLayout->addWidget(createHotkeySection());
    // Removed appearance section - theme not available in Configuration class
    
    contentLayout->addStretch();
    
    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);
    
    // Add button bar
    m_mainLayout->addLayout(createButtonBar());
}

QGroupBox* SettingsDialog::createGeneralSection()
{
    QGroupBox* groupBox = new QGroupBox("General Settings");
    QFormLayout* layout = new QFormLayout(groupBox);
    layout->setSpacing(10);
    
    // History limit
    m_historyLimitSpinBox = new QSpinBox();
    m_historyLimitSpinBox->setRange(10, 1000);
    m_historyLimitSpinBox->setSuffix(" items");
    m_historyLimitSpinBox->setToolTip("Maximum number of clipboard items to remember");
    layout->addRow("History Limit:", m_historyLimitSpinBox);
    
    // Start minimized (uses autostart setting)
    m_startMinimizedCheckBox = new QCheckBox("Start with system");
    m_startMinimizedCheckBox->setToolTip("Start clipboard manager when system boots");
    layout->addRow(m_startMinimizedCheckBox);
    
    // Show tray icon (uses showNotifications setting)
    m_showTrayIconCheckBox = new QCheckBox("Show notifications");
    m_showTrayIconCheckBox->setToolTip("Show tray notifications when clipboard changes");
    layout->addRow(m_showTrayIconCheckBox);
    
    return groupBox;
}

QGroupBox* SettingsDialog::createHotkeySection()
{
    QGroupBox* groupBox = new QGroupBox("Global Hotkey");
    QFormLayout* layout = new QFormLayout(groupBox);
    layout->setSpacing(10);
    
    // Hotkey input
    QHBoxLayout* hotkeyLayout = new QHBoxLayout();
    m_hotkeyLineEdit = new QLineEdit();
    m_hotkeyLineEdit->setPlaceholderText("e.g., Meta+V, Ctrl+Alt+V");
    m_hotkeyLineEdit->setToolTip("Global hotkey combination to show clipboard history");
    
    m_testHotkeyButton = new QPushButton("Test");
    m_testHotkeyButton->setToolTip("Test if the hotkey combination works");
    
    hotkeyLayout->addWidget(m_hotkeyLineEdit);
    hotkeyLayout->addWidget(m_testHotkeyButton);
    
    layout->addRow("Hotkey:", hotkeyLayout);
    
    // Help text
    QLabel* helpLabel = new QLabel("Use combinations like Meta+V, Ctrl+Alt+V, etc.\n"
                                  "Note: Some combinations may be reserved by your desktop environment.");
    helpLabel->setObjectName("helpLabel");
    helpLabel->setWordWrap(true);
    layout->addRow(helpLabel);
    
    // Connect test button
    connect(m_testHotkeyButton, &QPushButton::clicked, this, &SettingsDialog::onTestHotkeyClicked);
    
    return groupBox;
}

QHBoxLayout* SettingsDialog::createButtonBar()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    // Add stretch to push buttons to the right
    buttonLayout->addStretch();
    
    // Reset button
    m_resetButton = new QPushButton("Reset to Defaults");
    m_resetButton->setToolTip("Reset all settings to default values");
    buttonLayout->addWidget(m_resetButton);
    
    buttonLayout->addSpacing(20);
    
    // Apply button
    m_applyButton = new QPushButton("Apply");
    m_applyButton->setToolTip("Apply changes without closing dialog");
    buttonLayout->addWidget(m_applyButton);
    
    // Cancel button
    m_cancelButton = new QPushButton("Cancel");
    m_cancelButton->setToolTip("Close without saving changes");
    buttonLayout->addWidget(m_cancelButton);
    
    // OK button
    m_okButton = new QPushButton("OK");
    m_okButton->setDefault(true);
    m_okButton->setToolTip("Apply changes and close dialog");
    buttonLayout->addWidget(m_okButton);
    
    // Connect buttons
    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
    
    return buttonLayout;
}

void SettingsDialog::loadConfiguration()
{
    if (!m_config) return;
    
    m_historyLimitSpinBox->setValue(m_config->maxHistoryItems());
    m_hotkeyLineEdit->setText(m_config->hotkey());
    m_startMinimizedCheckBox->setChecked(m_config->autostart());
    m_showTrayIconCheckBox->setChecked(m_config->showNotifications());
    // Skip settings that aren't available in the Configuration class
}

void SettingsDialog::saveConfiguration()
{
    if (!m_config) return;
    
    m_config->setMaxHistoryItems(m_historyLimitSpinBox->value());
    m_config->setHotkey(m_hotkeyLineEdit->text().trimmed());
    m_config->setAutostart(m_startMinimizedCheckBox->isChecked());
    m_config->setShowNotifications(m_showTrayIconCheckBox->isChecked());
    
    // Save to file
    m_config->save();
}

void SettingsDialog::applyModernStyling()
{
    setStyleSheet(R"(
        QDialog {
            background-color: palette(window);
            border: 1px solid palette(mid);
            border-radius: 8px;
        }
        
        #titleLabel {
            font-size: 24px;
            font-weight: bold;
            color: palette(text);
            margin: 10px 0;
        }
        
        #subtitleLabel {
            font-size: 14px;
            color: palette(mid);
            margin-bottom: 10px;
        }
        
        #helpLabel {
            font-size: 12px;
            color: palette(mid);
            font-style: italic;
        }
        
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid palette(mid);
            border-radius: 6px;
            margin: 15px 0;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: palette(text);
        }
        
        QPushButton {
            background-color: palette(button);
            border: 1px solid palette(mid);
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 13px;
            min-width: 80px;
        }
        
        QPushButton:hover {
            background-color: palette(light);
            border-color: palette(highlight);
        }
        
        QPushButton:pressed {
            background-color: palette(dark);
        }
        
        QPushButton:default {
            border: 2px solid palette(highlight);
            font-weight: bold;
        }
        
        QLineEdit, QSpinBox, QComboBox {
            border: 1px solid palette(mid);
            border-radius: 4px;
            padding: 6px;
            background-color: palette(base);
            selection-background-color: palette(highlight);
        }
        
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border-color: palette(highlight);
        }
        
        QCheckBox {
            spacing: 8px;
        }
        
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid palette(mid);
            border-radius: 3px;
            background-color: palette(base);
        }
        
        QCheckBox::indicator:checked {
            background-color: palette(highlight);
            border-color: palette(highlight);
        }
        
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        
        QFrame[frameShape="4"] {
            color: palette(mid);
        }
    )");
}

bool SettingsDialog::validateInputs()
{
    // Validate hotkey format (basic check)
    QString hotkey = m_hotkeyLineEdit->text().trimmed();
    if (!hotkey.isEmpty()) {
        if (!hotkey.contains("+") || hotkey.startsWith("+") || hotkey.endsWith("+")) {
            QMessageBox::warning(this, "Invalid Hotkey", 
                                "Hotkey must be in format 'Modifier+Key' (e.g., 'Meta+V')");
            return false;
        }
    }
    
    return true;
}

void SettingsDialog::applySettings()
{
    if (!validateInputs()) {
        return;
    }
    
    saveConfiguration();
    emit settingsApplied();
    
    // Show brief confirmation
    m_applyButton->setText("Applied!");
    m_applyButton->setEnabled(false);
    
    QTimer::singleShot(1500, [this]() {
        m_applyButton->setText("Apply");
        m_applyButton->setEnabled(true);
    });
}

void SettingsDialog::resetToDefaults()
{
    if (QMessageBox::question(this, "Reset Settings", 
                             "Are you sure you want to reset all settings to defaults?",
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        m_historyLimitSpinBox->setValue(50);
        m_startMinimizedCheckBox->setChecked(false);     // autostart
        m_showTrayIconCheckBox->setChecked(true);        // showNotifications
        m_hotkeyLineEdit->setText("Meta+V");
    }
}

void SettingsDialog::onOkClicked()
{
    if (validateInputs()) {
        saveConfiguration();
        emit settingsApplied();
        accept();
    }
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

void SettingsDialog::onApplyClicked()
{
    applySettings();
}

void SettingsDialog::onTestHotkeyClicked()
{
    QString hotkey = m_hotkeyLineEdit->text().trimmed();
    if (hotkey.isEmpty()) {
        QMessageBox::information(this, "Test Hotkey", "Please enter a hotkey combination first.");
        return;
    }
    
    // For now, just show a message. In a real implementation, 
    // this would test the actual hotkey registration
    QMessageBox::information(this, "Test Hotkey", 
                           QString("Testing hotkey: %1\n\n"
                                  "This feature would test if the hotkey can be registered.\n"
                                  "Current implementation shows format validation only.").arg(hotkey));
}