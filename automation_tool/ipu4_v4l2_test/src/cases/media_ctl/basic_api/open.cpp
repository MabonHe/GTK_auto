#include "../media_ctl_test.h"

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Open_Normal_RO) {
    int fd = open(DEV_MEDIA_0, O_RDONLY);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    close(fd);
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Open_Normal_RW) {
    int fd = open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    close(fd);
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Open_Error_Twice_RO) {
    int fd = open(DEV_MEDIA_0, O_RDONLY);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    fd = open(DEV_MEDIA_0, O_RDONLY);
    EXPECT_NE(-1, fd) << "The driver in O_RDONLY mode does not support multiple opens and " << DEV_MEDIA_0 << " is already in use.";
    close(fd);
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Open_Error_Twice_RW) {
    int fd = open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    fd=open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << "The driver in O_RDWR mode does not support multiple opens and " << DEV_MEDIA_0 << " is already in use.";
    close(fd);
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Open_Error_Twice_RO_RW) {
    int fd = open(DEV_MEDIA_0, O_RDONLY);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    fd=open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_MEDIA_0 << " is already in use.";
    close(fd);
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Open_Error_Twice_RW_RO) {
    int fd = open(DEV_MEDIA_0, O_RDWR);
    EXPECT_NE(-1, fd) << DEV_MEDIA_0 << " is not opened normally.";

    fd=open(DEV_MEDIA_0, O_RDONLY);
    EXPECT_NE(-1, fd) << "The driver does not support multiple opens and " << DEV_MEDIA_0 << " is already in use.";
    close(fd);
}
