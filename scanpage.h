#ifndef SCANPAGE_H
#define SCANPAGE_H

#include "models/CreateExperimentSettings.h"
#include "WellPlateWidget.h"

#include <QWidget>

class CreateExperimentSubPage;

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
    void showCreateExperimentPage();
    void setExperimentActionEnabled(bool enabled);
    void applyExperimentSettings(const CreateExperimentSettings &settings);
    void setPlateFormat(WellPlateWidget::PlateFormat format);
    static QString plateFormatText(WellPlateWidget::PlateFormat format);

private:
    Ui::ScanPage *ui;
    CreateExperimentSubPage *m_pCreateExperimentSubPage;
};

#endif // SCANPAGE_H
