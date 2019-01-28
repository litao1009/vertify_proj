#include "gtest/gtest.h"

#include <filesystem>
#include <tuple>
#include <iostream>

TEST(cpp, filesytem)
{
    std::cout << "\t\t" << std::filesystem::current_path() << std::endl;
}

TEST(cpp, tuple)
{
    auto t = std::make_tuple(1, 2, 3);
    auto[a, b, c] = t;
}