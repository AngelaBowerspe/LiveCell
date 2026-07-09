#ifndef SCANPAGE_H
#define SCANPAGE_H

#include "CreateExperimentSubPage.h"
#include "FieldViewWidget.h"
#include "WellPlateWidget.h"

#include <QColor>
#include <QMap>
#include <QSet>
#include <QStringList>
#include <QVector>
#include <QWidget>

class QTimer;

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
    void applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage page);
    void enablePlateFieldSelection();
    void beginWellSelection();
    void cancelWellSelection();
    void confirmWellSelection();
    void beginFieldSelection();
    void cancelFieldSelection();
    void confirmFieldSelection();
    void handleWellClicked(const QString &well);
    void toggleScan();
    void runNextMockScanStep();
    void stopMockScan();
    void updateGroupColorButton();
    void updatePlateFieldControls();
    void setExperimentActionEnabled(bool enabled);
    void setPlateFormat(WellPlateWidget::PlateFormat format);
    bool hasSelectedFieldsForActiveWell() const;
    bool buildMockScanPlan();
    void restoreFieldsForActiveWell();
    void showCreateExperimentSelectionPage();
    void updateCreateExperimentPlateFieldSummary();
    QColor currentGroupColor() const;
    static QColor groupColor(int groupIndex);
    static QString plateFormatText(WellPlateWidget::PlateFormat format);

private:
    Ui::ScanPage *ui;
    CreateExperimentSubPage *m_pCreateExperimentSubPage;
    QTimer *m_pMockScanTimer;
    bool m_bPlateFieldSelectionEnabled;
    bool m_bWellSelectionMode;
    bool m_bFieldSelectionMode;
    bool m_bScanning;
    QString m_currentPreviewWell;
    QVector<QPair<QString, int>> m_mockScanPlan;
    int m_nMockScanIndex;
    QMap<QString, QSet<int>> m_selectedFieldsByWell;
    QPair<QString, int> m_currentScanningTarget;
};

#endif // SCANPAGE_H
