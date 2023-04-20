cmake_minimum_required(VERSION 3.5)

project(qt_serial VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

subdirs(
    serialBase
)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

qt_add_executable(qt_serial
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(qt_serial PRIVATE Qt6::Widgets Qt6::SerialPort serial_base)
target_include_directories(qt_serial PRIVATE serialBase)
set_target_properties(qt_serial PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(qt_serial)

