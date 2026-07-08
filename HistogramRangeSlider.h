#ifndef HISTOGRAMRANGESLIDER_H
#define HISTOGRAMRANGESLIDER_H

#include <QWidget>

class HistogramRangeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramRangeSlider(QWidget *parent = nullptr);

    int minimum() const;
    int maximum() const;
    int lowerValue() const;
    int upperValue() const;

    void setRange(int minimum, int maximum);
    void setLowerValue(int value);
    void setUpperValue(int value);
    void setValues(int lowerValue, int upperValue);

signals:
    void valuesChanged(int lowerValue, int upperValue);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    enum class ActiveHandle
    {
        None,
        Lower,
        Upper
    };

    QRectF grooveRect() const;
    qreal positionFromValue(int value) const;
    int valueFromPosition(qreal x) const;
    void setActiveHandleValue(int value);

private:
    int m_nMinimum;
    int m_nMaximum;
    int m_nLowerValue;
    int m_nUpperValue;
    ActiveHandle m_activeHandle;
};

#endif // HISTOGRAMRANGESLIDER_H
