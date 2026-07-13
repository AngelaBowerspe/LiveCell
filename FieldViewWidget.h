#ifndef FIELDVIEWWIDGET_H
#define FIELDVIEWWIDGET_H

#include "WellPlateWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QSet>
#include <QVector>
#include <QWidget>

class FieldViewWidget : public QWidget
{
    Q_OBJECT

public:
    enum class FieldState
    {
        Default,
        Previewing,
        Selected,
        Scanning,
        Completed
    };
    Q_ENUM(FieldState)

    explicit FieldViewWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    WellPlateWidget::PlateFormat plateFormat() const;
    void setPlateFormat(WellPlateWidget::PlateFormat format);

    int rowCount() const;
    int columnCount() const;
    void setSelectionEnabled(bool enabled);
    bool isSelectionEnabled() const;
    void setPreviewEnabled(bool enabled);
    bool isPreviewEnabled() const;
    void setPreviewFieldIndex(int index);
    int previewFieldIndex() const;
    FieldState fieldState(int row, int column) const;
    void setFieldState(int row, int column, FieldState state);
    void setFieldState(int index, FieldState state);
    void setFieldStates(const QSet<int> &indexes, FieldState state);
    void clearState(FieldState state);
    void clearAll();
    QSet<int> selectedFieldIndexes() const;
    QSet<int> fieldIndexesByState(FieldState state) const;

signals:
    void plateFormatChanged(WellPlateWidget::PlateFormat format);
    void fieldStateChanged(int row, int column, FieldViewWidget::FieldState state);
    void fieldSelectionChanged();
    void fieldPreviewed(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    static QSize dimensionsForFormat(WellPlateWidget::PlateFormat format);
    int stateIndex(int row, int column) const;
    int fieldIndexAt(const QPoint &point) const;
    bool isValidField(int row, int column) const;
    QRectF gridRect() const;
    QRectF fieldRect(int row, int column) const;
    QRect selectionRect() const;
    void updateSelectionFromDrag();
    void drawGrid(QPainter *pPainter, const QRectF &rect);
    void drawFieldStates(QPainter *pPainter, const QRectF &rect);

private:
    WellPlateWidget::PlateFormat m_plateFormat;
    int m_nRows;
    int m_nColumns;
    QVector<FieldState> m_states;
    bool m_bSelectionEnabled;
    bool m_bPreviewEnabled;
    int m_nPreviewFieldIndex;
    bool m_bDragging;
    QPoint m_dragStart;
    QPoint m_dragCurrent;
    QVector<FieldState> m_dragSnapshot;
};

#endif // FIELDVIEWWIDGET_H
