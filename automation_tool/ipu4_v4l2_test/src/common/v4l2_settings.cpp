#include "v4l2_settings.h"

#include "v4l2_test.h"

const char *V4L2_SETTINGS_TAG = "V4l2_Settings";

char V4l2_Settings::devName[50];
char V4l2_Settings::configShellPath[256];
char V4l2_Settings::resultFilePrefix[256];
int  V4l2_Settings::width = -1;
int  V4l2_Settings::height = -1;
int  V4l2_Settings::pixelFormat = -1;
int  V4l2_Settings::bufferType = -1;
int  V4l2_Settings::memoryType = -1;
int  V4l2_Settings::fieldOrder = -1;
bool V4l2_Settings::isBlockMode = false;
int V4l2_Settings::nstreams = 0;
int V4l2_Settings::stream_id = 0;
char V4l2_Settings::export_devName[50];

V4l2_Settings* V4l2_Settings::mInstance = NULL;
Mutex V4l2_Settings::mutex;

V4l2_Settings::V4l2_Settings() {
    LogI(V4L2_SETTINGS_TAG, "Set default parameters:");
    // Set default parameters
    if (!strlen(devName))
        strcpy(devName, DEV_VIDEO_5);
    if (!strlen(configShellPath))
        strcpy(configShellPath, "./subdev_config.sh");
    if (!strlen(resultFilePrefix))
        strcpy(resultFilePrefix, "result_frame_");
    width = width == -1 ? 1920 : width;
    height = height == -1 ? 1080 : height;
    pixelFormat = pixelFormat == -1 ? V4L2_PIX_FMT_SGRBG8 : pixelFormat;
//    bufferType = bufferType == -1 ? V4L2_BUF_TYPE_VIDEO_CAPTURE : bufferType;
    memoryType = memoryType == -1 ? V4L2_MEMORY_MMAP : memoryType;
}

V4l2_Settings* V4l2_Settings::instance() {
    if (NULL == mInstance) {
        mutex.lock();
        if (NULL == mInstance) {
            mInstance = new V4l2_Settings;
        }
        mutex.unlock();
    }
    return mInstance;
}

V4l2_Settings::~V4l2_Settings() {
    if (NULL != mInstance) {
        delete mInstance;
    }
}

void V4l2_Settings::usage() {
    LogI(V4L2_SETTINGS_TAG, "Usage: ./ipu4_v4l2_test --gtest-filter=\"MC_Stdioctl_Test.*\"");
    LogI(V4L2_SETTINGS_TAG, "  if not set value, will use default value in bracket.");
    LogI(V4L2_SETTINGS_TAG, "\t-d=devName [/dev/video5]");
    LogI(V4L2_SETTINGS_TAG, "\t-n=nodeName []");
    LogI(V4L2_SETTINGS_TAG, "\t-c=configShellPath []");
    LogI(V4L2_SETTINGS_TAG, "\t-r=resultFilePrefix [frame_]");
    LogI(V4L2_SETTINGS_TAG, "\t-w=width [1920]");
    LogI(V4L2_SETTINGS_TAG, "\t-h=height [1080]");
    LogI(V4L2_SETTINGS_TAG, "\t-p=pixelFormat [V4L2_PIX_FMT_SGRBG8]");
    LogI(V4L2_SETTINGS_TAG, "\t-b=bufferType [V4L2_BUF_TYPE_VIDEO_CAPTURE]");
    LogI(V4L2_SETTINGS_TAG, "\t-m=memoryType [V4L2_MEMORY_MMAP]");
    LogI(V4L2_SETTINGS_TAG, "\t-f=fieldOrder [V4L2_FIELD_NONE]");
    LogI(V4L2_SETTINGS_TAG, "\t-i=isBlockMode [false]");
    LogI(V4L2_SETTINGS_TAG, "\t-nstreams=1/2/3/4");
    LogI(V4L2_SETTINGS_TAG, "\t-stream_id=0/1/2/3");
    LogI(V4L2_SETTINGS_TAG, "\t-export_device=export_devName [/dev/video0]");
}

// spilt argv to key/value pair.
// e.g.: arg = "-b=/dev/video5"
// After spilt, key = "-b", value = "/dev/video5"
int spiltParameters(const char *arg, char *key, char *value) {
    char buffer[256];
    strcpy(buffer, arg);
    char *p = strtok(buffer, "=");
    if (!p)
        return -1;
    strcpy(key, p);
    p = strtok(NULL, "=");
    if (!p)
        return -1;
    strcpy(value, p);
    return 0;
}

