#ifndef V4L2_SUBDEV_H
#define V4L2_SUBDEV_H

#include "v4l2_test_env.h"
#include "v4l2_settings.h"

class V4L2_SubDev : public testing::Test {
public:
    explicit V4L2_SubDev();
    virtual ~V4L2_SubDev();

    virtual void setup_dev(const char *dev);
    virtual int v4l2_subdev_get_format();
    virtual int v4l2_subdev_set_format_return_value();
    virtual int v4l2_subdev_set_format(unsigned int width, unsigned int height, unsigned int code);
    virtual int v4l2_subdev_enum_mbus_code();
    virtual int v4l2_subdev_get_selection();
    virtual int v4l2_subdev_set_selection(unsigned int target, unsigned int flag);
    virtual void TearDown();

protected:
    int fd;
    struct v4l2_subdev_format fmt;
    struct v4l2_subdev_mbus_code_enum mbus_code_enum;
    struct v4l2_subdev_selection sel;
};
#endif // V4L2_SUBDEV_H
