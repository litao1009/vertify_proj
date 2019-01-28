cmake_minimum_required (VERSION 3.8)

project (unit_test)

file(GLOB SRC_LIST *.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# eigen include dir
target_include_directories(${PROJECT_NAME} PRIVATE
    ${EIGEN_INCLUDE_DIR}
    )

# link gtest
target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest
    )

# link std lib ( only valid for linux )
target_link_libraries(${PROJECT_NAME} PRIVATE
    c++fs
    pthread
    )

# link boost
target_link_libraries(${PROJECT_NAME} PRIVATE
    boost_date_time
    )

# link opencv
target_link_libraries(${PROJECT_NAME} PRIVATE
    opencv_imgcodecs
    opencv_imgproc
    opencv_core
    )

# link dlib
target_link_libraries(${PROJECT_NAME} PRIVATE
    dlib
    )

# link glog
target_link_libraries(${PROJECT_NAME} PRIVATE
    glog
    )

# link ceres
target_link_libraries(${PROJECT_NAME} PRIVATE
    ceres
    )