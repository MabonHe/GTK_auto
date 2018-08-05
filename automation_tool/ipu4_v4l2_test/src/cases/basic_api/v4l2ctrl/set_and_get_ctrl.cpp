#include "v4l2_ctrl.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

//Get the control for different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_CTRL_Normal_ISA_V4L2_CID_INTEL_IPU4_ISA_EN){
        int ret = get_ctrl(mSettings->devName, V4L2_CID_INTEL_IPU4_ISA_EN);
        EXPECT_EQ(0, ret) << "Device cannot get control V4L2_CID_INTEL_IPU4_ISA_EN.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_CTRL_Normal_OV_Binner_V4L2_CID_LINK_FREQ){
        int ret = get_ctrl(mSettings->devName, V4L2_CID_LINK_FREQ);
        EXPECT_EQ(0, ret) << "Device cannot get control V4L2_CID_LINK_FREQ.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_CTRL_Normal_ADV_Binner_V4L2_CID_LINK_FREQ){
        int ret = get_ctrl(mSettings->devName, V4L2_CID_LINK_FREQ);
        EXPECT_EQ(0, ret) << "Device cannot get control V4L2_CID_LINK_FREQ.";
}

//Set the control for different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Normal_ISA_V4L2_CID_INTEL_IPU4_ISA_EN_0){
        int ret = set_ctrl(mSettings->devName, V4L2_CID_INTEL_IPU4_ISA_EN, 0);
        EXPECT_EQ(0, ret) << "Device failed to set control V4L2_CID_INTEL_IPU4_ISA_EN.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Normal_ISA_V4L2_CID_INTEL_IPU4_ISA_EN_127){
        int ret = set_ctrl(mSettings->devName, V4L2_CID_INTEL_IPU4_ISA_EN, 127);
        EXPECT_EQ(0, ret) << "Device failed to set control V4L2_CID_INTEL_IPU4_ISA_EN.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Normal_OV_Binner_V4L2_CID_LINK_FREQ_0){
        int ret = set_ctrl(mSettings->devName, V4L2_CID_LINK_FREQ, 0);
        EXPECT_EQ(0, ret) << "Device failed to set V4L2_CID_LINK_FREQ.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Normal_ADV_Binner_V4L2_CID_LINK_FREQ_0){
        int ret = set_ctrl(mSettings->devName, V4L2_CID_LINK_FREQ, 0);
        EXPECT_EQ(0, ret) << "Device failed to set V4L2_CID_LINK_FREQ.";
}

//Check the control value for different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Check_ISA_V4L2_CID_INTEL_IPU4_ISA_EN_0){
        EXPECT_EQ(0, set_ctrl(mSettings->devName, V4L2_CID_INTEL_IPU4_ISA_EN, 0)) << "VIDIOC_S_CTRL failed.";

        struct v4l2_control gctrl;
        gctrl.id = V4L2_CID_INTEL_IPU4_ISA_EN;
        int ret = ioctl(fd, VIDIOC_G_CTRL, &gctrl);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }

        EXPECT_EQ(0, ret) << "VIDIOC_G_CTRL failed.";
        EXPECT_EQ(0, gctrl.value) << "Incorrect control value, actual value is " << gctrl.value;
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Check_ISA_V4L2_CID_INTEL_IPU4_ISA_EN_127){
        EXPECT_EQ(0, set_ctrl(mSettings->devName, V4L2_CID_INTEL_IPU4_ISA_EN, 127)) << "VIDIOC_S_CTRL failed.";

        struct v4l2_control gctrl;
        gctrl.id = V4L2_CID_INTEL_IPU4_ISA_EN;
        int ret = ioctl(fd, VIDIOC_G_CTRL, &gctrl);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }

        EXPECT_EQ(0, ret) << "VIDIOC_G_CTRL failed.";
        EXPECT_EQ(127, gctrl.value) << "Incorrect control value, actual value is " << gctrl.value;
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Check_OV_Binner_V4L2_CID_LINK_FREQ_0){
        EXPECT_EQ(0, set_ctrl(mSettings->devName, V4L2_CID_LINK_FREQ, 0)) << "VIDIOC_S_CTRL failed.";

        struct v4l2_control gctrl;
        gctrl.id = V4L2_CID_LINK_FREQ;
        int ret = ioctl(fd, VIDIOC_G_CTRL, &gctrl);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }

        EXPECT_EQ(0, ret) << "VIDIOC_G_CTRL failed.";
        EXPECT_EQ(0, gctrl.value) << "Incorrect control value, actual value is " << gctrl.value;
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_CTRL_Check_ADV_Binner_V4L2_CID_LINK_FREQ_0){
        EXPECT_EQ(0, set_ctrl(mSettings->devName, V4L2_CID_LINK_FREQ, 0)) << "VIDIOC_S_CTRL failed.";

        struct v4l2_control gctrl;
        gctrl.id = V4L2_CID_LINK_FREQ;
        int ret = ioctl(fd, VIDIOC_G_CTRL, &gctrl);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }

        EXPECT_EQ(0, ret) << "VIDIOC_G_CTRL failed.";
        EXPECT_EQ(0, gctrl.value) << "Incorrect control value, actual value is " << gctrl.value;
}
