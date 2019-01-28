cmake_minimum_required (VERSION 3.8)

# solution name
project (vertify_sln)


# values user could set
option(BUILD_SHARED_LIB "build shared or not" ON)
set(EIGEN_INCLUDE_DIR "usr/include/eigen3" CACHE PATH "Eigen include dir" )




# set compile flags
set( CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG" )

if (NOT WIN32 )
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++ -fPIC" )
	set( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -O3" )
else()
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS /wd\"4819\"" ) 
endif()

# set relative build dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(_LIB_TYPE "")
if ( BUILD_SHARED_LIB )
  set(_LIB_TYPE "SHARED")
else()
  set(_LIB_TYPE "STATIC")
endif()

add_subdirectory(UnitTest)