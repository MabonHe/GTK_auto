#include "../mc_stdioctl_test.h"

// -----------------------------------------------------------------------
static V4l2_Settings* settings = V4l2_Settings::instance();

static int fmt_type_check(const V4l2_Capture &capture, struct v4l2_format &format) {
    int ret = ioctl(capture.mFd, VIDIOC_G_FMT, &format);
    EXPECT_EQ(0, ret) << "VIDIOC_G_FMT call error!";
    EXPECT_EQ(settings->bufferType, format.type) << "Get different format type with set";
    return 0;
}

static int fmt_resolution_check(const V4l2_Capture &capture, struct v4l2_format &format) {
    int ret = ioctl(capture.mFd, VIDIOC_G_FMT, &format);
    EXPECT_EQ(0, ret) << "VIDIOC_G_FMT call error!";
    EXPECT_EQ(settings->width, format.fmt.pix.width) << "Get different format width with set";
    EXPECT_EQ(settings->height, format.fmt.pix.height) << "Get different format height with set";
    return 0;
}

static int fmt_pixelformat_check(const V4l2_Capture &capture, struct v4l2_format &format) {
    int ret = ioctl(capture.mFd, VIDIOC_G_FMT, &format);
    EXPECT_EQ(0, ret) << "VIDIOC_G_FMT call error!";
    EXPECT_EQ(settings->pixelFormat, format.fmt.pix.pixelformat) << "Get different pixelformat type with set";
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_G_FMT_Type) {
    mCapture.video_get_format_func = fmt_type_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_G_FMT_Res) {
    mCapture.video_get_format_func = fmt_resolution_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_G_FMT_PixelFmt) {
    mCapture.video_get_format_func = fmt_pixelformat_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int sfmt_type_err_0(const V4l2_Capture &capture, struct v4l2_format &format) {
    format.type = 0;
    int ret = ioctl(capture.mFd, VIDIOC_S_FMT, &format);
    EXPECT_NE(0, ret) << "VIDIOC_S_FMT call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid format type";
    SUCCEED();
}

static int sfmt_type_err_negative(const V4l2_Capture &capture, struct v4l2_format &format) {
    format.type = -1;
    int ret = ioctl(capture.mFd, VIDIOC_S_FMT, &format);
    EXPECT_NE(0, ret) << "VIDIOC_S_FMT call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid format type";
    SUCCEED();
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_S_FMT_Inv_Type_0) {
    mCapture.video_set_format_func = sfmt_type_err_0;
    EXPECT_NE(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_S_FMT_Inv_Type_Negative) {
    mCapture.video_set_format_func = sfmt_type_err_negative;
    EXPECT_NE(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int tfmt_type_err_0(const V4l2_Capture &capture, struct v4l2_format &format) {
    format.type = 0;
    int ret = ioctl(capture.mFd, VIDIOC_TRY_FMT, &format);
    EXPECT_NE(0, ret) << "VIDIOC_TRY_FMT call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid format type";
    SUCCEED();
}

static int tfmt_type_err_negative(const V4l2_Capture &capture, struct v4l2_format &format) {
    format.type = -1;
    int ret = ioctl(capture.mFd, VIDIOC_TRY_FMT, &format);
    EXPECT_NE(0, ret) << "VIDIOC_TRY_FMT call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid format type";
    SUCCEED();
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_TRY_FMT_Inv_Type_0) {
    mCapture.video_try_format_func = tfmt_type_err_0;
    EXPECT_NE(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_TRY_FMT_Inv_Type_Negative) {
    mCapture.video_try_format_func = tfmt_type_err_negative;
    EXPECT_NE(0, mCapture.do_capture());
}

