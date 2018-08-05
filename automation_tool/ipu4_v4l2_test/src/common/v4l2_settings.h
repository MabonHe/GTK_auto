#ifndef V4L2_SETTINGS_H
#define V4L2_SETTINGS_H

#include "mutex.h"

// Singleton setting class to config v4l2 related parameters
class V4l2_Settings
{
public:
    static V4l2_Settings* instance();

public:
    static void init(int argc, char **argv);
    static void print();
    static void usage();

protected:
    static const char* memoryType2String(int type);
    static int string2MemoryType(const char *type);
    static const char* bufferType2String(int type);
    static int string2BufferType(const char *type);
    static const char* fieldOrder2String(int field);
    static int string2fieldOrder(const char *field);
    static const char* pixelcode2String(int code);
    static int string2Pixelcode(const char *code);
    static const char* bool2String(bool b);
    static bool string2bool(const char *b);

public:
    static char devName[50];
    static char configShellPath[256];
    static char resultFilePrefix[256];
    static int width;
    static int height;
    static int pixelFormat;
    static int bufferType;
    static int fieldOrder;
    static int memoryType;
    static bool isBlockMode;
    static int nstreams;
    static int stream_id;
    static char export_devName[50];

private:
    static Mutex mutex;
    static V4l2_Settings *mInstance;
    V4l2_Settings();
    virtual ~V4l2_Settings();
};

#endif // V4L2_SETTINGS_H
