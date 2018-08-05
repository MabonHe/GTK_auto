#include "../media_ctl_test.h"

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Setup_Link_Normal_TPG_TPGCapture) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    struct media_link_desc link;
    memset(&link, 0, sizeof(link));

    //enable link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture
    link.flags |=  MEDIA_LNK_FL_ENABLED ;
    link.source.entity = entityId_TPG_0;
    link.source.index = P_Intel_IPU4_TPG_0_SOURCE;
    link.source.flags = MEDIA_PAD_FL_SOURCE;

    link.sink.entity = entityId_TPG_0_CAPTURE;
    link.sink.index = P_Intel_IPU4_TPG_0_CAPTURE_SINK;
    link.sink.flags = MEDIA_PAD_FL_SINK;

    ret = setup_link(&link);
    if (ret == 0)
        LogI(MEDIA_CTL_TEST, "%s: enable link successfully", __func__);
    EXPECT_EQ(0, ret) <<" Fail to enable link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture";

    //disable link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture
    link.flags &= ~MEDIA_LNK_FL_ENABLED ;
    link.source.entity = entityId_TPG_0;
    link.source.index = P_Intel_IPU4_TPG_0_SOURCE;
    link.source.flags = MEDIA_PAD_FL_SOURCE;

    link.sink.entity = entityId_TPG_0_CAPTURE;
    link.sink.index = P_Intel_IPU4_TPG_0_CAPTURE_SINK;
    link.sink.flags = MEDIA_PAD_FL_SINK;

    ret = setup_link(&link);
    if (ret == 0)
        LogI(MEDIA_CTL_TEST, "%s: disable link successfully", __func__);
    EXPECT_EQ(0, ret) <<" Fail to de-enable link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Setup_Link_Normal_TPG_TPGCapture_Twice) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    for (int setuptime=0; setuptime < 2; setuptime ++) {
        struct media_link_desc link;
        memset(&link, 0, sizeof(link));

        //enable links between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture
        link.flags |=  MEDIA_LNK_FL_ENABLED ;
        link.source.entity = entityId_TPG_0;
        link.source.index = P_Intel_IPU4_TPG_0_SOURCE;
        link.source.flags = MEDIA_PAD_FL_SOURCE;

        link.sink.entity = entityId_TPG_0_CAPTURE;
        link.sink.index = P_Intel_IPU4_TPG_0_CAPTURE_SINK;
        link.sink.flags = MEDIA_PAD_FL_SINK;

        ret = setup_link(&link);
        if (ret == 0)
            LogI(MEDIA_CTL_TEST, "%s: enable link successfully", __func__);
        EXPECT_EQ(0, ret) <<" Fail to enable link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture at " << setuptime << "time";

        //disable links between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture
        link.flags &= ~MEDIA_LNK_FL_ENABLED ;
        link.source.entity = entityId_TPG_0;
        link.source.index = P_Intel_IPU4_TPG_0_SOURCE;
        link.source.flags = MEDIA_PAD_FL_SOURCE;

        link.sink.entity = entityId_TPG_0_CAPTURE;
        link.sink.index = P_Intel_IPU4_TPG_0_CAPTURE_SINK;
        link.sink.flags = MEDIA_PAD_FL_SINK;

        ret = setup_link(&link);
        if (ret == 0)
            LogI(MEDIA_CTL_TEST, "%s: disable link successfully", __func__);
        EXPECT_EQ(0, ret) <<" Fail to de-enable link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture at " << setuptime << "time";
    }

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Setup_Link_Error_TPG_TPGCapture_RO) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDONLY)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    struct media_link_desc link;
    memset(&link, 0, sizeof(link));

    //enable links between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture
    link.flags |=  MEDIA_LNK_FL_ENABLED;
    link.source.entity = entityId_TPG_0;
    link.source.index = P_Intel_IPU4_TPG_0_SOURCE;
    link.source.flags = MEDIA_PAD_FL_SOURCE;

    link.sink.entity = entityId_TPG_0_CAPTURE;
    link.sink.index = P_Intel_IPU4_TPG_0_CAPTURE_SINK;
    link.sink.flags = MEDIA_PAD_FL_SINK;

    ret = setup_link(&link);
    EXPECT_NE(0, ret) <<" Link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture should not be enabled for read only media device";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Setup_Link_Error_TPG_TPGCapture_NonExist_Link) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = enum_entities();
    EXPECT_EQ(0, ret) <<" Fail to enumerate entities";

    struct media_link_desc link;
    memset(&link, 0, sizeof(link));

    //enable links between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture
    link.flags |=  MEDIA_LNK_FL_ENABLED;
    link.source.entity = entityId_TPG_0;
    link.source.index = -1;  // invalid pad index
    link.source.flags = MEDIA_PAD_FL_SOURCE;

    link.sink.entity = entityId_TPG_0_CAPTURE;
    link.sink.index = -1;   // invalid pad index
    link.sink.flags = MEDIA_PAD_FL_SINK;

    ret = setup_link(&link);
    EXPECT_NE(0, ret) <<" NoExist Link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture should not be enabled";

    close_dev();
}

TEST_F(Media_Ctl_Test, CI_IPU4_MC_Ioctl_Setup_Link_Error_Null_Argp) {
    EXPECT_EQ(0, open_dev(DEV_MEDIA_0, O_RDWR)) << DEV_MEDIA_0 << "is not opened normally";

    int ret = ioctl(fd, MEDIA_IOC_SETUP_LINK, (struct media_link_desc*) 0);
    EXPECT_NE(0, ret) <<" Link between Intel IPU4 TPG 0 and Intel IPU4 TPG 0 capture should not be enabled with a NULL Argp";
}
