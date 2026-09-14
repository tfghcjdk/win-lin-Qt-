INCLUDEPATH += $$PWD/src
SOURCES += $$PWD/src/core/vehicle_data_center.cpp \
           $$PWD/src/core/system_manager.cpp \
           $$PWD/src/core/navigation_service.cpp \
           $$PWD/src/hardware/camera_v4l2.cpp \
           $$PWD/src/hardware/monocular_distance.cpp \
           $$PWD/src/ui/main_window.cpp \
           $$PWD/src/ui/hmi_cards.cpp \
           $$PWD/src/ui/paint_helpers.cpp
HEADERS += $$PWD/src/core/vehicle_data_center.h \
           $$PWD/src/core/system_manager.h \
           $$PWD/src/core/navigation_service.h \
           $$PWD/src/hardware/camera_v4l2.h \
           $$PWD/src/hardware/monocular_distance.h \
           $$PWD/src/ui/main_window.h \
           $$PWD/src/ui/hmi_cards.h \
           $$PWD/src/ui/paint_helpers.h

linux {
    DEFINES += HMI_WITH_OPENCV
    isEmpty(OPENCV_ROOT): OPENCV_ROOT = $$(OPENCV_ROOT)
    isEmpty(OPENCV_ROOT) {
        !isEmpty(QMAKE_SYSROOT): OPENCV_ROOT = $$clean_path($$QMAKE_SYSROOT/usr)
    }
    isEmpty(OPENCV_ROOT) {
        OPENCV_TOOLCHAIN_SYSROOT = $$system($$QMAKE_CXX -print-sysroot)
        !isEmpty(OPENCV_TOOLCHAIN_SYSROOT): OPENCV_ROOT = $$clean_path($$OPENCV_TOOLCHAIN_SYSROOT/usr)
    }
    isEmpty(OPENCV_ROOT): error("OpenCV ARM development root is required. Run qmake with OPENCV_ROOT=/path/to/buildroot/output/staging/usr")

    exists($$OPENCV_ROOT/include/opencv2/calib3d.hpp) {
        OPENCV_INCLUDE_DIR = $$OPENCV_ROOT/include
    } else: exists($$OPENCV_ROOT/include/opencv4/opencv2/calib3d.hpp) {
        OPENCV_INCLUDE_DIR = $$OPENCV_ROOT/include/opencv4
    } else {
        error("Missing OpenCV header: $$OPENCV_ROOT/include/opencv2/calib3d.hpp. Install the matching ARM OpenCV 3.4.16 development files into the staging/sysroot")
    }

    message("Using ARM OpenCV root: $$OPENCV_ROOT")
    INCLUDEPATH += $$OPENCV_INCLUDE_DIR
    LIBS += -L$$OPENCV_ROOT/lib -lopencv_core -lopencv_imgproc -lopencv_calib3d
    QMAKE_RPATHDIR += /usr/lib
}
