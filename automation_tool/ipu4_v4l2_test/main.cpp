#include <iostream>
#include "gtest/gtest.h"
#include "v4l2_test_env.h"
#include "v4l2_settings.h"

int main(int argc, char** argv) {
    testing::AddGlobalTestEnvironment(new V4l2_Test_Env);
    testing::InitGoogleTest(&argc, argv);
    V4l2_Settings::init(argc, argv);

    // Runs all tests using Google Test.
    return RUN_ALL_TESTS();
}