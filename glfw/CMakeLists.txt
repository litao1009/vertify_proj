cmake_minimum_required (VERSION 3.8)

project (glfw)

add_library(${PROJECT_NAME} STATIC IMPORTED)

set(glfw_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

file(GLOB LIB_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/glfw3.lib)
file(GLOB LIB_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/release/glfw3.lib)

set_target_properties(${PROJECT_NAME} PROPERTIES
            IMPORTED_LOCATION_DEBUG ${LIB_DEBUG}
            IMPORTED_LOCATION_RELEASE ${LIB_RELEASE}
        )
        
set_target_properties(${PROJECT_NAME} PROPERTIES
        MAP_IMPORTED_CONFIG_MINSIZEREL Release
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
    )

if ( ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        IMPORTED_LOCATION ${LIB_DEBUG}
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        IMPORTED_LOCATION ${LIB_RELEASE}
    )
endif()