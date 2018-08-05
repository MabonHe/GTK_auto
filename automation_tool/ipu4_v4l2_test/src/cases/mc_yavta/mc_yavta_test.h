#ifndef MC_YAVTA_TEST_H
#define MC_YAVTA_TEST_H

#include "v4l2_test.h"
#include "mc_config.h"
#include "utils.h"
#include <stdint.h>

class MC_Yavta_Test : public V4l2_Test, public MC_Config
{
public:
    void setup_fpga_pixter_bayer(const char *format);
    void setup_tpg_bayer(const char *format);

    void setup_tpg_mipi(const char *format);

    void setup_tpg_isa(const char *format, bool stats);

    static void verify_frame_with_vector_and_size(unsigned int i, const uint8_t *vector, size_t len);
    static void frame_name(char *str, size_t size, unsigned int i);
};
#endif // MC_YAVTA_TEST_H