void V4l2_Settings::init(int argc, char **argv) {
    LogI(V4L2_SETTINGS_TAG, "------------------------------------------------------");
    LogI(V4L2_SETTINGS_TAG, "::init argc = %d", argc);
    char key[128], value[256];
    char info[256];
    for (int i = 1; i < argc; i++) {
        spiltParameters(argv[i], key, value);
        if (!strcmp("-d", key)) {
            strcpy(devName, value);
            sprintf(info, "devName: %s", value);
        } else if (!strcmp("-c", key)) {
            strcpy(configShellPath, value);
            sprintf(info, "configShellPath: %s", value);
        } else if (!strcmp("-r", key)) {
            strcpy(resultFilePrefix, value);
            sprintf(info, "resultFilePrefix: %s", value);
        } else if (!strcmp("-w", key)) {
            width = atoi(value);
            sprintf(info, "width: %s", value);
        } else if (!strcmp("-h", key)) {
            if (strlen(value) == 0) {
                usage();
                return;
            }
            height = atoi(value);
            sprintf(info, "height: %s", value);
        } else if (!strcmp("-p", key)) {
            pixelFormat = string2Pixelcode(value);
            sprintf(info, "pixelFormat: %s", value);
        } else if (!strcmp("-b", key)) {
            bufferType = string2BufferType(value);
            sprintf(info, "bufferType: %s", value);
        } else if (!strcmp("-m", key)) {
            memoryType = string2MemoryType(value);
            sprintf(info, "memoryType: %s", value);
        } else if (!strcmp("-i", key)) {
            isBlockMode = string2bool(value);
            sprintf(info, "isBlockMode: %s", value);
        } else if (!strcmp("-f", key)) {
            fieldOrder = string2fieldOrder(value);
            sprintf(info, "fieldOrder: %s", value);
        } else if (!strcmp("-nstreams", key)) {
            nstreams = atoi(value);
            sprintf(info, "nstreams: %d", nstreams);
        } else if (!strcmp("-stream_id", key)) {
            stream_id = atoi(value);
            sprintf(info, "stream_id: %s", value);
        } else if (!strcmp("-export_device", key)) {
            strcpy(export_devName, value);
            sprintf(info, "export_devName: %s", value);
        }
        LogI(V4L2_SETTINGS_TAG, "%s -> %s", argv[i], info);
    }
    LogI(V4L2_SETTINGS_TAG, "------------------------------------------------------");
}

void V4l2_Settings::print() {
    LogI(V4L2_SETTINGS_TAG, "------------------------------------------------------");
    LogI(V4L2_SETTINGS_TAG, "devName: %s", devName);
    LogI(V4L2_SETTINGS_TAG, "configShellPath: %s", configShellPath);
    LogI(V4L2_SETTINGS_TAG, "resultFilePrefix: %s", resultFilePrefix);
    LogI(V4L2_SETTINGS_TAG, "width: %d", width);
    LogI(V4L2_SETTINGS_TAG, "height: %d", height);
    LogI(V4L2_SETTINGS_TAG, "pixelFormat: %s", pixelcode2String(pixelFormat));
    LogI(V4L2_SETTINGS_TAG, "fieldOrder: %s", fieldOrder2String(fieldOrder));
    LogI(V4L2_SETTINGS_TAG, "memoryType: %s", memoryType2String(memoryType));
    LogI(V4L2_SETTINGS_TAG, "isBlockMode: %s", bool2String(isBlockMode));
    LogI(V4L2_SETTINGS_TAG, "export_devName: %s", export_devName);
    LogI(V4L2_SETTINGS_TAG, "------------------------------------------------------");
}

const char* V4l2_Settings::bool2String(bool b) {
    return b ? "true" : "false";
}

bool V4l2_Settings::string2bool(const char *b) {
    if (!strcmp(b, "true")) {
        return true;
    }
    return false;
}

