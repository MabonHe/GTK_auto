#include "mc_stdioctl_test.h"
#include "utils.h"
#include <time.h>

MC_Config* MC_Stdioctl_Test::mMC = new MC_Config;
V4l2_Settings* MC_Stdioctl_Test::mSettings = V4l2_Settings::instance();

const char *MC_STDIOCTL_TEST = "MC_Stdioctl_Test";

MC_Stdioctl_Test::MC_Stdioctl_Test() {
}

MC_Stdioctl_Test::~MC_Stdioctl_Test() {
}

void MC_Stdioctl_Test::SetUpTestCase() {
    char format[256];
    config_capture_link(format);
}

int MC_Stdioctl_Test::get_media_ctrl_format(char *format) {
    char buffer[128];
    if (mSettings->pixelFormat == V4L2_PIX_FMT_SGRBG8) {
            strcpy(buffer, "SGRBG8");
    } else if (mSettings->pixelFormat == V4L2_PIX_FMT_SGRBG10) {
            strcpy(buffer, "SGRBG10");
    } else if (mSettings->pixelFormat == V4L2_PIX_FMT_UYVY) {
            strcpy(buffer, "YUV422");
    } else {
            LogE(MC_STDIOCTL_TEST, "%s: Unknow format code: %d", __func__, mSettings->pixelFormat);
            return -1;
    }
    sprintf(format, "fmt:%s/%dx%d", buffer, mSettings->width, mSettings->height);
    LogI(MC_STDIOCTL_TEST, "media ctrl format: %s", format);
    return 0;
}

void MC_Stdioctl_Test::TearDownTestCase() {
}

void MC_Stdioctl_Test::init_capture() {
    mCapture.mFormat.fmt.pix.width = mSettings->width;
    mCapture.mFormat.fmt.pix.height = mSettings->height;
    mCapture.mFormat.fmt.pix.pixelformat = mSettings->pixelFormat;
    mCapture.mFormat.fmt.pix.field = mSettings->fieldOrder;

    mCapture.mIsBlockMode = mSettings->isBlockMode;

    mCapture.mBufType = (v4l2_buf_type) mSettings->bufferType;
    mCapture.mMemory = (v4l2_memory) mSettings->memoryType;
    mCapture.mDmabufMode = E_DMABUF_NULL;

    if (mCapture.mMemory == V4L2_MEMORY_DMABUF)
    {
        mCapture.mDmabufMode = E_DMABUF_IMPORT;
        
        mCaptureDMAExporter.mDmabufMode = E_DMABUF_EXPORT;
        mCaptureDMAExporter.mFormat.fmt.pix.width = mSettings->width;
        mCaptureDMAExporter.mFormat.fmt.pix.height = mSettings->height;
        mCaptureDMAExporter.mFormat.fmt.pix.pixelformat = mSettings->pixelFormat;
        mCaptureDMAExporter.mFormat.fmt.pix.field = mSettings->fieldOrder;
        mCaptureDMAExporter.mIsBlockMode = mSettings->isBlockMode;
        mCaptureDMAExporter.mBufType = (v4l2_buf_type) mSettings->bufferType;
        mCaptureDMAExporter.mMemory = V4L2_MEMORY_MMAP;
    }
}

void MC_Stdioctl_Test::SetUp() {
    LogI(MC_STDIOCTL_TEST, "SetUp MC_Stdioctl_Test env");

    mSettings->print();
    init_capture();
    EXPECT_EQ(0, mCapture.open_video(mSettings->devName));

    if (mCapture.mMemory == V4L2_MEMORY_DMABUF)
    {
        if ( ! strlen(mSettings->export_devName) ){
        // choose an exporter device which is not the same with the preview device from video0.
            for (int index = 0; index <= 20; index++){
                char export_device[50];
                sprintf(export_device, "/dev/video%d", index);
                if ( strcmp(mSettings->devName, export_device) ){
                    strcpy(mSettings->export_devName,export_device);
                    break;
                }
            }
        }
         LogI(MC_STDIOCTL_TEST, "exporter device chosen: %s", mSettings->export_devName);
         EXPECT_EQ(0, mCaptureDMAExporter.open_video(mSettings->export_devName)) << "Failed to open exporter device";
         EXPECT_EQ(0, mCaptureDMAExporter.prepare()) << "Failed to prepare capture for exporter device";

         mCapture.set_import_dmabuf_fds(mCaptureDMAExporter.mBuffers);
    }
}

void MC_Stdioctl_Test::TearDown() {
    EXPECT_EQ(0, mCapture.close_video());

    if (mCapture.mMemory == V4L2_MEMORY_DMABUF)
    {
        EXPECT_EQ(0, mCaptureDMAExporter.release()) << 
            "Failed to release DMA exporter resources";
         EXPECT_EQ(0, mCaptureDMAExporter.close_video()) << 
            "Failed to close exporter device";       
    }

    LogI(MC_STDIOCTL_TEST, "TearDown MC_Stdioctl_Test env");
}

void MC_Stdioctl_Test::config_capture_link(const char *format) {
    EXPECT_EQ(0, exec_cmd(mSettings->configShellPath));
}
