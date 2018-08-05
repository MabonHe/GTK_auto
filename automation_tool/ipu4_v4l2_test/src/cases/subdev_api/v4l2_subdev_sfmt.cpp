#include "v4l2_subdev_sfmt.h"

const char *V4L2_SUBDEV_SFMT_TAG = "V4L2_SubDev_SFmt";

void V4L2_SubDev_SFmt::setup_dev(const char *dev)
{
    V4L2_SubDev::setup_dev(dev);

    /* Get original format and save in struct fmt. */
    LogI(V4L2_SUBDEV_SFMT_TAG, "Case SetUp - Get original formt");
    memset(&ofmt, 0, sizeof(ofmt));
    int ret = ioctl(fd, VIDIOC_SUBDEV_G_FMT, &ofmt);
    EXPECT_NE(-1, ret) << "Fail to get original format.";
}

void V4L2_SubDev_SFmt::TearDown()
{
    /* Set original format back after TC running. */
    LogI(V4L2_SUBDEV_SFMT_TAG, "Case TearDown - Set original formt back");
    memset(&ofmt, 0, sizeof(ofmt));
    int ret = ioctl(fd, VIDIOC_SUBDEV_S_FMT, &ofmt);
    EXPECT_NE(-1, ret) << "Fail to set original format back.";

    V4L2_SubDev::TearDown();
    LogI(V4L2_SUBDEV_SFMT_TAG, "Case TearDown - Close device");
}
