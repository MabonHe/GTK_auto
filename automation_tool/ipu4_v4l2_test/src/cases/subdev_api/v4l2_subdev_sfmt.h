#ifndef V4L2_SUBDEV_SFMT_H
#define V4L2_SUBDEV_SFMT_H

#include "v4l2_subdev.h"

class V4L2_SubDev_SFmt : public V4L2_SubDev {
public:
    virtual void setup_dev(const char *dev);    /* open subdev -> get original format -> save it */
    virtual void TearDown();                    /* set original format back -> close subdev */
protected:
    struct v4l2_subdev_format ofmt;
};
#endif // V4L2_SUBDEV_SFMT_H
