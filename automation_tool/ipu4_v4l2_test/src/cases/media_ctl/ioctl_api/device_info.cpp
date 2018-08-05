#include "../media_ctl_test.h"

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Query_Info_Normal_RO) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = device_info();
    EXPECT_EQ(0, ret) <<" Fail to query media device info";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Query_Info_Normal_RW) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = device_info();
    EXPECT_EQ(0, ret) <<" Fail to query media device info";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Query_Info_Normal_Twice) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = device_info();
    EXPECT_EQ(0, ret) <<" Fail to query media device info once";

    ret = device_info();
    EXPECT_EQ(0, ret) <<" Fail to query media device info twice";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Query_Info_Error_Inv_Fd) {
    fd = -1;

    int ret = ioctl(fd, MEDIA_IOC_DEVICE_INFO, &devinfo);
    EXPECT_NE(0, ret) <<" Device information can be queried with a invalid fd";
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Query_Info_Error_NULL_Argp) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = ioctl(fd, MEDIA_IOC_DEVICE_INFO, (struct media_device_info*) 0);
    EXPECT_NE(0, ret) <<" Fail to query media device info";

    close_dev();
}
