#include "../mc_yavta_test.h"

// -------------------------------------1920x1080-------------------------------------------

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_1_Frame_RAW10_1920x1080) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1920x1080");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=1 -s 1920x1080 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_5_Frame_RAW10_1920x1080) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1920x1080");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=5 -s 1920x1080 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_100_Frame_RAW10_1920x1080) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1920x1080");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=100 -s 1920x1080 -F -f SGRBG10 /dev/video5"), 0);
}

// -------------------------------------640x480---------------------------------------------

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_1_Frame_RAW10_640x480) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/640x480");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=1 -s 640x480 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_5_Frame_RAW10_640x480) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/640x480");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=5 -s 640x480 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_100_Frame_RAW10_640x480) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/640x480");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=100 -s 640x480 -F -f SGRBG10 /dev/video5"), 0);
}

// -------------------------------------320x240---------------------------------------------

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_1_Frame_RAW10_320x240) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/320x240");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=1 -s 320x240 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_5_Frame_RAW10_320x240) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/320x240");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=5 -s 320x240 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_100_Frame_RAW10_320x240) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/320x240");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=100 -s 320x240 -F -f SGRBG10 /dev/video5"), 0);
}

// -------------------------------------1280x720---------------------------------------------

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_1_Frame_RAW10_1280x720) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1280x720");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=1 -s 1280x720 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_5_Frame_RAW10_1280x720) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1280x720");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=5 -s 1280x720 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_100_Frame_RAW10_1280x720) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1280x720");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=100 -s 1280x720 -F -f SGRBG10 /dev/video5"), 0);
}

// -------------------------------------1280x800---------------------------------------------

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_1_Frame_RAW10_1280x800) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1280x800");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=1 -s 1280x800 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_5_Frame_RAW10_1280x800) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1280x800");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=5 -s 1280x800 -F -f SGRBG10 /dev/video5"), 0);
}

TEST_F(MC_Yavta_Test, CI_ISP_IPU4_Yavta_Bayer_Capture_Pixter_100_Frame_RAW10_1280x800) {
    setup_fpga_pixter_bayer("fmt:SGRBG10/1280x800");
    EXPECT_EQ(exec_cmd("yavta -u -n1 --capture=100 -s 1280x800 -F -f SGRBG10 /dev/video5"), 0);
}
