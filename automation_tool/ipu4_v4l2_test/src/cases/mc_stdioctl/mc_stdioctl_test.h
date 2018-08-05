#ifndef MC_STDIOCTL_TEST_H
#define MC_STDIOCTL_TEST_H

#include "v4l2_test.h"
#include "mc_config.h"
#include "v4l2_capture.h"
#include "v4l2_settings.h"

// MC_Stdioctl_Test: Use media-ctl and standard ioctl API to test v4l2
class MC_Stdioctl_Test : public V4l2_Test
{
public:
    explicit MC_Stdioctl_Test();
    virtual ~MC_Stdioctl_Test();
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp();
    virtual void TearDown();

    V4l2_Capture mCapture;
    V4l2_Capture mCaptureDMAExporter;
    static V4l2_Settings *mSettings;
private:
    void init_capture();
    static int get_media_ctrl_format(char *format);
    static void config_capture_link(const char *format);
    static MC_Config *mMC;
};

#endif // MC_STDIOCTL_TEST_H
