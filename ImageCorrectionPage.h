#ifndef IMAGECORRECTIONPAGE_H
#define IMAGECORRECTIONPAGE_H

#include "models/ImageCorrectionSettings.h"

#include <QLineEdit>
#include <QWidget>

namespace Ui {
class ImageCorrectionPage;
}

class ImageCorrectionPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCorrectionPage(QWidget *parent = nullptr);
    ~ImageCorrectionPage();

    ImageCorrectionSettings correctionSettings() const;

signals:
    void correctionSettingsChanged();
    void flatFieldCollectRequested();
    void histogramAutoRequested();
    void cameraCalibrationRequested();
    void calibrationRequested();
    void measurementRequested();

private:
    void initValidators();
    void initConnections();
    void syncHistogramRange(int lowerValue, int upperValue);
    int lineEditIntValue(const QLineEdit *pLineEdit) const;

private:
    Ui::ImageCorrectionPage *m_pUi;
};

#endif // IMAGECORRECTIONPAGE_H
