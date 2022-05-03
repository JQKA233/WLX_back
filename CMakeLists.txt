cmake_minimum_required(VERSION 3.5)

project(WLX CXX)

add_executable(${PROJECT_NAME}
    Main.cpp
    libBase/FirstProcess.cpp
    libBase/SecondProcess.cpp
    libVision/MonoDistance/Mono.cpp
    libVision/MonoDistance/Mono.hpp
    libSolver/Predict/Predict.cpp
    libSolver/Predict/Predict.hpp
    #libSolver/Predict/KF.cpp
    libBase/Common.cpp
    libBase/Common.hpp
    libVision/ArmorDetect/ArmorDetect.cpp
    libVision/ArmorDetect/ArmorDetect.hpp
    libCamera/Camera.cpp
    libCamera/Camera.hpp
    libVision/FuwenDetect/FuwenDetect.cpp
    libVision/FuwenDetect/FuwenDetect.hpp
    libSolver/PID/PID.cpp
    libSolver/PID/PID.hpp
    libHardware/Transport/Serial.cpp
    libHardware/Transport/Serial.hpp
    libSolver/Pose/Pose.cpp
    libSolver/Pose/Pose.hpp
    libBase/FirstProcess.hpp
    libBase/FirstProcess.cpp
    libBase/SecondProcess.hpp
    libBase/SecondProcess.cpp
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
        pthread
)
#Thread
add_definitions("-g -lpthread")
add_compile_options(-lpthread)
#target_link_libraries(${PROJECT_NAME} )
#Eigen
#find_package(Eigen3 REQUIRED)

#INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
add_definitions(-DPREDICT_CFG="${CMAKE_CURRENT_LIST_DIR}/Predict.yaml")