const char* V4l2_Settings::memoryType2String(int type) {
    if (type == V4L2_MEMORY_MMAP) {
        return "V4L2_MEMORY_MMAP";
    } else if (type == V4L2_MEMORY_USERPTR) {
        return "V4L2_MEMORY_USERPTR";
    } else if (type == V4L2_MEMORY_DMABUF) {
        return "V4L2_MEMORY_DMABUF";
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Memory Type (%d)", __func__, type);
        return "Unknow";
    }
}

int V4l2_Settings::string2MemoryType(const char *type) {
    if (!strcmp(type, "V4L2_MEMORY_MMAP")) {
        return V4L2_MEMORY_MMAP;
    } else if (!strcmp(type, "V4L2_MEMORY_USERPTR")) {
        return V4L2_MEMORY_USERPTR;
    } else if (!strcmp(type, "V4L2_MEMORY_DMABUF")) {
        return V4L2_MEMORY_DMABUF;
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Memory Type (%d)", __func__, type);
        return -1;
    }
}

const char* V4l2_Settings::bufferType2String(int type) {
    if (type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return "V4L2_BUF_TYPE_VIDEO_CAPTURE";
    } else if (type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
        return "V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE";
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Buffer Type (%d)", __func__, type);
        return "Unknow";
    }
}

int V4l2_Settings::string2BufferType(const char *type) {
    if (!strcmp(type, "V4L2_BUF_TYPE_VIDEO_CAPTURE")) {
        return V4L2_BUF_TYPE_VIDEO_CAPTURE;
    } else if (!strcmp(type, "V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE")) {
        return V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Buffer Type (%d)", __func__, type);
        return -1;
    }
}

const char* V4l2_Settings::fieldOrder2String(int field) {
    if (field == V4L2_FIELD_NONE) {
        return "V4L2_FIELD_NONE";
    } else if (field == V4L2_FIELD_ANY) {
        return "V4L2_FIELD_ANY";
    } else if (field == V4L2_FIELD_ALTERNATE) {
        return "V4L2_FIELD_ALTERNATE";
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Field (%d)", __func__, field);
        return "Unknow";
    }
}

int V4l2_Settings::string2fieldOrder(const char *field) {
    if (!strcmp(field, "V4L2_FIELD_NONE")) {
        return V4L2_FIELD_NONE;
    } else if (!strcmp(field, "V4L2_FIELD_ANY")) {
        return V4L2_FIELD_ANY;
    } else if (!strcmp(field, "V4L2_FIELD_ALTERNATE")) {
        return V4L2_FIELD_ALTERNATE;
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Field (%d)", __func__, field);
        return -1;
    }
}

