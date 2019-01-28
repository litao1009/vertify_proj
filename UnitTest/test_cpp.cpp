#include "gtest/gtest.h"
#include "glog/logging.h"

#include <filesystem>
#include <tuple>
#include <iostream>

TEST(cpp, filesytem)
{
    LOG(INFO) << std::filesystem::current_path();
}

TEST(cpp, tuple)
{
    auto t = std::make_tuple(1, 2, 3);
    auto[a, b, c] = t;
}