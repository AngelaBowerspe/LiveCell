QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BasicSettingPage.cpp \
    CreateExperimentSubPage.cpp \
    FieldViewWidget.cpp \
    HistogramRangeSlider.cpp \
    ImageCorrectionPage.cpp \
    mocks/MockCameraService.cpp \
    mocks/MockStageService.cpp \
    PreviewStatusBarWidget.cpp \
    presenters/PreviewPresenter.cpp \
    SampleStageWidget.cpp \
    WellPlateWidget.cpp \
    ZStackSettingPage.cpp \
    datawidget.cpp \
    main.cpp \
    mainwindow.cpp \
    previewpage.cpp \
    scanpage.cpp

HEADERS += \
    BasicSettingPage.h \
    CreateExperimentSubPage.h \
    FieldViewWidget.h \
    HistogramRangeSlider.h \
    ImageCorrectionPage.h \
    interfaces/ICameraService.h \
    interfaces/IStageService.h \
    mocks/MockCameraService.h \
    mocks/MockStageService.h \
    models/CaptureSettings.h \
    models/ImageCorrectionSettings.h \
    models/StageControlSettings.h \
    models/ZStackSettings.h \
    PreviewStatusBarWidget.h \
    presenters/PreviewPresenter.h \
    SampleStageWidget.h \
    WellPlateWidget.h \
    ZStackSettingPage.h \
    datawidget.h \
    mainwindow.h \
    previewpage.h \
    scanpage.h

FORMS += \
    BasicSettingPage.ui \
    CreateExperimentSubPage.ui \
    ImageCorrectionPage.ui \
    PreviewStatusBarWidget.ui \
    ZStackSettingPage.ui \
    datawidget.ui \
    mainwindow.ui \
    previewpage.ui \
    scanpage.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
