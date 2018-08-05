#include "v4l2_subdev.h"

const char *V4L2_SUBDEV_TAG = "V4L2_SubDev";

V4L2_SubDev::V4L2_SubDev()
{
    fd = -1;
}

V4L2_SubDev::~V4L2_SubDev() {}

void V4L2_SubDev::setup_dev(const char *dev)
{
    LogI(V4L2_SUBDEV_TAG, "Case SetUp - Open device");
    fd = open(dev, O_RDWR | O_NONBLOCK);
    EXPECT_NE(-1, fd) << "Fail to open subdev device!";
}

int V4L2_SubDev::v4l2_subdev_get_format()
{
    memset(&fmt, 0, sizeof(fmt));
    int ret = ioctl(fd, VIDIOC_SUBDEV_G_FMT, &fmt);
    if (ret < 0) {
        LogE(V4L2_SUBDEV_TAG, "VIDIOC_SUBDEV_G_FMT failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int V4L2_SubDev::v4l2_subdev_set_format_return_value()
{
    memset(&fmt, 0, sizeof(fmt));
    int ret = ioctl(fd, VIDIOC_SUBDEV_S_FMT, &fmt);
    if (ret < 0) {
        LogE(V4L2_SUBDEV_TAG, "VIDIOC_SUBDEV_S_FMT failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int V4L2_SubDev::v4l2_subdev_set_format(unsigned int width, unsigned int height, unsigned int code)
{
    memset(&fmt, 0, sizeof(fmt));
    fmt.format.width = width;
    fmt.format.height = height;
    fmt.format.code = code;
    int ret = ioctl(fd, VIDIOC_SUBDEV_S_FMT, &fmt);
    if (ret < 0) {
        LogE(V4L2_SUBDEV_TAG, "VIDIOC_SUBDEV_S_FMT failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int V4L2_SubDev::v4l2_subdev_enum_mbus_code()
{
    memset(&mbus_code_enum, 0, sizeof(mbus_code_enum));
    int ret = ioctl(fd, VIDIOC_SUBDEV_ENUM_MBUS_CODE, &mbus_code_enum);
    if (ret < 0) {
        LogE(V4L2_SUBDEV_TAG, "VIDIOC_SUBDEV_ENUM_MBUS_CODE failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int V4L2_SubDev::v4l2_subdev_get_selection()
{
    memset(&sel, 0, sizeof(sel));
    int ret = ioctl(fd, VIDIOC_SUBDEV_G_SELECTION, &sel);
    if (ret < 0) {
        LogE(V4L2_SUBDEV_TAG, "VIDIOC_SUBDEV_S_SELECTION failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int V4L2_SubDev::v4l2_subdev_set_selection(unsigned int target, unsigned int flag)
{
    memset(&sel, 0, sizeof(sel));
    sel.target = target;
    sel.flags = flag;
    int ret = ioctl(fd, VIDIOC_SUBDEV_S_SELECTION, &sel);
    if (ret < 0) {
        LogE(V4L2_SUBDEV_TAG, "VIDIOC_SUBDEV_S_SELECTION failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

void V4L2_SubDev::TearDown()
{
    EXPECT_NE(-1, close(fd));
    LogI(V4L2_SUBDEV_TAG, "Case TearDown - Close device");
}
