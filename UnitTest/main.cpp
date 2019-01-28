#include "gtest/gtest.h"

#include "GlogEnv.h"

int main(int argc, char** argv)
{
    auto glogEnv = new GlogEnv;
    glogEnv->SetParam(argc, argv);

    testing::AddGlobalTestEnvironment(glogEnv);
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}