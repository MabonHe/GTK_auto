#ifndef CASE_QUERYSTD_H
#define CASE_QUERYSTD_H

#include "v4l2_test.h"
#include "v4l2_settings.h"

class Querystd_Test : public V4l2_Test {
public:
    explicit Querystd_Test();
    virtual ~Querystd_Test();

    virtual int vidioc_get_std(const char* dev);
    virtual int vidioc_set_std(const char* dev);
protected:
    v4l2_std_id std_id;
};


#endif
