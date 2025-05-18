cmake_minimum_required(VERSION 3.10)
project(GarbageClassifier)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 指定可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 找到 Qt5 的 Widgets、Multimedia、MultimediaWidgets
find_package(Qt5 COMPONENTS Widgets Multimedia MultimediaWidgets REQUIRED)
find_package(OpenCV REQUIRED)

# 包含头文件路径
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

# 源文件列表
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/Detector.h src/Detector.cpp
    src/VideoPlayer.h src/VideoPlayer.cpp
    src/CocoMap.h src/CocoMap.cpp
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    ${OpenCV_LIBS}
)
