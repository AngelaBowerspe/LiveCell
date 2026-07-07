#ifndef IMAGECORRECTIONPAGE_H
#define IMAGECORRECTIONPAGE_H

#include <QWidget>

namespace Ui {
class ImageCorrectionPage;
}

class ImageCorrectionPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCorrectionPage(QWidget *parent = nullptr);
    ~ImageCorrectionPage();

private:
    Ui::ImageCorrectionPage *m_pUi;
};

#endif // IMAGECORRECTIONPAGE_H
