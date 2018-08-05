#include "mc_config.h"
#include "log.h"
#include "utils.h"

#include "gtest/gtest.h"

void MC_Config::reset_links() {
    EXPECT_EQ(exec_cmd("media-ctl -r"), 0);
}

void MC_Config::enable_link(const char *source_entity, int source_pad,
    const char *sink_entity, int sink_pad) {
    char buffer[256];

    snprintf(buffer, sizeof(buffer),
         "media-ctl -l '\"%s\":%d -> \"%s\":%d[1]'", source_entity,
         source_pad, sink_entity, sink_pad);
    LogI("MC_Config", buffer);

    EXPECT_EQ(exec_cmd(buffer), 0);
}

void MC_Config::link_format(const char *entity, int pad, const char *format)
{
    char buffer[256];

    snprintf(buffer, sizeof(buffer), "media-ctl -V '\"%s\":%d [%s]'",
        entity, pad, format);
    LogI("MC_Config", buffer);

    EXPECT_EQ(exec_cmd(buffer), 0);
}