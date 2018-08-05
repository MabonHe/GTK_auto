#include "v4l2_test_env.h"

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Normal_DEV5) {
    int fd = open(DEV_VIDEO_5, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_5 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_5 << " is not closed normally.";
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Error_Double_DEV5) {
    int fd = open(DEV_VIDEO_5, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_5 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_5 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}
