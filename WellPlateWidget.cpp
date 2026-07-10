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
    , m_states(m_nRows * m_nColumns, WellState::Default)
    , m_backgroundColors(m_nRows * m_nColumns, QColor(0, 0, 0, 0))
    , m_groupColor(110, 164, 235, 120)
    , m_activeWell()
    , m_bSelectionEnabled(false)
    , m_bDragging(false)
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
    m_states.fill(WellState::Default, m_nRows * m_nColumns);
    m_backgroundColors.fill(QColor(0, 0, 0, 0), m_nRows * m_nColumns);
    m_activeWell.clear();
    m_bDragging = false;
    m_dragSnapshot.clear();

    emit plateFormatChanged(m_plateFormat);
    emit activeWellChanged(QString());
    emit wellSelectionChanged();
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

void WellPlateWidget::setSelectionEnabled(bool enabled)
{
    if (m_bSelectionEnabled == enabled)
    {
        return;
    }

    m_bSelectionEnabled = enabled;
    if (!m_bSelectionEnabled)
    {
        m_bDragging = false;
        m_dragSnapshot.clear();
    }

    update();
}

bool WellPlateWidget::isSelectionEnabled() const
{
    return m_bSelectionEnabled;
}

QColor WellPlateWidget::groupColor() const
{
    return m_groupColor;
}

void WellPlateWidget::setGroupColor(const QColor &color)
{
    if (!color.isValid() || m_groupColor == color)
    {
        return;
    }

    m_groupColor = color;
    update();
}

WellPlateWidget::WellState WellPlateWidget::wellState(int row, int column) const
{
    if (!isValidWell(row, column))
    {
        return WellState::Default;
    }

    return m_states.at(stateIndex(row, column));
}

WellPlateWidget::WellState WellPlateWidget::wellState(const QString &well) const
{
    int row = -1;
    int column = -1;
    if (!parseWell(well, &row, &column))
    {
        return WellState::Default;
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
        emit wellSelectionChanged();
        update();
    }
}

void WellPlateWidget::setGroupedWells(const QStringList &wells, const QColor &groupColor)
{
    bool changed = false;
    for (const QString &well : wells)
    {
        int row = -1;
        int column = -1;
        if (parseWell(well, &row, &column))
        {
            setWellBackgroundColor(row, column, groupColor);
            changed |= setWellStateInternal(row, column, WellState::Grouped, true);
        }
    }

    if (changed || !wells.isEmpty())
    {
        emit wellSelectionChanged();
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
                if (state == WellState::Grouped || state == WellState::Previewing)
                {
                    setWellBackgroundColor(row, column, QColor(0, 0, 0, 0));
                }
                changed |= setWellStateInternal(row, column, WellState::Default, true);
            }
        }
    }

    if (changed)
    {
        emit wellSelectionChanged();
        update();
    }
}

void WellPlateWidget::clearSelected()
{
    clearState(WellState::Selected);
}

void WellPlateWidget::confirmSelectedAsGroup(const QColor &groupColor)
{
    setGroupedWells(selectedWells(), groupColor);
}

void WellPlateWidget::clearAll()
{
    bool changed = false;
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            if (wellState(row, column) != WellState::Default
                || m_backgroundColors.at(stateIndex(row, column)).alpha() > 0)
            {
                setWellBackgroundColor(row, column, QColor(0, 0, 0, 0));
                changed |= setWellStateInternal(row, column, WellState::Default, true);
            }
        }
    }

    if (changed)
    {
        emit activeWellChanged(QString());
        emit wellSelectionChanged();
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

QStringList WellPlateWidget::selectedWells() const
{
    return wellsByState(WellState::Selected);
}

void WellPlateWidget::setActiveWell(const QString &well)
{
    int activeRow = -1;
    int activeColumn = -1;
    QString nextActive;
    if (parseWell(well, &activeRow, &activeColumn))
    {
        const WellState state = wellState(activeRow, activeColumn);
        if (state != WellState::Default)
        {
            nextActive = wellName(activeRow, activeColumn);
        }
    }

    if (m_activeWell == nextActive)
    {
        return;
    }

    m_activeWell = nextActive;
    emit activeWellChanged(m_activeWell);
    update();
}

QString WellPlateWidget::activeWell() const
{
    return m_activeWell;
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
            const WellState state = wellState(row, column);
            const QRectF cell = cellRect(row, column);
            QColor backgroundColor = wellBackgroundColor(row, column, state);
            if (wellName(row, column) == m_activeWell)
            {
                backgroundColor = QColor(82, 152, 245, 105);
            }
            if (backgroundColor.alpha() > 0)
            {
                painter.fillRect(cell.adjusted(1.0, 1.0, -1.0, -1.0), backgroundColor);
            }
            drawWell(&painter, cell, state);
        }
    }

    if (m_bDragging)
    {
        painter.setPen(QPen(QColor(55, 140, 255, 220), 1.2));
        painter.setBrush(QColor(80, 155, 255, 42));
        painter.drawRect(selectionRect());
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

    const WellState state = wellState(row, column);
    if (m_bSelectionEnabled
        && state != WellState::Grouped
        && state != WellState::Completed
        && state != WellState::Scanning)
    {
        m_bDragging = true;
        m_dragStart = event->pos();
        m_dragCurrent = event->pos();
        m_dragSnapshot = m_states;
        updateSelectionFromDrag();
        event->accept();
        return;
    }

    const QString well = wellName(row, column);
    emit wellClicked(well);
    setActiveWell(well);
    event->accept();
}

void WellPlateWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_bSelectionEnabled || !m_bDragging || !(event->buttons() & Qt::LeftButton))
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    m_dragCurrent = event->pos();
    updateSelectionFromDrag();
    event->accept();
}

void WellPlateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_bSelectionEnabled || event->button() != Qt::LeftButton)
    {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    m_dragCurrent = event->pos();
    updateSelectionFromDrag();
    m_bDragging = false;
    m_dragSnapshot.clear();
    update();
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

QRect WellPlateWidget::selectionRect() const
{
    return QRect(m_dragStart, m_dragCurrent).normalized();
}

void WellPlateWidget::updateSelectionFromDrag()
{
    const QRect dragRect = selectionRect();
    const bool pointPick = dragRect.width() < 3 && dragRect.height() < 3;
    QVector<WellState> nextStates = m_dragSnapshot.isEmpty() ? m_states : m_dragSnapshot;

    for (int index = 0; index < nextStates.size(); ++index)
    {
        if (nextStates.at(index) == WellState::Selected)
        {
            nextStates[index] = WellState::Default;
        }
    }

    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            const int index = stateIndex(row, column);
            if (nextStates.at(index) == WellState::Completed
                || nextStates.at(index) == WellState::Grouped
                || nextStates.at(index) == WellState::Scanning)
            {
                continue;
            }

            const QRectF cell = cellRect(row, column);
            const bool inside = pointPick ? cell.contains(m_dragCurrent) : cell.intersects(QRectF(dragRect));
            if (inside)
            {
                nextStates[index] = WellState::Selected;
            }
        }
    }

    if (nextStates != m_states)
    {
        m_states = nextStates;
        update();
        emit wellSelectionChanged();
    }
    else if (m_bDragging)
    {
        update();
    }
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

void WellPlateWidget::setWellBackgroundColor(int row, int column, const QColor &color)
{
    if (!isValidWell(row, column))
    {
        return;
    }

    m_backgroundColors[stateIndex(row, column)] = color;
}

QColor WellPlateWidget::wellBackgroundColor(int row, int column, WellState state) const
{
    if (!isValidWell(row, column))
    {
        return QColor(0, 0, 0, 0);
    }

    const QColor color = m_backgroundColors.at(stateIndex(row, column));
    if (color.alpha() > 0)
    {
        return color;
    }

    if (state == WellState::Grouped)
    {
        return m_groupColor;
    }
    if (state == WellState::Selected)
    {
        return m_groupColor;
    }
    return QColor(0, 0, 0, 0);
}

void WellPlateWidget::drawWell(QPainter *pPainter, const QRectF &rect, WellState state)
{
    QColor fillColor(250, 250, 250);
    QColor borderColor(130, 172, 215);

    switch (state)
    {
    case WellState::Grouped:
        break;
    case WellState::Selected:
        borderColor = QColor(230, 174, 35);
        break;
    case WellState::Scanning:
        fillColor = QColor(255, 224, 91);
        borderColor = QColor(230, 174, 35);
        break;
    case WellState::Completed:
        fillColor = QColor(113, 224, 72);
        borderColor = QColor(74, 180, 55);
        break;
    case WellState::Previewing:
    case WellState::Default:
        break;
    }

    const qreal padding = qMax(2.0, rect.width() * 0.08);
    const QRectF circleRect = rect.adjusted(padding, padding, -padding, -padding);
    pPainter->setPen(QPen(borderColor, qMax(1.2, rect.width() * 0.045)));
    pPainter->setBrush(fillColor);
    pPainter->drawEllipse(circleRect);
}
