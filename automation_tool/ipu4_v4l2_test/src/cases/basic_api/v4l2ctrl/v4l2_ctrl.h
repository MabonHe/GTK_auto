#ifndef CASE_QUERYCTL_H
#define CASE_QUERYCTL_H

#include "v4l2_test.h"
#include "v4l2_settings.h"
#define V4L2_CID_INTEL_IPU4_BASE    (V4L2_CID_USER_BASE + 0x1080)
#define V4L2_CID_INTEL_IPU4_ISA_EN  (V4L2_CID_INTEL_IPU4_BASE + 1)

extern const char *V4L2_CTRL_TEST;

class V4l2ctrl_Test: public V4l2_Test {
public:
    virtual void queryctrl_setup_dev(const char *dev, const int qctrlid);
    virtual int get_ctrl(const char *dev, const int ctrlid);
    virtual int set_ctrl(const char *dev, const int ctrlid, const int ctrlvalue);
    virtual void ext_ctrls_setup_dev(const char *dev);

protected:
    struct v4l2_queryctrl qctrl;
    struct v4l2_control ctrl;
};

#endif
