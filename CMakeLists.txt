cmake_minimum_required(VERSION 3.16)
project(PTZTrackerPro VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Evita avisos de MSVC e melhora compatibilidade OpenCV
if(MSVC)
    add_compile_options(/W3 /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

# ---- Qt6 ----
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort Multimedia)

# ---- OpenCV ----
# Ajuste o caminho conforme sua instalação
set(OpenCV_DIR "C:/opencv/build")
find_package(OpenCV REQUIRED)

# ---- Includes ----
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Fontes ----
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

# ---- Executável ----
if(WIN32)
    add_executable(PTZTrackerPro WIN32 ${SOURCES})
    set_target_properties(PTZTrackerPro PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
else()
    add_executable(PTZTrackerPro ${SOURCES})
endif()

# ---- Linkagem ----
target_link_libraries(PTZTrackerPro PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Multimedia
    ${OpenCV_LIBS}
)

# ---- Pós-build: Copiar dependências ----
if(WIN32)
    # Copia OpenCV DLL (opcional)
    if(EXISTS "C:/opencv/build/x64/vc16/bin/opencv_world480.dll")
        add_custom_command(TARGET PTZTrackerPro POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "C:/opencv/build/x64/vc16/bin/opencv_world480.dll"
                $<TARGET_FILE_DIR:PTZTrackerPro>
            COMMENT "📦 Copiando OpenCV DLL..."
        )
    endif()

    # windeployqt automático (Qt6)
    get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
    if(EXISTS "${QT_BIN_DIR}/windeployqt.exe")
        add_custom_command(TARGET PTZTrackerPro POST_BUILD
            COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                --no-translations
                $<TARGET_FILE:PTZTrackerPro>
            COMMENT "🚀 Executando windeployqt..."
        )
    endif()
endif()

# ---- Modelo YOLO (opcional) ----
if(EXISTS "${CMAKE_SOURCE_DIR}/yolov8n.onnx")
    add_custom_command(TARGET PTZTrackerPro POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/yolov8n.onnx"
            $<TARGET_FILE_DIR:PTZTrackerPro>
        COMMENT "🤖 Copiando modelo YOLO..."
    )
endif()

# ---- Informações de build ----
message(STATUS "Build do PTZTrackerPro configurado com sucesso")
message(STATUS "OpenCV include: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")
