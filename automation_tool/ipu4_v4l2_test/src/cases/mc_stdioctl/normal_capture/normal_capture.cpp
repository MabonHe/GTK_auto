#include "../mc_stdioctl_test.h"

static const char* NORMAL_CAPTURE_TAG = "Normal_Capture";
static V4l2_Settings* settings = V4l2_Settings::instance();

// -----------------------------------------------------------------------
static void frame_done(void *data, uint size, int index) {
    LogI(NORMAL_CAPTURE_TAG, "frame done: %d", index);
    char filename[256];
    sprintf(filename, "%s%d.bin", settings->resultFilePrefix, index);
    int fd = open(filename, O_CREAT | O_WRONLY);

    write(fd, data, size);
    close(fd);
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_1) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);
    EXPECT_EQ(0, mCapture.do_capture());
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_5) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);
    EXPECT_EQ(0, mCapture.do_capture(5));
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_30) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);
    EXPECT_EQ(0, mCapture.do_capture(30));
}
