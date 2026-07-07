#include "WellPlateWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QSizePolicy>
#include <QtMath>

WellPlateWidget::WellPlateWidget(QWidget *parent)
    : QWidget(parent)
    , m_plateFormat(PlateFormat::Plate24)
    , m_nRows(4)
    , m_nColumns(6)
    , m_states(m_nRows * m_nColumns, WellState::Empty)
{
    setMouseTracking(true);
    setMinimumSize(minimumSizeHint());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

QSize WellPlateWidget::sizeHint() const
{
    return QSize(340, 240);
}

QSize WellPlateWidget::minimumSizeHint() const
{
    return QSize(240, 165);
}

WellPlateWidget::PlateFormat WellPlateWidget::plateFormat() const
{
    return m_plateFormat;
}

void WellPlateWidget::setPlateFormat(PlateFormat format)
{
    if (m_plateFormat == format)
    {
        return;
    }

    m_plateFormat = format;
    const QSize size = dimensionsForFormat(m_plateFormat);
    m_nRows = size.height();
    m_nColumns = size.width();
    m_states.fill(WellState::Empty, m_nRows * m_nColumns);

    emit plateFormatChanged(m_plateFormat);
    emit activeWellChanged(QString());
    update();
}

int WellPlateWidget::rowCount() const
{
    return m_nRows;
}

int WellPlateWidget::columnCount() const
{
    return m_nColumns;
}

QString WellPlateWidget::wellName(int row, int column) const
{
    if (!isValidWell(row, column))
    {
        return QString();
    }

    return QString(QChar('A' + row)) + QString::number(column + 1);
}

WellPlateWidget::WellState WellPlateWidget::wellState(int row, int column) const
{
    if (!isValidWell(row, column))
    {
        return WellState::Empty;
    }

    return m_states.at(stateIndex(row, column));
}

WellPlateWidget::WellState WellPlateWidget::wellState(const QString &well) const
{
    int row = -1;
    int column = -1;
    if (!parseWell(well, &row, &column))
    {
        return WellState::Empty;
    }

    return wellState(row, column);
}

void WellPlateWidget::setWellState(int row, int column, WellState state)
{
    if (setWellStateInternal(row, column, state, true))
    {
        update();
    }
}

void WellPlateWidget::setWellState(const QString &well, WellState state)
{
    int row = -1;
    int column = -1;
    if (parseWell(well, &row, &column))
    {
        setWellState(row, column, state);
    }
}

void WellPlateWidget::setWellStates(const QStringList &wells, WellState state)
{
    bool changed = false;
    for (const QString &well : wells)
    {
        int row = -1;
        int column = -1;
        if (parseWell(well, &row, &column))
        {
            changed |= setWellStateInternal(row, column, state, true);
        }
    }

    if (changed)
    {
        update();
    }
}

void WellPlateWidget::clearState(WellState state)
{
    bool changed = false;
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            if (wellState(row, column) == state)
            {
                changed |= setWellStateInternal(row, column, WellState::Empty, true);
            }
        }
    }

    if (changed)
    {
        update();
    }
}

void WellPlateWidget::clearAll()
{
    bool changed = false;
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            if (wellState(row, column) != WellState::Empty)
            {
                changed |= setWellStateInternal(row, column, WellState::Empty, true);
            }
        }
    }

    if (changed)
    {
        emit activeWellChanged(QString());
        update();
    }
}

QStringList WellPlateWidget::wellsByState(WellState state) const
{
    QStringList wells;
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            if (wellState(row, column) == state)
            {
                wells.append(wellName(row, column));
            }
        }
    }

    return wells;
}

void WellPlateWidget::setActiveWell(const QString &well)
{
    QString nextActive;
    bool changed = false;

    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            if (wellState(row, column) == WellState::Focused)
            {
                changed |= setWellStateInternal(row, column, WellState::Confirmed, true);
            }
        }
    }

    int activeRow = -1;
    int activeColumn = -1;
    if (parseWell(well, &activeRow, &activeColumn))
    {
        nextActive = wellName(activeRow, activeColumn);
        changed |= setWellStateInternal(activeRow, activeColumn, WellState::Focused, true);
    }

    if (changed)
    {
        emit activeWellChanged(nextActive);
        update();
    }
}

