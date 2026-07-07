QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BasicSettingPage.cpp \
    FieldViewWidget.cpp \
    ImageCorrectionPage.cpp \
    PreviewStatusBarWidget.cpp \
    SampleStageWidget.cpp \
    WellPlateWidget.cpp \
    ZStackSettingPage.cpp \
    datawidget.cpp \
    form.cpp \
    main.cpp \
    mainwindow.cpp \
    previewpage.cpp \
    scanpage.cpp

HEADERS += \
    BasicSettingPage.h \
    FieldViewWidget.h \
    ImageCorrectionPage.h \
    PreviewStatusBarWidget.h \
    SampleStageWidget.h \
    WellPlateWidget.h \
    ZStackSettingPage.h \
    datawidget.h \
    form.h \
    mainwindow.h \
    previewpage.h \
    scanpage.h

FORMS += \
    BasicSettingPage.ui \
    ImageCorrectionPage.ui \
    PreviewStatusBarWidget.ui \
    ZStackSettingPage.ui \
    datawidget.ui \
    form.ui \
    mainwindow.ui \
    previewpage.ui \
    scanpage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
