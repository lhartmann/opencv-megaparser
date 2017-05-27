#-------------------------------------------------
#
# Project created by QtCreator 2017-05-21T09:21:17
#
#-------------------------------------------------

QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = megaparser
TEMPLATE = app

SOURCES += main.cpp\
    cv_saturator.cpp \
    megaparser.cpp

HEADERS  += \
    cv_saturator.h \
    megaparser.h

FORMS    += mainwindow.ui

LIBS += \
    -lopencv_aruco \
    -lopencv_bgsegm \
    -lopencv_bioinspired \
    -lopencv_calib3d \
    -lopencv_ccalib \
    -lopencv_core \
    -lopencv_cvv \
    -lopencv_datasets \
    -lopencv_dnn \
    -lopencv_dpm \
    -lopencv_face \
    -lopencv_features2d \
    -lopencv_flann \
    -lopencv_fuzzy \
    -lopencv_hdf \
    -lopencv_highgui \
    -lopencv_imgcodecs \
    -lopencv_imgproc \
    -lopencv_line_descriptor \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_optflow \
    -lopencv_photo \
    -lopencv_plot \
    -lopencv_reg \
    -lopencv_rgbd \
    -lopencv_saliency \
    -lopencv_shape \
    -lopencv_stereo \
    -lopencv_stitching \
    -lopencv_structured_light \
    -lopencv_superres \
    -lopencv_surface_matching \
    -lopencv_text \
    -lopencv_tracking \
    -lopencv_videoio \
    -lopencv_video \
    -lopencv_videostab \
    -lopencv_ximgproc \
    -lopencv_xobjdetect \
    -lopencv_xphoto
