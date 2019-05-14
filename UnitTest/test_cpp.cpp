#include "gtest/gtest.h"
#include "glog/logging.h"

#include <filesystem>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <random>

TEST(cpp, filesytem)
{
    LOG(INFO) << std::filesystem::current_path();
}

TEST(cpp, tuple)
{
    auto t = std::make_tuple(1, 2, 3);
    auto[a, b, c] = t;
}

TEST(cpp, random)
{
    std::vector vec = { 0,1,2,3,4,5,6 };
    std::random_device rd;
    std::mt19937 rand(rd());
    std::shuffle(vec.begin(), vec.end(), rand);
}