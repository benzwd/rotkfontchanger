#include "MainWindow.h"
#include "FontManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QFontDatabase>
#include <QFont>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFontInfo>
#include <QSettings>
#include <QStatusBar>
#include <QLabel>
#include <QFile>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QWindow>
#include <QStyle>
#include <QFrame>
#include <QSizePolicy>
#include <QSvgWidget>

class RotkTitleBar : public QWidget {
public:
    using QWidget::QWidget;
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton && window()->windowHandle()) {
            window()->windowHandle()->startSystemMove();
            event->accept();
            return;
        }
        QWidget::mousePressEvent(event);
    }
};

static QString detectGameInstallation()
{
    const QString defaultPath = "C:/Games/ROTK";

    if (QDir(defaultPath).exists("UI/Resource/Fonts")) {
        return defaultPath;
    }

    return {};
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ROTK Font Changer");
    resize(560, 460);
    setMinimumSize(560, 460);
    setObjectName("rootWindow");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFont(QFont("Arial", 10));
    setAcceptDrops(true);

    QFile styleFile(":/style.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    else
        qWarning() << "Cannot load" << styleFile.fileName();

    auto *windowLayout = new QVBoxLayout(this);
    windowLayout->setContentsMargins(1, 1, 1, 1);
    windowLayout->setSpacing(0);

    auto *titleBar = new RotkTitleBar(this);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(52);
    auto *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(9, 0, 0, 0);
    titleLayout->setSpacing(0);
    auto *logoContainer = new QWidget(titleBar);
    logoContainer->setFixedSize(95, 48);

    auto *logoLayout = new QVBoxLayout(logoContainer);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    logoLayout->setSpacing(0);

    auto *logo = new QSvgWidget(":/rotk-logo.svg", logoContainer);
    logo->setFixedSize(95, 35);

    auto *unofficial = new QLabel("UNOFFICIAL", logoContainer);
    unofficial->setObjectName("unofficialLabel");
    unofficial->setAlignment(Qt::AlignCenter);
    unofficial->setFixedHeight(12);

    auto *separator = new QLabel("│", titleBar);
    separator->setObjectName("separator");
    auto *appName = new QLabel("FONT CHANGER", titleBar);
    appName->setObjectName("appName");
    auto *versionLabel = new QLabel(QString("by Feniix (v%1)").arg(ROTK_APP_VERSION), titleBar);
    versionLabel->setObjectName("versionLabel");
    auto *minimizeButton = new QPushButton("−", titleBar);
    minimizeButton->setObjectName("minimizeButton");
    minimizeButton->setFixedSize(44, 52);
    auto *closeButton = new QPushButton("×", titleBar);
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(44, 52);

    logoLayout->addWidget(logo, 0, Qt::AlignHCenter);
    logoLayout->addWidget(unofficial, 0, Qt::AlignHCenter);
    titleLayout->addWidget(logoContainer);

    titleLayout->addSpacing(9);
    titleLayout->addWidget(separator);

    titleLayout->addSpacing(9);
    titleLayout->addWidget(appName);
    titleLayout->addSpacing(10);
    titleLayout->addWidget(versionLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeButton);
    titleLayout->addWidget(closeButton);
    windowLayout->addWidget(titleBar);
    connect(minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    auto *content = new QWidget(this);
    content->setObjectName("content");
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(18, 16, 18, 17);
    layout->setSpacing(10);
    windowLayout->addWidget(content, 1);

    auto *eyebrow = new QLabel("CUSTOMIZATION&nbsp;&nbsp;&nbsp;<span style='color:#e00000;'>━━━</span>&nbsp;&nbsp;&nbsp;FONTS", content);
    eyebrow->setObjectName("eyebrow");
    layout->addWidget(eyebrow);
    auto *headline = new QLabel("YOUR FONT. <span style='color:#e00000;'>YOUR GAME.</span>", content);
    headline->setObjectName("headline");
    layout->addWidget(headline);
    auto *subtitle = new QLabel("Customize the ROTK experience with your own font.", content);
    subtitle->setObjectName("subtitle");
    layout->addWidget(subtitle);

    auto *previewPanel = new QWidget(content);
    previewPanel->setObjectName("previewPanel");
    auto *panelLayout = new QVBoxLayout(previewPanel);
    panelLayout->setContentsMargins(14, 11, 14, 12);
    panelLayout->setSpacing(6);
    auto *previewCaption = new QLabel("FONT PREVIEW", previewPanel);
    previewCaption->setObjectName("previewCaption");
    panelLayout->addWidget(previewCaption);
    previewLabel = new QLabel("King of the Kill", previewPanel);
    previewLabel->setObjectName("preview");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setMinimumHeight(53);
    panelLayout->addWidget(previewLabel, 1);
    auto *line = new QFrame(previewPanel);
    line->setObjectName("panelDivider");
    line->setFrameShape(QFrame::HLine);
    panelLayout->addWidget(line);
    auto *fontRow = new QHBoxLayout();
    fontRow->setSpacing(8);
    fontNameLabel = new QLabel("Selected font: No font selected", previewPanel);
    fontNameLabel->setObjectName("fontNameLabel");
    fontNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    browseButton = new QPushButton("BROWSE FONT", previewPanel);
    browseButton->setObjectName("browseButton");
    fontRow->addWidget(fontNameLabel, 1);
    fontRow->addWidget(browseButton);
    panelLayout->addLayout(fontRow);
    layout->addWidget(previewPanel, 1);

    dropLabel = new QLabel("Drop a .ttf file anywhere in this window", content);
    dropLabel->setObjectName("dropLabel");
    dropLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(dropLabel);

    auto *actions = new QHBoxLayout();
    actions->setSpacing(8);
    resetButton = new QPushButton("RESTORE DEFAULTS", content);
    resetButton->setObjectName("resetButton");
    generateButton = new QPushButton("APPLY FONT     →", content);
    generateButton->setObjectName("generate");
    generateButton->setEnabled(false);
    resetButton->setFixedHeight(43);
    generateButton->setFixedHeight(43);
    actions->addWidget(resetButton, 1);
    actions->addWidget(generateButton, 1);
    layout->addLayout(actions);

    auto *footer = new QWidget(this);
    footer->setObjectName("footer");
    footer->setFixedHeight(58);

    auto *footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(15, 0, 15, 0);
    footerLayout->setSpacing(10);

    auto *ready = new QLabel("●  NOT CONFIGURED", footer);
    ready->setObjectName("ready");
    ready->setMinimumWidth(115);

    gamePathEdit = new QLineEdit(footer);
    gamePathEdit->setObjectName("gamePathEdit");
    gamePathEdit->setPlaceholderText("Select your ROTK installation folder...");
    gamePathEdit->setMinimumWidth(0);

    auto updateStatus = [this, ready]() {
        const bool gameFound = !gameFontsDirectory().isEmpty();

        ready->setText(gameFound ? "●  READY" : "●  NOT FOUND");
        ready->setProperty("state", gameFound ? "ready" : "missing");

        resetButton->setEnabled(gameFound);
        generateButton->setEnabled(gameFound && !selectedFontPath.isEmpty());

        ready->style()->unpolish(ready);
        ready->style()->polish(ready);
    };

    connect(gamePathEdit, &QLineEdit::textChanged,
            this, updateStatus);

    updateStatus();

    gameBrowseButton = new QPushButton("BROWSE", footer);
    gameBrowseButton->setObjectName("gameBrowseButton");
    gameBrowseButton->setFixedHeight(32);

    footerLayout->addWidget(ready);
    footerLayout->addWidget(gamePathEdit, 1);
    footerLayout->addWidget(gameBrowseButton);

    windowLayout->addWidget(footer);

    QSettings settings("ROTKFontChanger", "ROTKFontChanger");

    QString gamePath = settings.value("gameInstallation").toString();

    if (!QDir(gamePath).exists("UI/Resource/Fonts")) {
        gamePath = detectGameInstallation();
    }

    gamePathEdit->setText(gamePath);

    connect(gamePathEdit, &QLineEdit::editingFinished,
            this, [this]() {
                QSettings settings("ROTKFontChanger", "ROTKFontChanger");
                settings.setValue(
                    "gameInstallation",
                    gamePathEdit->text().trimmed()
                    );
            });

    connect(gameBrowseButton, &QPushButton::clicked,
            this, &MainWindow::browseGameInstallation);

    connect(browseButton, &QPushButton::clicked,
            this, &MainWindow::selectFont);

    connect(generateButton, &QPushButton::clicked,
            this, &MainWindow::generateFonts);

    connect(resetButton, &QPushButton::clicked,
            this, &MainWindow::resetFonts);
}



void MainWindow::loadFont(const QString &path)
{
    QFileInfo info(path);

    if (!info.isFile() ||
        info.suffix().compare(
            "ttf", Qt::CaseInsensitive
            ) != 0)
    {
        QMessageBox::warning(
            this,
            "Invalid file",
            "Please select a valid .ttf file."
            );
        return;
    }

    int fontId = QFontDatabase::addApplicationFont(path);

    if (fontId == -1) {
        QMessageBox::warning(
            this,
            "Error",
            "Unable to load this font."
            );
        return;
    }

    QStringList families =
        QFontDatabase::applicationFontFamilies(fontId);

    if (families.isEmpty()) {
        QFontDatabase::removeApplicationFont(fontId);
        return;
    }

    QString family = families.first();

    QFont previewFont(family);
    previewFont.setPixelSize(40);
    previewFont.setWeight(QFont::Normal);
    previewFont.setStyleStrategy(QFont::PreferMatch);

    if (loadedFontId != -1) {
        QFontDatabase::removeApplicationFont(loadedFontId);
    }

    loadedFontId = fontId;
    selectedFontPath = path;

    previewLabel->setFont(previewFont);

    QFontInfo fontInfo(previewLabel->font());

    previewLabel->setText("King of the Kill");

    fontNameLabel->setText("Selected font: " + info.fileName());
    dropLabel->setText("Selected: " + info.fileName());

    generateButton->setEnabled(!gameFontsDirectory().isEmpty());
}


void MainWindow::selectFont()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        "Select a font",
        QString(),
        "TrueType Fonts (*.ttf)"
        );

    if (!path.isEmpty()) {
        loadFont(path);
    }
}

