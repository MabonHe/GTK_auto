/*
 * Copyright (c) 2014 Intel Corporation. All rights Reserved.
 */

#include <stdio.h>

#define SKIP_FRAMES 0

#define STATS_IGNORE    0
#define STATS_AWB   (1 << 0)
#define STATS_AF    (1 << 1)
#define STATS_AE    (1 << 2)

int exec_cmd(const char *cmd);
void yavta_capture(bool data_prefix, bool mplane, bool userptr,
    unsigned int capture, unsigned int buffers,
    bool fill, unsigned int width, unsigned int height,
    bool write, const char *format, const char *video,
    bool use_isa_cfg_test, bool isa_active, unsigned int isa_stats, ...);
