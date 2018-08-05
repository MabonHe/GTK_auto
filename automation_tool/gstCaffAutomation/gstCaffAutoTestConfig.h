////////////////////////////////////////////////////////////////////////////////////
// Copyright (2014) Intel Corporation All Rights Reserved.
//
// The source code, information and material ("Material") contained herein is owned
// by Intel Corporation or its suppliers or licensors, and title to such Material
// remains with Intel Corporation or its suppliers or licensors. The Material
// contains proprietary information of Intel or its suppliers and licensors. The
// Material is protected by worldwide copyright laws and treaty provisions.
// No part of the Material may be used, copied, reproduced, modified, published,
// uploaded, posted, transmitted, distributed or disclosed in any way without Intel's
// prior express written permission. No license under any patent, copyright or other
// intellectual property rights in the Material is granted to or conferred upon you,
// either expressly, by implication, inducement, estoppel or otherwise. Any license
// under such intellectual property rights must be express and approved by Intel in
// writing.
// Unless otherwise agreed by Intel in writing, you may not remove or alter this
// notice or any other notice embedded in Materials by Intel or Intel’s suppliers
// or licensors in any way.
//
////////////////////////////////////////////////////////////////////////////////////

#ifndef __AUTOMATION_TEST_CONFIG_H__
#define __AUTOMATION_TEST_CONFIG_H__

#include "LibContentAnalyzer.h"
#include "LibToolFunction.h"
#include <vector>

using namespace std;

#define CONFIG_NAME "config"
#define CAMERA_NAME "camera"

#define MAX_CONCURRENT_CAM_COUNT 4
#define MAX_CONCURRENT_CONFIG_COUNT 1024
#define MAX_ERROR_MESSAGE_LEN 1024
#define MIN_MANUAL_WITHE_BALANCE_COLOR_TEMP 1000

#ifndef _strdup
#define _strdup strdup
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

#ifndef strncpy_s
#define strncpy_s(dst, maxLen, src, n) strncpy(dst, src, n)
#endif

#ifndef size_t
#define size_t unsigned int
#endif

typedef struct _S_CAMERA_FRAME
{
    Mat img;
    METADATA_INFO metadata;
}S_CAMERA_FRAME;


enum E_FEATURE_TYPE
{
    E_FEATURE_TYPE_NONE,
    E_FEATURE_TYPE_BRIGHTNESS,
    E_FEATURE_TYPE_CONTRAST,
    E_FEATURE_TYPE_SATURATION,
    E_FEATURE_TYPE_SHARPNESS,
    E_FEATURE_TYPE_EXPOSURE_TIME,
    E_FEATURE_TYPE_EXPOSURE_GAIN,
    E_FEATURE_TYPE_EXPOSURE_EV,
    E_FEATURE_TYPE_HUE,
    E_FEATURE_TYPE_WB_MODE,
    E_FEATURE_TYPE_WB_MANUAL_GAIN,
    E_FEATURE_TYPE_WB_CCT_RANGE,
    E_FEATURE_TYPE_WB_SHIFT,
    E_FEATURE_TYPE_HDR,
    E_FEATURE_TYPE_SIMILARITY,
    E_FEATURE_TYPE_CONTENT_CONSISTENT,
    E_FEATURE_TYPE_SHUTTER_SPEED,
    E_FEATURE_TYPE_BIAS,
    E_FEATURE_TYPE_ISO,
    E_FEATURE_TYPE_FLASH_MODE,
    E_FEATURE_TYPE_3A_AUTO_CONSISTENT,
    E_FEATURE_TYPE_AE_BRACKETING,
    E_FEATURE_TYPE_ULL,
    E_FEATURE_TYPE_GREEN_CORRUPTION_DETECTION,
    E_FEATURE_TYPE_ORIENTATION_DETECTION,
    E_FEATURE_TYPE_COLOR_CAST
};

typedef enum{
    GST_CAMERASRC_AWB_MODE_AUTO = 0,
    GST_CAMERASRC_AWB_MODE_PARTLY_OVERCAST = 1,
    GST_CAMERASRC_AWB_MODE_FULLY_OVERCAST = 2,
    GST_CAMERASRC_AWB_MODE_FLUORESCENT = 3,
    GST_CAMERASRC_AWB_MODE_INCANDESCENT = 4,
    GST_CAMERASRC_AWB_MODE_SUNSET = 5,
    GST_CAMERASRC_AWB_MODE_VIDEO_CONFERENCING = 6,
    GST_CAMERASRC_AWB_MODE_DAYLIGHT = 7,
    GST_CAMERASRC_AWB_MODE_CCT_RANGE = 8,
    GST_CAMERASRC_AWB_MODE_WHITE_POINT = 9,
    GST_CAMERASRC_AWB_MODE_MANUAL_GAIN = 10
} GstCamerasrcAwbMode;

typedef struct _S_AWB_MODE_RANGE
{
    GstCamerasrcAwbMode mode;
    int min;
    int max;
}S_AWB_MODE_RANGE;



