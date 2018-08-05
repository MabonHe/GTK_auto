#include "../mc_yavta_test.h"

// -------------------------------------1920x1080-------------------------------------------

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Capture_1_Frame_RAW8_1920x1080)
{
    setup_tpg_isa("fmt:SGRBG8/1920x1080", false);

    yavta_capture(true, false, true, 1, 5, true, 1920, 1080, true, "SGRBG12", "/dev/video8",
              true, false, STATS_IGNORE, "bxtA0_inl_blc_3a_1920x1080_0.pgd.txt");
}

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Capture_5_Frame_RAW8_1920x1080)
{
    setup_tpg_isa("fmt:SGRBG8/1920x1080", false);

    yavta_capture(true, false, true, 5, 5, true, 1920, 1080, true, "SGRBG12", "/dev/video8",
              true, false, STATS_IGNORE, "bxtA0_inl_blc_3a_1920x1080_0.pgd.txt");
}

// --------------------------------------256x128--------------------------------------------

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Capture_1_Frame_RAW8_256x128)
{
    setup_tpg_isa("fmt:SGRBG8/256x128", false);

    yavta_capture(true, false, true, 1, 5, true, 256, 128, true, "SGRBG12", "/dev/video8",
              true, false, STATS_IGNORE, "bxtA0_inl_blc_3a_1920x1080_0.pgd.txt");
}

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Capture_5_Frame_RAW8_256x128)
{
    setup_tpg_isa("fmt:SGRBG8/256x128", false);

    yavta_capture(true, false, true, 5, 5, true, 256, 128, true, "SGRBG12", "/dev/video8",
              true, false, STATS_IGNORE, "bxtA0_inl_blc_3a_1920x1080_0.pgd.txt");
}

// ------------------------------isa active 1920x1080------------------------------

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Active_1_Frame_RAW8_1920x1080) {
    setup_tpg_isa("fmt:SGRBG8/1920x1080", false);

    yavta_capture(true, false, true, 1, 5, true, 1920, 1080, true, "SGRBG12", "/dev/video8",
          true, true, STATS_IGNORE);
}

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Active_5_Frame_RAW8_1920x1080) {
    setup_tpg_isa("fmt:SGRBG8/1920x1080", false);

    yavta_capture(true, false, true, 5, 5, true, 1920, 1080, true, "SGRBG12", "/dev/video8",
          true, true, STATS_IGNORE);
}

// ------------------------------isa active 256x128------------------------------

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Active_1_Frame_RAW8_256x128) {
    setup_tpg_isa("fmt:SGRBG8/256x128", false);

    yavta_capture(true, false, true, 1, 5, true, 256, 128, true, "SGRBG12", "/dev/video8",
          true, true, STATS_IGNORE);
}

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Active_5_Frame_RAW8_256x128) {
    setup_tpg_isa("fmt:SGRBG8/256x128", false);

    yavta_capture(true, false, true, 5, 5, true, 256, 128, true, "SGRBG12", "/dev/video8",
          true, true, STATS_IGNORE);
}

// ------------------------------isa stats 1920x1080------------------------------

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Stats_1_Frame_RAW8_1920x1080) {
    setup_tpg_isa("fmt:SGRBG8/1920x1080", true);

    yavta_capture(true, false, true, 1, 5, true, 1920, 1080, true, "SGRBG12", "/dev/video10",
          true, false, STATS_AWB | STATS_AF | STATS_AE);
}

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Stats_5_Frame_RAW8_1920x1080) {
    setup_tpg_isa("fmt:SGRBG8/1920x1080", true);

    yavta_capture(true, false, true, 5, 5, true, 1920, 1080, true, "SGRBG12", "/dev/video10",
          true, false, STATS_AWB | STATS_AF | STATS_AE);
}

// ------------------------------isa stats 256x128------------------------------

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Stats_1_Frame_RAW8_256x128) {
    setup_tpg_isa("fmt:SGRBG8/256x128", true);

    yavta_capture(true, false, true, 1, 5, true, 256, 128, true, "SGRBG12", "/dev/video10",
          true, false, STATS_AWB | STATS_AF | STATS_AE);
}

TEST_F(MC_Yavta_Test, CI_TPG_IPU4_Yavta_ISA_Stats_5_Frame_RAW8_256x128) {
    setup_tpg_isa("fmt:SGRBG8/256x128", true);

    yavta_capture(true, false, true, 5, 5, true, 256, 128, true, "SGRBG12", "/dev/video10",
          true, false, STATS_AWB | STATS_AF | STATS_AE);
}

