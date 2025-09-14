#include "about_dialog.h"
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QFont>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>

AboutDialog::AboutDialog(int clipboardCount, QWidget* parent)
    : QDialog(parent)
    , m_clipboardCount(clipboardCount)
    , m_closeButton(nullptr)
    , m_mainLayout(nullptr)
{
    setupUI();
    applyGlassDesign();
    
    // Set dialog properties for glass design
    setWindowTitle("About Clipboard Manager");
    setWindowIcon(QIcon::fromTheme("help-about"));
    setModal(true);
    setFixedSize(500, 600);
    
    // Make window frameless with translucent background for glass effect
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    
    // Add drop shadow effect
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(25);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 8);
    setGraphicsEffect(shadowEffect);
    
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
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create main container with glass background
    QWidget* container = new QWidget();
    container->setObjectName("glassContainer");
    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(25);
    containerLayout->setContentsMargins(40, 30, 40, 30);
    
    // Add header with close button
    QWidget* headerWidget = new QWidget();
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    // Add stretch to push close button to the right
    headerLayout->addStretch();
    
    // Close button
    m_closeButton = new QPushButton("✕");
    m_closeButton->setObjectName("closeButton");
    m_closeButton->setFixedSize(35, 35);
    m_closeButton->setToolTip("Close");
    connect(m_closeButton, &QPushButton::clicked, this, &AboutDialog::accept);
    headerLayout->addWidget(m_closeButton);
    
    containerLayout->addWidget(headerWidget);
    
    // Add sections with proper spacing
    containerLayout->addWidget(createHeaderSection());
    containerLayout->addWidget(createInfoSection());
    containerLayout->addWidget(createStatsSection());
    
    containerLayout->addStretch();
    
    // Add footer
    containerLayout->addLayout(createFooter());
    
    m_mainLayout->addWidget(container);
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

void AboutDialog::applyGlassDesign()
{
    setStyleSheet(R"(
        QWidget#glassContainer {
            background: rgba(255, 255, 255, 0.85);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 15px;
        }
        
        QPushButton#closeButton {
            background: rgba(255, 255, 255, 0.8);
            border: 1px solid rgba(0, 0, 0, 0.1);
            border-radius: 17px;
            color: #e74c3c;
            font-size: 16px;
            font-weight: bold;
        }
        
        QPushButton#closeButton:hover {
            background: rgba(231, 76, 60, 0.1);
            border: 1px solid #e74c3c;
        }
        
        QPushButton#closeButton:pressed {
            background: rgba(231, 76, 60, 0.2);
        }
        
        #titleLabel {
            font-size: 32px;
            font-weight: bold;
            color: #2c3e50;
            margin: 10px 0;
            background: transparent;
        }
        
        #versionLabel {
            font-size: 18px;
            color: #3498db;
            font-weight: 600;
            background: transparent;
        }
        
        #subtitleLabel {
            font-size: 16px;
            color: #7f8c8d;
            margin-bottom: 15px;
            font-style: italic;
            background: transparent;
        }
        
        #sectionTitle {
            font-size: 18px;
            font-weight: bold;
            color: #2c3e50;
            margin: 8px 0;
            background: transparent;
        }
        
        #featuresList {
            font-size: 14px;
            color: #2c3e50;
            line-height: 1.6;
            padding: 15px;
            background: rgba(255, 255, 255, 0.6);
            border-radius: 10px;
            border-left: 4px solid #3498db;
            border: 1px solid rgba(52, 152, 219, 0.3);
        }
        
        #techInfo {
            font-size: 13px;
            color: #7f8c8d;
            line-height: 1.5;
            background: transparent;
        }
        
        #statsGrid {
            background: rgba(255, 255, 255, 0.6);
            border-radius: 12px;
            padding: 20px;
            border: 1px solid rgba(52, 152, 219, 0.3);
        }
        
        #statsLabel {
            font-size: 14px;
            color: #2c3e50;
            font-weight: 600;
            background: transparent;
        }
        
        #statsValue {
            font-size: 16px;
            color: #3498db;
            font-weight: bold;
            background: transparent;
        }
        
        #footerText {
            font-size: 12px;
            color: #95a5a6;
            background: transparent;
        }
        
        QLabel {
            background: transparent;
        }
    )");
}

void AboutDialog::onCloseClicked()
{
    accept();
}