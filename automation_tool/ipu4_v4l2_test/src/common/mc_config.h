#ifndef MC_CONFIG_H
#define MC_CONFIG_H

// Configuration via media-ctl tool
class MC_Config {
public:
    void reset_links();
    void link_format(const char *entity, int pad, const char *format);
    void enable_link(const char *source_entity, int source_pad,
        const char *sink_entity, int sink_pad);
};

#endif // MC_CONFIG_H