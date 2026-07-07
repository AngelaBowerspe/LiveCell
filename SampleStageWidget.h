#ifndef SAMPLESTAGEWIDGET_H
#define SAMPLESTAGEWIDGET_H

#include "WellPlateWidget.h"

#include "FieldViewWidget.h"

#include <QComboBox>
#include <QLabel>
#include <QStringList>
#include <QWidget>

class SampleStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SampleStageWidget(QWidget *parent = nullptr);

    WellPlateWidget *wellPlateWidget() const;
    FieldViewWidget *fieldViewWidget() const;

    void setPlateFormat(WellPlateWidget::PlateFormat format);
    WellPlateWidget::PlateFormat plateFormat() const;

    void setWellState(const QString &well, WellPlateWidget::WellState state);
    void setWellStates(const QStringList &wells, WellPlateWidget::WellState state);
    void clearWellState(WellPlateWidget::WellState state);
    void clearAllWellStates();

    void setFieldState(int row, int column, FieldViewWidget::FieldState state);
    void clearFieldState(FieldViewWidget::FieldState state);
    void clearAllFieldStates();

private:
    void initUi();
    void initConnections();
    void updatePlateComboText();
    static QString plateFormatText(WellPlateWidget::PlateFormat format);

private:
    QLabel *m_pTitleLabel;
    QLabel *m_pPlateLabel;
    QLabel *m_pWellTitleLabel;
    QLabel *m_pFieldTitleLabel;
    QComboBox *m_pPlateComboBox;
    WellPlateWidget *m_pWellPlateWidget;
    FieldViewWidget *m_pFieldViewWidget;
};

#endif // SAMPLESTAGEWIDGET_H
