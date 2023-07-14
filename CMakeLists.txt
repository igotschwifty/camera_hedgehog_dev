cmake_minimum_required(VERSION 3.25)
project(camera_hedgehog_dev)

set(CMAKE_CXX_STANDARD 20)
set(EGRABBER_INCLUDE "/opt/euresys/egrabber/include")
SET(CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/opencv4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/lib/x86_64-linux-gnu/cmake/opencv4)

find_package(OpenCV REQUIRED)

add_executable(camera_hedgehog_dev main.cpp abstract_priority_task.h RealTimeCameraTask.h pico_info.h)

set(Hedgehog_INCLUDE_DIR "/home/soc2/CLionProjects/hedgehog")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/home/soc2/CLionProjects/hedgehog/cmake_modules")

find_package(Hedgehog REQUIRED)

TARGET_LINK_LIBRARIES(
        camera_hedgehog_dev
        ${OpenCV_LIBS}
)


target_include_directories(camera_hedgehog_dev PUBLIC ${EGRABBER_INCLUDE})

