#ifndef V4L2_TEST_H
#define V4L2_TEST_H

#include "v4l2_test_env.h"

class V4l2_Test : public testing::Test {
public:
    explicit V4l2_Test();
    virtual ~V4l2_Test();

    virtual void setup_dev(const char *dev);
    virtual void TearDown();
protected:
    int fd;
};
#endif // V4L2_TEST_H
