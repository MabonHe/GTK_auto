#include "../media_ctl_test.h"

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Links_Normal_RW) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    ret = enum_links();
    EXPECT_EQ(0, ret) <<" Fail to enumerate links";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Links_Normal_RO) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    ret = enum_links();
    EXPECT_EQ(0, ret) <<" Fail to enumerate links";
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Links_Normal_Twice) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    ret = enum_links();
    EXPECT_EQ(0, ret) <<" Fail to enumerate links once";

    ret = enum_links();
    EXPECT_EQ(0, ret) <<" Fail to enumerate links twice";
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Links_Error_NULL_Argp) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    ret = ioctl(fd, MEDIA_IOC_ENUM_LINKS, (struct media_links_enum*) 0);
    EXPECT_NE(0, ret) <<" Link with NULL Argp should not be enumerated";
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Links_Error_NonExist_Entity_Id) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    struct media_entity_desc entity;
    struct media_links_enum linksenumerator;
    struct media_pad_desc *pads;
    struct media_link_desc *links;

    pads = (struct media_pad_desc*) malloc (sizeof(struct media_pad_desc) * entity.pads);
    links = (struct media_link_desc*) malloc (sizeof(struct media_link_desc) * entity.links);

    linksenumerator.entity = -1;
    linksenumerator.pads = pads;
    linksenumerator.links = links;

    ret = ioctl(fd, MEDIA_IOC_ENUM_LINKS, &linksenumerator);
    EXPECT_NE(0, ret) <<" Link with NULL Argp should not be enumerated";
}
