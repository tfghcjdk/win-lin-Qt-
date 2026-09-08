cmake_minimum_required(VERSION 3.5)

# Framework stage: source browsing only, no compiler or Qt SDK required.
# Replace LANGUAGES NONE with CXX and add Qt targets when implementation starts.
project(NEV_SmartHMI LANGUAGES NONE)

set(HMI_FRAMEWORK_FILES
    README.md
    PRD.md
    TECH_SPEC.md
    BUG_TRACKER.md
    CHANGELOG.md
    NEV-SmartHMI.pro
    git_commit_push.bat
    src/main.cpp
    src/core/vehicle_data_center.h
    src/core/vehicle_data_center.cpp
    src/core/system_manager.h
    src/core/system_manager.cpp
    src/hardware/can_bus.h
    src/hardware/can_bus.cpp
    src/hardware/serial_port.h
    src/hardware/serial_port.cpp
    src/hardware/camera_v4l2.h
    src/hardware/camera_v4l2.cpp
    src/ui/main_window.h
    src/ui/main_window.cpp
    src/common/theme_manager.h
    src/common/theme_manager.cpp
    src/common/vehicle_types.h
    assets/qss/dark_theme.qss
    scripts/git_commit.py
    scripts/build_arm.sh
    scripts/deploy_to_board.sh
    tests/mock_can_publisher.py
)

file(GLOB_RECURSE HMI_DOCUMENT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*README.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*README.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*README.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*README.md"
)

add_custom_target(NEV-SmartHMI-Skeleton
    SOURCES ${HMI_FRAMEWORK_FILES} ${HMI_DOCUMENT_FILES}
)
message(STATUS "NEV-SmartHMI: framework only; no application target is implemented.")
