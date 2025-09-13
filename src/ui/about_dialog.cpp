#include "about_dialog.h"
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QFont>
#include <QDateTime>

AboutDialog::AboutDialog(int clipboardCount, QWidget* parent)
    : QDialog(parent)
    , m_clipboardCount(clipboardCount)
    , m_closeButton(nullptr)
    , m_mainLayout(nullptr)
{
    setupUI();
    applyModernStyling();
    
    // Set dialog properties
    setWindowTitle("About Clipboard Manager");
    setWindowIcon(QIcon::fromTheme("help-about"));
    setModal(true);
    setFixedSize(450, 500);
    
    // Center on screen
    if (QScreen* screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.center() - rect().center());
    }
}

AboutDialog::~AboutDialog()
{
    // QDialog handles cleanup of child widgets
}

void AboutDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Add sections
    m_mainLayout->addWidget(createHeaderSection());
    m_mainLayout->addWidget(createInfoSection());
    m_mainLayout->addWidget(createStatsSection());
    
    m_mainLayout->addStretch();
    
    // Add footer
    m_mainLayout->addLayout(createFooter());
}

QWidget* AboutDialog::createHeaderSection()
{
    QWidget* header = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(header);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignCenter);
    
    // Application icon (using system icon or emoji as fallback)
    QLabel* iconLabel = new QLabel();
    QIcon appIcon = QIcon::fromTheme("edit-paste");
    if (!appIcon.isNull()) {
        iconLabel->setPixmap(appIcon.pixmap(64, 64));
    } else {
        iconLabel->setText("📋");
        iconLabel->setAlignment(Qt::AlignCenter);
        QFont iconFont = iconLabel->font();
        iconFont.setPointSize(48);
        iconLabel->setFont(iconFont);
    }
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);
    
    // Application title
    QLabel* titleLabel = new QLabel("Clipboard History Manager");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Version
    QLabel* versionLabel = new QLabel("Version 1.0.0");
    versionLabel->setObjectName("versionLabel");
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);
    
    // Subtitle
    QLabel* subtitleLabel = new QLabel("A lightweight, modern clipboard manager for Linux");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    layout->addWidget(subtitleLabel);
    
    return header;
}

QWidget* AboutDialog::createInfoSection()
{
    QWidget* info = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(info);
    layout->setSpacing(15);
    
    // Features section
    QLabel* featuresTitle = new QLabel("✨ Features");
    featuresTitle->setObjectName("sectionTitle");
    layout->addWidget(featuresTitle);
    
    QLabel* featuresText = new QLabel(
        "• Global hotkey support (Meta+V)\n"
        "• System tray integration\n"
        "• Pin important clipboard items\n"
        "• Configurable history limit\n"
        "• Cross-platform X11 and Wayland support\n"
        "• Modern Qt6 interface\n"
        "• Minimal memory footprint\n"
        "• JSON-based configuration"
    );
    featuresText->setObjectName("featuresList");
    featuresText->setWordWrap(true);
    layout->addWidget(featuresText);
    
    // Technical info
    QLabel* techTitle = new QLabel("🔧 Technical Information");
    techTitle->setObjectName("sectionTitle");
    layout->addWidget(techTitle);
    
    QLabel* techText = new QLabel(
        QString("Built with Qt %1 and C++20\n"
                "Licensed under MIT License\n"
                "Copyright © 2025 Abdullah Al Mahmud")
                .arg(QT_VERSION_STR)
    );
    techText->setObjectName("techInfo");
    techText->setWordWrap(true);
    layout->addWidget(techText);
    
    return info;
}

QWidget* AboutDialog::createStatsSection()
{
    QWidget* stats = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(stats);
    layout->setSpacing(10);
    
    QLabel* statsTitle = new QLabel("📊 Current Session");
    statsTitle->setObjectName("sectionTitle");
    layout->addWidget(statsTitle);
    
    // Statistics grid
    QWidget* statsGrid = new QWidget();
    statsGrid->setObjectName("statsGrid");
    QVBoxLayout* gridLayout = new QVBoxLayout(statsGrid);
    gridLayout->setSpacing(8);
    
    // Clipboard items count
    QLabel* itemsLabel = new QLabel(QString("Clipboard Items: %1").arg(m_clipboardCount));
    itemsLabel->setObjectName("statItem");
    gridLayout->addWidget(itemsLabel);
    
    // Uptime (placeholder)
    QLabel* uptimeLabel = new QLabel("Session Started: " + QDateTime::currentDateTime().toString("MMM dd, yyyy hh:mm"));
    uptimeLabel->setObjectName("statItem");
    gridLayout->addWidget(uptimeLabel);
    
    // Platform info
    QLabel* platformLabel = new QLabel(QString("Platform: %1").arg(QSysInfo::prettyProductName()));
    platformLabel->setObjectName("statItem");
    gridLayout->addWidget(platformLabel);
    
    layout->addWidget(statsGrid);
    
    return stats;
}

QHBoxLayout* AboutDialog::createFooter()
{
    QHBoxLayout* footer = new QHBoxLayout();
    footer->addStretch();
    
    m_closeButton = new QPushButton("Close");
    m_closeButton->setDefault(true);
    m_closeButton->setMinimumWidth(100);
    
    connect(m_closeButton, &QPushButton::clicked, this, &AboutDialog::onCloseClicked);
    
    footer->addWidget(m_closeButton);
    
    return footer;
}

void AboutDialog::applyModernStyling()
{
    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                       stop: 0 palette(window),
                                       stop: 1 palette(base));
            border: 1px solid palette(mid);
            border-radius: 12px;
        }
        
        #titleLabel {
            font-size: 28px;
            font-weight: bold;
            color: palette(text);
            margin: 5px 0;
        }
        
        #versionLabel {
            font-size: 16px;
            color: palette(highlight);
            font-weight: 600;
        }
        
        #subtitleLabel {
            font-size: 14px;
            color: palette(mid);
            margin-bottom: 10px;
            font-style: italic;
        }
        
        #sectionTitle {
            font-size: 16px;
            font-weight: bold;
            color: palette(text);
            margin: 5px 0;
        }
        
        #featuresList {
            font-size: 13px;
            color: palette(text);
            line-height: 1.4;
            padding: 10px;
            background-color: palette(alternate-base);
            border-radius: 6px;
            border-left: 4px solid palette(highlight);
        }
        
        #techInfo {
            font-size: 12px;
            color: palette(mid);
            line-height: 1.3;
        }
        
        #statsGrid {
            background-color: palette(alternate-base);
            border-radius: 8px;
            padding: 15px;
            border: 1px solid palette(mid);
        }
        
        #statItem {
            font-size: 13px;
            color: palette(text);
            padding: 3px 0;
        }
        
        QPushButton {
            background-color: palette(button);
            border: 2px solid palette(highlight);
            border-radius: 6px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
            color: palette(text);
        }
        
        QPushButton:hover {
            background-color: palette(highlight);
            color: palette(highlighted-text);
        }
        
        QPushButton:pressed {
            background-color: palette(dark);
        }
    )");
}

void AboutDialog::onCloseClicked()
{
    accept();
}