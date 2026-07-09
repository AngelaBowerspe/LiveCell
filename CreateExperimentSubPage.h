#ifndef CREATEEXPERIMENTSUBPAGE_H
#define CREATEEXPERIMENTSUBPAGE_H

#include "models/CreateExperimentSettings.h"

#include <QButtonGroup>
#include <QWidget>

namespace Ui {
class CreateExperimentSubPage;
}

class CreateExperimentSubPage : public QWidget
{
    Q_OBJECT

public:
    explicit CreateExperimentSubPage(QWidget *parent = nullptr);
    ~CreateExperimentSubPage();

    CreateExperimentSettings experimentSettings() const;

signals:
    void experimentSettingsChanged(const CreateExperimentSettings &settings);
    void closeRequested();
    void choosePlateFieldsRequested();

private:
    void initControls();
    void initConnections();
    void updatePageIndicator();
    void emitExperimentSettingsChanged();
    QString selectedExperimentType() const;
    QString selectedScanChannelText() const;
    QString selectedFocusChannelText() const;

private:
    Ui::CreateExperimentSubPage *m_pUi;
    QButtonGroup *m_pExperimentTypeGroup;
    QButtonGroup *m_pScanModeGroup;
};

#endif // CREATEEXPERIMENTSUBPAGE_H
