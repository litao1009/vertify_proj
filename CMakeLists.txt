cmake_minimum_required (VERSION 3.8)

# solution name
project (vertify_sln)

enable_testing()

# CppLibPack
find_package(CppLibPack REQUIRED)

if ( WIN32 )

    add_subdirectory(gl3w)
    
    add_library(glfw STATIC IMPORTED)

    set(glfw_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)

    file(GLOB glfw_LIB_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib/debug/glfw3.lib)
    file(GLOB glfw_LIB_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib/release/glfw3.lib)

    if ( ${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set_target_properties(glfw PROPERTIES
            IMPORTED_LOCATION ${glfw_LIB_DEBUG}
            IMPORTED_LOCATION_DEBUG ${glfw_LIB_DEBUG}
            IMPORTED_LOCATION_RELEASE ${glfw_LIB_RELEASE}
        )
    else()
        set_target_properties(glfw PROPERTIES
            IMPORTED_LOCATION ${glfw_LIB_RELEASE}
            IMPORTED_LOCATION_DEBUG ${glfw_LIB_DEBUG}
            IMPORTED_LOCATION_RELEASE ${glfw_LIB_RELEASE}
        )
    endif()

    set_target_properties(glfw PROPERTIES
            MAP_IMPORTED_CONFIG_MINSIZEREL Release
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
        )
endif()

add_subdirectory(UnitTest)

CppLibPack_CopyDLL(
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    ceres
    z
    pcl_kdtree
    pcl_common
    )