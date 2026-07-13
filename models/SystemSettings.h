#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <QColor>
#include <QString>

struct SystemSettings
{
    QString storagePath;
    QString language = QStringLiteral("中文");
    QString imageFormat = QStringLiteral("JPG");
    int videoFrameRate = 5;
    double brightFieldRatio = 1.0;
    double blueRatio = 0.25;
    double greenRatio = 0.25;
    double redRatio = 0.25;

    QString plateType = QStringLiteral("自由区域扫描");
    QString plateSubType = QStringLiteral("A");
    double wellAreaLength = 10000.0;
    double wellAreaWidth = 10000.0;
    double firstWellX = 0.0;
    double firstWellY = 0.0;
    double wellPitchX = 10000.0;
    double wellPitchY = 10000.0;
    double plateRotation = 0.0;
    double fieldOverlap = 10.0;

    bool scaleEnabled = true;
    double scaleLength = 100.0;
    double scaleLineWidth = 2.0;
    double scaleFontSize = 12.0;
    QColor scaleColor = Qt::red;

    int cellSegmentationLevel = 1;
    int cellEdgeWidth = 0;
    int cellDiameterLength = 50;
    int cellAreaThreshold = 0;
    int cellBrightnessThreshold = 0;
};

#endif // SYSTEMSETTINGS_H
