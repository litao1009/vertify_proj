#pragma once

#include "gtest/gtest.h"

#include <memory>

class GlogEnv : public testing::Environment
{
    class Imp;
    std::unique_ptr<Imp>    ImpUPtr_;

public:

    GlogEnv();

    ~GlogEnv();

public:

    void SetParam(int argc, char** argv);

public:

    virtual void SetUp() override;

    virtual void TearDown() override;
};