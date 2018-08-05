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

#define LOG_TAG "EventControl"

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "EventControl.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "gstCaffUI.h"

#define MSGKEY 1024

struct msgstru
{
    long msgtype;
    char msgtext[2048];
};

namespace gstCaff {

EventControl::EventControl(bool needUpdateUi):
mNeedUpdateUi(needUpdateUi), mGstCaffUI(NULL)
{
    int err;
    m_ThreadID= 0;
    err = pthread_create(&m_ThreadID, NULL, messageThreadLoop, this);
    if (err != 0)
        printf("can't create thread: %s\n", strerror(err));
    printf("main thread:");
}

void EventControl::setGstCaffUIHandle(GstCaffUI *pGstCaffUI)
{
    mGstCaffUI = pGstCaffUI;
}

void EventControl::updateUICameraNum(int ncount)
{
    if (NULL != mGstCaffUI)
    {
        printf("Update camera num to %d", ncount);
        mGstCaffUI->UpdateCameraNum(ncount);
    }
}

status_t EventControl::setStartEvent(int camId)
{
    printf("%s\n",__func__);

    PipelineStatus status;
    status.camId = camId;
    return sendmsg(EVENT_ID_START, (char *)&status, sizeof(status));
}

status_t EventControl::setPauseEvent(int camId )
{
    printf("%s\n",__func__);

    PipelineStatus status;
    status.camId = camId;
    return sendmsg(EVENT_ID_PAUSE, (char *)&status, sizeof(status));
}

status_t EventControl::setCameraState(int camId, CAMERA_STATE state)
{
    switch(state) {
        case CAMERA_STATE_START:
            setStartEvent(camId);
            break;
        case CAMERA_STATE_PAUSE:
            setPauseEvent(camId);
            break;
        case CAMERA_STATE_STOP:
            setStopEvent(camId);
            break;
        default:
            printf("unknown camera state\n");
            return ERROR;
    }
    return NO_ERROR;
}

status_t EventControl::getCameraState(int camId, CAMERA_STATE &state)
{
    return pipelineMange->getCameraState(camId, state);
}

status_t EventControl::setStopEvent(int camId )
{
    printf("%s\n",__func__);
    PipelineStatus status;
    status.camId = camId;
    return sendmsg(EVENT_ID_STOP, (char *)&status, sizeof(status));
}

status_t EventControl::setResolution(int camId, int width, int height)
{
    printf("%s\n",__func__);
    PipelineStatus status;
    status.camId = camId;
    status.width = width;
    status.height = height;

    return sendmsg(EVENT_ID_SETTING_RESO, (char *)&status, sizeof(status));
}

status_t EventControl::setInterlaceMode(int camId, bool value)
{
    printf("%s\n",__func__);
    PipelineStatus status;
    status.camId = camId;
    status.interlace_mode = value;

    return sendmsg(EVENT_ID_SETTING_INTERLACE_MODE, (char *)&status, sizeof(status));
}

status_t EventControl::setDeInterlace(int camId, char * type)
{
    printf("%s\n",__func__);
    PipelineStatus status;
    status.camId = camId;
    if (NULL == type) {
        printf("%s set type is NULL\n",__func__);
        return ERROR;
    }
    strcpy(status.deInterlace,type);
    return sendmsg(EVENT_ID_SETTING_DEINTERLACE, (char *)&status, sizeof(status));
}

status_t EventControl::setDeviceName(int camId, char * type)
{
    printf("%s\n",__func__);
    PipelineStatus status;
    status.camId = camId;
    if (NULL == type) {
        printf("%s set type is NULL\n",__func__);
        return ERROR;
    }
    strcpy(status.deviceName,type);
    return sendmsg(EVENT_ID_SETTING_DEVICENAME, (char *)&status, sizeof(status));
}

status_t EventControl::setFormat(int camId, char * format)
{
    printf("%s\n",__func__);
    PipelineStatus status;
    status.camId = camId;
    strcpy(status.format,format);

    return sendmsg(EVENT_ID_SETTING_FORMAT, (char *)&status, sizeof(status));
}

status_t EventControl::setFramerate(int camId, char * framerate)
{
    printf("%s:%s\n",__func__, framerate);
    PipelineStatus status;
    status.camId = camId;
    strcpy(status.framerate,framerate);

    return sendmsg(EVENT_ID_SETTING_FRAMERATE, (char *)&status, sizeof(status));
}

status_t EventControl::setVcEnable(int camId, bool enable)
{
    return pipelineMange->setVcEnable(camId, enable);
}

status_t EventControl::setSrcElementType(int camId,char * type)
{
    return pipelineMange->setSrcElementType(camId, type);
}

status_t EventControl::setConvertElementType(int camId,char * type)
{
    return pipelineMange->setConvertElementType(camId, type);
}

status_t EventControl::setSinkElementType(int camId,char * type)
{
    return pipelineMange->setSinkElementType(camId, type);
}

status_t EventControl::getResolution(int camId, int &width, int &height)
{
    return pipelineMange->getReso(camId, width, height);
}


status_t EventControl::getInterlaceMode(int camId, bool &value)
{
    return pipelineMange->getInterlaceMode(camId, value);
}


status_t EventControl::getDeInterlace(int camId, char * type)
{
    return pipelineMange->getDeInterlace(camId, type);
}

status_t EventControl::getDeviceName(int camId, char * type)
{
    return pipelineMange->getDeviceName(camId, type);
}

status_t EventControl::getFormat(int camId, char * format)
{
    return pipelineMange->getFormat(camId, format);
}


status_t EventControl::setVideoWindow(int camId,guintptr mEmbedXid )
{
    return pipelineMange->setVideoWindow(camId, mEmbedXid);
}


status_t EventControl::setDrawingAreaPosition(int camId, GtkWidget *mDrawArea )
{
    return pipelineMange->setDrawingAreaPosition(camId, mDrawArea);
}


status_t EventControl::setStreamType(int camId ,STREAM_TYPE type)
{
    return pipelineMange->setStreamType(camId, type);
}

status_t EventControl::getStreamType(int camId ,STREAM_TYPE &type)
{
    return pipelineMange->getStreamType(camId, type);
}

status_t EventControl::setRemoteIp(int camId ,int ip)
{
    return pipelineMange->setRemoteIp(camId, ip);
}

status_t EventControl::getRemoteIp(int camId ,int &ip)
{
    return pipelineMange->getRemoteIp(camId, ip);
}

EventControl * EventControl::sInstance;

/*static*/
EventControl* EventControl::getInstance(bool needUpdateUi)
{
    if (!sInstance) {
        sInstance = new EventControl(needUpdateUi);
    }
    return sInstance;
}

int EventControl::waitSem()
{
    printf("%s \n",__func__);
    return sem_wait(&input_sem);
}


int EventControl::postSem()
{
    printf("%s \n",__func__);
    return sem_post(&input_sem);
}


status_t EventControl::sendmsg(EVENT_ID eventId, char *data, long unsigned int size)
{
    struct msgstru msgs;
    int msg_type;
    char str[256];
    int msqid,ret_value;

    msqid=msgget(MSGKEY,IPC_EXCL);/*get the queue id by MSGKEY*/
    if (msqid < 0) {
        return -1;
    }

    printf("sent message to %d size %lu  %s:%d\n",msqid,size,__FUNCTION__,__LINE__);
    msgs.msgtype = eventId;
    memcpy(msgs.msgtext, data, size);
    /*send msg to the queue */
        ret_value = msgsnd(msqid,&msgs,sizeof(struct msgstru), 0);
        if (ret_value < 0) {
            printf("msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
            exit(-1);
            }

    return NO_ERROR;
}


status_t EventControl::init()
{
    sem_init(&input_sem, 0, 1);
    pipelineMange = PipelineManage::getInstance();
    pipelineMange->init();
    pipelineMange->notifyEOF = EventControl::handleEofEvent;

    return NO_ERROR;
}

void EventControl::handleEofEvent(int camId)
{
    printf("handleEofEvent, cameraId: %d\n", camId);
    getInstance(false)->notifyEOF(camId);
}

status_t EventControl::setCaptureNum(int camId, int num)
{
    return pipelineMange->setCaptureNum(camId,num);
}
status_t EventControl::getIOMode(int camId, char *IoModeOld)
{
    return pipelineMange->getIOMode(camId,IoModeOld);
}
status_t EventControl::setIoMode(int camId, char *IoModePtr)
{
    return pipelineMange->setIoMode(camId,IoModePtr);
}
status_t EventControl::getIrisMode(int camId, char *formatOld)
{
    return pipelineMange->getIrisMode(camId,formatOld);
}
status_t EventControl::setIrisMode(int camId, char *irisModePtr)
{
    return pipelineMange->setIrisMode(camId,irisModePtr);
}
status_t EventControl::getIrisLevel(int camId, int &oldValue)
{
    return pipelineMange->getIrisLevel(camId,oldValue);
}
status_t EventControl::setIrisLevel(int camId, int value)
{
    return pipelineMange->setIrisLevel(camId,value);
}
status_t EventControl::getAeMode(int camId, char *value)
{
    return pipelineMange->getAeMode(camId,value);
}
status_t EventControl::setAeMode(int camId, char *value)
{
    return pipelineMange->setAeMode(camId,value);
}
status_t EventControl::getFisheyeMode(int camId, char *value)
{
    return pipelineMange->getFisheyeMode(camId,value);
}
status_t EventControl::setFisheyeMode(int camId, char *value)
{
    return pipelineMange->setFisheyeMode(camId,value);
}
status_t EventControl::getConvergeSpeedMode(int camId, char *value)
{
    return pipelineMange->getConvergeSpeedMode(camId, value);
}
status_t EventControl::setConvergeSpeedMode(int camId, char *value)
{
    return pipelineMange->setConvergeSpeedMode(camId, value);
}
status_t EventControl::getConvergeSpeedLevel(int camId, char *value)
{
    return pipelineMange->getConvergeSpeedLevel(camId, value);
}
status_t EventControl::setConvergeSpeedLevel(int camId, char *value)
{
    return pipelineMange->setConvergeSpeedLevel(camId, value);
}
status_t EventControl::getExposureTime(int camId, int &oldValue)
{
    return pipelineMange->getExposureTime(camId,oldValue);
}
status_t EventControl::setExposureTime(int camId, int value)
{
    return pipelineMange->setExposureTime(camId,value);
}
status_t EventControl::getExposureGain(int camId, float &oldValue)
{
    return pipelineMange->getExposureGain(camId,oldValue);
}
status_t EventControl::setExposureGain(int camId, float value)
{
    return pipelineMange->setExposureGain(camId,value);
}
status_t EventControl::getExposureEv(int camId, int &oldValue)
{
    return pipelineMange->getExposureEv(camId,oldValue);
}
status_t EventControl::setExposureEv(int camId, int value)
{
    return pipelineMange->setExposureEv(camId,value);
}
status_t EventControl::getAwbMode(int camId, char *awbModeOld)
{
    return pipelineMange->getAwbMode(camId,awbModeOld);
}
status_t EventControl::setAwbMode(int camId, char *awbModePtr)
{
    return pipelineMange->setAwbMode(camId,awbModePtr);
}
status_t EventControl::getAwbRGain(int camId, int &oldValue)
{
    return pipelineMange->getAwbRGain(camId,oldValue);
}
status_t EventControl::setAwbRGain(int camId, int value)
{
    return pipelineMange->setAwbRGain(camId,value);
}
status_t EventControl::getAwbGGain(int camId, int &oldValue)
{
    return pipelineMange->getAwbGGain(camId,oldValue);
}
status_t EventControl::setAwbGGain(int camId, int value)
{
    return pipelineMange->setAwbGGain(camId,value);
}
status_t EventControl::getAwbBGain(int camId, int &oldValue)
{
    return pipelineMange->getAwbBGain(camId,oldValue);
}
status_t EventControl::setAwbBGain(int camId, int value)
{
    return pipelineMange->setAwbBGain(camId,value);
}
status_t EventControl::getAwbRange(int camId, char *awbRangeOld)
{
    return pipelineMange->getAwbRange(camId,awbRangeOld);
}
status_t EventControl::setAwbRange(int camId, char *awbRangePtr)
{
    return pipelineMange->setAwbRange(camId,awbRangePtr);
}
status_t EventControl::getAwbWp(int camId, char *awbWpOld)
{
    return pipelineMange->getAwbWp(camId,awbWpOld);
}
status_t EventControl::setAwbWp(int camId, char *awbWpPtr)
{
    return pipelineMange->setAwbWp(camId,awbWpPtr);
}
status_t EventControl::getWindowSize(int camId, int &oldValue)
{
    return pipelineMange->getWindowSize(camId,oldValue);
}
status_t EventControl::setWindowSize(int camId, int value)
{
    return pipelineMange->setWindowSize(camId,value);
}
status_t EventControl::getColorTransform(int camId, char *value)
{
    return pipelineMange->getColorTransform(camId, value);
}
status_t EventControl::setColorTransform(int camId, char *value)
{
    return pipelineMange->setColorTransform(camId, value);
}
status_t EventControl::getAwbShiftR(int camId, int &oldValue)
{
    return pipelineMange->getAwbShiftR(camId,oldValue);
}
status_t EventControl::setAwbShiftR(int camId, int value)
{
    return pipelineMange->setAwbShiftR(camId,value);
}
status_t EventControl::getAwbShiftG(int camId, int &oldValue)
{
    return pipelineMange->getAwbShiftG(camId,oldValue);
}
status_t EventControl::setAwbShiftG(int camId, int value)
{
    return pipelineMange->setAwbShiftG(camId,value);
}
status_t EventControl::getAwbShiftB(int camId, int &oldValue)
{
    return pipelineMange->getAwbShiftB(camId,oldValue);
}
status_t EventControl::setAwbShiftB(int camId, int value)
{
    return pipelineMange->setAwbShiftB(camId,value);
}
status_t EventControl::setAeRegion(int camId, char *region)
{
    return pipelineMange->setAeRegion(camId, region);
}
status_t EventControl::setAwbRegion(int camId, char *region)
{
    return pipelineMange->setAwbRegion(camId, region);
}
status_t EventControl::getDayNightMode(int camId, char *dayNightModeOld)
{
    return pipelineMange->getDayNightMode(camId,dayNightModeOld);
}
status_t EventControl::setDayNightMode(int camId, char *dayNightModePtr)
{
    return pipelineMange->setDayNightMode(camId,dayNightModePtr);
}
status_t EventControl::getSharpness(int camId, int &oldValue)
{
    return pipelineMange->getSharpness(camId,oldValue);
}
status_t EventControl::setSharpness(int camId, int value)
{
    return pipelineMange->setSharpness(camId,value);
}
status_t EventControl::getBrightness(int camId, int &oldValue)
{
    return pipelineMange->getBrightness(camId,oldValue);
}
status_t EventControl::setBrightness(int camId, int value)
{
    return pipelineMange->setBrightness(camId,value);
}
status_t EventControl::getContrast(int camId, int &oldValue)
{
    return pipelineMange->getContrast(camId,oldValue);
}
status_t EventControl::setContrast(int camId, int value)
{
    return pipelineMange->setContrast(camId,value);
}
status_t EventControl::getHue(int camId, int &oldValue)
{
    return pipelineMange->getHue(camId,oldValue);
}
status_t EventControl::setHue(int camId, int value)
{
    return pipelineMange->setHue(camId,value);
}
status_t EventControl::getSaturation(int camId, int &oldValue)
{
    return pipelineMange->getSaturation(camId,oldValue);
}
status_t EventControl::setSaturation(int camId, int value)
{
    return pipelineMange->setSaturation(camId,value);
}
status_t EventControl::getWdrMode(int camId, char *wdrModeOld)
{
    return pipelineMange->getWdrMode(camId,wdrModeOld);
}
status_t EventControl::setWdrMode(int camId, char *wdrModePtr)
{
    return pipelineMange->setWdrMode(camId,wdrModePtr);
}
status_t EventControl::getWdrLevel(int camId, int &oldValue)
{
    return pipelineMange->getWdrLevel(camId,oldValue);
}
status_t EventControl::setWdrLevel(int camId, int value)
{
    return pipelineMange->setWdrLevel(camId,value);
}
status_t EventControl::getBlcMode(int camId, char *wdrModeOld)
{
    return pipelineMange->getBlcMode(camId,wdrModeOld);
}
status_t EventControl::setBlcMode(int camId, char *enableBlcPtr)
{
    return pipelineMange->setBlcMode(camId,enableBlcPtr);
}
status_t EventControl::getBlcAreaLevel(int camId, int &oldValue)
{
    return pipelineMange->getBlcAreaLevel(camId,oldValue);
}
status_t EventControl::setBlcAreaLevel(int camId, int value)
{
    return pipelineMange->setBlcAreaLevel(camId,value);
}
status_t EventControl::getNoiseReductionMode(int camId, char *noiseReductionModeOld)
{
    return pipelineMange->getNoiseReductionMode(camId,noiseReductionModeOld);
}
status_t EventControl::setNoiseReductionMode(int camId, char *wdrModePtr)
{
    return pipelineMange->setNoiseReductionMode(camId,wdrModePtr);
}
status_t EventControl::getNoiseFilter(int camId, int &oldValue)
{
    return pipelineMange->getNoiseFilter(camId,oldValue);
}
status_t EventControl::setNoiseFilter(int camId, int value)
{
    return pipelineMange->setNoiseFilter(camId,value);
}
status_t EventControl::getSpatialLevel(int camId, int &oldValue)
{
    return pipelineMange->getSpatialLevel(camId,oldValue);
}
status_t EventControl::setSpatialLevel(int camId, int value)
{
    return pipelineMange->setSpatialLevel(camId,value);
}
status_t EventControl::getTemporalLevel(int camId, int &oldValue)
{
    return pipelineMange->getTemporalLevel(camId,oldValue);
}
status_t EventControl::setTemporalLevel(int camId, int value)
{
    return pipelineMange->setTemporalLevel(camId,value);
}
status_t EventControl::getMirrorEffectMode(int camId, char *mirrorEffectModeOld)
{
    return pipelineMange->getMirrorEffectMode(camId,mirrorEffectModeOld);
}
status_t EventControl::setMirrorEffectMode(int camId, char *mirrorEffectModePtr)
{
    return pipelineMange->setMirrorEffectMode(camId,mirrorEffectModePtr);
}
status_t EventControl::getSceneMode(int camId, char *mirrorEffectModeOld)
{
    return pipelineMange->getSceneMode(camId,mirrorEffectModeOld);
}
status_t EventControl::setSceneMode(int camId, char *sceneModePtr)
{
    return pipelineMange->setSceneMode(camId,sceneModePtr);
}
status_t EventControl::getSensorMode(int camId, char *sensorModeOld)
{
    return pipelineMange->getSensorMode(camId,sensorModeOld);
}
status_t EventControl::setSensorMode(int camId, char *sensorModePtr)
{
    return pipelineMange->setSensorMode(camId,sensorModePtr);
}
status_t EventControl::getCaptureFps(int camId, char *sensorModeOld)
{
    return pipelineMange->getCaptureFps(camId,sensorModeOld);
}
status_t EventControl::setCaptureFps(int camId, char *captureFpsPtr)
{
    return pipelineMange->setCaptureFps(camId,captureFpsPtr);
}
status_t EventControl::getVideoStandard(int camId, char *videoStandardOld)
{
    return pipelineMange->getVideoStandard(camId,videoStandardOld);
}
status_t EventControl::setVideoStandard(int camId, char *videoStandardPtr)
{
    return pipelineMange->setVideoStandard(camId,videoStandardPtr);
}
status_t EventControl::getAdvanceFeature(int camId, char *advancedOld)
{
    return pipelineMange->getAdvanceFeature(camId,advancedOld);
}
status_t EventControl::setAdvanceFeature(int camId, char *advancedPtr)
{
    return pipelineMange->setAdvanceFeature(camId,advancedPtr);
}
status_t EventControl::setCustomAicParam(int camId, char *aicParam)
{
    return pipelineMange->setCustomAicParam(camId,aicParam);
}
status_t EventControl::setDebugLevel(int camId, int value)
{
    return pipelineMange->setDebugLevel(camId,value);
}

status_t EventControl::setNumBuffers(int camId, int value)
{
    return pipelineMange->setNumBuffers(camId, value);
}

status_t EventControl::setTestDualMode(bool enable)
{
    return pipelineMange->setTestDualMode(enable);
}

EventControl::~EventControl()
{
    printf("@%s:%d\n", __FUNCTION__,__LINE__);
    quitMessageThreadLoop();

    if (pipelineMange != NULL)
    {
        printf("delete pipelinemanage\n");
        delete pipelineMange;
        pipelineMange = NULL;
    }
}

/**********************************************************************
private functions
 */
status_t  EventControl::handleEventStart(int camId)
{
    printf("%s\n",__func__);
    return pipelineMange->pipelineSetState(camId, CAMERA_STATE_START);
}

status_t  EventControl::handleEventPause(int camId )
{
    printf("%s\n",__func__);
    return pipelineMange->pipelineSetState(camId,CAMERA_STATE_PAUSE);
}

status_t  EventControl::handleEventStop(int camId )
{
    printf("%s\n",__func__);
    return pipelineMange->pipelineSetState(camId,CAMERA_STATE_STOP);

}

status_t  EventControl::handleSettingInterlaceMode(int camId ,bool mode)
{
    int ret = 0;
    printf("%s\n",__func__);

    ret = pipelineMange->setInterlaceMode(camId, mode);
    if (mNeedUpdateUi)
        updateUiInterface(camId, UPDATE_UI_SETTING_INTERLACE);
    return ret;
}

status_t  EventControl::handleSettingFormat(int camId ,char * format)
{
    int ret = 0;
    printf("%s\n",__func__);

    ret =  pipelineMange->setFormat(camId, format);
    if (mNeedUpdateUi)
        updateUiInterface(camId, UPDATE_UI_SETTING_FMT);
    return ret;
}

status_t  EventControl::handleSettingFramerate(int camId ,char * framerate)
{
    int ret = 0;
    printf("%s\n",__func__);

    ret =  pipelineMange->setFramerate(camId, framerate);
    if (mNeedUpdateUi)
        updateUiInterface(camId, UPDATE_UI_SETTING_FRT);
    return ret;
}

status_t EventControl::handleSettingDeInterlace(int camId ,char * type)
{
    int ret = 0;
    printf("%s\n",__func__);

    ret = pipelineMange->setDeInterlace(camId, type);
    if (mNeedUpdateUi)
        updateUiInterface(camId, UPDATE_UI_SETTING_DEINTERLACE);
    return ret;
}

status_t EventControl::handleSettingDeviceName(int camId ,char * type)
{
    int ret = 0;
    printf("%s\n",__func__);

    ret = pipelineMange->setDeviceName(camId, type);
    if (mNeedUpdateUi)
        updateUiInterface(camId, UPDATE_UI_SETTING_DEINTERLACE);
    return ret;
}

status_t  EventControl::handleSettingReso(int camId ,int width, int height)
{
    int ret = 0;
    printf("%s\n",__func__);

    ret = pipelineMange->setReso(camId, width, height);
    if (mNeedUpdateUi)
        updateUiInterface(camId, UPDATE_UI_SETTING_RES);
    return ret;
}

status_t EventControl::quitMessageThreadLoop()
{
    int ret = 0;
    printf("%s\n",__func__);
    //waiting the status change to STOP for pipeline.
    sleep(2);
    ret = sendmsg(EVENT_ID_EXIT, "EXIT", 4);
    pthread_join(m_ThreadID, NULL);
    return ret;
}

void* EventControl::messageThreadLoop(void *ptr)
{
    struct msgstru msgs;
    int msqid,ret_value;
    char str[512];
    bool threadRunning = true;
    printf("@%s: Start", __FUNCTION__);
    PipelineStatus *status = NULL;

    msqid=msgget(MSGKEY,IPC_EXCL);  /*get the queue id by MSGKEY*/
    if (msqid < 0) {
        msqid = msgget(MSGKEY,IPC_CREAT|0666);/*create the queue with MSGKEY*/
        if (msqid <0) {
            printf("failed to create msq | errno=%d [%s]\n",errno,strerror(errno));
            exit(-1);
        }
        printf("create message que  %d \n",msqid);
    }
    while(threadRunning) {
        /*receive from the queue*/
        printf("wait message\n");
        ret_value = msgrcv(msqid,&msgs,sizeof(struct msgstru),0,0);
        printf("text=[%p] pid=[%d] event=[%lu]\n", msgs.msgtext, getpid(), msgs.msgtype);
        status = (PipelineStatus*)msgs.msgtext;
        switch (msgs.msgtype) {
            case EVENT_ID_EXIT:
                printf("received EVENT_ID_EXIT\n\r");
                threadRunning = false;
                break;
            case EVENT_ID_SETTING_RESO:
                printf("received EVENT_ID_SETTING_RESO \n\r");
                ((EventControl*)ptr)->handleSettingReso(status->camId,status->width,status->height);
                break;
            case EVENT_ID_SETTING_FORMAT:
                printf("received EVENT_ID_SETTING_FORMAT event ok %s \n\r",status->format);
                ((EventControl*)ptr)->handleSettingFormat(status->camId,status->format);
                break;
            case EVENT_ID_SETTING_FRAMERATE:
                printf("received EVENT_ID_SETTING_FRAMERATE event ok %s \n\r",status->framerate);
                ((EventControl*)ptr)->handleSettingFramerate(status->camId,status->framerate);
                break;
            case EVENT_ID_SETTING_INTERLACE_MODE:
                printf("received EVENT_ID_SETTING_INTERLACE_MODE \n\r");
                ((EventControl*)ptr)->handleSettingInterlaceMode(status->camId,status->interlace_mode);
                break;
            case EVENT_ID_SETTING_DEINTERLACE:
                printf("received EVENT_ID_SETTING_DEINTERLACE event ok \n\r");
                ((EventControl*)ptr)->handleSettingDeInterlace(status->camId,status->deInterlace);
                break;
            case EVENT_ID_SETTING_DEVICENAME:
                printf("received EVENT_ID_SETTING_DEVICENAME event ok \n\r");
                ((EventControl*)ptr)->handleSettingDeviceName(status->camId,status->deviceName);
                break;
            case EVENT_ID_START:
                printf("received EVENT_ID_START \n\r");
                ((EventControl*)ptr)->handleEventStart(status->camId);
                break;
            case EVENT_ID_PAUSE:
                printf("received EVENT_ID_PAUSE \n\r");
                ((EventControl*)ptr)->handleEventPause(status->camId);
                break;
            case EVENT_ID_STOP:
                printf("received EVENT_ID_STOP \n\r");
                ((EventControl*)ptr)->handleEventStop(status->camId);
                break;
            default:
                break;
        }
    }

    printf("msq deleting %s %s %d\n\r ",__FILE__,__FUNCTION__,__LINE__);
    if ((msgctl( msqid, IPC_RMID, NULL)) < 0) { /* delete the message queue*/
        perror ("msgctl");
        exit (1 );
    }
    return NULL;
}

};  // namespac
