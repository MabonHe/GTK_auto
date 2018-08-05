#ifndef _GST_PROPERTY_H_
#define _GST_PROPERTY_H_

#include <string.h>
#include <math.h>
#include "case_gst_common.h"

// GST Property API test base class based on gtest
class Gst_property_test : public testing::Test
{
public:

    // usage information (called in main) 
    static void usage();

    // init parameters for the test (called in main)
    static int init(int argc, char** argv);

    // Setup function (empty currently)
    void SetUp() {
    }

    // TearDown function (empty currently)
    void TearDown() {
    }

    // Function to test all kinds of gst properties
    void test_property(
        const char *base_property, //< dependency property for property to test
        const char *property,      //< property to test
        PROPERTY_CHECK_TYPE type); //< property check type, including max, min, default, negative etc.
    
public:
    // icamerasrc basic input properties or capbilities
    static int  m_num_buffers;
    static char m_device_name[128];
    static int  m_width;
    static int  m_height;
    static char m_format[16];
    static char m_iomode[16];
    static char m_sinktype[16];
    static char m_scene_mode[16];
};

enum PROPERTY_VAL_TYPE
{
    PROPERTY_INT,
    PROPERTY_BOOL,
    PROPERTY_STRING,
    PROPERTY_ENUM,
    PROPERTY_FLOAT
};

typedef struct _S_PROPERTY_DEFAULT
{
    const char *property_name;
    PROPERTY_VAL_TYPE type;
    int val_int;
    bool val_bool;
    const char *val_string;
    int val_enum;
    float val_float;
}S_PROPERTY_DEFAULT;

#endif
