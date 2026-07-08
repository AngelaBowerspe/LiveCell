#ifndef SCANPAGE_H
#define SCANPAGE_H

#include "WellPlateWidget.h"

#include <QWidget>

namespace Ui {
class ScanPage;
}

class ScanPage : public QWidget
{
    Q_OBJECT

public:
    explicit ScanPage(QWidget *parent = nullptr);
    ~ScanPage();

signals:
    void createExperimentRequested();
    void editExperimentConfigRequested();
    void saveExperimentConfigRequested();
    void stopScanRequested();
    void browseDataRequested();
    void selectWellsRequested();
    void cancelWellSelectionRequested();
    void selectFieldsRequested();
    void cancelFieldSelectionRequested();
    void confirmSelectionRequested();
    void plateFormatChanged(WellPlateWidget::PlateFormat format);
    void groupChanged(int groupIndex);

private:
    void initControls();
    void initConnections();
    void setPlateFormat(WellPlateWidget::PlateFormat format);
    static QString plateFormatText(WellPlateWidget::PlateFormat format);

private:
    Ui::ScanPage *ui;
};

#endif // SCANPAGE_H
