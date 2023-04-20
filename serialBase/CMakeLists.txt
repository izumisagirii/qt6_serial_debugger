


find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort)

set(SERIAL_SOURCES
        serialBase.h
        serialBase.cpp
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

qt_add_library(serial_base
    MANUAL_FINALIZATION
    ${SERIAL_SOURCES}
)

target_link_libraries(serial_base PRIVATE Qt6::Widgets Qt6::SerialPort)

qt_finalize_target(serial_base)

