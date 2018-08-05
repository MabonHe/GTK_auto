#include "media_ctl_test.h"
#include <strings.h>

extern const char *MEDIA_CTL_TEST = "MEDIA_Ctl_Test";

Media_Ctl_Test::Media_Ctl_Test() {
    fd = -1;
    entitynum = 0;
    memset(&linksenum, 0, sizeof(linksenum));
}

Media_Ctl_Test::~Media_Ctl_Test() {
    if(devName) {
        delete[] devName;
        devName = 0;
    }
}

void Media_Ctl_Test::SetUp() {
    LogI(MEDIA_CTL_TEST, "Set up Media_Ctl_Test env");
}

int Media_Ctl_Test::open_dev(const char *dev, int mode) {
    devName = new char [strlen(dev)+1];
    strcpy(devName, dev);
    fd = open(dev, mode);
    if (fd < 0) {
        LogE(MEDIA_CTL_TEST, "%s: Failed to open media device %s %s", __func__, dev, strerror(errno));
        return -1;
    }

    LogI(MEDIA_CTL_TEST, "%s: Open media device %s successfully", __func__, dev);
    return 0;
}

int Media_Ctl_Test::close_dev() {
    if (fd > 0) {
        int ret = close(fd);
        return ret;
    }
    return -1;
}

int Media_Ctl_Test::device_info() {
    if (fd < 0) {
        LogE(MEDIA_CTL_TEST, "%s: Failed to query media device info on fd = %d", __func__, fd);
        return -1;
    }

    int ret = ioctl(fd, MEDIA_IOC_DEVICE_INFO, &devinfo);
    if (ret <0) {
        LogE(MEDIA_CTL_TEST, "%s: Unable to query media device info of %s %s", __func__, devName, strerror(errno));
        return ret;
    }

    LogI(MEDIA_CTL_TEST, "%s: Query media device info successfully", __func__);
    return ret;
}

int Media_Ctl_Test::enum_entities() {
    if (fd < 0) {
        LogE(MEDIA_CTL_TEST, "%s: Failed to enumerate entities on fd = %d", __func__, fd);
        return -1;
    }

    while(1) {
        memset(&entity[entitynum], 0, sizeof(struct media_entity_desc));
        entity[entitynum].id = entitynum | MEDIA_ENT_ID_FLAG_NEXT;

        int ret = ioctl(fd, MEDIA_IOC_ENUM_ENTITIES, &entity[entitynum]);
        if (ret <0) {
            if (errno == EINVAL) {
                LogI(MEDIA_CTL_TEST, "%s: All entities are enumerated.", __func__);
                break;
            }
            else {
                LogE(MEDIA_CTL_TEST, "%s: Failed to enumerate entity %s %s", __func__, entity[entitynum].name, strerror(errno));
                return ret;
            }
        }
        else {
            LogI(MEDIA_CTL_TEST, "%s: Current entitynum is %d.", __func__, entitynum);
            if (!strcmp(entity[entitynum].name, E_Intel_IPU4_TPG_0_NAME)) {
                entityId_TPG_0 = entity[entitynum].id;
                LogI(MEDIA_CTL_TEST, "%s: entityId_TPG_0 value: %d.", __func__, entityId_TPG_0);
                LogI(MEDIA_CTL_TEST, "%s: entityId_TPG_0 entity id: %d.", __func__, entity[entitynum].id);
                LogI(MEDIA_CTL_TEST, "%s: entityId_TPG_0 entity name: %s.", __func__, entity[entitynum].name);
            }
            else if (!strcmp(entity[entitynum].name, E_Intel_IPU4_TPG_0_CAPTURE_NAME)) {
                entityId_TPG_0_CAPTURE = entity[entitynum].id;
                LogI(MEDIA_CTL_TEST, "%s: entityId_TPG_0_CAPTURE value: %d.", __func__, entityId_TPG_0_CAPTURE);
                LogI(MEDIA_CTL_TEST, "%s: entityId_TPG_0_CAPTURE entity id: %d.", __func__, entity[entitynum].id);
                LogI(MEDIA_CTL_TEST, "%s: entityId_TPG_0_CAPTURE name: %s.", __func__, entity[entitynum].name);
            }
            else
            {
                LogI(MEDIA_CTL_TEST, "%s: entity id: %d.", __func__, entity[entitynum].id);
                LogI(MEDIA_CTL_TEST, "%s: entity name: %s.", __func__, entity[entitynum].name);
            }
        }
        entitynum ++;
    }
    entities_count = entitynum;
    LogI(MEDIA_CTL_TEST, "%s: Total number of entities is %d.", __func__, entities_count);

    return 0;
}

