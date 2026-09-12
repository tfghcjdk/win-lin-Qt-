cmake_minimum_required(VERSION 3.5)
project(NEV_SmartHMI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.4.1 REQUIRED COMPONENTS Core Gui Widgets)
add_executable(NEV-SmartHMI WIN32
    src/main.cpp src/core/vehicle_data_center.cpp src/core/vehicle_data_center.h
    src/core/system_manager.cpp src/core/system_manager.h
    src/hardware/camera_v4l2.cpp src/hardware/camera_v4l2.h
    src/hardware/monocular_distance.cpp src/hardware/monocular_distance.h
    src/ui/main_window.cpp src/ui/main_window.h
    src/ui/hmi_cards.cpp src/ui/hmi_cards.h
    src/ui/paint_helpers.cpp src/ui/paint_helpers.h assets/hmi.qrc)
target_include_directories(NEV-SmartHMI PRIVATE src)
target_link_libraries(NEV-SmartHMI Qt5::Core Qt5::Gui Qt5::Widgets)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(OpenCV 3.4 REQUIRED COMPONENTS core imgproc calib3d)
    target_compile_definitions(NEV-SmartHMI PRIVATE HMI_WITH_OPENCV)
    target_include_directories(NEV-SmartHMI PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(NEV-SmartHMI ${OpenCV_LIBS})
    set_target_properties(NEV-SmartHMI PROPERTIES INSTALL_RPATH "/usr/lib")
endif()
if(MSVC)
    target_compile_options(NEV-SmartHMI PRIVATE /utf-8)
endif()
