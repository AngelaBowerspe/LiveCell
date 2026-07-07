#include "FieldViewWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QSizePolicy>

FieldViewWidget::FieldViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_plateFormat(WellPlateWidget::PlateFormat::Plate24)
    , m_nRows(11)
    , m_nColumns(11)
    , m_states(m_nRows * m_nColumns, FieldState::Empty)
{
    setMinimumSize(minimumSizeHint());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

QSize FieldViewWidget::sizeHint() const
{
    return QSize(340, 280);
}

QSize FieldViewWidget::minimumSizeHint() const
{
    return QSize(240, 200);
}

WellPlateWidget::PlateFormat FieldViewWidget::plateFormat() const
{
    return m_plateFormat;
}

void FieldViewWidget::setPlateFormat(WellPlateWidget::PlateFormat format)
{
    if (m_plateFormat == format)
    {
        return;
    }

    m_plateFormat = format;
    const QSize size = dimensionsForFormat(m_plateFormat);
    m_nRows = size.height();
    m_nColumns = size.width();
    m_states.fill(FieldState::Empty, m_nRows * m_nColumns);

    emit plateFormatChanged(m_plateFormat);
    update();
}

int FieldViewWidget::rowCount() const
{
    return m_nRows;
}

int FieldViewWidget::columnCount() const
{
    return m_nColumns;
}

FieldViewWidget::FieldState FieldViewWidget::fieldState(int row, int column) const
{
    if (!isValidField(row, column))
    {
        return FieldState::Empty;
    }

    return m_states.at(stateIndex(row, column));
}

void FieldViewWidget::setFieldState(int row, int column, FieldState state)
{
    if (!isValidField(row, column))
    {
        return;
    }

    FieldState &slot = m_states[stateIndex(row, column)];
    if (slot == state)
    {
        return;
    }

    slot = state;
    emit fieldStateChanged(row, column, state);
    update();
}

void FieldViewWidget::clearState(FieldState state)
{
    bool changed = false;
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            FieldState &slot = m_states[stateIndex(row, column)];
            if (slot == state)
            {
                slot = FieldState::Empty;
                emit fieldStateChanged(row, column, slot);
                changed = true;
            }
        }
    }

    if (changed)
    {
        update();
    }
}

void FieldViewWidget::clearAll()
{
    bool changed = false;
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            FieldState &slot = m_states[stateIndex(row, column)];
            if (slot != FieldState::Empty)
            {
                slot = FieldState::Empty;
                emit fieldStateChanged(row, column, slot);
                changed = true;
            }
        }
    }

    if (changed)
    {
        update();
    }
}

void FieldViewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(250, 250, 250));

    const QRectF rect = gridRect();
    drawGrid(&painter, rect);
    drawFieldStates(&painter, rect);

    painter.setPen(QPen(QColor(150, 185, 225), 1.2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect);
}

QSize FieldViewWidget::dimensionsForFormat(WellPlateWidget::PlateFormat format)
{
    switch (format)
    {
    case WellPlateWidget::PlateFormat::Plate6:
        return QSize(24, 24);
    case WellPlateWidget::PlateFormat::Plate12:
        return QSize(18, 18);
    case WellPlateWidget::PlateFormat::Plate24:
        return QSize(11, 11);
    case WellPlateWidget::PlateFormat::Plate48:
        return QSize(8, 8);
    case WellPlateWidget::PlateFormat::Plate96:
        return QSize(5, 5);
    }

    return QSize(11, 11);
}

int FieldViewWidget::stateIndex(int row, int column) const
{
    return row * m_nColumns + column;
}

bool FieldViewWidget::isValidField(int row, int column) const
{
    return row >= 0 && row < m_nRows && column >= 0 && column < m_nColumns;
}

QRectF FieldViewWidget::gridRect() const
{
    return QRectF(rect()).adjusted(14, 14, -14, -14);
}

void FieldViewWidget::drawGrid(QPainter *pPainter, const QRectF &rect)
{
    pPainter->setPen(QPen(QColor(190, 212, 235), 1));
    const qreal cellWidth = rect.width() / qMax(1, m_nColumns);
    const qreal cellHeight = rect.height() / qMax(1, m_nRows);

    for (int column = 0; column <= m_nColumns; ++column)
    {
        const qreal x = rect.left() + column * cellWidth;
        pPainter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    for (int row = 0; row <= m_nRows; ++row)
    {
        const qreal y = rect.top() + row * cellHeight;
        pPainter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void FieldViewWidget::drawFieldStates(QPainter *pPainter, const QRectF &rect)
{
    const qreal cellWidth = rect.width() / qMax(1, m_nColumns);
    const qreal cellHeight = rect.height() / qMax(1, m_nRows);

    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            const FieldState state = fieldState(row, column);
            QColor color(0, 0, 0, 0);
            switch (state)
            {
            case FieldState::Selected:
                color = QColor(80, 155, 255, 80);
                break;
            case FieldState::Acquired:
                color = QColor(84, 190, 120, 95);
                break;
            case FieldState::Focused:
                color = QColor(40, 122, 255, 120);
                break;
            case FieldState::Scanning:
                color = QColor(252, 205, 72, 120);
                break;
            case FieldState::Disabled:
                color = QColor(180, 180, 180, 70);
                break;
            case FieldState::Empty:
                break;
            }

            if (color.alpha() == 0)
            {
                continue;
            }

            const QRectF cellRect(rect.left() + column * cellWidth,
                                  rect.top() + row * cellHeight,
                                  cellWidth,
                                  cellHeight);
            pPainter->fillRect(cellRect.adjusted(1, 1, -1, -1), color);
        }
    }
}
