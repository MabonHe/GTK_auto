#include "v4l2_test_env.h"

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Normal_DEV0) {
    int fd = open(DEV_VIDEO_0, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_0 << " is not opened normally.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Normal_DEV4) {
    int fd = open(DEV_VIDEO_4, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_4 << " is not opened normally.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Normal_DEV6) {
    int fd = open(DEV_VIDEO_6, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_6 << " is not opened normally.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Normal_DEV7) {
    int fd = open(DEV_VIDEO_7, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_7 << " is not opened normally.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Normal_DEV8) {
    int fd = open(DEV_VIDEO_8, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_8 << " is not opened normally.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Double_DEV0) {
    int fd = open(DEV_VIDEO_0, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_0 << " is not opened normally.";

    fd = open(DEV_VIDEO_0, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_VIDEO_0 << " is already in use.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Double_DEV4) {
    int fd = open(DEV_VIDEO_4, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_4 << " is not opened normally.";

    fd = open(DEV_VIDEO_4, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_VIDEO_4 << " is already in use.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Double_DEV6) {
    int fd = open(DEV_VIDEO_6, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_6 << " is not opened normally.";

    fd = open(DEV_VIDEO_6, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_VIDEO_6 << " is already in use.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Double_DEV7) {
    int fd = open(DEV_VIDEO_7, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_7 << " is not opened normally.";

    fd = open(DEV_VIDEO_7, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_VIDEO_7 << " is already in use.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Double_DEV8) {
    int fd = open(DEV_VIDEO_8, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_8 << " is not opened normally.";

    fd = open(DEV_VIDEO_8, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_VIDEO_8 << " is already in use.";

    close(fd);
}
