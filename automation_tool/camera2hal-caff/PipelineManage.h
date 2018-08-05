/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2016 Intel Corporation
 * All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#ifndef _PIPELINE_MANAGEMENT_H_
#define _PIPELINE_MANAGEMENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <gst/gst.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <gst/video/videooverlay.h>


#define MAX_PLUGIN_SIZE 10
#define MAX_CAMERA_NUM 4
#define status_t int

enum ERRROR_ID{
    NO_ERROR = 0,
    ERROR = -1
};

namespace gstCaff {

/* gstmessage.h needs State */
/**
 * GstState:
 * @GST_STATE_VOID_PENDING: no pending state.
 * @GST_STATE_NULL        : the NULL state or initial state of an element.
 * @GST_STATE_READY       : the element is ready to go to PAUSED.
 * @GST_STATE_PAUSED      : the element is PAUSED, it is ready to accept and
 *                          process data. Sink elements however only accept one
 *                          buffer and then block.
 * @GST_STATE_PLAYING     : the element is PLAYING, the #GstClock is running and
 *                          the data is flowing.
 *
 * The possible states an element can be in. States can be changed using
 * gst_element_set_state() and checked using gst_element_get_state().

typedef enum {
  GST_STATE_VOID_PENDING        = 0,
  GST_STATE_NULL                = 1,
  GST_STATE_READY               = 2,
  GST_STATE_PAUSED              = 3,
  GST_STATE_PLAYING             = 4
} GstState;

*/

enum CAMERA_STATE{
    CAMERA_STATE_STOP, /*fe*/
    CAMERA_STATE_START,
    CAMERA_STATE_PAUSE
};

enum STREAM_TYPE{
    STREAM_TYPE_NONE = -1,
    STREAM_TYPE_PREVIEW,
    STREAM_TYPE_RECORDING,
    STREAM_TYPE_PLAYER,
    STREAM_TYPE_REMOTE  /*stream send to remote client*/
};

enum FORMAT_TYPE{
    FORMAT_YUYV,
    FORMAT_NV12
};

typedef struct _Properties
{
    char IrisMode[20];
    int irisLevel;
    char aeMode[20];
    char fisheyemode[20];
    char convergeSpeedMode[20];
    char convergeSpeedLevel[20];
    int exposureTime;
    float exposureGain;
    int exposureEv;

    char awbMode[20];
    char awbRange[20];
    char awbWp[30];
    int awbRGain;
    int awbBGain;
    int awbGGain;
    int awbRshift;
    int awbGshift;
    int awbBshift;
    int windowSize;
    char regionMode[20];
    char daynightMode[20];
    int sharpness;
    int brightness;
    int contrast;
    int hue;
    int saturation;
    char WdrMode[20];
    char BlcArea[20];
    int WdrLevel;
    char noiseReductionMode[20];
    int noiseFilterLevel;
    int spatialLevel;
    int temporialLevel;
    char mirrorEffectMode[20];
    char sceneMode[20];
    char sensorResolution[20];
    char captureFps[20];
    char videoStandard[20];
    char advanced[20];
    int frameNum;
    int debugLevel;
} CamProperties;

struct PipelineStatus {
    int camId;
    GstElement* activePipeline;
    GstElement* sourceElement;
    STREAM_TYPE stream_type;
    CAMERA_STATE state;
    int width;
    int height;
    int end;
    bool interlace_mode;
    bool vcEnable;
    char format[16];   /*NV12 or YUY2*/
    char framerate[16];/*30/1 or 60/1*/
    char deInterlace[16];  /*sw or hw intelace*/
    char ioMode[16];
    char deviceName[128];
    char srcElementType[64];/*Element type name specified by user */
    char convertElementType[64];
    char sinkElementType[64];
    char fisheyeDewarpingMode[16];
    guintptr videoWindow;
    GtkWidget *drawArea;
    /*below is for player and recording mode*/
    char open_file_path[256];
    char save_file_name[256];
    int remoteIp; /*ip address of remote client*/
    CamProperties properties;
};

struct Plugin {
    const char *plugin_name;
    const char *pro_attrs;
    const char *caps_filter;
};

/**
 * class PipelineManage
 */
class PipelineManage {
public:
    PipelineManage();
    ~PipelineManage();
    status_t pipelineSetState(int camId, CAMERA_STATE state);
    status_t setVideoWindow(int camId,guintptr mEmbedXid);
    status_t setDrawingAreaPosition(int camId,GtkWidget *mDrawArea);
    status_t init();
    status_t setReso(int camId ,int width, int height);
    status_t setStreamType(int camId ,STREAM_TYPE type);
    status_t setFormat(int camId ,char * format);
    status_t setFramerate(int camId ,char * framerate);
    status_t setInterlaceMode(int camId ,bool mode);
    status_t setVcEnable(int camId, bool enable);
    status_t setDeInterlace(int camId ,char * format);
    status_t setDeviceName(int camId, char *type);
    status_t setSrcElementType(int camId,char * type);
    status_t setConvertElementType(int camId,char * type);
    status_t setSinkElementType(int camId,char * type);
    status_t setRemoteIp(int camId,int ip);
    status_t getRemoteIp(int camId, int &ip);
    status_t getReso(int camId ,int &width, int &height);
    status_t getStreamType(int camId ,STREAM_TYPE &type);
    status_t getFormat(int camId ,char * format);
    status_t getInterlaceMode(int camId ,bool &mode);
    status_t getDeInterlace(int camId ,char * format);
    status_t getDeviceName(int camId, char * type);
    status_t getCameraState(int camId , CAMERA_STATE &state);
    status_t setCaptureNum(int camId, int num);
    status_t getIOMode(int cameraId, char *IoModeOld);
    status_t setIoMode(int cameraId, char *IoModePtr);
    status_t getIrisMode(int camId, char *formatOld);
    status_t setIrisMode(int camId, char *irisModePtr);
    status_t getIrisLevel(int camId, int &oldValue);
    status_t setIrisLevel(int camId, int value);
    status_t getAeMode(int camId, char *value);
    status_t setAeMode(int camId, char *mode);
    status_t getFisheyeMode(int camId, char *value);
    status_t setFisheyeMode(int camId, char *value);
    status_t getConvergeSpeedMode(int camId, char *value);
    status_t setConvergeSpeedMode(int camId, char *speed);
    status_t getConvergeSpeedLevel(int camId, char *value);
    status_t setConvergeSpeedLevel(int camId, char *speed);
    status_t getExposureTime(int camId, int &oldValue);
    status_t setExposureTime(int camId, int value);
    status_t getExposureGain(int camId, float &oldValue);
    status_t setExposureGain(int camId, float value);
    status_t getExposureEv(int camId, int &oldValue);
    status_t setExposureEv(int camId, int value);
    status_t getAwbMode(int camId, char *awbModeOld);
    status_t setAwbMode(int camId, char *awbModePtr);
    status_t getAwbRGain(int camId, int &oldValue);
    status_t setAwbRGain(int camId, int value);
    status_t getAwbGGain(int camId, int &oldValue);
    status_t setAwbGGain(int camId, int value);
    status_t getAwbBGain(int camId, int &oldValue);
    status_t setAwbBGain(int camId, int value);
    status_t getAwbRange(int camId, char *awbRangeOld);
    status_t setAwbRange(int camId, char *awbRangePtr);
    status_t getAwbWp(int camId, char *value);
    status_t setAwbWp(int camId, char *value);
    status_t getWindowSize(int camId, int &oldValue);
    status_t setWindowSize(int camId, int value);
    status_t setColorTransform(int camId, char *value);
    status_t getColorTransform(int camId, char *value);
    status_t getAwbShiftR(int camId, int &oldValue);
    status_t setAwbShiftR(int camId, int value);
    status_t getAwbShiftG(int camId, int &oldValue);
    status_t setAwbShiftG(int camId, int value);
    status_t getAwbShiftB(int camId, int &oldValue);
    status_t setAwbShiftB(int camId, int value);
    status_t setAeRegion(int camId, char *region);
    status_t setAwbRegion(int camId, char *region);
    status_t getDayNightMode(int camId, char *dayNightModeOld);
    status_t setDayNightMode(int camId, char *dayNightModePtr);
    status_t getSharpness(int camId, int &oldValue);
    status_t setSharpness(int camId, int value);
    status_t getBrightness(int camId, int &oldValue);
    status_t setBrightness(int camId, int value);
    status_t getContrast(int camId, int &oldValue);
    status_t setContrast(int camId, int value);
    status_t getHue(int camId, int &oldValue);
    status_t setHue(int camId, int value);
    status_t getSaturation(int camId, int &oldValue);
    status_t setSaturation(int camId, int value);
    status_t getWdrMode(int camId, char *wdrModeOld);
    status_t setWdrMode(int camId, char *wdrModePtr);
    status_t getWdrLevel(int camId, int &oldValue);
    status_t setWdrLevel(int camId, int value);
    status_t getBlcMode(int camId, char *wdrModeOld);
    status_t setBlcMode(int camId, char *enableBlcPtr);
    status_t getBlcAreaLevel(int camId, int &oldValue);
    status_t setBlcAreaLevel(int camId, int value);
    status_t getNoiseReductionMode(int camId, char *noiseReductionModeOld);
    status_t setNoiseReductionMode(int camId, char *wdrModePtr);
    status_t getNoiseFilter(int camId, int &oldValue);
    status_t setNoiseFilter(int camId, int value);
    status_t getSpatialLevel(int camId, int &oldValue);
    status_t setSpatialLevel(int camId, int value);
    status_t getTemporalLevel(int camId, int &oldValue);
    status_t setTemporalLevel(int camId, int value);
    status_t getMirrorEffectMode(int camId, char *mirrorEffectModeOld);
    status_t setMirrorEffectMode(int camId, char *mirrorEffectModePtr);
    status_t getSceneMode(int camId, char *mirrorEffectModeOld);
    status_t setSceneMode(int camId, char *sceneModePtr);
    status_t getSensorMode(int camId, char *sensorModeOld);
    status_t setSensorMode(int camId, char *sensorModePtr);
    status_t getCaptureFps(int camId, char *sensorModeOld);
    status_t setCaptureFps(int camId, char *captureFpsPtr);
    status_t getVideoStandard(int camId, char *videoStandardOld);
    status_t setVideoStandard(int camId, char *videoStandardPtr);
    status_t getAdvanceFeature(int camId, char *advancedOld);
    status_t setAdvanceFeature(int camId, char *advancedPtr);
    status_t setCustomAicParam(int camId, char *aicParam);
    status_t setDebugLevel(int camId, int value);
    status_t getNumBuffers(int camId, int value);
    status_t setNumBuffers(int camId, int value);
    status_t setTestDualMode(bool enable);
    int getEnumValueFromNick(GstElement *element, const char * property, const char *nick);
    int getNickFromEnumValue(GstElement *element, const char * property, const int value, char* output);
    static PipelineManage* getInstance();
    void (*notifyEOF)  (int camId);


private:
    // Thread methods
    void clearPlugins(void);
    int getFileNameFromTime(char *fileName);
    int addPlugins(const char *plugin_name, const char *pro_attrs, const char *caps_filter);
    int getEnumValueFromField(GParamSpec * prop, const char *nick);
    void setProperties(GstElement *element, const char * properties);
    status_t createAndlinkElement(GstElement *pipeline);
    status_t setManualProperties(GstElement *src, CamProperties *pro);
    int pipelinePreviewStart(int camId);
    int pipelineRecordingStart(int camId);
    int pipelinePlayerStart(int camId);
    int pipelineRemoteStart(int camId);
    void getCapsStringfromStatus(PipelineStatus *status,char *string);
    static gboolean bus_call(GstBus *bus,GstMessage *msg, gpointer userData);
    void pipelineConnectWatch(GstElement *pipeline, int camId);
    status_t pipelineStart(int camId);
    status_t pipelinePause(int camId);
    status_t pipelineStop(int camId);
    bool mTestDualMode;

private:
    PipelineStatus pipelineStatus[MAX_CAMERA_NUM];
    static PipelineManage *sInstance;
	int camids[4]={0,1,2,3};
}; // class PipelineManage

} // namespace android

#endif // _PIPELINE_MANAGEMENT_H_