const char* V4l2_Settings::pixelcode2String(int code)
{
    /* for subdevs */
    if (code == V4L2_MBUS_FMT_SBGGR12_1X12) {
        return "V4L2_MBUS_FMT_SBGGR12_1X12";
    } else if (code == V4L2_MBUS_FMT_SGBRG12_1X12) {
        return "V4L2_MBUS_FMT_SGBRG12_1X12";
    } else if (code == V4L2_MBUS_FMT_SGRBG12_1X12) {
        return "V4L2_MBUS_FMT_SGRBG12_1X12";
    } else if (code == V4L2_MBUS_FMT_SRGGB12_1X12) {
        return "V4L2_MBUS_FMT_SRGGB12_1X12";
    } else if (code == V4L2_MBUS_FMT_SBGGR10_1X10) {
        return "V4L2_MBUS_FMT_SBGGR10_1X10";
    } else if (code == V4L2_MBUS_FMT_SGBRG10_1X10) {
        return "V4L2_MBUS_FMT_SGBRG10_1X10";
    } else if (code == V4L2_MBUS_FMT_SGRBG10_1X10) {
        return "V4L2_MBUS_FMT_SGRBG10_1X10";
    } else if (code == V4L2_MBUS_FMT_SRGGB10_1X10) {
        return "V4L2_MBUS_FMT_SRGGB10_1X10";
    } else if (code == V4L2_MBUS_FMT_SBGGR8_1X8) {
        return "V4L2_MBUS_FMT_SBGGR8_1X8";
    } else if (code == V4L2_MBUS_FMT_SGBRG8_1X8) {
        return "V4L2_MBUS_FMT_SGBRG8_1X8";
    } else if (code == V4L2_MBUS_FMT_SGRBG8_1X8) {
        return "V4L2_MBUS_FMT_SGRBG8_1X8";
    } else if (code == V4L2_MBUS_FMT_SRGGB8_1X8) {
        return "V4L2_MBUS_FMT_SRGGB8_1X8";
    /* for nodes */
    } else if (code == V4L2_PIX_FMT_SBGGR12) {
        return "V4L2_PIX_FMT_SBGGR12";
    } else if (code == V4L2_PIX_FMT_SGBRG12) {
        return "V4L2_PIX_FMT_SGBRG12";
    } else if (code == V4L2_PIX_FMT_SGRBG12) {
        return "V4L2_PIX_FMT_SGRBG12";
    } else if (code == V4L2_PIX_FMT_SRGGB12) {
        return "V4L2_PIX_FMT_SRGGB12";
    } else if (code == V4L2_PIX_FMT_SBGGR10) {
        return "V4L2_PIX_FMT_SBGGR10";
    } else if (code == V4L2_PIX_FMT_SGBRG10) {
        return "V4L2_PIX_FMT_SGBRG10";
    } else if (code == V4L2_PIX_FMT_SGRBG10) {
        return "V4L2_PIX_FMT_SGRBG10";
    } else if (code == V4L2_PIX_FMT_SRGGB10) {
        return "V4L2_PIX_FMT_SRGGB10";
    } else if (code == V4L2_PIX_FMT_SBGGR8) {
        return "V4L2_PIX_FMT_SBGGR8";
    } else if (code == V4L2_PIX_FMT_SGBRG8) {
        return "V4L2_PIX_FMT_SGBRG8";
    } else if (code == V4L2_PIX_FMT_SGRBG8) {
        return "V4L2_PIX_FMT_SGRBG8";
    } else if (code == V4L2_PIX_FMT_SRGGB8) {
        return "V4L2_PIX_FMT_SRGGB8";
    /* for stream codes */
    } else if (code == V4L2_PIX_FMT_NV12) {
        return "V4L2_PIX_FMT_NV12";
	} else if (code == V4L2_PIX_FMT_NV16) {
        return "V4L2_PIX_FMT_NV16";
    } else if (code == V4L2_PIX_FMT_JPEG) {
        return "V4L2_PIX_FMT_JPEG";
    } else if (code == V4L2_PIX_FMT_YUV420) {
        return "V4L2_PIX_FMT_YUV420";
    } else if (code == V4L2_PIX_FMT_NV21) {
        return "V4L2_PIX_FMT_NV21";
    } else if (code == V4L2_PIX_FMT_YUV422P) {
        return "V4L2_PIX_FMT_YUV422P";
    } else if (code == V4L2_PIX_FMT_YVU420) {
        return "V4L2_PIX_FMT_YVU420";
    } else if (code == V4L2_PIX_FMT_YUYV) {
        return "V4L2_PIX_FMT_YUYV";
    } else if (code == V4L2_PIX_FMT_RGB565) {
        return "V4L2_PIX_FMT_RGB565";
    } else if (code == V4L2_PIX_FMT_RGB24) {
        return "V4L2_PIX_FMT_RGB24";
    } else if (code == V4L2_PIX_FMT_BGR32) {
        return "V4L2_PIX_FMT_BGR32";
    } else if (code == V4L2_PIX_FMT_BGR24) {
        return "V4L2_PIX_FMT_BGR24";
    } else if (code == V4L2_PIX_FMT_UYVY) {
        return "V4L2_PIX_FMT_UYVY";
    } else if (code == V4L2_PIX_FMT_XBGR32) {
        return "V4L2_PIX_FMT_XBGR32";
    } else if (code == V4L2_PIX_FMT_XRGB32) {
        return "V4L2_PIX_FMT_XRGB32";
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Pixel Format (%d)", __func__, code);
        return "Unknow";
    }
}

