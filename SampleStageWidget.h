#ifndef SAMPLESTAGEWIDGET_H
#define SAMPLESTAGEWIDGET_H

#include <QWidget>

class QPainter;

class SampleStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SampleStageWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawHeader(QPainter *pPainter, const QRect &rect);
    void drawPlate(QPainter *pPainter, const QRect &rect);
    void drawFieldView(QPainter *pPainter, const QRect &rect);
};

#endif // SAMPLESTAGEWIDGET_H
