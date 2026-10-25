
#include "FontManager.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringList>

const QStringList &FontManager::fontNames()
{
    static const QStringList names = {
        "ARIALUNI.TTF",
        "leelawad.ttf",
        "Montserrat-Bold.ttf",
        "Montserrat-ExtraBold.ttf",
        "Montserrat-Medium.ttf",
        "Montserrat-Regular.ttf",
        "Montserrat-SemiBold.ttf",
        "NanumBarunGothicBold.ttf",
        "NotoSansJP-Regular.ttf",
        "Oswald-Bold_0.ttf",
        "Oswald-Light_0.ttf",
        "Oswald-Regular_0.ttf",
        "Roboto-Bold.ttf",
        "Roboto-Light.ttf",
        "Roboto-Medium.ttf",
        "RobotoMono-Regular.ttf",
        "Roboto-Regular.ttf",
        "ROSA Verde Normal.ttf",
        "simhei.ttf"
    };

    return names;
}

// Writes a complete file before replacing its destination
static bool writeFile(
    const QString &sourcePath,
    const QString &destination,
    QString &errorMessage
)
{
    QFile source(sourcePath);

    if (!source.open(QIODevice::ReadOnly)) {
        errorMessage =
            "Cannot read source:\n" + sourcePath;
        return false;
    }

    QSaveFile output(destination);

    if (!output.open(QIODevice::WriteOnly)) {
        errorMessage =
            "Cannot open destination:\n" + destination;
        return false;
    }

    char buffer[65536];

    while (true) {
        qint64 bytesRead =
            source.read(buffer, sizeof(buffer));

        if (bytesRead < 0) {
            output.cancelWriting();
            errorMessage =
                "Error reading:\n" + sourcePath;
            return false;
        }

        if (bytesRead == 0)
            break;

        qint64 totalWritten = 0;

        while (totalWritten < bytesRead) {
            qint64 written = output.write(
                buffer + totalWritten,
                bytesRead - totalWritten
            );

            if (written <= 0) {
                output.cancelWriting();
                errorMessage =
                    "Error writing:\n" + destination;
                return false;
            }

            totalWritten += written;
        }
    }

    if (!output.commit()) {
        errorMessage =
            "Cannot save:\n" + destination;
        return false;
    }

    return true;
}


bool FontManager::generateFonts(
    const QString &sourcePath,
    const QString &outputDirectory,
    QString &errorMessage
)
{
    QFileInfo sourceInfo(sourcePath);
    QDir outputDir(outputDirectory);

    if (!sourceInfo.isFile() ||
        sourceInfo.suffix().compare(
            "ttf", Qt::CaseInsensitive
        ) != 0)
    {
        errorMessage = "Invalid source font.";
        return false;
    }

    if (!outputDir.exists()) {
        errorMessage = "Output directory does not exist.";
        return false;
    }

    // Check that the source is not one of the target files
    const QString sourceCanonical = sourceInfo.canonicalFilePath();

    for (const QString &name : fontNames()) {
        QFileInfo targetInfo(outputDir.filePath(name));

        if (targetInfo.exists() &&
            targetInfo.canonicalFilePath() == sourceCanonical)
        {
            errorMessage =
                "The source font cannot be one of the "
                "destination files.";
            return false;
        }
    }

    // Create or replace the 19 font files
    for (const QString &name : fontNames()) {
        QString destination = outputDir.filePath(name);

        if (!writeFile(
                sourcePath, destination, errorMessage))
        {
            return false;
        }
    }

    return true;
}


bool FontManager::resetFonts(
    const QString &outputDirectory,
    QString &errorMessage
)
{
    QDir outputDir(outputDirectory);

    if (!outputDir.exists()) {
        errorMessage = "Output directory does not exist.";
        return false;
    }

    // Verify all embedded fonts first
    for (const QString &name : fontNames()) {
        QString resourcePath =
            ":/default-fonts/" + name;

        if (!QFileInfo(resourcePath).isFile()) {
            errorMessage =
                "Missing default font:\n" + name;
            return false;
        }
    }

    for (const QString &name : fontNames()) {
        QString resourcePath =
            ":/default-fonts/" + name;

        QString destination =
            outputDir.filePath(name);

        if (!writeFile(
                resourcePath, destination, errorMessage))
        {
            return false;
        }
    }

    return true;
}
