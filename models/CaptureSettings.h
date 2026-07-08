#ifndef CAPTURESETTINGS_H
#define CAPTURESETTINGS_H

#include <QVector>

enum class ObjectiveMagnification
{
    Objective10X,
    Objective20X
};

enum class CaptureChannel
{
    BrightField,
    Blue,
    Green,
    Red
};

struct CaptureSettings
{
    ObjectiveMagnification objective = ObjectiveMagnification::Objective10X;
    QVector<CaptureChannel> activeChannels;
    int lightIntensity = 50;
    bool autoExposure = true;
    int exposure = 500;
    int gain = 1;
    int contrast = 0;
    QVector<CaptureChannel> multiChannels;
};

#endif // CAPTURESETTINGS_H
