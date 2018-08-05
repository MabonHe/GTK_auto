#include "v4l2_ctrl.h"

extern const char *V4L2_CTRL_TEST = "V4L2_Ctrl_Test";

void V4l2ctrl_Test::queryctrl_setup_dev(const char *dev, const int qctrlid) {
    V4l2_Test::setup_dev(dev);
    qctrl.id = qctrlid;

    int ret = ioctl(fd, VIDIOC_QUERYCTRL, &qctrl);
    if (ret < 0){
        LogE(V4L2_CTRL_TEST, "VIDIOC_QUERYCTRL failed: %s", strerror(errno));
    }

    EXPECT_EQ(0, ret) << "VIDIOC_QUERYCTRL failed.";
}

int V4l2ctrl_Test::get_ctrl(const char *dev, const int ctrlid) {
    V4l2_Test::setup_dev(dev);
    ctrl.id = ctrlid;

    int ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if (ret < 0){
        LogE(V4L2_CTRL_TEST, "VIDIOC_G_CTRL failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int V4l2ctrl_Test::set_ctrl(const char *dev, const int ctrlid, const int ctrlvalue) {
    V4l2_Test::setup_dev(dev);
    ctrl.id = ctrlid;
    ctrl.value = ctrlvalue;

    int ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0){
        LogE(V4L2_CTRL_TEST, "VIDIOC_S_CTRL failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

void V4l2ctrl_Test::ext_ctrls_setup_dev(const char *dev) {
    V4l2_Test::setup_dev(dev);
}
