#ifndef MEDIA_CTL_TEST_H
#define MEDIA_CTL_TEST_H

#include "media_ctl_define.h"
#include "v4l2_test_env.h"

#include <linux/media.h>

#define ENTITY_COUNT 50

/* Media entity names */
#define E_Intel_IPU4_TPG_0_NAME "Intel IPU4 TPG 0"
#define E_Intel_IPU4_TPG_0_CAPTURE_NAME "Intel IPU4 TPG 0 capture"

/* pad id's as enumerated by media device*/
#define P_Intel_IPU4_TPG_0_SOURCE 0
#define P_Intel_IPU4_TPG_0_CAPTURE_SINK 0

extern const char *MEDIA_CTL_TEST;

class Media_Ctl_Test : public testing::Test {
public:
    explicit Media_Ctl_Test();
    virtual ~Media_Ctl_Test();
    virtual void SetUp();
    virtual void TearDown();
    virtual int open_dev(const char *dev, int mode);
    virtual int close_dev();
    virtual int device_info();
    virtual int enum_entities();
    virtual int enum_links();
    virtual int setup_link(struct media_link_desc *links);

public:
    struct media_device_info devinfo;
    struct media_entity_desc entity[ENTITY_COUNT];
    struct media_links_enum linksenum;
    struct media_pad_desc *pads;
    struct media_link_desc *links;

    int fd;
    int entitynum;
    char * devName;
    int entities_count;
    int entityId_TPG_0;
    int entityId_TPG_0_CAPTURE;
};

#endif // MEDIA_CTL_TEST_H
