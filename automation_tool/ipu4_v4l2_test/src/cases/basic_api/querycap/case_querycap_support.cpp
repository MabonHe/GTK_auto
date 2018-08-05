#include "case_querycap.h"

/*************************** supported capabilities for /dev/video5  ***************************/

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_VIDEO_CAPTURE_DEV5) {
    setup_dev(DEV_VIDEO_5);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) << "Device should support V4L2_CAP_VIDEO_CAPTURE.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_VIDEO_CAPTURE_MPLANE_DEV5) {
    setup_dev(DEV_VIDEO_5);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) << "Device should support V4L2_CAP_VIDEO_CAPTURE_MPLANE.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_STREAMING_DEV5) {
    setup_dev(DEV_VIDEO_5);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_STREAMING) << "Device should support V4L2_CAP_STREAMING.";
}

TEST_F(V4l2_Test, CI_PRI_IPU4_BASIC_API_Querycap_Null_Argp_DEV5) {
    setup_dev(DEV_VIDEO_5);
    int ret = ioctl(fd, VIDIOC_QUERYCAP, (struct v4l2_capability*) 0);
    EXPECT_EQ(-1, ret);
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_DEVICE_CAPS_DEV5) {
    setup_dev(DEV_VIDEO_5);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_DEVICE_CAPS) << "Device should support V4L2_CAP_DEVICE_CAPS.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_VIDEO_OUTPUT_MPLANE_DEV5) {
    setup_dev(DEV_VIDEO_5);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE) << "Device should support V4L2_CAP_VIDEO_OUTPUT_MPLANE.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_All_DEV5) {
    setup_dev(DEV_VIDEO_5);
    EXPECT_NE(0, (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
            && (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            && (cap.capabilities & V4L2_CAP_STREAMING)
            && (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE)
            && (cap.capabilities & V4L2_CAP_DEVICE_CAPS))
        << "Device should support V4L2_CAP_VIDEO_CAPTURE, V4L2_CAP_VIDEO_OUTPUT_MPLANE, V4L2_CAP_STREAMING, V4L2_CAP_VIDEO_OUTPUT_MPLANE and V4L2_CAP_DEVICE_CAPS.";
}

/*************************** supported capabilities for /dev/video0  ***************************/

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_VIDEO_CAPTURE_DEV0) {
    setup_dev(DEV_VIDEO_0);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) << "Device should support V4L2_CAP_VIDEO_CAPTURE.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_VIDEO_CAPTURE_MPLANE_DEV0) {
    setup_dev(DEV_VIDEO_0);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) << "Device should support V4L2_CAP_VIDEO_CAPTURE_MPLANE.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_STREAMING_DEV0) {
    setup_dev(DEV_VIDEO_0);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_STREAMING) << "Device should support V4L2_CAP_STREAMING.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_VIDEO_OUTPUT_MPLANE_DEV0) {
    setup_dev(DEV_VIDEO_0);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE) << "Device should support V4L2_CAP_VIDEO_OUTPUT_MPLANE.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_CAP_DEVICE_CAPS_DEV0) {
    setup_dev(DEV_VIDEO_0);
    EXPECT_NE(0, cap.capabilities & V4L2_CAP_DEVICE_CAPS) << "Device should support V4L2_CAP_DEVICE_CAPS.";
}

TEST_F(Querycap_Test, CI_PRI_IPU4_BASIC_API_Querycap_Supported_V4L2_All_DEV0) {
    setup_dev(DEV_VIDEO_0);
    EXPECT_NE(0, (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
            && (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            && (cap.capabilities & V4L2_CAP_STREAMING)
            && (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE)
            && (cap.capabilities & V4L2_CAP_DEVICE_CAPS))
        << "Device should support V4L2_CAP_VIDEO_CAPTURE, V4L2_CAP_VIDEO_CAPTURE_MPLANE, V4L2_CAP_STREAMING, V4L2_CAP_VIDEO_OUTPUT_MPLANE and V4L2_CAP_DEVICE_CAPS.";
}

TEST_F(V4l2_Test, CI_PRI_IPU4_BASIC_API_Querycap_Null_Argp_DEV0) {
    setup_dev(DEV_VIDEO_0);
    int ret = ioctl(fd, VIDIOC_QUERYCAP, (struct v4l2_capability*) 0);
    EXPECT_EQ(-1, ret);
}
