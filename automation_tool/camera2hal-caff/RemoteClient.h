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
/*client*/
#ifndef _REMOTE_CLIENT_H_
#define _REMOTE_CLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <gst/gst.h>
#include "PipelineManage.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>

//#include "CameraProperty.h"
#include <sys/msg.h>

#define PORT    4321
#define BUFFER_SIZE 1024

using namespace std ;


/* the caps of the sender RTP stream. This is usually negotiated out f band with
 * SDP or RTSP. */
#define VIDEO_CAPS "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:0, depth=(string)8, width=(string)320, height=(string)240, colorimetry=(string)SMPTE240M"


#define AVINAME "camera.avi"
#define UDPPORT 9996

/* the destination machine to send RTCP to. This is the address of the sender and
 * is used to send back the RTCP reports of this receiver. If the data is sent
 * from another machine, change this address. */
#define DEST_HOST "127.0.0.1"

enum RCMSGTYPE {
RCMSGTYPE_COMMAND_STRING = 1, /*can not be started with 0 ,for msgsnd not happy*/
RCMSGTYPE_EXIT,
RCMSGTYPE_MAX
};



namespace gstCaff {
class RemoteClient;

struct clientsupportProperty{
   char *keyString;
   status_t(*process)(char *,RemoteClient*);
};

struct CameraProperty {
    int camId;
    GstElement* activePipeline;
    char width[16];
    char height[16];
    int end;
    bool interlace_mode;
    char format[16];   /*NV12 or YUY2*/
    char deInterlace[16];  /*sw or hw intelace*/
    char srcElementType[64];/*Element type name specified by user */
    char convertElementType[64];
    char sinkElementType[64];
    guintptr videoWindow;
    GtkWidget *drawArea;

    /*below is for player and recording mode*/
    char open_file_path[256];
    char save_file_name[256];
    int remoteIp; /*ip address for remote display*/
    STREAM_TYPE streamType;
    CAMERA_STATE state;
};





/**
 * class RemoteProxy
 */
class RemoteClient {
public:
    RemoteClient();
    ~RemoteClient();
    status_t connectToServer(char* ip);
    status_t setCmd2RemoteClient(int camId, char* ptr, RCMSGTYPE type=RCMSGTYPE_COMMAND_STRING);


