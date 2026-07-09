#ifndef WELLPLATEWIDGET_H
#define WELLPLATEWIDGET_H

#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <QStringList>
#include <QVector>
#include <QWidget>

class WellPlateWidget : public QWidget
{
    Q_OBJECT

public:
    enum class WellState
    {
        Default,
        Previewing,
        Grouped,
        Selected,
        Scanning,
        Completed
    };
    Q_ENUM(WellState)

    enum class PlateFormat
    {
        Plate6,
        Plate12,
        Plate24,
        Plate48,
        Plate96
    };
    Q_ENUM(PlateFormat)

    explicit WellPlateWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    PlateFormat plateFormat() const;
    void setPlateFormat(PlateFormat format);

    int rowCount() const;
    int columnCount() const;
    QString wellName(int row, int column) const;

    void setSelectionEnabled(bool enabled);
    bool isSelectionEnabled() const;

    QColor groupColor() const;
    void setGroupColor(const QColor &color);

    WellState wellState(int row, int column) const;
    WellState wellState(const QString &well) const;
    void setWellState(int row, int column, WellState state);
    void setWellState(const QString &well, WellState state);
    void setWellStates(const QStringList &wells, WellState state);
    void setGroupedWells(const QStringList &wells, const QColor &groupColor);

    void clearState(WellState state);
    void clearSelected();
    void confirmSelectedAsGroup(const QColor &groupColor);
    void clearAll();
    QStringList wellsByState(WellState state) const;
    QStringList selectedWells() const;

    void setActiveWell(const QString &well);
    QString activeWell() const;

signals:
    void plateFormatChanged(WellPlateWidget::PlateFormat format);
    void wellClicked(const QString &well);
    void wellStateChanged(const QString &well, WellPlateWidget::WellState state);
    void wellSelectionChanged();
    void activeWellChanged(const QString &well);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    struct PlateMetrics
    {
        QRectF contentRect;
        qreal rowLabelWidth;
        qreal columnLabelHeight;
        qreal gap;
        qreal cellSize;
        qreal gridX;
        qreal gridY;
    };

    static QSize dimensionsForFormat(PlateFormat format);
    PlateMetrics computeMetrics() const;
    int stateIndex(int row, int column) const;
    bool isValidWell(int row, int column) const;
    bool parseWell(const QString &well, int *row, int *column) const;
    bool hitTest(const QPoint &point, int *row, int *column) const;
    QRect selectionRect() const;
    void updateSelectionFromDrag();
    QRectF cellRect(int row, int column) const;
    bool setWellStateInternal(int row, int column, WellState state, bool emitSignal);
    void setWellBackgroundColor(int row, int column, const QColor &color);
    QColor wellBackgroundColor(int row, int column, WellState state) const;
    void drawWell(QPainter *pPainter, const QRectF &rect, WellState state);

private:
    PlateFormat m_plateFormat;
    int m_nRows;
    int m_nColumns;
    QVector<WellState> m_states;
    QVector<QColor> m_backgroundColors;
    QColor m_groupColor;
    QString m_activeWell;
    bool m_bSelectionEnabled;
    bool m_bDragging;
    QPoint m_dragStart;
    QPoint m_dragCurrent;
    QVector<WellState> m_dragSnapshot;
};

#endif // WELLPLATEWIDGET_H
