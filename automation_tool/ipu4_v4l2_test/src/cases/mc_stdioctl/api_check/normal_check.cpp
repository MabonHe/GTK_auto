#include "../mc_stdioctl_test.h"

// -----------------------------------------------------------------------
static int sfmt_normal_check(const V4l2_Capture &capture, struct v4l2_format &format) {
    int ret = ioctl(capture.mFd, VIDIOC_S_FMT, &format);
    EXPECT_EQ(0, ret) << "VIDIOC_S_FMT call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_S_FMT_Normal_Check) {
    mCapture.video_set_format_func = sfmt_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int qbuf_normal_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_QBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_QBUF call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QBUF_Normal_Check) {
    mCapture.video_qbuf_func = qbuf_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int dqbuf_normal_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_DQBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_DQBUF call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_DQBUF_Normal_Check) {
    mCapture.video_dqbuf_func = dqbuf_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int querybuf_normal_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_QUERYBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_QUERYBUF call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QUERYBUF_Normal_Check) {
    mCapture.video_querybuf_func = querybuf_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int reqbufs_normal_check(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs) {
    int ret = ioctl(capture.mFd, VIDIOC_REQBUFS, &reqbufs);
    EXPECT_EQ(0, ret) << "VIDIOC_REQBUFS call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_REQBUFS_Normal_Check) {
    mCapture.video_reqbufs_func = reqbufs_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int streamon_normal_check(const V4l2_Capture &capture, int type) {
    int ret = ioctl(capture.mFd, VIDIOC_STREAMON, &type);
    EXPECT_EQ(0, ret) << "VIDIOC_STREAMON call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_STREAMON_Normal_Check) {
    mCapture.video_stream_on_func = streamon_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int streamoff_normal_check(const V4l2_Capture &capture, int type) {
    int ret = ioctl(capture.mFd, VIDIOC_STREAMOFF, &type);
    EXPECT_EQ(0, ret) << "VIDIOC_STREAMOFF call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_STREAMOFF_Normal_Check) {
    mCapture.video_stream_off_func = streamoff_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int querycap_normal_check(const V4l2_Capture &capture, struct v4l2_capability &cap) {
    int ret = ioctl(capture.mFd, VIDIOC_QUERYCAP, &cap);
    EXPECT_EQ(0, ret) << "VIDIOC_QUERYCAP call error!";
    return ret;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QUERYCAP_Normal) {
    mCapture.video_querycap_func = querycap_normal_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_ENUM_FMT_Normal) {
    EXPECT_EQ(0, mCapture.video_enum_format(mCapture.mFmtdesc, mCapture.mBufType));
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_ENUM_FMT_Inv_Type) {
    EXPECT_NE(0, mCapture.video_enum_format(mCapture.mFmtdesc, V4L2_BUF_TYPE_VIDEO_OUTPUT));
}
