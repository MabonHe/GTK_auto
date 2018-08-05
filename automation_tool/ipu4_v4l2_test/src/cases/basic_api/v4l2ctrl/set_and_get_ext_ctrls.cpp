#include "v4l2_ctrl.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

//Get controls of different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_EXT_CTRLS_Normal_ISA_V4L2_CTRL_CLASS_USER){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_USER;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_INTEL_IPU4_ISA_EN;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_EXT_CTRLS_Normal_OV_PixelArray_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_PIXEL_RATE;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_EXT_CTRLS_Normal_OV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[2];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 2;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_LINK_FREQ;
        ctrl[1].id = V4L2_CID_PIXEL_RATE;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_EXT_CTRLS_Normal_ADV_PixelArray_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_PIXEL_RATE;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_G_EXT_CTRLS_Normal_ADV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[2];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 2;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_LINK_FREQ;
        ctrl[1].id = V4L2_CID_PIXEL_RATE;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";
}

//Set controls of different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_EXT_CTRLS_Normal_ISA_V4L2_CTRL_CLASS_USER){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_USER;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_INTEL_IPU4_ISA_EN;
        ctrl[0].value = 0;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_S_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_S_EXT_CTRLS failed.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_EXT_CTRLS_Normal_OV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_LINK_FREQ;
        ctrl[0].value = 0;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_S_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_S_EXT_CTRLS failed.";
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_EXT_CTRLS_Normal_ADV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_LINK_FREQ;
        ctrl[0].value = 0;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_S_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_S_EXT_CTRLS failed.";
}

//Check the control value of different sub devices.

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_EXT_CTRLS_Check_ISA_V4L2_CTRL_CLASS_USER){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_USER;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_INTEL_IPU4_ISA_EN;
        ctrl[0].value = 0;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_S_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_S_EXT_CTRLS failed.";

        ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";

        EXPECT_EQ(0, ctrl[0].value) << "Incorrect control value, actual value is " << ctrl[0].value;
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_EXT_CTRLS_Check_OV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_LINK_FREQ;
        ctrl[0].value = 0;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_S_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_S_EXT_CTRLS failed.";

        ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";

        EXPECT_EQ(0, ctrl[0].value) << "Incorrect value, actual value is " << ctrl[0].value;
}

TEST_F(V4l2ctrl_Test, CI_PRI_IPU4_IOCTL_S_EXT_CTRLS_Check_ADV_Binner_V4L2_CTRL_CLASS_IMAGE_PROC){
        struct v4l2_ext_control ctrl[1];
        struct v4l2_ext_controls ctrls;

        ctrls.ctrl_class = V4L2_CTRL_CLASS_IMAGE_PROC;
        ctrls.count = 1;
        ctrls.controls = ctrl;
        ctrl[0].id = V4L2_CID_LINK_FREQ;
        ctrl[0].value = 0;

        ext_ctrls_setup_dev(mSettings->devName);
        int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_S_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_S_EXT_CTRLS failed.";

        ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
        if (ret < 0){
            LogE(V4L2_CTRL_TEST, "VIDIOC_G_EXT_CTRLS: %s", strerror(errno));
        }
        EXPECT_EQ(0, ret) << "VIDIOC_G_EXT_CTRLS failed.";

        EXPECT_EQ(0, ctrl[0].value) << "Incorrect control value, actual value is " << ctrl[0].value;
}
