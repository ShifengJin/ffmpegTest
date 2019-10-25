#-------------------------------------------------
#
# Project created by QtCreator 2019-10-10T14:21:45
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtFFmpeg
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix{
    INCLUDEPATH += $$PWD/libffmpeg/include

    //LIBS += -L$$PWD/libffmpeg/lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale
    LIBS += -L$$PWD/libffmpeg/lib -lavfilter -lavformat -lavdevice -lavcodec -lswscale -lavutil -lswresample -lpostproc
    LIBS += -L$$PWD/libx264 -lx264
    LIBS += -L$$PWD/libx264 -lfdk-aac
    LIBS += -L/usr/lib -lz -lm -ldl -lpthread -lrt

    INCLUDEPATH += $$PWD/libfaac/include
    LIBS += -L$$PWD/libfaac/lib -lfaac

    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /usr/local/include/opencv
    INCLUDEPATH += /usr/local/include/opencv2

    LIBS += /usr/local/lib/libopencv_highgui.so \
            /usr/local/lib/libopencv_core.so    \
            /usr/local/lib/libopencv_imgproc.so \
            /usr/local/lib/libopencv_imgcodecs.so\
            /usr/local/lib/libopencv_shape.so\
            /usr/local/lib/libopencv_videoio.so\
            /usr/local/lib/libopencv_calib3d.so \
            /usr/local/lib/libopencv_features2d.so \
            /usr/local/lib/libopencv_flann.so \
            /usr/local/lib/libopencv_ml.so \
            /usr/local/lib/libopencv_objdetect.so \
            /usr/local/lib/libopencv_photo.so \
            /usr/local/lib/libopencv_stitching.so \
            /usr/local/lib/libopencv_superres.so \
            /usr/local/lib/libopencv_video.so \
            /usr/local/lib/libopencv_videostab.so
}


SOURCES += \
        CCVideoWriter.cpp \
        CameraManager.cpp \
        Interface.cpp \
        MainWindow.cpp \
        ShowVideoWidget.cpp \
        main.cpp

HEADERS += \
        CCVideoWriter.h \
        CameraManager.h \
        Interface.h \
        MainWindow.h \
        ShowVideoWidget.h

FORMS += \
    MainWindow.ui
