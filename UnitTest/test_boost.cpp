#include "gtest/gtest.h"

#include <boost/version.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

TEST(boost, version)
{
    std::cout << "\t\t" << BOOST_VERSION << std::endl;
}

TEST(boost, date_time)
{
    auto curPtime = boost::posix_time::second_clock::local_time();
    std::cout << "\t\t" << boost::posix_time::to_iso_string(curPtime) << std::endl;
}