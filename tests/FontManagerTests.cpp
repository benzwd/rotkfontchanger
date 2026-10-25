
#include <QtTest/QtTest>

#include "FontManager.hpp"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

class FontManagerTests : public QObject
{
    Q_OBJECT

private slots:
    void hasExactly19Fonts();
    void hasCorrectArialFilename();
    void generateCreates19Files();
    void generateReplacesExistingFiles();
    void resetRestoresOriginalFonts();
};

void FontManagerTests::hasExactly19Fonts()
{
    QCOMPARE(FontManager::fontNames().size(), 19);
}

void FontManagerTests::hasCorrectArialFilename()
{
    QVERIFY(FontManager::fontNames().contains("ARIALUNI.TTF"));
    QVERIFY(!FontManager::fontNames().contains("ARIALUNI.ttf"));
}

void FontManagerTests::generateCreates19Files()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());

    const QString source = ":/default-fonts/Roboto-Regular.ttf";
    QString error;

    QVERIFY2(
        FontManager::generateFonts(source, temp.path(), error),
        qPrintable(error)
    );

    QDir dir(temp.path());

    for (const QString &name : FontManager::fontNames()) {
        QFile generated(dir.filePath(name));
        QFile original(source);

        QVERIFY(generated.open(QIODevice::ReadOnly));
        QVERIFY(original.open(QIODevice::ReadOnly));

        QCOMPARE(generated.readAll(), original.readAll());
    }
}

void FontManagerTests::generateReplacesExistingFiles()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());

    QString error;

    const QString first = ":/default-fonts/Roboto-Regular.ttf";
    const QString second = ":/default-fonts/Montserrat-Bold.ttf";

    QVERIFY2(
        FontManager::generateFonts(first, temp.path(), error),
        qPrintable(error)
    );

    QVERIFY2(
        FontManager::generateFonts(second, temp.path(), error),
        qPrintable(error)
    );

    for (const QString &name : FontManager::fontNames()) {
        QFile generated(QDir(temp.path()).filePath(name));
        QFile expected(second);

        QVERIFY(generated.open(QIODevice::ReadOnly));
        QVERIFY(expected.open(QIODevice::ReadOnly));

        QCOMPARE(generated.readAll(), expected.readAll());
    }
}

void FontManagerTests::resetRestoresOriginalFonts()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());

    QString error;

    QVERIFY2(
        FontManager::generateFonts(
            ":/default-fonts/Roboto-Regular.ttf",
            temp.path(),
            error
        ),
        qPrintable(error)
    );

    QVERIFY2(
        FontManager::resetFonts(temp.path(), error),
        qPrintable(error)
    );

    for (const QString &name : FontManager::fontNames()) {
        QFile restored(QDir(temp.path()).filePath(name));
        QFile original(":/default-fonts/" + name);

        QVERIFY(restored.open(QIODevice::ReadOnly));
        QVERIFY(original.open(QIODevice::ReadOnly));

        QCOMPARE(restored.readAll(), original.readAll());
    }
}

QTEST_GUILESS_MAIN(FontManagerTests)

#include "FontManagerTests.moc"
