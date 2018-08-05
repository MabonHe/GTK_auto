#include "v4l2_subdev_ssel.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

TEST_F(V4L2_SubDev_SSel, CI_PRI_IPU4_SUBDEV_S_SELECTION_COMPOSE_GE) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_selection(V4L2_SEL_TGT_COMPOSE, V4L2_SEL_FLAG_GE));
}

TEST_F(V4L2_SubDev_SSel, CI_PRI_IPU4_SUBDEV_S_SELECTION_COMPOSE_LE) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_selection(V4L2_SEL_TGT_COMPOSE, V4L2_SEL_FLAG_LE));
}

TEST_F(V4L2_SubDev_SSel, CI_PRI_IPU4_SUBDEV_S_SELECTION_COMPOSE_KEEP_CONFIG) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_selection(V4L2_SEL_TGT_COMPOSE, V4L2_SEL_FLAG_KEEP_CONFIG));
}

TEST_F(V4L2_SubDev_SSel, CI_PRI_IPU4_SUBDEV_S_SELECTION_CROP_GE) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_selection(V4L2_SEL_TGT_CROP, V4L2_SEL_FLAG_GE));
}

TEST_F(V4L2_SubDev_SSel, CI_PRI_IPU4_SUBDEV_S_SELECTION_CROP_LE) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_selection(V4L2_SEL_TGT_CROP, V4L2_SEL_FLAG_LE));
}

TEST_F(V4L2_SubDev_SSel, CI_PRI_IPU4_SUBDEV_S_SELECTION_CROP_KEEP_CONFIG) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_set_selection(V4L2_SEL_TGT_CROP, V4L2_SEL_FLAG_KEEP_CONFIG));
}
