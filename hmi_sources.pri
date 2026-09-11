INCLUDEPATH += $$PWD/src
SOURCES += $$PWD/src/core/vehicle_data_center.cpp \
           $$PWD/src/hardware/camera_v4l2.cpp \
           $$PWD/src/hardware/monocular_distance.cpp \
           $$PWD/src/ui/main_window.cpp \
           $$PWD/src/ui/hmi_cards.cpp \
           $$PWD/src/ui/paint_helpers.cpp
HEADERS += $$PWD/src/core/vehicle_data_center.h \
           $$PWD/src/core/system_manager.h \
           $$PWD/src/hardware/camera_v4l2.h \
           $$PWD/src/hardware/monocular_distance.h \
           $$PWD/src/ui/main_window.h \
           $$PWD/src/ui/hmi_cards.h \
           $$PWD/src/ui/paint_helpers.h

linux {
    DEFINES += HMI_WITH_OPENCV
    isEmpty(OPENCV_ROOT) {
        isEmpty(QMAKE_SYSROOT): OPENCV_ROOT = /usr
        else: OPENCV_ROOT = $$QMAKE_SYSROOT/usr
    }
    INCLUDEPATH += $$OPENCV_ROOT/include $$OPENCV_ROOT/include/opencv4
    LIBS += -L$$OPENCV_ROOT/lib -lopencv_core -lopencv_imgproc -lopencv_calib3d
    QMAKE_RPATHDIR += /usr/lib
}
