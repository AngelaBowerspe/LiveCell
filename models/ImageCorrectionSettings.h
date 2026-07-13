#ifndef IMAGECORRECTIONSETTINGS_H
#define IMAGECORRECTIONSETTINGS_H

struct ImageCorrectionSettings
{
    bool flatFieldEnabled = true;
    bool brightnessBalanceEnabled = true;
    int threshold = 50;
    int histogramMin = 0;
    int histogramMax = 255;
};

#endif // IMAGECORRECTIONSETTINGS_H
