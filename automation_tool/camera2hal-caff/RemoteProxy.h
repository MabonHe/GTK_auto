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
#ifndef _REMOTE_PROXY_H_
#define _REMOTE_PROXY_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include "EventControl.h"

#define PORT                4321
#define MAX_QUE_CONN_NM         5
#define MAX_SOCK_FD         FD_SETSIZE
#define BUFFER_SIZE         1024

namespace gstCaff {
class RemoteProxy;

struct supportProperty{
   char *keyString;
   status_t(*process)(char *,RemoteProxy*);
};

/**
 * class RemoteProxy
 */
class RemoteProxy {
public:
    RemoteProxy();
    ~RemoteProxy();
    status_t init();
    int tcp_server(bool needUpdateUi);
    static void * tcp_server_loop(void*);
    status_t processCommand(char *buf);
    static status_t setCameraId(char *, RemoteProxy*);
    static status_t setCameraState(char *, RemoteProxy*);
    static status_t setResolution(char *, RemoteProxy*);
    static status_t setStreamType(char *, RemoteProxy*);
    static status_t setFormat(char *, RemoteProxy* );
    static status_t setInterlaceMode(char *, RemoteProxy*);
    static status_t setDeInterlaceMethod(char *, RemoteProxy*);
    static status_t setSrcElementType(char *, RemoteProxy*);
    static status_t setConvertElementType(char *, RemoteProxy*);
    static status_t setSinkElementType(char *, RemoteProxy*);
    static status_t setCaptureNum(char *, RemoteProxy*);
    static status_t getIOMode(char *, RemoteProxy*);
    static status_t setIoMode(char *, RemoteProxy*);
    static status_t getIrisMode(char *, RemoteProxy*);
    static status_t setIrisMode(char *, RemoteProxy*);
    static status_t getIrisLevel(char *, RemoteProxy*);
    static status_t setIrisLevel(char *, RemoteProxy*);
    static status_t getExposureTime(char *, RemoteProxy*);
    static status_t setExposureTime(char *, RemoteProxy*);
    static status_t getExposureGain(char *, RemoteProxy*);
    static status_t setExposureGain(char *, RemoteProxy*);
    static status_t getExposureEv(char *, RemoteProxy*);
    static status_t setExposureEv(char *, RemoteProxy*);
    static status_t getAwbMode(char *, RemoteProxy*);
    static status_t setAwbMode(char *, RemoteProxy*);
    static status_t getAwbRGain(char *, RemoteProxy*);
    static status_t setAwbRGain(char *, RemoteProxy*);
    static status_t getAwbGGain(char *, RemoteProxy*);
    static status_t setAwbGGain(char *, RemoteProxy*);
    static status_t getAwbBGain(char *, RemoteProxy*);
    static status_t setAwbBGain(char *, RemoteProxy*);
    static status_t getAwbRange(char *, RemoteProxy*);
    static status_t setAwbRange(char *, RemoteProxy*);
    static status_t getWindowSize(char *, RemoteProxy*);
    static status_t setWindowSize(char *, RemoteProxy*);
    static status_t getDayNightMode(char *, RemoteProxy*);
    static status_t setDayNightMode(char *, RemoteProxy*);
    static status_t getSharpness(char *, RemoteProxy*);
    static status_t setSharpness(char *, RemoteProxy*);
    static status_t getBrightness(char *, RemoteProxy*);
    static status_t setBrightness(char *, RemoteProxy*);
    static status_t getContrast(char *, RemoteProxy*);
    static status_t setContrast(char *, RemoteProxy*);
    static status_t getHue(char *, RemoteProxy*);
    static status_t setHue(char *, RemoteProxy*);
    static status_t getSaturation(char *, RemoteProxy*);
    static status_t setSaturation(char *, RemoteProxy*);
    static status_t getWdrMode(char *, RemoteProxy*);
    static status_t setWdrMode(char *, RemoteProxy*);
    static status_t getBlcMode(char *, RemoteProxy*);
    static status_t setBlcMode(char *, RemoteProxy*);
    static status_t getBlcAreaLevel(char *, RemoteProxy*);
    static status_t setBlcAreaLevel(char *, RemoteProxy*);
    static status_t getNoiseReductionMode(char *, RemoteProxy*);
    static status_t setNoiseReductionMode(char *, RemoteProxy*);
    static status_t getNoiseFilter(char *, RemoteProxy*);
    static status_t setNoiseFilter(char *, RemoteProxy*);
    static status_t getSpatialLevel(char *, RemoteProxy*);
    static status_t setSpatialLevel(char *, RemoteProxy*);
    static status_t getTemporalLevel(char *, RemoteProxy*);
    static status_t setTemporalLevel(char *, RemoteProxy*);
    static status_t getMirrorEffectMode(char *, RemoteProxy*);
    static status_t setMirrorEffectMode(char *, RemoteProxy*);
    static status_t getSceneMode(char *, RemoteProxy*);
    static status_t setSceneMode(char *, RemoteProxy*);
    static status_t getSensorMode(char *, RemoteProxy*);
    static status_t setSensorMode(char *, RemoteProxy*);
    static status_t getCaptureFps(char *, RemoteProxy*);
    static status_t setCaptureFps(char *, RemoteProxy*);
    static status_t getVideoStandard(char *, RemoteProxy*);
    static status_t setVideoStandard(char *, RemoteProxy*);
    static status_t getAdvanceFeature(char *, RemoteProxy*);
    static status_t setAdvanceFeature(char *, RemoteProxy*);
    static status_t getDebugLevel(char *, RemoteProxy*);
    static status_t setDebugLevel(char *, RemoteProxy*);

private:
    int cameraId;
    int needToRestart;
    EventControl *eventControl;

};
}
#endif

