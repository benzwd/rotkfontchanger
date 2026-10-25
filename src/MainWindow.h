#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QCheckBox>
#include <QLineEdit>

class QDragEnterEvent;
class QDropEvent;
class QDragLeaveEvent;

class MainWindow : public QWidget
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QLabel *fontNameLabel;
    QLabel *previewLabel;
    QLabel *dropLabel;

    QLineEdit *gamePathEdit;

    QPushButton *gameBrowseButton;
    QPushButton *browseButton;
    QPushButton *generateButton;
    QPushButton *resetButton;

    QString selectedFontPath;
    QString gameFontsDirectory() const;

    int loadedFontId = -1;

    void selectFont();
    void loadFont(const QString &path);
    void generateFonts();
    void resetFonts();
    void browseGameInstallation();
};
