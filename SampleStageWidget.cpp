#include "SampleStageWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QSizePolicy>
#include <QStringList>
#include <QtMath>

SampleStageWidget::SampleStageWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(390);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void SampleStageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(250, 250, 250));

    const QRect headerRect(0, 0, width(), 42);
    const QRect plateRect(14, 96, width() - 28, 290);
    const QRect fieldRect(14, 420, width() - 28, 300);

    drawHeader(&painter, headerRect);

    painter.setPen(QColor(40, 40, 40));
    painter.drawText(QRect(14, 56, 70, 28), Qt::AlignVCenter | Qt::AlignLeft, QStringLiteral("样品板"));

    QRect comboRect(width() - 150, 58, 136, 26);
    painter.setPen(QColor(210, 210, 210));
    painter.setBrush(QColor(245, 245, 245));
    painter.drawRect(comboRect);
    painter.setPen(QColor(20, 20, 20));
    painter.drawText(comboRect.adjusted(10, 0, -24, 0), Qt::AlignVCenter | Qt::AlignLeft, QStringLiteral("24孔板"));
    painter.drawLine(comboRect.right() - 18, comboRect.center().y() - 2, comboRect.right() - 13, comboRect.center().y() + 3);
    painter.drawLine(comboRect.right() - 13, comboRect.center().y() + 3, comboRect.right() - 8, comboRect.center().y() - 2);

    drawPlate(&painter, plateRect);
    drawFieldView(&painter, fieldRect);
}

void SampleStageWidget::drawHeader(QPainter *pPainter, const QRect &rect)
{
    pPainter->fillRect(rect, QColor(238, 241, 244));
    pPainter->setPen(QColor(50, 50, 50));
    pPainter->drawText(rect.adjusted(14, 0, -40, 0), Qt::AlignVCenter | Qt::AlignLeft, QStringLiteral("样品台"));

    const QPoint center(rect.right() - 22, rect.center().y());
    QPen pen(QColor(145, 145, 145), 2);
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawEllipse(center, 8, 8);
    for (int i = 0; i < 6; ++i)
    {
        const qreal angle = i * 60.0;
        const qreal radian = angle * 3.14159265358979323846 / 180.0;
        const QPointF p1(center.x() + qCos(radian) * 11.0, center.y() + qSin(radian) * 11.0);
        const QPointF p2(center.x() + qCos(radian) * 14.0, center.y() + qSin(radian) * 14.0);
        pPainter->drawLine(p1, p2);
    }
}

void SampleStageWidget::drawPlate(QPainter *pPainter, const QRect &rect)
{
    pPainter->fillRect(rect, QColor(246, 246, 246));
    pPainter->setPen(QColor(90, 90, 90));
    pPainter->drawText(rect.adjusted(16, 10, 0, 0), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("孔板"));

    const int rowLabelWidth = 28;
    const int leftMargin = rect.left() + 72;
    const int rightMargin = rect.right() - 20;
    const int availableWidth = qMax(1, rightMargin - leftMargin);
    const qreal columnSpacing = availableWidth / 5.0;
    const int radius = qBound(14, qRound(columnSpacing / 2.0) - 6, 20);
    const int startX = leftMargin;
    const int startY = rect.top() + 80;
    const int rowSpacing = 48;

    QPen gridPen(QColor(130, 172, 215), 2);
    pPainter->setPen(gridPen);
    pPainter->setBrush(Qt::NoBrush);

    QStringList rows;
    rows << QStringLiteral("A") << QStringLiteral("B") << QStringLiteral("C") << QStringLiteral("D");

    pPainter->setPen(QColor(85, 130, 185));
    for (int column = 0; column < 6; ++column)
    {
        const int x = qRound(startX + column * columnSpacing);
        QRect numberRect(x - 12, startY - 42, 24, 20);
        pPainter->drawText(numberRect, Qt::AlignCenter, QString::number(column + 1));
    }

    for (int row = 0; row < 4; ++row)
    {
        const int rowTextRight = startX - radius - 8;
        QRect rowRect(rowTextRight - rowLabelWidth, startY + row * rowSpacing - 10, rowLabelWidth, 20);
        pPainter->drawText(rowRect, Qt::AlignRight | Qt::AlignVCenter, rows.at(row));
    }

    pPainter->setPen(gridPen);
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 6; ++column)
        {
            const QPoint center(qRound(startX + column * columnSpacing), startY + row * rowSpacing);
            pPainter->drawEllipse(center, radius, radius);
        }
    }

    pPainter->setPen(QColor(220, 220, 220));
    pPainter->drawLine(rect.left() + 10, rect.bottom() - 16, rect.right() - 10, rect.bottom() - 16);
}

void SampleStageWidget::drawFieldView(QPainter *pPainter, const QRect &rect)
{
    pPainter->setPen(QColor(90, 90, 90));
    pPainter->drawText(rect.adjusted(16, 0, 0, 24), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("视野"));

    const QRect gridRect = rect.adjusted(18, 42, -18, -12);
    pPainter->fillRect(gridRect, QColor(250, 250, 250));

    QPen thinPen(QColor(190, 212, 235), 1);
    pPainter->setPen(thinPen);
    const int cell = qMax(16, gridRect.width() / 16);
    for (int x = gridRect.left(); x <= gridRect.right(); x += cell)
    {
        pPainter->drawLine(x, gridRect.top(), x, gridRect.bottom());
    }
    for (int y = gridRect.top(); y <= gridRect.bottom(); y += cell)
    {
        pPainter->drawLine(gridRect.left(), y, gridRect.right(), y);
    }

    pPainter->setPen(QPen(QColor(150, 185, 225), 1));
    pPainter->setBrush(Qt::NoBrush);
    const int radius = qMax(1, qMin(gridRect.width(), gridRect.height()) / 2 - 8);
    pPainter->drawEllipse(gridRect.center(), radius, radius);
}
