#pragma once

#include <QString>

class FontManager
{
public:
    static const QStringList &fontNames();

    static bool generateFonts(
        const QString &sourcePath,
        const QString &outputDirectory,
        QString &errorMessage
    );

    static bool resetFonts(
        const QString &outputDirectory,
        QString &errorMessage
    );
};
