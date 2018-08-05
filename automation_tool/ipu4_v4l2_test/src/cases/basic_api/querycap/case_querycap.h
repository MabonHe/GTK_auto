#ifndef CASE_QUERYCAP_H
#define CASE_QUERYCAP_H

#include "v4l2_test.h"
#define V4L2_CAP_DEVICE_CAPS 0x80000000 /* sets device capabilities field */

class Querycap_Test : public V4l2_Test {
public:
    virtual void setup_dev(const char *dev);
protected:
    struct v4l2_capability cap;
};

#endif
