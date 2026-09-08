# Framework stage: Qt Creator file browsing, no application binary.
TEMPLATE = aux
CONFIG -= qt

DISTFILES += \
    README.md \
    PRD.md \
    TECH_SPEC.md \
    BUG_TRACKER.md \
    CHANGELOG.md \
    CMakeLists.txt \
    git_commit_push.bat \
    src/main.cpp \
    src/core/vehicle_data_center.h \
    src/core/vehicle_data_center.cpp \
    src/core/system_manager.h \
    src/core/system_manager.cpp \
    src/hardware/can_bus.h \
    src/hardware/can_bus.cpp \
    src/hardware/serial_port.h \
    src/hardware/serial_port.cpp \
    src/hardware/camera_v4l2.h \
    src/hardware/camera_v4l2.cpp \
    src/ui/main_window.h \
    src/ui/main_window.cpp \
    src/common/theme_manager.h \
    src/common/theme_manager.cpp \
    src/common/vehicle_types.h \
    assets/qss/dark_theme.qss \
    scripts/git_commit.py \
    scripts/build_arm.sh \
    scripts/deploy_to_board.sh \
    tests/mock_can_publisher.py

DISTFILES += $$files($$PWD/doc/*.md)
DISTFILES += $$files($$PWD/src/*README.md, true)
DISTFILES += $$files($$PWD/assets/*README.md, true)
DISTFILES += scripts/README.md tests/README.md
