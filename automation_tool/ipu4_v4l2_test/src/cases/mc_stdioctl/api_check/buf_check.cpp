#include "../mc_stdioctl_test.h"

V4l2_Settings* setting = V4l2_Settings::instance();
const char* INDEX_TAG = "Buffer_Index";

// -----------------------------------------------------------------------
static int reqbuf_type_err_0(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs) {
    reqbufs.type = 0;
    int ret = ioctl(capture.mFd, VIDIOC_REQBUFS, &reqbufs);
    EXPECT_NE(0, ret) << "VIDIOC_REQBUFS call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid buffer type";
    SUCCEED();
}

static int reqbuf_type_err_negative(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs) {
    reqbufs.type = -1;
    int ret = ioctl(capture.mFd, VIDIOC_REQBUFS, &reqbufs);
    EXPECT_NE(0, ret) << "VIDIOC_REQBUFS call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid buffer type";
    SUCCEED();
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_REQBUFS_Inv_Type_0) {
    mCapture.video_reqbufs_func = reqbuf_type_err_0;
    EXPECT_NE(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_REQBUFS_Inv_Type_Negative) {
    mCapture.video_reqbufs_func = reqbuf_type_err_negative;
    EXPECT_NE(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int reqbuf_memory_err_0(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs) {
    reqbufs.memory = 0;
    int ret = ioctl(capture.mFd, VIDIOC_REQBUFS, &reqbufs);
    EXPECT_NE(0, ret) << "VIDIOC_REQBUFS call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid buffer memory";
    SUCCEED();
}

static int reqbuf_memory_err_negative(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs) {
    reqbufs.memory = -1;
    int ret = ioctl(capture.mFd, VIDIOC_REQBUFS, &reqbufs);
    EXPECT_NE(0, ret) << "VIDIOC_REQBUFS call error!";
    EXPECT_EQ(EINVAL, errno) << "Invalid buffer memory";
    SUCCEED();
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_REQBUFS_Inv_Mem_0) {
    mCapture.video_reqbufs_func = reqbuf_memory_err_0;
    EXPECT_NE(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_REQBUFS_Inv_Mem_Negative) {
    mCapture.video_reqbufs_func = reqbuf_memory_err_negative;
    EXPECT_NE(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int querybuf_type_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_QUERYBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_QUERYBUF call error!";
    EXPECT_EQ(setting->bufferType, buf.type) << "Buffer type is different with settings";
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QUERYBUF_Type) {
    mCapture.video_querybuf_func = querybuf_type_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

static int qbuf_type_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_QBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_QBUF call error!";
    EXPECT_EQ(setting->bufferType, buf.type) << "Buffer type is different with settings";
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QBUF_Type) {
    mCapture.video_qbuf_func = qbuf_type_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

static int dqbuf_type_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_DQBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_DQBUF call error!";
    EXPECT_EQ(setting->bufferType, buf.type) << "Buffer type is different with settings";
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_DQBUF_Type) {
    mCapture.video_dqbuf_func = dqbuf_type_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

// -----------------------------------------------------------------------
static int querybuf_index_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_QUERYBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_QUERYBUF call error!";
    EXPECT_LE(0, buf.index) << "Buffer index is out of bounds";
    EXPECT_GE(capture.mReqBufs.count, buf.index) << "Buffer index is out of bounds";
    if (ret != 0)
    {
        LogE(INDEX_TAG, "VIDIOC_QUERYBUF failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QUERYBUF_Index) {
    mCapture.video_querybuf_func = querybuf_index_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

static int qbuf_index_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_QBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_QBUF call error!";
    EXPECT_LE(0, buf.index) << "Buffer index is out of bounds";
    EXPECT_GE(capture.mReqBufs.count, buf.index) << "Buffer index is out of bounds";
    if (ret != 0)
    {
        LogE(INDEX_TAG, "VIDIOC_QBUF failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_QBUF_Index) {
    mCapture.video_qbuf_func = qbuf_index_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

static int dqbuf_index_check(const V4l2_Capture &capture, struct v4l2_buffer &buf) {
    int ret = ioctl(capture.mFd, VIDIOC_DQBUF, &buf);
    EXPECT_EQ(0, ret) << "VIDIOC_DQBUF call error!";
    EXPECT_LE(0, buf.index) << "Buffer index is out of bounds";
    EXPECT_GE(capture.mReqBufs.count, buf.index) << "Buffer index is out of bounds";
    if (ret != 0)
    {
        LogE(INDEX_TAG, "VIDIOC_DQBUF failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_DQBUF_Index) {
    mCapture.video_dqbuf_func = dqbuf_index_check;
    EXPECT_EQ(0, mCapture.do_capture());
}