void MainWindow::generateFonts()
{
    if (selectedFontPath.isEmpty()) {
        QMessageBox::warning(
            this,
            "Error",
            "Please select a font first."
            );
        return;
    }

    QString outputDir = gameFontsDirectory();

    if (outputDir.isEmpty()) {
        QMessageBox::warning(
            this,
            "Game installation",
            "Please select a valid ROTK fonts directory."
            );
        return;
    }

    auto confirmation = QMessageBox::warning(
        this,
        "Apply Font",
        "Apply this font to ROTK?\n\n"
        "This will replace the current game fonts.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (confirmation != QMessageBox::Yes)
        return;

    QString errorMessage;

    bool success = FontManager::generateFonts(
        selectedFontPath,
        outputDir,
        errorMessage
        );

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Font files generated successfully!"
            );
    } else {
        QMessageBox::critical(
            this,
            "Generation failed",
            errorMessage
            );
    }
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mime = event->mimeData();

    if (!mime->hasUrls() || mime->urls().size() != 1) {
        event->ignore();
        return;
    }

    QUrl url = mime->urls().constFirst();

    if (url.isLocalFile() &&
        url.toLocalFile().endsWith(
            ".ttf", Qt::CaseInsensitive
            ))
    {
        event->acceptProposedAction();
        dropLabel->setProperty("dragActive", true);
        dropLabel->style()->unpolish(dropLabel);
        dropLabel->style()->polish(dropLabel);
    } else {
        event->ignore();
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    dropLabel->setProperty("dragActive", false);
    dropLabel->style()->unpolish(dropLabel);
    dropLabel->style()->polish(dropLabel);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    dropLabel->setProperty("dragActive", false);
    dropLabel->style()->unpolish(dropLabel);
    dropLabel->style()->polish(dropLabel);
    const QMimeData *mime = event->mimeData();

    if (mime->hasUrls() && mime->urls().size() == 1) {
        QUrl url = mime->urls().constFirst();

        if (url.isLocalFile()) {
            loadFont(url.toLocalFile());
            event->acceptProposedAction();
        }
    }

}