typedef struct _S_FEATURE_ANALYZER
{
    E_FEATURE_TYPE feature_type;
    const char *feature_name;
    AnalyzerType analyzer;
}S_FEATURE_ANALYZER;

typedef struct _DATA_TAG
{
    const char *tag_name;
    size_t offset_org;
    size_t offset_enable;
    DATA_PARSER *pdata_parser;
    _DATA_TAG *childs;
}DATA_TAG;

typedef struct _DATA_TAG_NODE
{
    DATA_TAG  *tag;
    TiXmlNode *node;
    void *pdata;
}DATA_TAG_NODE;

typedef struct _MODE_DESCRIPTION
{
    int  flag;
    char *disp;
}MODE_DESCRIPTION;

typedef struct _ANALYZER_CONFIG
{
    _ANALYZER_CONFIG() :
        enable(false),
        analyzer_config(NULL)
    {}

    bool enable;
    char *analyzer_config;
} ANALYZER_CONFIG;

typedef struct _CAMERA_COMMON_CONFIG
{
    _CAMERA_COMMON_CONFIG() :
        enable(false),
        pixelformat(NULL),
        capture_mode(NULL),
        interlace_mode(NULL),
        deinterlace_method(NULL),
        io_mode(NULL),
        framerate(NULL),
        fisheye_mode(NULL)
    {}
    bool enable;
    int width;                  //< Integer. Range: 0 - 2147483647 Default: 1920
    int height;                 //< Integer. Range: 0 - 2147483647 Default: 1080
    char *pixelformat;          //< NV12/YUY2/UYVY/RGBx/BGRx
    char *capture_mode;         //< Preview/video/still
    int num_buffers;            //< Integer. Range: -1 - 2147483647 Default: -1
    int printfps;               //< Boolean. Default: false
    char *interlace_mode;       //< "any"/"alternate"
    char *deinterlace_method;   //< none/sw_bob/hw_bob
    char *io_mode;              //< USERPTR/MMAP/DMA/DMA_IMPORT
    char *framerate;
    char *fisheye_mode;         //< rearview / hitchview / off >
    //char *device_name;          //< “tpg”/“ov13860”/”imx185”
} CAMERA_COMMON_CONFIG;

typedef struct _CAMERA_3A_CONTROL_CONFIG
{
    _CAMERA_3A_CONTROL_CONFIG() :
            enable(false),
            day_night_mode(NULL),
            iris_mode(NULL),
            ae_mode(NULL),
            wdr_mode(NULL),
            blc_area_mode(NULL),
            awb_mode(NULL),
            wp_point(NULL),
            nr_mode(NULL),
            scene_mode(NULL)
    {}

    bool enable;

    // Day_Night Switch
    char *day_night_mode;      //< Day mode / Night mode / Auto mode

    // Image Adjustment
    int sharpness;             //< [0, 100], 0
    int brightness;            //< [0, 100], 0
    int contrast;              //< [0, 100], 0
    int hue;                   //< [0, 100], 0
    int saturation;            //< [0, 100], 0

    // Exposure Settings
    char *iris_mode;           //< auto/manual/customized, auto
    int iris_level;            //< [0, 100], 0
    int exposure_time;         //< [0, 1000000], 0
    float gain;                  //< [0, 60], 0
    int ev;                    //< [_4, 4], 0
    char *ae_mode;             //< auto / manual, auto

    // Backlight Setting
    char *wdr_mode;            //< off/on/auto, off
    char *blc_area_mode;       //< off/on, off
    int wdr_level;             //< [0, 15], 0

    // White Balance Mode
    // auto/partly_overcast/fully_overcast/fluorescent/
    // incandescent/sunset/video_conferencing/daylight/
    // cct_range/white_point/manual_gain, auto
    char *awb_mode;
    int cct_range_min;         //< [0, 100], 0
    int cct_range_max;         //< [0, 100], 0
    char *wp_point;            //< ”width=1920,height=1080…”
    int awb_shift_r;           //< [0, 255], 0
    int awb_shift_g;           //< [0, 255], 0
    int awb_shift_b;           //< [0, 255], 0
    int awb_gain_r;            //< [0, 255], 0
    int awb_gain_g;            //< [0, 255], 0
    int awb_gain_b;            //< [0, 255], 0

    // Noise Reduction Mode
    char *nr_mode;             //< off/auto/normal/expert
    int nr_filter_level;       //< [0, 100], 0

    // Video Adjustment
    char *scene_mode;          //< auto/indoor/outdoor/disabled/ull/hdr
} CAMERA_3A_CONTROL_CONFIG;

typedef struct _S_FEATURE_SETTING
{
public:
    _S_FEATURE_SETTING() :
        enable(false),
        feature_name(NULL),
        values(NULL),
        value_format(NULL),
        value_count(0),
        duration_each(0),
        frame_count_each(1),
        loop_mode(0)
    {}

    bool enable;
    char *feature_name;
    char *values;
    char *value_format;
    int value_count;
    int duration_each;
    int frame_count_each;
    int loop_mode;

    bool GetAnalyzerByFeatureName(S_FEATURE_ANALYZER &fa);
    bool GetValue(vector<PARAMETER> &parameters);
} S_FEATURE_SETTING;

