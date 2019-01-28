#include "gtest/gtest.h"
#include "glog/logging.h"

TEST(glog, logging)
{
    LOG(INFO) << "Info";
    LOG(WARNING) << "Warning";
    LOG(ERROR) << "Error";
}