void MainWindow::resetFonts()
{
    QString outputDir = gameFontsDirectory();

    if (outputDir.isEmpty()) {
        QMessageBox::warning(
            this,
            "Game installation",
            "Please select a valid ROTK fonts directory."
            );
        return;
    }

    auto confirmation = QMessageBox::warning(
        this,
        "Reset to Default",
        "This will replace font files in:\n\n"
            + outputDir +
            "\n\nExisting files will be overwritten.\n"
            "Continue?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (confirmation != QMessageBox::Yes)
        return;

    QString errorMessage;

    bool success = FontManager::resetFonts(
        outputDir,
        errorMessage
        );

    if (success) {
        QMessageBox::information(
            this,
            "Reset complete",
            "Default fonts restored successfully!"
            );
    } else {
        QMessageBox::critical(
            this,
            "Reset failed",
            errorMessage
            );
    }
}

void MainWindow::browseGameInstallation()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Select ROTK installation folder",
        gamePathEdit->text().isEmpty()
            ? QDir::homePath()
            : gamePathEdit->text()
        );

    if (folder.isEmpty())
        return;

    gamePathEdit->setText(folder);

    QSettings settings(
        "ROTKFontChanger",
        "ROTKFontChanger"
        );

    settings.setValue("gameInstallation", folder);
}

QString MainWindow::gameFontsDirectory() const
{
    QString installationPath =
        gamePathEdit->text().trimmed();

    if (installationPath.isEmpty())
        return {};

    QDir gameDir(installationPath);

    if (!gameDir.exists())
        return {};

    const QString fontsRelativePath =
        "UI/Resource/Fonts";

    QString fontsPath =
        gameDir.filePath(fontsRelativePath);

    QDir fontsDir(fontsPath);

    if (!fontsDir.exists())
        return {};

    return fontsDir.absolutePath();
}
