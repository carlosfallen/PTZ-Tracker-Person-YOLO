cmake_minimum_required(VERSION 3.16)
project(PTZTrackerPro VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort)

# OpenCV
set(OpenCV_DIR "C:/opencv/build")
find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/VideoWidget.cpp
    src/PTZPanel.cpp
    src/LogPanel.cpp
    src/CaptureEngine.cpp
    src/PTZController.cpp
    src/YOLODetector.cpp
)

if(WIN32)
    add_executable(PTZTrackerPro WIN32 ${SOURCES})
    set_target_properties(PTZTrackerPro PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
else()
    add_executable(PTZTrackerPro ${SOURCES})
endif()

target_link_libraries(PTZTrackerPro PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
    ${OpenCV_LIBS}
)

# Copia DLLs
if(WIN32)
    add_custom_command(TARGET PTZTrackerPro POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/opencv/build/x64/vc16/bin/opencv_world480.dll"
        $<TARGET_FILE_DIR:PTZTrackerPro>
    )
    
    # windeployqt automático
    get_target_property(QMAKE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN "${QMAKE}" DIRECTORY)
    add_custom_command(TARGET PTZTrackerPro POST_BUILD
        COMMAND "${QT_BIN}/windeployqt.exe" 
        --no-translations $<TARGET_FILE:PTZTrackerPro>
    )
endif()

# Copia modelo YOLO
add_custom_command(TARGET PTZTrackerPro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/yolov8n.onnx"
    $<TARGET_FILE_DIR:PTZTrackerPro>
)
