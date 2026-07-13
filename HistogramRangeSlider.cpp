#include "HistogramRangeSlider.h"

#include <QMouseEvent>
#include <QPainter>

HistogramRangeSlider::HistogramRangeSlider(QWidget *parent)
    : QWidget(parent)
    , m_nMinimum(0)
    , m_nMaximum(255)
    , m_nLowerValue(0)
    , m_nUpperValue(255)
    , m_activeHandle(ActiveHandle::None)
{
    setMinimumHeight(28);
    setMouseTracking(true);
}

int HistogramRangeSlider::minimum() const
{
    return m_nMinimum;
}

int HistogramRangeSlider::maximum() const
{
    return m_nMaximum;
}

int HistogramRangeSlider::lowerValue() const
{
    return m_nLowerValue;
}

int HistogramRangeSlider::upperValue() const
{
    return m_nUpperValue;
}

void HistogramRangeSlider::setRange(int minimum, int maximum)
{
    if (minimum >= maximum)
    {
        return;
    }

    m_nMinimum = minimum;
    m_nMaximum = maximum;
    setValues(m_nLowerValue, m_nUpperValue);
}

void HistogramRangeSlider::setLowerValue(int value)
{
    setValues(value, m_nUpperValue);
}

void HistogramRangeSlider::setUpperValue(int value)
{
    setValues(m_nLowerValue, value);
}

void HistogramRangeSlider::setValues(int lowerValue, int upperValue)
{
    const int boundedLowerValue = qBound(m_nMinimum, lowerValue, m_nMaximum);
    const int boundedUpperValue = qBound(m_nMinimum, upperValue, m_nMaximum);
    const int newLowerValue = qMin(boundedLowerValue, boundedUpperValue);
    const int newUpperValue = qMax(boundedLowerValue, boundedUpperValue);

    if (m_nLowerValue == newLowerValue && m_nUpperValue == newUpperValue)
    {
        return;
    }

    m_nLowerValue = newLowerValue;
    m_nUpperValue = newUpperValue;
    update();
    emit valuesChanged(m_nLowerValue, m_nUpperValue);
}

void HistogramRangeSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF groove = grooveRect();
    const qreal lowerX = positionFromValue(m_nLowerValue);
    const qreal upperX = positionFromValue(m_nUpperValue);

    painter.setPen(QPen(QColor(190, 205, 225), 1));
    painter.setBrush(QColor(238, 242, 247));
    painter.drawRoundedRect(groove, 2, 2);

    QRectF selectedRect(QPointF(lowerX, groove.top()), QPointF(upperX, groove.bottom()));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(92, 150, 230));
    painter.drawRoundedRect(selectedRect.normalized(), 2, 2);

    painter.setPen(QPen(QColor(45, 118, 210), 1));
    painter.setBrush(QColor(245, 248, 252));
    painter.drawEllipse(QPointF(lowerX, groove.center().y()), 6, 6);
    painter.drawEllipse(QPointF(upperX, groove.center().y()), 6, 6);
}

void HistogramRangeSlider::mousePressEvent(QMouseEvent *event)
{
    const qreal lowerDistance = qAbs(event->position().x() - positionFromValue(m_nLowerValue));
    const qreal upperDistance = qAbs(event->position().x() - positionFromValue(m_nUpperValue));
    m_activeHandle = lowerDistance <= upperDistance ? ActiveHandle::Lower : ActiveHandle::Upper;
    setActiveHandleValue(valueFromPosition(event->position().x()));
}

void HistogramRangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (m_activeHandle == ActiveHandle::None)
    {
        return;
    }

    setActiveHandleValue(valueFromPosition(event->position().x()));
}

void HistogramRangeSlider::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_activeHandle = ActiveHandle::None;
}

QRectF HistogramRangeSlider::grooveRect() const
{
    constexpr qreal handleRadius = 6.0;
    return QRectF(handleRadius,
        height() / 2.0 - 3.0,
        width() - handleRadius * 2.0,
        6.0);
}

qreal HistogramRangeSlider::positionFromValue(int value) const
{
    const QRectF groove = grooveRect();
    const qreal ratio = static_cast<qreal>(value - m_nMinimum) / (m_nMaximum - m_nMinimum);
    return groove.left() + ratio * groove.width();
}

int HistogramRangeSlider::valueFromPosition(qreal x) const
{
    const QRectF groove = grooveRect();
    const qreal boundedX = qBound(groove.left(), x, groove.right());
    const qreal ratio = (boundedX - groove.left()) / groove.width();
    return qRound(m_nMinimum + ratio * (m_nMaximum - m_nMinimum));
}

void HistogramRangeSlider::setActiveHandleValue(int value)
{
    if (m_activeHandle == ActiveHandle::Lower)
    {
        setValues(qMin(value, m_nUpperValue), m_nUpperValue);
        return;
    }

    if (m_activeHandle == ActiveHandle::Upper)
    {
        setValues(m_nLowerValue, qMax(value, m_nLowerValue));
    }
}
