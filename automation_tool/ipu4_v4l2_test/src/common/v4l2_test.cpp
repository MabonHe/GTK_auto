#include "v4l2_test.h"

const char *V4L2_TEST_TAG = "V4l2_Test";

V4l2_Test::V4l2_Test() {
    fd = -1;
}

V4l2_Test::~V4l2_Test() {
}

void V4l2_Test::setup_dev(const char *dev) {
    // To do in every case
    LogI(V4L2_TEST_TAG, "Case SetUp");

    // open device
    fd = open(dev, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd);
}

void V4l2_Test::TearDown() {
    // To do in every case
    // EXPECT_NE(-1, close(fd));

    LogI(V4L2_TEST_TAG, "Case TearDown");
}
