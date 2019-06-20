cmake_minimum_required (VERSION 3.8)

project (unit_test)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.c*)

add_executable(${PROJECT_NAME} ${SRC_LIST})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if ( CppLibPack_OpenGL_Support )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        gl3w
        glfw3
        )
endif()

# link gtest
target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest
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

# grpc
target_link_libraries(${PROJECT_NAME} PRIVATE
    grpc++_unsecure
    grpc_unsecure
    grpc
    gpr
    cares
    address_sorting
    protobuf
    z
    )

# link zip
target_link_libraries(${PROJECT_NAME} PRIVATE
    zip
    )

# link pcl
target_link_libraries(${PROJECT_NAME} PRIVATE
    pcl_kdtree
    )

add_test(
    NAME "unit_test"
    COMMAND unit_test
)