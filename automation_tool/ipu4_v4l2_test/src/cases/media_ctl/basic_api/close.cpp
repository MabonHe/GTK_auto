#include "../media_ctl_test.h"

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Close_Normal_RO) {
    int fd = open(DEV_MEDIA_0, O_RDONLY);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_MEDIA_0 << " is not closed normally.";
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Close_Normal_RW) {
    int fd = open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_MEDIA_0 << " is not closed normally.";
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Close_Error_Twice) {
    int fd = open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    int ret = close(fd);
    EXPECT_EQ(0, ret) << DEV_MEDIA_0 << " is not closed normally.";

    ret = close(fd);
    EXPECT_EQ(-1, ret);
}
