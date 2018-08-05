#include "../mc_stdioctl_test.h"

static const char* NORMAL_CAPTURE_TAG = "Normal_Capture";

// -----------------------------------------------------------------------
static struct timespec start_time;
static struct timespec end_time;

static void start_calc_fps() {
    memset(&start_time, 0, sizeof (struct timespec));
    memset(&end_time, 0, sizeof (struct timespec));
}

// calc fps, You need call start_calc_fps to clear time.
static double get_fps(int nframes) {
    double fps = 0.0;
    if (nframes == 0)
        return 0.0;
    uint nsec_delta = (1000000000 + end_time.tv_nsec - start_time.tv_nsec)%1000000000;
    uint sec_delta = (end_time.tv_nsec > start_time.tv_nsec)?0:1;
    //nsec_delta < 0 ? (nsec_delta + 1000000000) : nsec_delta;

    LogD(NORMAL_CAPTURE_TAG, "Total time = %d s, %d ns, Frame Count = %d", (end_time.tv_sec - start_time.tv_sec - sec_delta), nsec_delta, nframes);

    fps = ((end_time.tv_sec - start_time.tv_sec - sec_delta) * 1000000
        + nsec_delta/1000)/nframes;
    fps = fps ? 1000000.0 / fps : 0.0;
    return fps;
}

static void frame_done(void *data, uint size, int index) {
    if (index == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    } else {
        clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_360_FPS_60) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(360, 60));
    double fps = get_fps(360);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(60.0, fps) << "FPS should up to 60fps, but actually FPS = " << fps;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_360_FPS_30) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(360, 60));
    double fps = get_fps(360);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(30.0, fps) << "FPS should up to 30fps, but actually FPS = " << fps;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_360_FPS_50) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(360, 60));
    double fps = get_fps(360);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(50.0, fps) << "FPS should up to 50fps, but actually FPS = " << fps;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_6000_FPS_60) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(6000, 60));
    double fps = get_fps(6000);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(60.0, fps) << "FPS should up to 60fps, but actually FPS = " << fps;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_6000_FPS_50) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(6000, 50));
    double fps = get_fps(6000);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(50.0, fps) << "FPS should up to 50fps, but actually FPS = " << fps;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_30_FPS_60) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(30, 60));
    double fps = get_fps(30);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(60.0, fps) << "FPS should up to 60fps, but actually FPS = " << fps;
}

TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_30_FPS_50) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(30, 50));
    double fps = get_fps(30);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(50.0, fps) << "FPS should up to 50fps, but actually FPS = " << fps;
}


TEST_F(MC_Stdioctl_Test, CI_PRI_IPU4_IOCTL_Capture_Frame_30_FPS_30) {
    EXPECT_EQ(mCapture.video_set_callback(frame_done), 0);

    start_calc_fps();
    EXPECT_EQ(0, mCapture.do_capture(30, 60));
    double fps = get_fps(30);

    LogD(NORMAL_CAPTURE_TAG, "FPS = %f", fps);

    EXPECT_LE(30.0, fps) << "FPS should up to 30fps, but actually FPS = " << fps;
}

