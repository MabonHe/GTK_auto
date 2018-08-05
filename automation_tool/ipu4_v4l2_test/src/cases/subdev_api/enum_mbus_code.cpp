#include "v4l2_subdev.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

TEST_F(V4L2_SubDev, CI_PRI_IPU4_SUBDEV_ENUM_MBUS_CODE_Normal) {
    setup_dev(mSettings->devName);
    EXPECT_EQ(0, v4l2_subdev_enum_mbus_code());
}