const char* GetFeatureNameByFeatureType(E_FEATURE_TYPE feature_type);

S_AWB_MODE_RANGE GetAwbModeColorTempRange(GstCamerasrcAwbMode mode);

typedef struct _CAM_CONFIG
{
    _CAM_CONFIG() {}
    bool enable;

    int duration;

    CAMERA_COMMON_CONFIG common_config;
    CAMERA_3A_CONTROL_CONFIG cam_3a_control_config;

    S_FEATURE_SETTING feature;
    ANALYZER_CONFIG   analyzer;
    ANALYZER_CONFIG   perf;
    ANALYZER_CONFIG   convergency_3a;
    ANALYZER_CONFIG   green_corruption_analyzer;
    ANALYZER_CONFIG   orientation_analyzer;
    ANALYZER_CONFIG   color_cast_analyzer;

}CAM_CONFIG;

typedef struct _MULTI_CAM_CONFIG
{
    _MULTI_CAM_CONFIG() :
        enable(false),
        id(0)
    {}
    bool enable;
    int id;
    CAM_CONFIG cam[MAX_CONCURRENT_CAM_COUNT];
}MULTI_CAM_CONFIG;

typedef struct _STRESS_CONFIG
{
    _STRESS_CONFIG() :
        enable(false),
        loop_count(1)
    {}
    bool enable;
    int loop_count;
}STRESS_CONFIG;

typedef struct _CASE_INFO
{
    _CASE_INFO() :
        enable(false),
        case_name(NULL),
        description(NULL)
    {}
    bool enable;
    char *case_name;
    char *description;
}CASE_INFO;

typedef struct _SUB_RESULT
{
    _SUB_RESULT() :
        enable(true),
        result(NULL),
        error_message(NULL)
    {}
    bool enable;
    char *result;
    char *error_message;
}SUB_RESULT;

typedef struct _CASE_RESULT
{
    _CASE_RESULT() :
        enable(false),
        status(0),
        loop_count_completed(0),
        start_time(NULL),
        end_time(NULL)
    {}
    bool enable;
    int status;
    int loop_count_completed;
    char *start_time;
    char *end_time;
    SUB_RESULT basic_result;
    SUB_RESULT performance_result;
    SUB_RESULT feature_result;
}CASE_RESULT;

typedef struct _CASE_CONFIG
{
    CASE_INFO case_info;
    CASE_RESULT case_result;
    STRESS_CONFIG stress_config;
    vector<MULTI_CAM_CONFIG> multi_cam_configs;
}CASE_CONFIG;

class CCaseInfo
{
public:
    CCaseInfo(const char *case_id);
    ~CCaseInfo();

    // Get
    int GetStatus();
    const char* GetExeStartTime();
    const char* GetExeStopTime();
    const char* GetCaseID();
    int GetLoopCount();
    size_t GetMultiCamConfigCount();
    CASE_CONFIG* GetCaseConfig();
    MULTI_CAM_CONFIG* GetMultiCamConfigByIndex(size_t index);
    const char* GetBasicErrorMessage();
    const char* GetPerformanceErrorMessage();
    const char* GetFeatureErrorMessage();

    // >0: pass, <0: fail, =0: no result
    int GetExecutionBasicResult();
    int GetExecutionPerformanceResult();
    int GetExecutionFeatureResult();

    bool IsResultEnabled();

    // Set
    void SetCaseID(const char *case_id);
    void SetStatus(int status);
    void SetExeStartTime(const char *time);
    void SetExeStopTime(const char *time);

    void SetExecutionBasicResult(bool bpass);
    void SetExecutionPerformanceResult(bool bpass);
    void SetExecutionFeatureResult(bool bpass);
    void SetLoopCountCompleted(int nloops);

    void AddBasicErrorMessage(const char* message);
    void AddPerformanceErrorMessage(const char* message);
    void AddFeatureErrorMessage(const char* message);

    // Read and write
    bool Read(TiXmlDocument *doc);
    void Write(TiXmlDocument *doc);

protected:
    void Depth_first_read(vector<DATA_TAG_NODE> nodes);
    void Depth_first_write(vector<DATA_TAG_NODE> nodes);
    void Depth_first_delete(vector<DATA_TAG_NODE> nodes);

    void SetBasicErrorMessage(const char* message);
    void SetPerformanceErrorMessage(const char* message);
    void SetFeatureErrorMessage(const char* message);

private:
    char *m_case_id;
    CASE_CONFIG m_case_config;
    int  m_comment_index;
    string m_basic_error;
    string m_performance_error;
    string m_feature_error;
};

#endif //__AUTOMATION_TEST_CONFIG_H__
