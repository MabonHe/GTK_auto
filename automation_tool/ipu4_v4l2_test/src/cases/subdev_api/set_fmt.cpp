#include "v4l2_subdev.h"
#include "v4l2_subdev_sfmt.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

TEST_F(V4L2_SubDev, CI_PRI_IPU4_SUBDEV_S_FMT_Normal) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_format_return_value());
}

TEST_F(V4L2_SubDev_SFmt, CI_PRI_IPU4_SUBDEV_S_FMT_Normal_Raw10) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_format(mSettings->width, mSettings->height, V4L2_MBUS_FMT_SGRBG10_1X10));
}

TEST_F(V4L2_SubDev_SFmt, CI_PRI_IPU4_SUBDEV_S_FMT_Check_Raw10) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_format(mSettings->width, mSettings->height, V4L2_MBUS_FMT_SGRBG10_1X10));
    struct v4l2_subdev_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    int ret = ioctl(fd, VIDIOC_SUBDEV_G_FMT, &fmt);
    EXPECT_EQ(0, ret) << "VIDIOC_SUBDEV_G_FMT failed.";;
    EXPECT_EQ(mSettings->width, fmt.format.width) << "Wrong width.";
    EXPECT_EQ(mSettings->height, fmt.format.height) << "Wrong height.";
    EXPECT_EQ(V4L2_MBUS_FMT_SGRBG10_1X10, fmt.format.code) << "Wrong code.";
}
