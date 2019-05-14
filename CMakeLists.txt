cmake_minimum_required (VERSION 3.8)

# solution name
project (vertify_sln)

enable_testing()

# CppLibPack
find_package(CppLibPack REQUIRED)

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