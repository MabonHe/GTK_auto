#ifndef V4L2_TEST_ENV_H
#define V4L2_TEST_ENV_H

#include "v4l2_define.h"
#include "log.h"

#include "gtest/gtest.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <linux/videodev2.h>
#include <linux/v4l2-subdev.h>
#include <poll.h>


class V4l2_Test_Env : public testing::Environment {
public:
    explicit V4l2_Test_Env();
    virtual ~V4l2_Test_Env();

    virtual void SetUp();
    virtual void TearDown();
};
#endif // V4L2_TEST_ENV_H