int Media_Ctl_Test::enum_links() {
    if (fd < 0) {
        LogE(MEDIA_CTL_TEST, "%s: Failed to enumerate links on fd = %d", __func__, fd);
        return -1;
    }


    for (entitynum = 0; entitynum < entities_count; entitynum ++) {
        linksenum.pads = (struct media_pad_desc*) malloc (sizeof(struct media_pad_desc) * entity[entitynum].pads);
        linksenum.links = (struct media_link_desc*) malloc (sizeof(struct media_link_desc) * entity[entitynum].links);
        linksenum.entity = entity[entitynum].id;

        int ret = ioctl(fd, MEDIA_IOC_ENUM_LINKS, &linksenum);
        if (ret <0) {
            if (errno == EINVAL) {
                LogI(MEDIA_CTL_TEST, "%s: Links are enumerated for each entity.", __func__);
                break;
            }
            else {
                LogE(MEDIA_CTL_TEST, "%s: Unable to enumarate links for entity ID %s %s", __func__, linksenum.entity, strerror(errno));
                return ret;
            }
        }
        else {
            if (entity[entitynum].pads)
                LogI(MEDIA_CTL_TEST, "%s: entity %d: %s (%d pads)", __func__, entity[entitynum].id, entity[entitynum].name, entity[entitynum].pads);

            for (int i = 0; i < entity[entitynum].pads; i++){
                LogI(MEDIA_CTL_TEST, "%s: pad%d, %s", __func__, linksenum.pads->index,(linksenum.pads->flags & MEDIA_PAD_FL_SINK) ? "Sink" : "Source");
            }

            for (int i = 0; i < entity[entitynum].links; i++){
                if(linksenum.links->flags & MEDIA_LNK_FL_ENABLED)
                    LogI(MEDIA_CTL_TEST, "%s: link[%d:%d]------->[%d:%d]:ENABLED", __func__, linksenum.links->source.entity, linksenum.links->source.index, linksenum.links->sink.entity, linksenum.links->sink.index);
                else {
                    LogI(MEDIA_CTL_TEST, "%s: link[%d:%d]------->[%d:%d]:UNENABLED", __func__, linksenum.links->source.entity, linksenum.links->source.index, linksenum.links->sink.entity, linksenum.links->sink.index);
                }
            }
        }
        LogI(MEDIA_CTL_TEST, "%s:", __func__);

        free(linksenum.pads);
        free(linksenum.links);
        linksenum.pads = 0;
        linksenum.links = 0;
    }

    LogI(MEDIA_CTL_TEST, "%s: Finish enumerate links for each entity", __func__);
    return 0;
}

int Media_Ctl_Test::setup_link(struct media_link_desc *setlinks){
    if (fd < 0) {
        LogE(MEDIA_CTL_TEST, "%s: Failed to setup link on fd = %d", __func__, fd);
        return -1;
    }

    int ret = ioctl(fd, MEDIA_IOC_SETUP_LINK, setlinks);
    if (ret <0) {
        LogE(MEDIA_CTL_TEST, "%s: Unable to setup link for %s %s", __func__, devName, strerror(errno));
        return ret;
    }

    return ret;
}

void Media_Ctl_Test::TearDown() {
    LogI(MEDIA_CTL_TEST, "Tear down Media_Ctl_Test env");
}
