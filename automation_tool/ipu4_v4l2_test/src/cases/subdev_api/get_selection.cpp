#include "v4l2_subdev.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

TEST_F(V4L2_SubDev, CI_PRI_IPU4_SUBDEV_G_SELECTION_Normal) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_get_selection());
}
