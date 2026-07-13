#ifndef SCANPAGE_H
#define SCANPAGE_H

#include "CreateExperimentSubPage.h"
#include "FieldViewWidget.h"
#include "WellPlateWidget.h"
#include "models/CaptureSettings.h"

#include <QColor>
#include <QMap>
#include <QSet>
#include <QStringList>
#include <QTimer>
#include <QVector>
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

    void setObjectiveMagnification(ObjectiveMagnification objective);

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
    void fieldPreviewRequested(const QString &well, int fieldIndex);
    void plateFormatChanged(WellPlateWidget::PlateFormat format);
    void groupChanged(int groupIndex);

private:
    void initControls();
    void initConnections();
    void showCreateExperimentPage();
    void showEditExperimentPage();
    void applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage page);
    void completeExperimentConfiguration();
    void enablePlateFieldSelection();
    void handleSelectWellsButtonClicked();
    void handleSelectFieldsButtonClicked();
    void finishPlateFieldSelection();
    void beginWellSelection();
    void cancelWellSelection();
    void confirmWellSelection();
    void beginFieldSelectionForWell(const QString &well);
    void cancelFieldSelection();
    void confirmFieldSelection();
    void handleWellClicked(const QString &well);
    void handleWellSelectionChanged();
    void toggleScan();
    void runNextMockScanStep();
    void stopMockScan();
    void updateGroupColorButton();
    void updatePlateFieldControls();
    void setExperimentActionEnabled(bool enabled);
    void setPlateFormat(WellPlateWidget::PlateFormat format);
    bool buildMockScanPlan();
    void discardUnconfirmedFieldSelection();
    void restoreFieldsForActiveWell();
    void showCreateExperimentSelectionPage();
    void updateCreateExperimentPlateFieldSummary();
    void refreshPlateSelectionFromModel();
    bool canEditCurrentWellFields() const;
    QStringList groupedWells() const;
    QStringList selectedWellsInPlateOrder() const;
    QString selectedGroupsText() const;
    QString firstWellWithoutFieldsInGroup(int groupIndex) const;
    QString firstWellWithoutFields() const;
    QColor currentGroupColor() const;
    static QColor groupColor(int groupIndex);
    static QString plateFormatText(WellPlateWidget::PlateFormat format);

private:
    Ui::ScanPage *ui;
    CreateExperimentSubPage *m_pCreateExperimentSubPage;
    QTimer *m_pMockScanTimer;
    bool m_bPlateFieldSelectionEnabled;
    bool m_bFieldSelectionMode;
    bool m_bScanning;
    bool m_bExperimentConfigured;
    QString m_currentPreviewWell;
    QVector<QPair<QString, int>> m_mockScanPlan;
    int m_nMockScanIndex;
    QMap<QString, QSet<int>> m_selectedFieldsByWell;
    QMap<QString, int> m_selectedGroupByWell;
    QPair<QString, int> m_currentScanningTarget;
    int m_nCurrentGroupIndex;
};

#endif // SCANPAGE_H
