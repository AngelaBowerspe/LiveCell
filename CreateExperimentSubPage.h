#ifndef CREATEEXPERIMENTSUBPAGE_H
#define CREATEEXPERIMENTSUBPAGE_H

#include <QButtonGroup>
#include <QEvent>
#include <QPoint>
#include <QString>
#include <QWidget>

namespace Ui {
class CreateExperimentSubPage;
}

class CreateExperimentSubPage : public QWidget
{
    Q_OBJECT

public:
    enum class AcceptedPage
    {
        CreateMode,
        ExperimentType,
        DelaySetting,
        PlateField,
        ScanParameter,
        Summary
    };
    Q_ENUM(AcceptedPage)

    explicit CreateExperimentSubPage(QWidget *parent = nullptr);
    ~CreateExperimentSubPage();

    QString experimentName() const;
    QString experimentType() const;
    QString scanChannelText() const;
    QString scanModeText() const;
    QString intervalTimeText() const;
    QString loopCountText() const;
    QString fieldOverlapText() const;
    QString focusIntervalText() const;
    QString focusChannelText() const;
    void showCenteredIn(QWidget *container);
    void resetToFirstPage();
    void setPlateFieldSelectionSummary(const QString &plateType,
        const QString &selectedWells,
        const QString &selectedGroups,
        const QString &selectedFields);

signals:
    void experimentPageAccepted(CreateExperimentSubPage::AcceptedPage page);
    void experimentFinished();
    void choosePlateFieldsRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initControls();
    void initConnections();
    void centerInContainer(QWidget *container);
    void updatePageIndicator();
    void updateSummary();
    void updateDelaySettingState();
    void updateAutoCycleCount();
    void resetPlateFieldSelectionSummary();
    bool isLoopScanMode() const;
    int intervalDurationMinutes() const;
    int totalDurationMinutes() const;
    int calculatedCycleCount() const;
    void goPreviousPage();
    void goNextPage();
    bool validateCurrentPage();
    bool validateFirstPage();
    bool validateExperimentTypePage();
    bool validatePlateFieldPage();
    bool validateScanParameterPage();
    QString checkedButtonText(const QButtonGroup *buttonGroup) const;
    QString checkedChannelText() const;
    QString checkedFocusChannelText() const;

    Ui::CreateExperimentSubPage *ui;
    QButtonGroup *m_pCreateModeButtonGroup;
    QButtonGroup *m_pExperimentTypeButtonGroup;
    QButtonGroup *m_pScanModeButtonGroup;
    bool m_bDragging;
    QPoint m_dragOffset;
};

#endif // CREATEEXPERIMENTSUBPAGE_H
