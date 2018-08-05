#include "case_querycap.h"

void Querycap_Test::setup_dev(const char *dev) {
    V4l2_Test::setup_dev(dev);
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    EXPECT_EQ(0, ret) << "VIDIOC_QUERYCAP failed!";
}