int V4l2_Settings::string2Pixelcode(const char *code)
{
    /* for subdevs */
    if (!strcmp(code, "V4L2_MBUS_FMT_SBGGR12_1X12")) {
        return V4L2_MBUS_FMT_SBGGR12_1X12;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SGBRG12_1X12")) {
        return V4L2_MBUS_FMT_SGBRG12_1X12;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SGRBG12_1X12")) {
        return V4L2_MBUS_FMT_SGRBG12_1X12;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SRGGB12_1X12")) {
        return V4L2_MBUS_FMT_SRGGB12_1X12;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SBGGR10_1X10")) {
        return V4L2_MBUS_FMT_SBGGR10_1X10;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SGBRG10_1X10")) {
        return V4L2_MBUS_FMT_SGBRG10_1X10;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SGRBG10_1X10")) {
        return V4L2_MBUS_FMT_SGRBG10_1X10;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SRGGB10_1X10")) {
        return V4L2_MBUS_FMT_SRGGB10_1X10;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SBGGR8_1X8")) {
        return V4L2_MBUS_FMT_SBGGR8_1X8;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SGBRG8_1X8")) {
        return V4L2_MBUS_FMT_SGBRG8_1X8;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SGRBG8_1X8")) {
        return V4L2_MBUS_FMT_SGRBG8_1X8;
    } else if (!strcmp(code, "V4L2_MBUS_FMT_SRGGB8_1X8")) {
        return V4L2_MBUS_FMT_SRGGB8_1X8;
    /* for nodes */
    } else if (!strcmp(code, "V4L2_PIX_FMT_SBGGR12")) {
        return V4L2_PIX_FMT_SBGGR12;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SGBRG12")) {
        return V4L2_PIX_FMT_SGBRG12;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SGRBG12")) {
        return V4L2_PIX_FMT_SGRBG12;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SRGGB12")) {
        return V4L2_PIX_FMT_SRGGB12;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SBGGR10")) {
        return V4L2_PIX_FMT_SBGGR10;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SGBRG10")) {
        return V4L2_PIX_FMT_SGBRG10;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SGRBG10")) {
        return V4L2_PIX_FMT_SGRBG10;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SRGGB10")) {
        return V4L2_PIX_FMT_SRGGB10;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SBGGR8")) {
        return V4L2_PIX_FMT_SBGGR8;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SGBRG8")) {
        return V4L2_PIX_FMT_SGBRG8;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SGRBG8")) {
        return V4L2_PIX_FMT_SGRBG8;
    } else if (!strcmp(code, "V4L2_PIX_FMT_SRGGB8")) {
        return V4L2_PIX_FMT_SRGGB8;
    /* for stream codes */
    } else if (!strcmp(code, "V4L2_PIX_FMT_NV12")) {
        return V4L2_PIX_FMT_NV12;
	} else if (!strcmp(code, "V4L2_PIX_FMT_NV16")) {
        return V4L2_PIX_FMT_NV16;	
    } else if (!strcmp(code, "V4L2_PIX_FMT_JPEG")) {
        return V4L2_PIX_FMT_JPEG;
    } else if (!strcmp(code, "V4L2_PIX_FMT_YUV420")) {
        return V4L2_PIX_FMT_YUV420;
    } else if (!strcmp(code, "V4L2_PIX_FMT_NV21")) {
        return V4L2_PIX_FMT_NV21;
    } else if (!strcmp(code, "V4L2_PIX_FMT_YUV422P")) {
        return V4L2_PIX_FMT_YUV422P;
    } else if (!strcmp(code, "V4L2_PIX_FMT_YVU420")) {
        return V4L2_PIX_FMT_YVU420;
    } else if (!strcmp(code, "V4L2_PIX_FMT_YUYV")) {
        return V4L2_PIX_FMT_YUYV;
    } else if (!strcmp(code, "V4L2_PIX_FMT_RGB565")) {
        return V4L2_PIX_FMT_RGB565;
    } else if (!strcmp(code, "V4L2_PIX_FMT_RGB24")) {
        return V4L2_PIX_FMT_RGB24;
    } else if (!strcmp(code, "V4L2_PIX_FMT_BGR32")) {
        return V4L2_PIX_FMT_BGR32;
    } else if (!strcmp(code, "V4L2_PIX_FMT_BGR24")) {
        return V4L2_PIX_FMT_BGR24;
    }  else if (!strcmp(code, "V4L2_PIX_FMT_UYVY")) {
        return V4L2_PIX_FMT_UYVY;
    }  else if (!strcmp(code, "V4L2_PIX_FMT_XBGR32")) {
        return V4L2_PIX_FMT_XBGR32;
    }  else if (!strcmp(code, "V4L2_PIX_FMT_XRGB32")) {
        return V4L2_PIX_FMT_XRGB32;
    } else {
        LogE(V4L2_SETTINGS_TAG, "%s, Unknown Pixel Format (%s)", __func__, code);
        return -1;
    }
}
