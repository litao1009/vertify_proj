#include "gtest/gtest.h"

#include "glog/logging.h"

TEST(glog, logging)
{
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging("asdf");
    LOG(INFO) << "Info";
    LOG(WARNING) << "Warning";
    LOG(ERROR) << "Warning";
}