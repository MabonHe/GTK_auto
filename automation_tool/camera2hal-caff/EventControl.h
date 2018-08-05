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

#ifndef _EVENT_CONTROL_H_
#define _EVENT_CONTROL_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "PipelineManage.h"
#include <semaphore.h>


namespace gstCaff {

typedef enum {
    UPDATE_UI_SETTING_RES,
    UPDATE_UI_SETTING_FMT,
    UPDATE_UI_SETTING_FRT,
    UPDATE_UI_SETTING_INTERLACE,
    UPDATE_UI_SETTING_DEINTERLACE,
    UPDATE_UI_SETTING_DEVICENAME,
    UPDATE_UI_SETTING_AE,
    UPDATE_UI_SETTING_AWB,
} UpdateUi;

class GstCaffUI;

/**
 * \class EventControl
 */
class EventControl {
public:
    EventControl(bool needUpdateUi);
    ~EventControl();
    void setGstCaffUIHandle(GstCaffUI *pGstCaffUI);
    void updateUICameraNum(int ncount);
    status_t setCameraState(int camId, CAMERA_STATE state);
    status_t getCameraState(int camId, CAMERA_STATE &state);
    status_t setResolution(int camId, int width, int height);
    status_t getResolution(int camId, int &width, int &height);
    status_t setInterlaceMode(int camId, bool value);
    status_t setVcEnable(int camId, bool enable);
    status_t setDeInterlace(int camId, char * type);
    status_t setDeviceName(int camId, char * type);
    status_t setFormat(int camId, char * format);
    status_t setFramerate(int camId, char * framerate);
    status_t setVideoWindow(int camId,guintptr mEmbedXid);
    status_t setDrawingAreaPosition(int camId,GtkWidget *mDrawArea);
    status_t setStreamType(int camId ,STREAM_TYPE type);
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
    void (*updateUiInterface)  (int camId, UpdateUi update);
    void (*notifyEOF) (int camId);
    static void handleEofEvent(int camId);
    static EventControl* getInstance(bool needUpdateUi);
    int waitSem();
    int postSem();
    status_t init();
    status_t setCaptureNum(int camId, int num);
    status_t getIOMode(int cameraId, char *IoModeOld);
    status_t setIoMode(int cameraId, char *IoModePtr);
    status_t getIrisMode(int camId, char *formatOld);
    status_t setIrisMode(int camId, char *irisModePtr);
    status_t getIrisLevel(int camId, int &oldValue);
    status_t setIrisLevel(int camId, int value);
    status_t getAeMode(int camId, char *value);
    status_t setAeMode(int camId, char *value);
    status_t getExposureTime(int camId, int &oldValue);
    status_t setExposureTime(int camId, int value);
    status_t getExposureGain(int camId, float &oldValue);
    status_t setExposureGain(int camId, float value);
    status_t getExposureEv(int camId, int &oldValue);
    status_t setExposureEv(int camId, int value);
    status_t getConvergeSpeedMode(int camId, char *value);
    status_t setConvergeSpeedMode(int camId, char *value);
    status_t getConvergeSpeedLevel(int camId, char *value);
    status_t setConvergeSpeedLevel(int camId, char *value);
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
    status_t getAwbShiftR(int camId, int &oldValue);
    status_t setAwbShiftR(int camId, int value);
    status_t getAwbShiftG(int camId, int &oldValue);
    status_t setAwbShiftG(int camId, int value);
    status_t getAwbShiftB(int camId, int &oldValue);
    status_t setAwbShiftB(int camId, int value);
    status_t getAwbWp(int camId, char *awbWpOld);
    status_t setAwbWp(int camId, char *awbWpPtr);
    status_t getWindowSize(int camId, int &oldValue);
    status_t setWindowSize(int camId, int value);
    status_t setColorTransform(int camId, char *value);
    status_t getColorTransform(int camId, char *value);
    status_t setAwbRegion(int camId, char *region);
    status_t setAeRegion(int camId, char *region);
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
    status_t setNumBuffers(int camId, int value);
    status_t setTestDualMode(bool enable);
    status_t quitMessageThreadLoop();
    status_t getFisheyeMode(int camId, char *value);
    status_t setFisheyeMode(int camId, char *value);

private:
// Thread methods
    enum EVENT_ID {
    EVENT_ID_SETTING_RESO = 1, /*can not be started with 0 ,for msgsnd not happy*/
    EVENT_ID_SETTING_INTERLACE_MODE = 2,
    EVENT_ID_SETTING_DEINTERLACE,
    EVENT_ID_SETTING_DEVICENAME,
    EVENT_ID_SETTING_FORMAT,
    EVENT_ID_SETTING_FRAMERATE,
    EVENT_ID_START,
    EVENT_ID_PAUSE,
    EVENT_ID_STOP,
    EVENT_ID_EXIT,
    EVENT_ID_MAX
    };

    PipelineManage *pipelineMange;
    static EventControl *sInstance;
    bool mNeedUpdateUi;
    sem_t input_sem;
    pthread_t m_ThreadID;

    GstCaffUI *mGstCaffUI;

private:
    /* IMessageHandler overloads */
    static void* messageThreadLoop(void *);
    status_t handleEventStart(int camId);
    status_t  handleEventPause(int camId );
    status_t  handleEventStop(int camId );
    status_t handleSettingReso(int camId ,int width, int height);
    status_t handleSettingInterlaceMode(int camId ,bool mode);
    status_t handleSettingFormat(int camId ,char * format);
    status_t handleSettingFramerate(int camId ,char * framerate);
    status_t handleSettingDeInterlace(int camId ,char * type );
    status_t handleSettingDeviceName(int camId ,char * type);
    status_t sendmsg(EVENT_ID eventId, char *date, long unsigned int size) ;
    status_t setStartEvent(int camId );
    status_t setPauseEvent(int camId );
    status_t setStopEvent(int camId );

}; // class EventControl

} // namespace android

#endif // _EVENT_CONTROL_H_
