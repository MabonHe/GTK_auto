#include "v4l2_subdev_ssel.h"

const char *V4L2_SUBDEV_SSEL_TAG = "V4L2_SubDev_SSel";

void V4L2_SubDev_SSel::setup_dev(const char *dev)
{
    V4L2_SubDev::setup_dev(dev);

    /* Get original selection and save in struct sel. */
    LogI(V4L2_SUBDEV_SSEL_TAG, "Case SetUp - Get original selection");
    memset(&osel, 0, sizeof(osel));
    int ret = ioctl(fd, VIDIOC_SUBDEV_G_SELECTION, &osel);
    EXPECT_NE(-1, ret) << "Fail to get original selection.";
}

void V4L2_SubDev_SSel::TearDown()
{
    /* Set original selection back after TC running. */
    LogI(V4L2_SUBDEV_SSEL_TAG, "Case TearDown - Set original selection back");
    memset(&osel, 0, sizeof(osel));
    int ret = ioctl(fd, VIDIOC_SUBDEV_S_SELECTION, &osel);
    EXPECT_NE(-1, ret) << "Fail to set original selection back.";

    V4L2_SubDev::TearDown();
    LogI(V4L2_SUBDEV_SSEL_TAG, "Case TearDown - Close device");
}
