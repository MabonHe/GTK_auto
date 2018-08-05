#include "v4l2_ctrl.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

//Query controls of different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ISA_V4L2_CID_INTEL_IPU4_ISA_EN) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_INTEL_IPU4_ISA_EN);
    EXPECT_NE(0, qctrl.id & V4L2_CID_INTEL_IPU4_ISA_EN) << "Device should support control V4L2_CID_INTEL_IPU4_ISA_EN.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_OV_PixelArray_V4L2_CID_PIXEL_RATE) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_PIXEL_RATE);
    EXPECT_NE(0, qctrl.id & V4L2_CID_PIXEL_RATE) << "Device should support control V4L2_CID_PIXEL_RATE.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_OV_Binner_V4L2_CID_PIXEL_RATE) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_PIXEL_RATE);
    EXPECT_NE(0, qctrl.id & V4L2_CID_PIXEL_RATE) << "Device should support control V4L2_CID_PIXEL_RATE.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_OV_Binner_V4L2_CID_LINK_FREQ) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_LINK_FREQ);
    EXPECT_NE(0, qctrl.id & V4L2_CID_LINK_FREQ) << "Device should support control V4L2_CID_LINK_FREQ.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ADV_PixelArray_V4L2_CID_PIXEL_RATE) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_PIXEL_RATE);
    EXPECT_NE(0, qctrl.id & V4L2_CID_PIXEL_RATE) << "Device should support control V4L2_CID_PIXEL_RATE.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ADV_Binner_V4L2_CID_PIXEL_RATE) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_PIXEL_RATE);
    EXPECT_NE(0, qctrl.id & V4L2_CID_PIXEL_RATE) << "Device should support control V4L2_CID_PIXEL_RATE.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ADV_Binner_V4L2_CID_LINK_FREQ) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CID_LINK_FREQ);
    EXPECT_NE(0, qctrl.id & V4L2_CID_LINK_FREQ) << "Device should support control V4L2_CID_LINK_FREQ.";
}

//Query control class of different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ISA_V4L2_CTRL_CLASS_USER) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CTRL_CLASS_USER | V4L2_CTRL_FLAG_NEXT_CTRL);
    EXPECT_NE(0, V4L2_CTRL_ID2CLASS(qctrl.id) & V4L2_CTRL_CLASS_USER) << "Device should support control class V4L2_CTRL_CLASS_USER.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_OV_PixelArray_V4L2_CTRL_CLASS_IMAGE_PROC) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CTRL_CLASS_IMAGE_PROC | V4L2_CTRL_FLAG_NEXT_CTRL);
    EXPECT_NE(0, V4L2_CTRL_ID2CLASS(qctrl.id) & V4L2_CTRL_CLASS_IMAGE_PROC) << "Device should support control class V4L2_CTRL_CLASS_IMAGE_PROC.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_OV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CTRL_CLASS_IMAGE_PROC | V4L2_CTRL_FLAG_NEXT_CTRL);
    EXPECT_NE(0, V4L2_CTRL_ID2CLASS(qctrl.id) & V4L2_CTRL_CLASS_IMAGE_PROC) << "Device should support control class V4L2_CTRL_CLASS_IMAGE_PROC.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ADV_PixelArray_V4L2_CTRL_CLASS_IMAGE_PROC) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CTRL_CLASS_IMAGE_PROC | V4L2_CTRL_FLAG_NEXT_CTRL);
    EXPECT_NE(0, V4L2_CTRL_ID2CLASS(qctrl.id) & V4L2_CTRL_CLASS_IMAGE_PROC) << "Device should support control class V4L2_CTRL_CLASS_IMAGE_PROC.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_Queryctrl_Supported_ADV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC) {
    queryctrl_setup_dev(mSettings->devName, V4L2_CTRL_CLASS_IMAGE_PROC | V4L2_CTRL_FLAG_NEXT_CTRL);
    EXPECT_NE(0, V4L2_CTRL_ID2CLASS(qctrl.id) & V4L2_CTRL_CLASS_IMAGE_PROC) << "Device should support control class V4L2_CTRL_CLASS_IMAGE_PROC.";
}
