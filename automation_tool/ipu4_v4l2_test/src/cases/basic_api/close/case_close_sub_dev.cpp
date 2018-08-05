#include "v4l2_test_env.h"

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Normal_DEV0) {
    int fd = open(DEV_VIDEO_0, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_0 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_0 << " is not closed normally.";
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Normal_DEV4) {
    int fd = open(DEV_VIDEO_4, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_4 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_4 << " is not closed normally.";
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Normal_DEV6) {
    int fd = open(DEV_VIDEO_6, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_6 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_6 << " is not closed normally.";
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Normal_DEV7) {
    int fd = open(DEV_VIDEO_7, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_7 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_7 << " is not closed normally.";
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Normal_DEV8) {
    int fd = open(DEV_VIDEO_8, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_8 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_8 << " is not closed normally.";
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Error_Double_DEV0) {
    int fd = open(DEV_VIDEO_0, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_0 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_0 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Error_Double_DEV4) {
    int fd = open(DEV_VIDEO_4, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_4 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_4 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Error_Double_DEV6) {
    int fd = open(DEV_VIDEO_6, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_6 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_6 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Error_Double_DEV7) {
    int fd = open(DEV_VIDEO_7, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_7 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_7 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}

TEST(V4L2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Close_Error_Double_DEV8) {
    int fd = open(DEV_VIDEO_8, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_8 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_VIDEO_8 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}