QString WellPlateWidget::activeWell() const
{
    const QStringList wells = wellsByState(WellState::Focused);
    return wells.isEmpty() ? QString() : wells.first();
}

void WellPlateWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(246, 246, 246));

    const PlateMetrics metrics = computeMetrics();

    painter.setPen(QColor(85, 130, 185));
    QFont labelFont = painter.font();
    labelFont.setPixelSize(qMax(11, qRound(metrics.cellSize * 0.34)));
    painter.setFont(labelFont);

    for (int column = 0; column < m_nColumns; ++column)
    {
        const qreal x = metrics.gridX + column * (metrics.cellSize + metrics.gap);
        const QRectF numberRect(x, metrics.gridY - metrics.columnLabelHeight, metrics.cellSize, metrics.columnLabelHeight - 2.0);
        painter.drawText(numberRect, Qt::AlignCenter, QString::number(column + 1));
    }

    for (int row = 0; row < m_nRows; ++row)
    {
        const qreal y = metrics.gridY + row * (metrics.cellSize + metrics.gap);
        const QRectF rowRect(metrics.gridX - metrics.rowLabelWidth, y, metrics.rowLabelWidth - 6.0, metrics.cellSize);
        painter.drawText(rowRect, Qt::AlignRight | Qt::AlignVCenter, QString(QChar('A' + row)));
    }

    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            drawWell(&painter, cellRect(row, column), wellState(row, column));
        }
    }
}

void WellPlateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    int row = -1;
    int column = -1;
    if (!hitTest(event->pos(), &row, &column))
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const QString well = wellName(row, column);
    emit wellClicked(well);
    setActiveWell(well);
    event->accept();
}

QSize WellPlateWidget::dimensionsForFormat(PlateFormat format)
{
    switch (format)
    {
    case PlateFormat::Plate6:
        return QSize(3, 2);
    case PlateFormat::Plate12:
        return QSize(4, 3);
    case PlateFormat::Plate24:
        return QSize(6, 4);
    case PlateFormat::Plate48:
        return QSize(8, 6);
    case PlateFormat::Plate96:
        return QSize(12, 8);
    }

    return QSize(6, 4);
}

WellPlateWidget::PlateMetrics WellPlateWidget::computeMetrics() const
{
    PlateMetrics metrics;
    metrics.contentRect = rect().adjusted(12, 10, -12, -12);

    const qreal sparsePlateFactor = (m_nColumns <= 4 || m_nRows <= 3) ? 1.0 : 0.0;
    metrics.gap = qMax(4.0, qMin(10.0, qMin(metrics.contentRect.width(), metrics.contentRect.height()) / 42.0));
    metrics.rowLabelWidth = qMax(26.0, qMin(42.0, metrics.contentRect.width() / 10.0 + sparsePlateFactor * 8.0));
    metrics.columnLabelHeight = qMax(22.0, qMin(34.0, metrics.contentRect.height() / 9.0 + sparsePlateFactor * 4.0));

    const qreal availableWidth = metrics.contentRect.width() - metrics.rowLabelWidth;
    const qreal availableHeight = metrics.contentRect.height() - metrics.columnLabelHeight;
    const qreal cellWidth = (availableWidth - metrics.gap * (m_nColumns - 1)) / qMax(1, m_nColumns);
    const qreal cellHeight = (availableHeight - metrics.gap * (m_nRows - 1)) / qMax(1, m_nRows);
    metrics.cellSize = qMax(12.0, qMin(cellWidth, cellHeight));

    const qreal gridWidth = metrics.cellSize * m_nColumns + metrics.gap * (m_nColumns - 1);
    const qreal gridHeight = metrics.cellSize * m_nRows + metrics.gap * (m_nRows - 1);
    metrics.gridX = metrics.contentRect.left() + metrics.rowLabelWidth + (availableWidth - gridWidth) / 2.0;
    metrics.gridY = metrics.contentRect.top() + metrics.columnLabelHeight + (availableHeight - gridHeight) / 2.0;
    return metrics;
}

int WellPlateWidget::stateIndex(int row, int column) const
{
    return row * m_nColumns + column;
}

