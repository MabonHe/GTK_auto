#include "../media_ctl_test.h"

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Entities_Normal_RO) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Entities_Normal_RW) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Entities_Normal_Twice) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities once";

    ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities twice";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Entities_Error_Inv_Entity_Id) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    struct media_entity_desc entity;
    entity.id = -1;

    int ret = ioctl(fd, MEDIA_IOC_ENUM_ENTITIES, &entity);
    EXPECT_NE(0, ret) <<" Entity with invalid Entity ID should not be enumerated";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Enum_Entities_Error_NULL_Argp) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = ioctl(fd, MEDIA_IOC_ENUM_ENTITIES,(struct media_entity_desc*) 0);
    EXPECT_NE(0, ret) <<" Fail to enumerate entity with NULL Argp";

    close_dev();
}
