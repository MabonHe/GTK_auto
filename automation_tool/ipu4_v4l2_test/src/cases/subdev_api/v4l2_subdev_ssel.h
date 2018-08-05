#ifndef V4L2_SUBDEV_SSEL_H
#define V4L2_SUBDEV_SSEL_H

#include "v4l2_subdev.h"

class V4L2_SubDev_SSel : public V4L2_SubDev {
public:
    virtual void setup_dev(const char *dev);    /* open subdev -> get original selection -> save it */
    virtual void TearDown();                    /* set original selection back -> close subdev */
protected:
    struct v4l2_subdev_selection osel;
};
#endif // V4L2_SUBDEV_SSEL_H
