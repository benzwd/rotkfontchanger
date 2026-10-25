
#include "MainWindow.hpp"
#include "FontManager.hpp"

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
#include <QDebug>
#include <QFontInfo>
#include <QSettings>
#include <QStatusBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ROTK Font Changer");
    resize(650, 480);
    QFont defaultFont("Arial", 14);
    setFont(defaultFont);
    setAcceptDrops(true);

    setStyleSheet(R"(
        QWidget {
            background-color: #101116;
            color: #f4f4f5;
        }

        QLabel#title {
            font-size: 26px;
            font-weight: bold;
        }

        QLabel#subtitle {
            color: #a1a1aa;
            font-size: 13px;
        }

        QLabel#preview {
            background-color: #1c1e27;
            border: 1px solid #363846;
            border-radius: 10px;
            padding: 25px;
        }

        QPushButton {
            background-color: #323442;
            border: none;
            border-radius: 8px;
            padding: 12px;
        }

        QPushButton:hover {
            background-color: #444657;
        }

        QPushButton#generate {
            background-color: #7655d6;
            font-weight: bold;
        }

        QPushButton#generate:hover {
            background-color: #8b6aee;
        }

        QPushButton#generate:disabled {
            background-color: #34313e;
            color: #888888;
        }

        QLabel#statusLabel {
            color: #777777;
            font-size: 11px;
            padding: 8px;
            border-top: 1px solid #333333;
        }

        QLineEdit#gamePathEdit {
            background-color: #1c1e27;
            color: #f5f5f5;
            border: 1px solid #363846;
            border-radius: 8px;
            padding: 10px 12px;
            selection-background-color: #7655d6;
            selection-color: #ffffff;
        }

        QLineEdit#gamePathEdit:hover {
            border: 1px solid #55566a;
        }

        QLineEdit#gamePathEdit:focus {
            border: 1px solid #8b5cf6;
            background-color: #222430;
        }

        QLineEdit#gamePathEdit:disabled {
            background-color: #17181e;
            color: #777777;
            border: 1px solid #292a33;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    // Title
    auto *title = new QLabel("ROTK Font Changer");
    title->setObjectName("title");

    auto *subtitle = new QLabel(
        "Customize your Z1 Battle Royale fonts"
    );
    subtitle->setObjectName("subtitle");

    layout->addWidget(title);
    layout->addWidget(subtitle);


    auto *gameTitle = new QLabel("GAME INSTALLATION");

    auto *gameLayout = new QHBoxLayout();

    gamePathEdit = new QLineEdit();
    gamePathEdit->setObjectName("gamePathEdit");
    gamePathEdit->setPlaceholderText(
        "Select your ROTK installation folder..."
    );

    gameBrowseButton = new QPushButton("Browse...");

    gameLayout->addWidget(gamePathEdit, 1);
    gameLayout->addWidget(gameBrowseButton);

    layout->addWidget(gameTitle);
    layout->addLayout(gameLayout);

    // Restore the last selected path
    QSettings settings("ROTKFontChanger", "ROTKFontChanger");

    gamePathEdit->setText(
        settings.value("gameInstallation").toString()
    );

    connect(
        gameBrowseButton,
        &QPushButton::clicked,
        this,
        &MainWindow::browseGameInstallation
    );

    connect(
        gamePathEdit,
        &QLineEdit::editingFinished,
        this,
        [this]() {
            QSettings settings(
                "ROTKFontChanger",
                "ROTKFontChanger"
            );

            settings.setValue(
                "gameInstallation",
                gamePathEdit->text().trimmed()
            );
        }
    );


    dropLabel = new QLabel(
        "Drop your .ttf font here\n"
        "or use the Browse button below"
    );

    dropLabel->setAlignment(Qt::AlignCenter);
    dropLabel->setMinimumHeight(100);
    dropLabel->setStyleSheet(R"(
        QLabel {
            background-color: #1c1e27;
            border: 2px dashed #7655d6;
            border-radius: 10px;
            color: #a1a1aa;
            padding: 15px;
        }
    )");

    layout->addWidget(dropLabel);

    // Font selection
    auto *fontLayout = new QHBoxLayout();

    fontNameLabel = new QLabel("No font selected");

    browseButton = new QPushButton("Browse...");

    fontLayout->addWidget(fontNameLabel, 1);
    fontLayout->addWidget(browseButton);

    layout->addLayout(fontLayout);

    // Font preview
    previewLabel = new QLabel("GAGNEZ");
    previewLabel->setObjectName("preview");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setMinimumHeight(140);

    layout->addWidget(previewLabel);

    layout->addStretch();

    // Generate button
    generateButton = new QPushButton("Apply");
    generateButton->setObjectName("generate");
    generateButton->setEnabled(false);

    layout->addWidget(generateButton);

    resetButton = new QPushButton("Reset to Default");

    layout->addWidget(resetButton);

    connect(
        resetButton,
        &QPushButton::clicked,
        this,
        &MainWindow::resetFonts
    );

    // Button connections
    connect(
        browseButton,
        &QPushButton::clicked,
        this,
        &MainWindow::selectFont
    );

    connect(
        generateButton,
        &QPushButton::clicked,
        this,
        &MainWindow::generateFonts
    );

    auto *statusLabel = new QLabel(
        QString("ROTK Font Changer v%1 | by Feniix")
            .arg(ROTK_APP_VERSION)
    );

    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(statusLabel);
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

    // Release previous custom font
    if (loadedFontId != -1) {
        QFontDatabase::removeApplicationFont(loadedFontId);
    }

    loadedFontId = fontId;
    selectedFontPath = path;

    previewLabel->setFont(previewFont);

    QFontInfo fontInfo(previewLabel->font());

    qDebug() << "Requested family:" << family;
    qDebug() << "Resolved family:" << fontInfo.family();
    qDebug() << "Exact match:" << fontInfo.exactMatch();

    previewLabel->setText("GAGNEZ");

    fontNameLabel->setText(info.fileName());
    dropLabel->setText("Selected: " + info.fileName());

    generateButton->setEnabled(true);

    qDebug() << "Font loaded:" << family;
    qDebug() << "Preview font:" << previewLabel->font().family();
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
        "Apply / Generate",
        "This will create or replace 19 font files in:\n\n"
            + outputDir +
        "\n\nExisting files will be overwritten "
        "without backup.\n\nContinue?",
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
            "19 font files generated successfully!"
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

    QUrl url = mime->urls().first();

    if (url.isLocalFile() &&
        url.toLocalFile().endsWith(
            ".ttf", Qt::CaseInsensitive
        ))
    {
        event->acceptProposedAction();

        dropLabel->setStyleSheet(R"(
            QLabel {
                background-color: #29233d;
                border: 2px dashed #a78bfa;
                border-radius: 10px;
                color: white;
                padding: 15px;
            }
        )");
    } else {
        event->ignore();
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();

    dropLabel->setStyleSheet(R"(
        QLabel {
            background-color: #1c1e27;
            border: 2px dashed #7655d6;
            border-radius: 10px;
            color: #a1a1aa;
            padding: 15px;
        }
    )");
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();

    if (mime->hasUrls() && mime->urls().size() == 1) {
        QUrl url = mime->urls().first();

        if (url.isLocalFile()) {
            loadFont(url.toLocalFile());
            event->acceptProposedAction();
        }
    }

    dropLabel->setStyleSheet(R"(
        QLabel {
            background-color: #1c1e27;
            border: 2px dashed #7655d6;
            border-radius: 10px;
            color: #a1a1aa;
            padding: 15px;
        }
    )");
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
        "This will replace the 19 font files in:\n\n"
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
            "19 default fonts restored successfully!"
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
        "UI/Fonts";

    QString fontsPath =
        gameDir.filePath(fontsRelativePath);

    QDir fontsDir(fontsPath);

    if (!fontsDir.exists())
        return {};

    return fontsDir.absolutePath();
}
