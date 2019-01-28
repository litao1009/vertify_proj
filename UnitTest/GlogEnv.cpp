#include "GlogEnv.h"

#include "glog/logging.h"

class GlogEnv::Imp
{
public:

    int     Argc_{};
    char**  Argv_{};
};

GlogEnv::GlogEnv():ImpUPtr_(std::make_unique<Imp>())
{

}

GlogEnv::~GlogEnv()
{

}

void GlogEnv::SetParam(int argc, char** argv)
{
    ImpUPtr_->Argc_ = argc;
    ImpUPtr_->Argv_ = argv;
}

void GlogEnv::SetUp()
{
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging(ImpUPtr_->Argv_[0]);
}


void GlogEnv::TearDown()
{
    
}