    /* print the stats of a source */
    static void* messageThreadLoop(void *);
    static void print_source_stats (GObject * source) ;
    static void on_ssrc_active_cb (GstElement * rtpbin, guint sessid, guint ssrc, GstElement * depay);
    static void pad_added_cb (GstElement * rtpbin, GstPad * new_pad, GstElement * depay);
    int stopPipeline( );
    int getCapsStringfromProperty(char *VideoCapsTemp);
    int startClientPipeline();
    static int setResolution(char * reso,RemoteClient*);
    static int setFormat(char * format,RemoteClient*);
    static int setInterlace(char * format,RemoteClient*);
    static int setDeInterlaceMethod(char * format,RemoteClient*);
    static int setStreamType(char * format,RemoteClient*);
    static int setCameraState(char * format,RemoteClient*);
    static status_t setSrcElementType(char *, RemoteClient*);
    static status_t setConvertElementType(char *, RemoteClient*);
    static status_t setSinkElementType(char *, RemoteClient*);
    static status_t setCaptureNum(char *, RemoteClient*);
    static status_t getIOMode(char *, RemoteClient*);
    static status_t setIoMode(char *, RemoteClient*);
    static status_t getIrisMode(char *, RemoteClient*);
    static status_t setIrisMode(char *, RemoteClient*);
    static status_t getIrisLevel(char *, RemoteClient*);
    static status_t setIrisLevel(char *, RemoteClient*);
    static status_t getExposureTime(char *, RemoteClient*);
    static status_t setExposureTime(char *, RemoteClient*);
    static status_t getExposureGain(char *, RemoteClient*);
    static status_t setExposureGain(char *, RemoteClient*);
    static status_t getExposureEv(char *, RemoteClient*);
    static status_t setExposureEv(char *, RemoteClient*);
    static status_t getAeWindowSize(char *, RemoteClient*);
    static status_t setAeWindowSize(char *, RemoteClient*);
    static status_t getAwbMode(char *, RemoteClient*);
    static status_t setAwbMode(char *, RemoteClient*);
    static status_t getAwbRGain(char *, RemoteClient*);
    static status_t setAwbRGain(char *, RemoteClient*);
    static status_t getAwbGGain(char *, RemoteClient*);
    static status_t setAwbGGain(char *, RemoteClient*);
    static status_t getAwbBGain(char *, RemoteClient*);
    static status_t setAwbBGain(char *, RemoteClient*);
    static status_t getAwbRange(char *, RemoteClient*);
    static status_t setAwbRange(char *, RemoteClient*);
    static status_t getAwbWindowSize(char *, RemoteClient*);
    static status_t setAwbWindowSize(char *, RemoteClient*);
    static status_t getDayNightMode(char *, RemoteClient*);
    static status_t setDayNightMode(char *, RemoteClient*);
    static status_t getSharpness(char *, RemoteClient*);
    static status_t setSharpness(char *, RemoteClient*);
    static status_t getBrightness(char *, RemoteClient*);
    static status_t setBrightness(char *, RemoteClient*);
    static status_t getContrast(char *, RemoteClient*);
    static status_t setContrast(char *, RemoteClient*);
    static status_t getHue(char *, RemoteClient*);
    static status_t setHue(char *, RemoteClient*);
    static status_t getSaturation(char *, RemoteClient*);
    static status_t setSaturation(char *, RemoteClient*);
    static status_t getWdrMode(char *, RemoteClient*);
    static status_t setWdrMode(char *, RemoteClient*);
    static status_t getBlcMode(char *, RemoteClient*);
    static status_t setBlcMode(char *, RemoteClient*);
    static status_t getBlcAreaLevel(char *, RemoteClient*);
    static status_t setBlcAreaLevel(char *, RemoteClient*);
    static status_t getNoiseReductionMode(char *, RemoteClient*);
    static status_t setNoiseReductionMode(char *, RemoteClient*);
    static status_t getNoiseFilter(char *, RemoteClient*);
    static status_t setNoiseFilter(char *, RemoteClient*);
    static status_t getSpatialLevel(char *, RemoteClient*);
    static status_t setSpatialLevel(char *, RemoteClient*);
    static status_t getTemporalLevel(char *, RemoteClient*);
    static status_t getMirrorEffectMode(char *, RemoteClient*);
    static status_t setMirrorEffectMode(char *, RemoteClient*);
    static status_t getSceneMode(char *, RemoteClient*);
    static status_t setSceneMode(char *, RemoteClient*);
    static status_t getSensorMode(char *, RemoteClient*);
    static status_t setSensorMode(char *, RemoteClient*);
    static status_t getCaptureFps(char *, RemoteClient*);
    static status_t setCaptureFps(char *, RemoteClient*);
    static status_t getVideoStandard(char *, RemoteClient*);
    static status_t setVideoStandard(char *, RemoteClient*);
    static status_t getAdvanceFeature(char *, RemoteClient*);
    static status_t setAdvanceFeature(char *, RemoteClient*);
    static status_t getDebugLevel(char *, RemoteClient*);
    static status_t setDebugLevel(char *, RemoteClient*);
    int setVideoWindow(guintptr mEmbedXid);
    int setDrawingAreaPosition(GtkWidget *mDrawArea);
    int testIsServerBusy(int sockfd);
    int processCommandInClient(char *buf);
public:
int mConnected;

private:
    pthread_t m_ThreadID;
    struct CameraProperty mRCameraPropery;
    int mRemote;
    int mNeedToRestart;
    GstElement *mPipeline;
    int mSocket;

};
}
#endif


