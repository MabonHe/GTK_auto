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
#include "RemoteProxy.h"

namespace gstCaff {

static const supportProperty propery[] =
{
    {(char*)"cameraId", RemoteProxy::setCameraId}, //cameraId should be process first
    {(char*)"type", RemoteProxy::setStreamType},
    {(char*)"resolution", RemoteProxy::setResolution},
    {(char*)"format", RemoteProxy::setFormat},
    {(char*)"interlace", RemoteProxy::setInterlaceMode},
    {(char*)"deinterlace", RemoteProxy::setDeInterlaceMethod},
    {(char*)"state", RemoteProxy::setCameraState},
    {(char*)"sourceElement", RemoteProxy::setSrcElementType},
    {(char*)"convertElement", RemoteProxy::setConvertElementType},
    {(char*)"sinkElement", RemoteProxy::setSinkElementType},
    {(char*)"capture-num", RemoteProxy::setCaptureNum},
    {(char*)"IO-mode", RemoteProxy::setIoMode},
    {(char*)"IRIS-mode", RemoteProxy::setIrisMode},
    {(char*)"IRIS-level", RemoteProxy::setIrisLevel},
    {(char*)"exposure-time", RemoteProxy::setExposureTime},
    {(char*)"exposure-gain", RemoteProxy::setExposureGain},
    {(char*)"exposure-ev", RemoteProxy::setExposureEv},
    {(char*)"awb-mode", RemoteProxy::setAwbMode},
    {(char*)"awb-rgain", RemoteProxy::setAwbRGain},
    {(char*)"awb-ggain", RemoteProxy::setAwbGGain},
    {(char*)"awb-bgain", RemoteProxy::setAwbBGain},
    {(char*)"awb-range", RemoteProxy::setAwbRange},
    {(char*)"windowsize", RemoteProxy::setWindowSize},
    {(char*)"daynight-mode", RemoteProxy::setDayNightMode},
    {(char*)"sharpness", RemoteProxy::setSharpness},
    {(char*)"brigheness", RemoteProxy::setBrightness},
    {(char*)"constrast", RemoteProxy::setContrast},
    {(char*)"hue", RemoteProxy::setHue},
    {(char*)"saturation", RemoteProxy::setSaturation},
    {(char*)"WDR-mode", RemoteProxy::setWdrMode},
    {(char*)"BLC-mode", RemoteProxy::setBlcMode},
    {(char*)"blc-level", RemoteProxy::setBlcAreaLevel},
    {(char*)"noiseReduction-mode", RemoteProxy::setNoiseReductionMode},
    {(char*)"noiseFilterLevel", RemoteProxy::setNoiseFilter},
    {(char*)"spatialLevel", RemoteProxy::setSpatialLevel},
    {(char*)"temporialLevel", RemoteProxy::setTemporalLevel},
    {(char*)"mirrorEffect-mode", RemoteProxy::setMirrorEffectMode},
    {(char*)"scene-mode", RemoteProxy::setSceneMode},
    {(char*)"sensor-mode", RemoteProxy::setSensorMode},
    {(char*)"capture-fps", RemoteProxy::setCaptureFps},
    {(char*)"video-standard", RemoteProxy::setVideoStandard},
    {(char*)"advance-feature", RemoteProxy::setAdvanceFeature},
    {(char*)"debugLevel", RemoteProxy::setDebugLevel},
    {NULL,NULL}
};

RemoteProxy::RemoteProxy():
cameraId(0)
{

}

RemoteProxy::~RemoteProxy()
{

}

status_t RemoteProxy::init()
{
    //cameraId = 0; //default set cameraId
    return NO_ERROR;
}

status_t RemoteProxy::setCameraId(char *resPtr, RemoteProxy *pthis)
{
    int temp;
    printf("remoteproxy %s %s \n",__func__, resPtr);

    temp = atoi(resPtr);
    if (temp < MAX_CAMERA_NUM) {
        pthis->cameraId = temp;
        return NO_ERROR;
    } else {
        return ERROR;
    }
}

status_t RemoteProxy::setCameraState(char *resPtr, RemoteProxy *pthis)
{
    enum CAMERA_STATE temp;
    printf("remoteproxy %s %s \n",__func__, resPtr);

    if (strcmp(resPtr,"play") == 0)
        temp = CAMERA_STATE_START;
    else if (strcmp(resPtr,"pause") == 0)
        temp = CAMERA_STATE_PAUSE;
    else if (strcmp(resPtr,"stop") == 0)
        temp = CAMERA_STATE_STOP;
    else
        return ERROR;

    pthis->eventControl->setCameraState(pthis->cameraId, temp);
    return NO_ERROR;
}

status_t RemoteProxy::setResolution(char *resPtr , RemoteProxy *pthis)
{
    int width = 0, height = 0;
    int widthOld = 0,heightOld = 0;
    char *token = NULL;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    printf("remoteproxy %s %s \n",__func__, resPtr);

    token = strtok((char *)resPtr, "x");
    if (token != NULL) {
        width = atoi(token);
    }
    token = strtok(NULL, "x");
    if (token != NULL) {
        height = atoi(token);
    }

    pthis->eventControl->getResolution(pthis->cameraId, widthOld,heightOld);
    printf("change resolution from %dx%d to %dx%d, cameraId: %d\n",
                                widthOld, heightOld, width, height, pthis->cameraId);

    if(widthOld != width || heightOld != height) {
        pthis->eventControl->setResolution(pthis->cameraId, width, height);
        pthis->eventControl->getCameraState(pthis->cameraId, stateOld);
        if(stateOld != CAMERA_STATE_STOP)
            pthis->needToRestart++;
    }

    return NO_ERROR;
}

status_t RemoteProxy::setFormat(char *resPtr, RemoteProxy *pthis)
{
    char * formatPtr = resPtr;
    char formatOld[64] = {0};
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    printf("remoteproxy %s %s \n",__func__, resPtr);

    pthis->eventControl->getFormat(pthis->cameraId, formatOld);
    printf("formatOld is %s \n",formatOld);
    if(0 == strcmp(formatOld,formatPtr))
        return NO_ERROR;

    pthis->eventControl->setFormat(pthis->cameraId, formatPtr);
    pthis->eventControl->getCameraState(pthis->cameraId, stateOld);
    if(stateOld != CAMERA_STATE_STOP)
        pthis->needToRestart++;

    return NO_ERROR;
}

status_t RemoteProxy::setInterlaceMode(char *resPtr, RemoteProxy *pthis)
{
    char * formatPtr = resPtr;
    bool mode = false;
    bool modeOld = false;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    printf("remoteproxy %s %s \n",__func__, resPtr);

    if (strcmp(resPtr,"true") == 0 || strcmp(resPtr,"TRUE") == 0)
        mode = true;
    else if (strcmp(resPtr,"false") == 0 || strcmp(resPtr,"FALSE") == 0)
        mode = false;
    else
        return NO_ERROR;

    pthis->eventControl->getInterlaceMode(pthis->cameraId, modeOld);
    if(modeOld == mode) {
        printf("the interlaceMode is same with before \n");
        return NO_ERROR;
    }

    pthis->eventControl->setInterlaceMode(pthis->cameraId, mode);
    pthis->eventControl->getCameraState(pthis->cameraId, stateOld);
    if(stateOld != CAMERA_STATE_STOP)
        pthis->needToRestart++;

    return NO_ERROR;
}

status_t RemoteProxy::setDeInterlaceMethod(char *resPtr, RemoteProxy *pthis)
{
    char *deInterlacePtr = resPtr;
    char deInterlaceOld[64] = {0};
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    printf("remoteproxy %s %s \n",__func__, resPtr);

    pthis->eventControl->getDeInterlace(pthis->cameraId, deInterlaceOld);
    printf("deInterlaceOld is %s \n",deInterlaceOld);
    if(0 == strcmp(deInterlaceOld, deInterlacePtr))
        return NO_ERROR;

    pthis->eventControl->setDeInterlace(pthis->cameraId, deInterlacePtr);
    pthis->eventControl->getCameraState(pthis->cameraId, stateOld);
    if(stateOld != CAMERA_STATE_STOP)
        pthis->needToRestart++;

    return NO_ERROR;
}

status_t RemoteProxy::setStreamType(char *ptrType, RemoteProxy *pthis)
{

    char * streamType = ptrType;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    STREAM_TYPE type = STREAM_TYPE_PREVIEW;
    STREAM_TYPE streamTypeOld = STREAM_TYPE_PREVIEW;

    if (strcmp(ptrType,"preview") == 0)
        type = STREAM_TYPE_PREVIEW;
    else if (strcmp(ptrType,"player") == 0)
        type = STREAM_TYPE_PLAYER;
    else if (strcmp(ptrType,"recording") == 0)
        type = STREAM_TYPE_RECORDING;
    else if (strcmp(ptrType,"remote") == 0)
        type = STREAM_TYPE_REMOTE;
    else
        return ERROR;

    printf("remoteproxy %s %s \n",__func__, ptrType);

    pthis->eventControl->getStreamType(pthis->cameraId, streamTypeOld);
    printf("streamTypeOld is %d \n",streamTypeOld);
    if (streamTypeOld == type)
    {
        return NO_ERROR;
    }
    pthis->eventControl->setStreamType(pthis->cameraId, type);
    pthis->eventControl->getCameraState(pthis->cameraId, stateOld);
    if(stateOld != CAMERA_STATE_STOP)
    {
        pthis->needToRestart++;
    }
    return NO_ERROR;
}

status_t RemoteProxy::setSrcElementType(char *ptrType, RemoteProxy *pthis)
{
    printf("remoteproxy %s %s \n",__func__, ptrType);
    pthis->needToRestart++;
    return pthis->eventControl->setSrcElementType(pthis->cameraId, ptrType);
}
status_t RemoteProxy::setConvertElementType(char *ptrType, RemoteProxy *pthis)
{
    printf("remoteproxy %s %s \n",__func__, ptrType);
    pthis->needToRestart++;
    return pthis->eventControl->setConvertElementType(pthis->cameraId, ptrType);
}
status_t RemoteProxy::setSinkElementType(char *ptrType, RemoteProxy *pthis)
{
    printf("remoteproxy %s %s \n",__func__, ptrType);
    pthis->needToRestart++;
    return pthis->eventControl->setSinkElementType(pthis->cameraId, ptrType);
}
status_t RemoteProxy::setCaptureNum(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setCaptureNum(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getIOMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getIOMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setIoMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setIoMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getIrisMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getIrisMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setIrisMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setIrisMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getIrisLevel(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getIrisLevel(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setIrisLevel(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setIrisLevel(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getExposureTime(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getExposureTime(pthis->cameraId,oldValue);
}
status_t RemoteProxy::setExposureTime(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setExposureTime(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getExposureGain(char * ptr, RemoteProxy *pthis)
{
    float oldValue = 0;
    return pthis->eventControl->getExposureGain(pthis->cameraId,oldValue);
}
status_t RemoteProxy::setExposureGain(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setExposureGain(pthis->cameraId, atof(ptr));
}
status_t RemoteProxy::getExposureEv(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getExposureEv(pthis->cameraId,oldValue);
}
status_t RemoteProxy::setExposureEv(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setExposureEv(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getAwbMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getAwbMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setAwbMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setAwbMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getAwbRGain(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getAwbRGain(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setAwbRGain(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setAwbRGain(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getAwbGGain(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getAwbGGain(pthis->cameraId,oldValue);
}
status_t RemoteProxy::setAwbGGain(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setAwbGGain(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getAwbBGain(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getAwbBGain(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setAwbBGain(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setAwbBGain(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getAwbRange(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getAwbRange(pthis->cameraId, ptr);
}
status_t RemoteProxy::setAwbRange(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setAwbRange(pthis->cameraId, ptr);
}
status_t RemoteProxy::getWindowSize(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getWindowSize(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setWindowSize(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setWindowSize(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getDayNightMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getDayNightMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setDayNightMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setDayNightMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getSharpness(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getSharpness(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setSharpness(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setSharpness(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getBrightness(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getBrightness(pthis->cameraId,oldValue);
}
status_t RemoteProxy::setBrightness(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setBrightness(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getContrast(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getContrast(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setContrast(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setContrast(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getHue(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getHue(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setHue(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setHue(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getSaturation(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getSaturation(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setSaturation(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setSaturation(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getWdrMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getWdrMode(pthis->cameraId ,ptr);
}
status_t RemoteProxy::setWdrMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setWdrMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getBlcMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getBlcMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setBlcMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setBlcMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getBlcAreaLevel(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getBlcAreaLevel(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setBlcAreaLevel(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setBlcAreaLevel(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getNoiseReductionMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getNoiseReductionMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setNoiseReductionMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setNoiseReductionMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getNoiseFilter(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getNoiseFilter(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setNoiseFilter(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setNoiseFilter(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getSpatialLevel(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getSpatialLevel(pthis->cameraId,oldValue);
}
status_t RemoteProxy::setSpatialLevel(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setSpatialLevel(pthis->cameraId, atoi(ptr));
}
status_t RemoteProxy::getTemporalLevel(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->getTemporalLevel(pthis->cameraId, oldValue);
}
status_t RemoteProxy::setTemporalLevel(char * ptr, RemoteProxy *pthis)
{
    int oldValue = 0;
    return pthis->eventControl->setTemporalLevel(pthis->cameraId, oldValue);
}
status_t RemoteProxy::getMirrorEffectMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getMirrorEffectMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setMirrorEffectMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setMirrorEffectMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getSceneMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getSceneMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setSceneMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setSceneMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getSensorMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getSensorMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::setSensorMode(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setSensorMode(pthis->cameraId, ptr);
}
status_t RemoteProxy::getCaptureFps(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getCaptureFps(pthis->cameraId, ptr);
}
status_t RemoteProxy::setCaptureFps(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setCaptureFps(pthis->cameraId, ptr);
}
status_t RemoteProxy::getVideoStandard(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getVideoStandard(pthis->cameraId, ptr);
}
status_t RemoteProxy::setVideoStandard(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setVideoStandard(pthis->cameraId, ptr);
}
status_t RemoteProxy::getAdvanceFeature(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->getAdvanceFeature(pthis->cameraId, ptr);
}
status_t RemoteProxy::setAdvanceFeature(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setAdvanceFeature(pthis->cameraId, ptr);
}
status_t RemoteProxy::setDebugLevel(char * ptr, RemoteProxy *pthis)
{
    return pthis->eventControl->setDebugLevel(pthis->cameraId, atoi(ptr));
}

status_t RemoteProxy::processCommand(char *buf)
{
    int i = 0;
    char *s = NULL;
    char temp[BUFFER_SIZE] = {0};
    CAMERA_STATE state = CAMERA_STATE_STOP;
    printf("@%s\n", __func__);
    needToRestart = 0;
    cameraId = 0;

    eventControl->waitSem();
    while(propery[i].keyString != NULL)
    {
        s = strstr(buf, propery[i].keyString);
        if(s != NULL) {
            printf("find str in command %s %s\n", propery[i].keyString, s+ strlen(propery[i].keyString)+1);
            strcpy(temp,s+ strlen(propery[i].keyString)+1);
            propery[i].process(strtok(temp,","), this);
        }
        i++;
    }

    eventControl->getCameraState(cameraId, state);
    if(needToRestart && state != CAMERA_STATE_STOP) {
        eventControl->setCameraState(cameraId, CAMERA_STATE_STOP);
        eventControl->setCameraState(cameraId, CAMERA_STATE_START);
    }
    eventControl->postSem();
    return NO_ERROR;
}

void * RemoteProxy::tcp_server_loop(void* ptr)
{
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int sin_size, count;
    fd_set inset, tmp_inset;
    int sockfd, client_fd, fd;
    char buf[BUFFER_SIZE];
    RemoteProxy * pthis = (RemoteProxy *)ptr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(PORT);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_sockaddr.sin_zero), 8);

    int i = 1;/* SO_REUSEADDR reuse the socket */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
    if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if(listen(sockfd, MAX_QUE_CONN_NM) == -1) {
        perror("listen");
        exit(1);
    }
    printf("listening....\n");

    /*socket */
    FD_ZERO(&inset);
    FD_SET(sockfd, &inset);
    while (1) {
        tmp_inset = inset;
        sin_size=sizeof(struct sockaddr_in);
        memset(buf, 0, sizeof(buf));
        /*select*/
        if (!(select(MAX_SOCK_FD, &tmp_inset, NULL, NULL, NULL) > 0)) {
            perror("select");
            close(sockfd);
            exit(1);
        }

        for (fd = 0; fd < MAX_SOCK_FD; fd++) {
            if (FD_ISSET(fd, &tmp_inset) > 0 ) {
                if (fd == sockfd) { /*accept the connect*/
                    int ip = 0;
                    client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, (socklen_t*)&sin_size);
                    if (client_fd == -1) {
                        perror("accept");
                        exit(1);
                    }
                    FD_SET(client_fd, &inset);
                    printf("New connection from %d(socket)\n", client_fd);
                    printf("New connection from 0x%x(ip)\n", (int)client_sockaddr.sin_addr.s_addr);
                    pthis->eventControl->getRemoteIp(0,ip);
                    if(ip != 0) {
                        const char *serverBusy = "server device is busy\n";
                        printf("server device is busy, for connection with 0x%x\n",ip);
                        send(client_fd, serverBusy, (strlen(serverBusy) + 1), 0);
                        close(client_fd);
                        FD_CLR(client_fd, &inset);
                        printf("close fd %d \n",client_fd);
                    }
                    else
                        pthis->eventControl->setRemoteIp(0,(int)client_sockaddr.sin_addr.s_addr);
                }
                else /* process the data from client*/
                {
                    if ((count = recv(fd, buf, BUFFER_SIZE, 0)) > 0)
                    {
                        printf("Received a message from %d: %s\n", fd, buf);
                        pthis->processCommand(buf);
                        send(fd,"Welcometo my server.\n",22,0);
                        printf("send to  fd %d \n",fd);
                    }
                    else
                    {
                        STREAM_TYPE type;
                        close(fd);
                        FD_CLR(fd, &inset);
                        printf("Client %d(socket) has left\n", fd);
                        pthis->eventControl->getStreamType(0,type);
                        /*if it is remote type, server has a udp sink pipeline running ,stop it */
                        if(type == STREAM_TYPE_REMOTE)
                            pthis->eventControl->setCameraState(0,CAMERA_STATE_STOP);
                        pthis->eventControl->setRemoteIp(0,0);
                    }
                }
            } /*end of if FD_ISSET*/
        } /*end of for fd*/
    } /*end of for while*/

    close(sockfd);
    exit(0);
}

int RemoteProxy::tcp_server(bool needUpdateUi)
{
    int err;
    pthread_t ntid;

    eventControl = EventControl::getInstance(needUpdateUi);

    err = pthread_create(&ntid, NULL, tcp_server_loop, this);
    if (err != 0)
        printf("can't create thread: %s\n", strerror(err));
    printf("tcp_server thread:");
    return NO_ERROR;
}
}

