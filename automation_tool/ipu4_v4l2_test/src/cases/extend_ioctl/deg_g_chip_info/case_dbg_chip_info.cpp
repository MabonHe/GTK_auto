#include "v4l2_test.h"
#include "v4l2_settings.h"

static V4l2_Settings* settings = V4l2_Settings::instance();

TEST_F(V4l2_Test, CI_PRI_IPU4_EXTEND_API_DBG_G_CHIP_INFO_Normal) {
    setup_dev(settings->devName);
    struct v4l2_dbg_chip_info dbg_chip_info;
    int ret = ioctl(fd, VIDIOC_DBG_G_CHIP_INFO, &dbg_chip_info);
    EXPECT_EQ(0, ret) << "VIDIOC_DBG_G_CHIP_INFO failed";
}
