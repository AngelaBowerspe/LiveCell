#include "FieldViewWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QSizePolicy>

FieldViewWidget::FieldViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_plateFormat(WellPlateWidget::PlateFormat::Plate24)
    , m_nRows(11)
    , m_nColumns(11)
    , m_states(m_nRows * m_nColumns, FieldState::Default)
    , m_bSelectionEnabled(false)
    , m_bPreviewEnabled(false)
    , m_nPreviewFieldIndex(-1)
    , m_bDragging(false)
{
    setMouseTracking(true);
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
    m_states.fill(FieldState::Default, m_nRows * m_nColumns);
    m_nPreviewFieldIndex = -1;
    m_bDragging = false;
    m_dragSnapshot.clear();

    emit plateFormatChanged(m_plateFormat);
    emit fieldSelectionChanged();
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

void FieldViewWidget::setSelectionEnabled(bool enabled)
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

bool FieldViewWidget::isSelectionEnabled() const
{
    return m_bSelectionEnabled;
}

void FieldViewWidget::setPreviewEnabled(bool enabled)
{
    if (m_bPreviewEnabled == enabled)
    {
        return;
    }

    m_bPreviewEnabled = enabled;
    if (!m_bPreviewEnabled)
    {
        m_nPreviewFieldIndex = -1;
    }

    update();
}

bool FieldViewWidget::isPreviewEnabled() const
{
    return m_bPreviewEnabled;
}

void FieldViewWidget::setPreviewFieldIndex(int index)
{
    const int nextIndex = index >= 0 && index < m_states.size() ? index : -1;
    if (m_nPreviewFieldIndex == nextIndex)
    {
        return;
    }

    m_nPreviewFieldIndex = nextIndex;
    update();
}

int FieldViewWidget::previewFieldIndex() const
{
    return m_nPreviewFieldIndex;
}

FieldViewWidget::FieldState FieldViewWidget::fieldState(int row, int column) const
{
    if (!isValidField(row, column))
    {
        return FieldState::Default;
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
    emit fieldSelectionChanged();
    update();
}

void FieldViewWidget::setFieldState(int index, FieldState state)
{
    if (index < 0 || index >= m_states.size())
    {
        return;
    }

    setFieldState(index / m_nColumns, index % m_nColumns, state);
}

void FieldViewWidget::setFieldStates(const QSet<int> &indexes, FieldState state)
{
    bool changed = false;
    for (const int index : indexes)
    {
        if (index < 0 || index >= m_states.size())
        {
            continue;
        }

        if (m_states.at(index) != state)
        {
            m_states[index] = state;
            emit fieldStateChanged(index / m_nColumns, index % m_nColumns, state);
            changed = true;
        }
    }

    if (changed)
    {
        emit fieldSelectionChanged();
        update();
    }
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
                slot = FieldState::Default;
                emit fieldStateChanged(row, column, slot);
                changed = true;
            }
        }
    }

    if (changed)
    {
        emit fieldSelectionChanged();
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
            if (slot != FieldState::Default)
            {
                slot = FieldState::Default;
                emit fieldStateChanged(row, column, slot);
                changed = true;
            }
        }
    }

    if (m_nPreviewFieldIndex >= 0)
    {
        m_nPreviewFieldIndex = -1;
        changed = true;
    }

    if (changed)
    {
        emit fieldSelectionChanged();
        update();
    }
}

QSet<int> FieldViewWidget::selectedFieldIndexes() const
{
    QSet<int> indexes;
    for (int index = 0; index < m_states.size(); ++index)
    {
        if (m_states.at(index) == FieldState::Selected
            || m_states.at(index) == FieldState::Scanning
            || m_states.at(index) == FieldState::Completed)
        {
            indexes.insert(index);
        }
    }

    return indexes;
}

QSet<int> FieldViewWidget::fieldIndexesByState(FieldState state) const
{
    QSet<int> indexes;
    for (int index = 0; index < m_states.size(); ++index)
    {
        if (m_states.at(index) == state)
        {
            indexes.insert(index);
        }
    }

    return indexes;
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

    if (m_bDragging)
    {
        painter.setPen(QPen(QColor(55, 140, 255, 220), 1.2));
        painter.setBrush(QColor(80, 155, 255, 42));
        painter.drawRect(selectionRect());
    }
}

void FieldViewWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if (!m_bSelectionEnabled)
    {
        if (m_bPreviewEnabled)
        {
            const int index = fieldIndexAt(event->pos());
            if (index >= 0)
            {
                setPreviewFieldIndex(index);
                emit fieldPreviewed(index);
                event->accept();
                return;
            }
        }

        QWidget::mousePressEvent(event);
        return;
    }

    m_bDragging = true;
    m_dragStart = event->pos();
    m_dragCurrent = event->pos();
    m_dragSnapshot = m_states;
    updateSelectionFromDrag();
    event->accept();
}

void FieldViewWidget::mouseMoveEvent(QMouseEvent *event)
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

void FieldViewWidget::mouseReleaseEvent(QMouseEvent *event)
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

int FieldViewWidget::fieldIndexAt(const QPoint &point) const
{
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            if (fieldRect(row, column).contains(point))
            {
                return stateIndex(row, column);
            }
        }
    }

    return -1;
}

bool FieldViewWidget::isValidField(int row, int column) const
{
    return row >= 0 && row < m_nRows && column >= 0 && column < m_nColumns;
}

QRectF FieldViewWidget::gridRect() const
{
    return QRectF(rect()).adjusted(14, 14, -14, -14);
}

QRectF FieldViewWidget::fieldRect(int row, int column) const
{
    const QRectF rect = gridRect();
    const qreal cellWidth = rect.width() / qMax(1, m_nColumns);
    const qreal cellHeight = rect.height() / qMax(1, m_nRows);
    return QRectF(rect.left() + column * cellWidth,
                  rect.top() + row * cellHeight,
                  cellWidth,
                  cellHeight);
}

QRect FieldViewWidget::selectionRect() const
{
    return QRect(m_dragStart, m_dragCurrent).normalized();
}

void FieldViewWidget::updateSelectionFromDrag()
{
    const QRect dragRect = selectionRect();
    const bool pointPick = dragRect.width() < 3 && dragRect.height() < 3;
    QVector<FieldState> nextStates = m_dragSnapshot.isEmpty() ? m_states : m_dragSnapshot;

    for (int index = 0; index < nextStates.size(); ++index)
    {
        if (nextStates.at(index) == FieldState::Selected)
        {
            nextStates[index] = FieldState::Default;
        }
    }

    for (int row = 0; row < m_nRows; ++row)
    {
        for (int column = 0; column < m_nColumns; ++column)
        {
            const int index = stateIndex(row, column);
            const QRectF cell = fieldRect(row, column);
            const bool inside = pointPick ? cell.contains(m_dragCurrent) : cell.intersects(QRectF(dragRect));
            if (inside)
            {
                nextStates[index] = FieldState::Selected;
            }
        }
    }

    if (nextStates != m_states)
    {
        m_states = nextStates;
        emit fieldSelectionChanged();
        update();
    }
    else if (m_bDragging)
    {
        update();
    }
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
            case FieldState::Previewing:
                color = QColor(80, 155, 255, 70);
                break;
            case FieldState::Selected:
                color = QColor(155, 155, 155, 120);
                break;
            case FieldState::Scanning:
                color = QColor(84, 190, 120, 135);
                break;
            case FieldState::Completed:
                color = QColor(252, 205, 72, 120);
                break;
            case FieldState::Default:
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

    if (m_nPreviewFieldIndex >= 0 && m_nPreviewFieldIndex < m_states.size())
    {
        const int row = m_nPreviewFieldIndex / m_nColumns;
        const int column = m_nPreviewFieldIndex % m_nColumns;
        const QRectF previewRect = fieldRect(row, column).adjusted(1.5, 1.5, -1.5, -1.5);
        pPainter->fillRect(previewRect, QColor(80, 155, 255, 80));
        pPainter->setPen(QPen(QColor(55, 130, 240), 1.2));
        pPainter->setBrush(Qt::NoBrush);
        pPainter->drawRect(previewRect);
    }
}
