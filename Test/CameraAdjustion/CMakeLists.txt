cmake_minimum_required(VERSION 3.5)

project(Test CXX)

add_executable(${PROJECT_NAME}
    CameraAdjustion.cpp
)

find_package(OpenCV 4 REQUIRED)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        "include"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${OpenCV_LIBS}
)