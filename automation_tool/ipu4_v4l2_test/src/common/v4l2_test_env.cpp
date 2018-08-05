#include "v4l2_test_env.h"

const char *V4L2_TEST_ENV_TAG = "V4l2_Test_Env";

V4l2_Test_Env::V4l2_Test_Env() {
}

V4l2_Test_Env::~V4l2_Test_Env() {
}

void V4l2_Test_Env::SetUp() {
    LogI(V4L2_TEST_ENV_TAG, "Environment SetUp");
    // To do global thing
}

void V4l2_Test_Env::TearDown() {
    // To do global thing

    LogI(V4L2_TEST_ENV_TAG, "Environment TearDown");
}