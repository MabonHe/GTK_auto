#include "case_querystd.h"

static V4l2_Settings* mSettings = V4l2_Settings::instance();

TEST_F(Querystd_Test, CI_PRI_IPU4_BASIC_API_Querystd_Supported_V4L2_VIDIOC_G_STD) {
    EXPECT_EQ(0, vidioc_get_std(mSettings->devName)) << "Device cannot get std flags";
}

TEST_F(Querystd_Test, CI_PRI_IPU4_BASIC_API_Querystd_Supported_V4L2_VIDIOC_S_STD) {
    EXPECT_EQ(0, vidioc_set_std(mSettings->devName)) <<"Device cannot set std flags";
}
