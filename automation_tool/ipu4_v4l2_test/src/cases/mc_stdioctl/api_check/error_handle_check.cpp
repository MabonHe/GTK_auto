#include "../mc_stdioctl_test.h"

// ------------------------------------------------------------------------------------
static int sfmt_do_nothing(const V4l2_Capture &capture, struct v4l2_format &format) {
    return 0;
}

static int stream_do_nothing(const V4l2_Capture &capture, int type) {
    return 0;
}

static int reqbufs_do_nothing(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs) {
    return 0;
}

static int buf_do_nothing(const V4l2_Capture &capture, struct v4l2_buffer &buffer) {
    return 0;
}

// -------------------------------------------------------------------------------------
TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_S_FMT_Missing_Check) {
    mCapture.video_set_format_func = sfmt_do_nothing;
    EXPECT_EQ(-1, mCapture.do_capture());
}

// The case will block when not call VIDIOC_QBUF, this will be test when use poll.
TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QBUF_Missing_Check) {
    mCapture.video_qbuf_func = buf_do_nothing;
    // This case only works on non-block mode, will wait endless on block mode.
    mCapture.mIsBlockMode = false;
    EXPECT_EQ(-1, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_DQBUF_Missing_Check) {
    mCapture.video_dqbuf_func = buf_do_nothing;
    EXPECT_EQ(-1, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QUERYBUF_Missing_Check) {
    mCapture.video_querybuf_func = buf_do_nothing;
    EXPECT_EQ(-1, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_REQBUFS_Missing_Check) {
    mCapture.video_reqbufs_func = reqbufs_do_nothing;
    EXPECT_EQ(-1, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_STREAMON_Missing_Check) {
    mCapture.video_stream_on_func = stream_do_nothing;
    EXPECT_EQ(-1, mCapture.do_capture());
}
