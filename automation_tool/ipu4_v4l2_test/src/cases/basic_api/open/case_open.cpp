#include "v4l2_test_env.h"

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Normal_DEV5) {
    int fd = open(DEV_VIDEO_5, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_5 << " is not opened normally.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Double_DEV5) {
    int fd = open(DEV_VIDEO_5, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << DEV_VIDEO_5 << " is not opened normally.";

    fd = open(DEV_VIDEO_5, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_VIDEO_5 << " is already in use.";

    close(fd);
}

TEST(V4l2_Basic_API_Test, CI_PRI_IPU4_BASIC_API_Open_Error_Devname) {
    int fd = open("/dev/", O_RDWR | O_NONBLOCK);
    EXPECT_EQ(-1, fd) << "Device /dev/ should not be opened.";

    fd = open("-1", O_RDWR | O_NONBLOCK);
    EXPECT_EQ(-1, fd) << "Device -1 should not be opened.";

    fd = open("*&d", O_RDWR | O_NONBLOCK);
    EXPECT_EQ(-1, fd) << "Device *&d should not be opened.";
}