bool WellPlateWidget::isValidWell(int row, int column) const
{
    return row >= 0 && row < m_nRows && column >= 0 && column < m_nColumns;
}

bool WellPlateWidget::parseWell(const QString &well, int *row, int *column) const
{
    const QString text = well.trimmed().toUpper();
    if (text.size() < 2)
    {
        return false;
    }

    const QChar rowChar = text.at(0);
    if (!rowChar.isLetter())
    {
        return false;
    }

    bool ok = false;
    const int columnNumber = text.mid(1).toInt(&ok);
    if (!ok)
    {
        return false;
    }

    const int parsedRow = rowChar.unicode() - 'A';
    const int parsedColumn = columnNumber - 1;
    if (!isValidWell(parsedRow, parsedColumn))
    {
        return false;
    }

    if (row != nullptr)
    {
        *row = parsedRow;
    }
    if (column != nullptr)
    {
        *column = parsedColumn;
    }
    return true;
}

bool WellPlateWidget::hitTest(const QPoint &point, int *row, int *column) const
{
    for (int r = 0; r < m_nRows; ++r)
    {
        for (int c = 0; c < m_nColumns; ++c)
        {
            if (cellRect(r, c).contains(point))
            {
                if (row != nullptr)
                {
                    *row = r;
                }
                if (column != nullptr)
                {
                    *column = c;
                }
                return true;
            }
        }
    }

    return false;
}

QRectF WellPlateWidget::cellRect(int row, int column) const
{
    const PlateMetrics metrics = computeMetrics();
    return QRectF(metrics.gridX + column * (metrics.cellSize + metrics.gap),
                  metrics.gridY + row * (metrics.cellSize + metrics.gap),
                  metrics.cellSize,
                  metrics.cellSize);
}

bool WellPlateWidget::setWellStateInternal(int row, int column, WellState state, bool emitSignal)
{
    if (!isValidWell(row, column))
    {
        return false;
    }

    WellState &slot = m_states[stateIndex(row, column)];
    if (slot == state)
    {
        return false;
    }

    slot = state;
    if (emitSignal)
    {
        emit wellStateChanged(wellName(row, column), slot);
    }
    return true;
}

void WellPlateWidget::drawWell(QPainter *pPainter, const QRectF &rect, WellState state)
{
    QColor fillColor(250, 250, 250);
    QColor borderColor(130, 172, 215);
    QColor markerColor(0, 0, 0, 0);

    switch (state)
    {
    case WellState::Selected:
        fillColor = QColor(220, 236, 255);
        borderColor = QColor(40, 122, 255);
        break;
    case WellState::Confirmed:
        fillColor = QColor(235, 225, 252);
        borderColor = QColor(134, 79, 215);
        break;
    case WellState::Focused:
        fillColor = QColor(219, 244, 229);
        borderColor = QColor(45, 154, 94);
        markerColor = QColor(45, 154, 94);
        break;
    case WellState::Scanning:
        fillColor = QColor(252, 230, 146);
        borderColor = QColor(230, 174, 35);
        break;
    case WellState::Disabled:
        fillColor = QColor(230, 230, 230);
        borderColor = QColor(180, 180, 180);
        break;
    case WellState::Error:
        fillColor = QColor(255, 225, 225);
        borderColor = QColor(220, 70, 70);
        break;
    case WellState::Empty:
        break;
    }

    const qreal padding = qMax(2.0, rect.width() * 0.08);
    const QRectF circleRect = rect.adjusted(padding, padding, -padding, -padding);
    pPainter->setPen(QPen(borderColor, qMax(1.2, rect.width() * 0.045)));
    pPainter->setBrush(fillColor);
    pPainter->drawEllipse(circleRect);

    if (markerColor.alpha() > 0)
    {
        const qreal markerSize = qMax(5.0, circleRect.width() * 0.18);
        const QRectF markerRect(circleRect.right() - markerSize,
                                circleRect.top(),
                                markerSize,
                                markerSize);
        pPainter->setPen(Qt::NoPen);
        pPainter->setBrush(markerColor);
        pPainter->drawEllipse(markerRect);
    }
}
