#ifndef FIELDVIEWWIDGET_H
#define FIELDVIEWWIDGET_H

#include "WellPlateWidget.h"

#include <QPainter>
#include <QVector>
#include <QWidget>

class FieldViewWidget : public QWidget
{
    Q_OBJECT

public:
    enum class FieldState
    {
        Empty,
        Selected,
        Acquired,
        Focused,
        Scanning,
        Disabled
    };
    Q_ENUM(FieldState)

    explicit FieldViewWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    WellPlateWidget::PlateFormat plateFormat() const;
    void setPlateFormat(WellPlateWidget::PlateFormat format);

    int rowCount() const;
    int columnCount() const;
    FieldState fieldState(int row, int column) const;
    void setFieldState(int row, int column, FieldState state);
    void clearState(FieldState state);
    void clearAll();

signals:
    void plateFormatChanged(WellPlateWidget::PlateFormat format);
    void fieldStateChanged(int row, int column, FieldViewWidget::FieldState state);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static QSize dimensionsForFormat(WellPlateWidget::PlateFormat format);
    int stateIndex(int row, int column) const;
    bool isValidField(int row, int column) const;
    QRectF gridRect() const;
    void drawGrid(QPainter *pPainter, const QRectF &rect);
    void drawFieldStates(QPainter *pPainter, const QRectF &rect);

private:
    WellPlateWidget::PlateFormat m_plateFormat;
    int m_nRows;
    int m_nColumns;
    QVector<FieldState> m_states;
};

#endif // FIELDVIEWWIDGET_H
