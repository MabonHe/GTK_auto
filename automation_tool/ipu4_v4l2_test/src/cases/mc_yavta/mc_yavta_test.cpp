#include "mc_yavta_test.h"

#include <stdio.h>

void MC_Yavta_Test::setup_fpga_pixter_bayer(const char *format) {
    reset_links();

    link_format("Intel IPU4 CSI-2 0", 0, format);
    link_format("Intel IPU4 CSI2 BE", 0, format);
    link_format("pixter", 0, format);

    enable_link("pixter", 0, "Intel IPU4 CSI-2 0", 0);
    enable_link("Intel IPU4 CSI-2 0", 1, "Intel IPU4 CSI2 BE", 0);
    enable_link("Intel IPU4 CSI2 BE", 1, "Intel IPU4 CSI2 BE capture", 0);
}

void MC_Yavta_Test::setup_tpg_bayer(const char *format) {
    reset_links();

    link_format("Intel IPU4 TPG 0", 0, format);
    link_format("Intel IPU4 CSI2 BE", 0, format);

    enable_link("Intel IPU4 TPG 0", 0, "Intel IPU4 CSI2 BE", 0);
    enable_link("Intel IPU4 CSI2 BE", 1, "Intel IPU4 CSI2 BE capture", 0);
}

void MC_Yavta_Test::setup_tpg_mipi(const char *format) {
    reset_links();

    link_format("Intel IPU4 TPG 0", 0, format);

    enable_link("Intel IPU4 TPG 0", 0, "Intel IPU4 TPG 0 capture", 0);
}

void MC_Yavta_Test::setup_tpg_isa(const char *format, bool stats) {
    reset_links();

    link_format("Intel IPU4 TPG 0", 0, format);
    link_format("Intel IPU4 CSI2 BE", 1, format);

    enable_link("Intel IPU4 TPG 0", 0, "Intel IPU4 CSI2 BE", 0);
    enable_link("Intel IPU4 CSI2 BE", 1, "Intel IPU4 ISA", 0);
    enable_link("Intel IPU4 ISA", 1, "Intel IPU4 ISA capture", 0);
    enable_link("Intel IPU4 ISA config", 0, "Intel IPU4 ISA", 2);
    enable_link("Intel IPU4 ISA", 3, "Intel IPU4 ISA 3A stats", 0);

    if (stats)
        enable_link("Intel IPU4 ISA", 3, "Intel IPU4 3A statistics", 0);
}

// -------------------------------------protected-----------------------------------------------------------

#define FRAME_STR_SIZE 128

void MC_Yavta_Test::verify_frame_with_vector_and_size(unsigned int i, const uint8_t *vector, size_t len)
{
    uint8_t *buf = (uint8_t*)malloc(len);
    char file[FRAME_STR_SIZE];

    frame_name(file, sizeof file, i);

    FILE *f1 = fopen(file, "r+b");
    SCOPED_TRACE(::std::string("verifying file ") + file);
    EXPECT_NE((FILE*)NULL, f1);
    if (f1) {
        if (vector) {
            size_t r = fread(buf, 1, len, f1);
            size_t n;
            int result = 0;
            EXPECT_EQ(r, len);
            for(n = 0; n < len; n++) {
            if (buf[n] != vector[n])
                result = -1;
        }
        EXPECT_EQ(result, 0);
        }
        fclose(f1);
    }
    free (buf);
}

void MC_Yavta_Test::frame_name(char *str, size_t size, unsigned int i)
{
    snprintf(str, size, "frame-%6.6u.bin", SKIP_FRAMES + i);
}
