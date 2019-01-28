#include "gtest/gtest.h"
#include "glog/logging.h"

#include "boost/version.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

TEST(boost, version)
{
    LOG(INFO) << BOOST_VERSION;
}

TEST(boost, date_time)
{
    auto curPtime = boost::posix_time::second_clock::local_time();

    LOG(INFO) << boost::posix_time::to_iso_string(curPtime);
}