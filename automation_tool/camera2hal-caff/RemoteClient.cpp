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

#include "RemoteClient.h"

namespace gstCaff {


#define RCMSGKEY 1048

struct rcmsgstru
{
    long msgtype;
    char msgtext[2048];
};

static char mVideo_caps[256] = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, depth=(string)8, colorimetry=(string)SMPTE240M" ;

static const clientsupportProperty propery[] =
{
   // {(char*)"cameraId", RemoteProxy::setCameraId}, //cameraId should be process first
    {(char*)"type", RemoteClient::setStreamType},
    {(char*)"resolution", RemoteClient::setResolution},
    {(char*)"format", RemoteClient::setFormat},
    {(char*)"interlace", RemoteClient::setInterlace},
    {(char*)"deinterlace", RemoteClient::setDeInterlaceMethod},
    {(char*)"state", RemoteClient::setCameraState},
    {(char*)"sourceElement", RemoteClient::setSrcElementType},
    {(char*)"convertElement", RemoteClient::setConvertElementType},
    {(char*)"sinkElement", RemoteClient::setSinkElementType},

    {NULL,NULL}
};


RemoteClient::RemoteClient()
{
    int err;
    m_ThreadID = 0;

    mConnected = 0;
    mPipeline = NULL;

    mRemote = 0;
    mNeedToRestart = 0;
    mSocket = 0;

    memset((void*)&mRCameraPropery ,0 ,sizeof(mRCameraPropery));

    err = pthread_create(&m_ThreadID, NULL, messageThreadLoop, this);
    if (err != 0)
        printf("can't create thread: %s\n", strerror(err));
    printf("main thread:");
}


RemoteClient::~RemoteClient()
{
    setCmd2RemoteClient(0, NULL, RCMSGTYPE_EXIT);
    pthread_join(m_ThreadID, NULL);
}

int RemoteClient::connectToServer(char* ip)
{
    int sockfd, sendbytes, num, ret;
    char buf[BUFFER_SIZE];
    struct hostent *host;
    struct sockaddr_in serv_addr;

    /*get ip from input*/
    host = gethostbyname(ip);
    if (host == NULL) {
        perror("gethostbyname");
        return ERROR;
    }

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr))== -1)
    {
        perror("connect");

    }

    /*if server has been connect with another client ,quit the connection now*/
    ret = testIsServerBusy(sockfd);
    if(ret == 0)
    {
        mConnected = 1;
        mSocket = sockfd;
    }
    return ret;
}




/* print the stats of a source */
void RemoteClient::print_source_stats (GObject * source) {
    GstStructure *stats;
    gchar *str;

    g_return_if_fail (source != NULL);

    /* get the source stats */
    g_object_get (source, "stats", &stats, NULL);

    /* simply dump the stats structure */
    str = gst_structure_to_string (stats);
    g_print ("source stats: %s\n", str);

    gst_structure_free (stats);
    g_free (str);
}

/* will be called when gstrtpbin signals on-ssrc-active. It means that an RTCP
* packet was received from another source. */
void RemoteClient::on_ssrc_active_cb (GstElement * rtpbin, guint sessid, guint ssrc, GstElement * depay)
{
    GObject *session, *isrc, *osrc;
    g_print ("got RTCP from session %u, SSRC %u\n", sessid, ssrc);

    /* get the right session */
    g_signal_emit_by_name (rtpbin, "get-internal-session", sessid, &session);

    /* get the internal source (the SSRC allocated to us, the receiver */
    g_object_get (session, "internal-source", &isrc, NULL);
    print_source_stats (isrc);

    /* get the remote source that sent us RTCP */
    g_signal_emit_by_name (session, "get-source-by-ssrc", ssrc, &osrc);
    print_source_stats (osrc);
}

/* will be called when rtpbin has validated a payload that we can depayload */
void RemoteClient::pad_added_cb (GstElement * rtpbin, GstPad * new_pad, GstElement * depay)
{
    GstPad *sinkpad;
    GstPadLinkReturn lres;

    g_print ("new payload on pad: %s\n", GST_PAD_NAME (new_pad));

    sinkpad = gst_element_get_static_pad (depay, "sink");
    if (!sinkpad) {
        g_print("make sinkpad error\n");
        return;
    }

    lres = gst_pad_link (new_pad, sinkpad);
    g_assert (lres == GST_PAD_LINK_OK);
    gst_object_unref (sinkpad);
}

int RemoteClient::stopPipeline( )
{
    printf("stopPipeline \n");
    if(mPipeline)
    {
        g_print ("stopping receiver pipeline\n");
        gst_element_set_state (mPipeline, GST_STATE_NULL);
        gst_object_unref (mPipeline);
    }
    mPipeline = NULL;
    return NO_ERROR;
}

int RemoteClient::getCapsStringfromProperty(char *VideoCapsTemp)
{
    int pos = 0;

    pos += sprintf(VideoCapsTemp + pos,"%s", mVideo_caps);

    pos += sprintf(VideoCapsTemp + pos,"%s", ", sampling=(string)" );
    if (0 == strcmp(mRCameraPropery.convertElementType, "videoconvert")) {
        pos += sprintf(VideoCapsTemp + pos,"%s", "YCbCr-4:2:0");
    } else {
        pos += sprintf(VideoCapsTemp + pos,"%s", "YCbCr-4:2:2" );
    }

    pos += sprintf(VideoCapsTemp + pos,"%s", ", width=(string)" );
    if (mRCameraPropery.width[0]!= 0) {
        pos += sprintf(VideoCapsTemp + pos,"%s", mRCameraPropery.width );
    } else {
        pos += sprintf(VideoCapsTemp + pos,"%s", "320" );
    }

    pos += sprintf(VideoCapsTemp + pos,"%s", ", height=(string)" );
    if (mRCameraPropery.height[0]!= 0) {
        pos += sprintf(VideoCapsTemp + pos,"%s", mRCameraPropery.height );
    } else {
        pos += sprintf(VideoCapsTemp + pos,"%s", "240" );
    }

    pos += sprintf(VideoCapsTemp + pos,"%s", ", format=(string)" );

    /*format has been changed in remote server,so do not change format here in client*/
    if (mRCameraPropery.format[0]!= 0 && 0) {
        pos += sprintf(VideoCapsTemp + pos,"%s", mRCameraPropery.format);
    } else {
        pos += sprintf(VideoCapsTemp + pos,"%s", "UYVY" );
    }

    return 0;
}

int RemoteClient::startClientPipeline()
{
    GstElement *rtpbin, *rtpsrc, *rtcpsrc, *rtcpsink;
    GstElement *videodepay,
        *videodec,
        *videoqueue,
        *videoconv,
        *videosink,
        *tee,
        *aviqueue,
        *aviconv,
        *avidenc,
        *avifmux,
        *avifilesink;

    GstElement *pipeline;
    GMainLoop *loop;
    GstCaps *caps;
    gboolean res1,res2;
    GstPadLinkReturn lres;
    GstPad *srcpad, *sinkpad;
    char VideoCapsTemp[256]= {0};

    printf("%s\n",__func__);

    /* always init first */
    gst_init (0, 0);

    /* the pipeline to hold everything */
    pipeline = gst_pipeline_new (NULL);
    if (!pipeline) {
        g_print("error to make the pipeline\n");
        return -1;
    }

    /* the udp src and source we will use for RTP and RTCP */
    rtpsrc = gst_element_factory_make ("udpsrc", "rtpsrc");
    if (!rtpsrc) {
        g_print("error to make rtpsrc \n");
        return -1;
    }
    g_object_set (rtpsrc, "port", UDPPORT, NULL);
    /* we need to set caps on the udpsrc for the RTP data */

    caps = gst_caps_from_string (VIDEO_CAPS);
    getCapsStringfromProperty(VideoCapsTemp);

    printf("VideoCapsTemp is %s \n", VideoCapsTemp);
    if(VideoCapsTemp[0] != 0)
        caps = gst_caps_from_string (VideoCapsTemp);
    g_object_set (rtpsrc, "caps", caps, NULL);
    gst_caps_unref (caps);

    rtcpsrc = gst_element_factory_make ("udpsrc", "rtcpsrc");
    if (!rtcpsrc) {
        g_print("error to make rtpsrc \n");
        return -1;
    }
    g_object_set (rtcpsrc, "port", 9997, NULL);

    rtcpsink = gst_element_factory_make ("udpsink", "rtcpsink");
    if (!rtcpsink) {
        g_print("error to make rtcpsink \n");
        return -1;
    }
    g_object_set (rtcpsink, "port", 9999, "host", DEST_HOST, NULL);
    /* no need for synchronisation or preroll on the RTCP sink */
    g_object_set (rtcpsink, "async", FALSE, "sync", FALSE, NULL);

    gst_bin_add_many (GST_BIN (pipeline), rtpsrc, rtcpsrc, rtcpsink, NULL);

    /* the depayloading and decoding */
    videodepay = gst_element_factory_make ("rtpvrawdepay", "videodepay");
    if (!videodepay) {
        g_print("error to make videodepay\n");
        return -1;
    }
    videoqueue=gst_element_factory_make ("queue","videoqueue");
    if (!videoqueue) {
        g_print("error to make videoqueue\n");
        return -1;
    }

    tee = gst_element_factory_make ("tee","tee");
    if (!tee) {
        g_print("error to make tee\n");
        return -1;
    }

    aviqueue=gst_element_factory_make ("queue","aviqueue");
    if (!aviqueue) {
        g_print("error to make aviqueue\n");
        return -1;
    }

    if (mRCameraPropery.sinkElementType[0] != 0)
        videosink = gst_element_factory_make (mRCameraPropery.sinkElementType, "videosink");
    else
        videosink = gst_element_factory_make ("xvimagesink", "videosink");
    if (!videosink) {
        g_print("error make videosink \n");
        return -1;
    }

    avifmux = gst_element_factory_make ("avimux","avifmux");
    if (!avifmux) {
        g_print("error to make avimux\n");
        return -1;
    }
    avifilesink = gst_element_factory_make ("filesink","avifilesink");
    if (!avifilesink) {
        g_print("error make the avifilesink\n");
        return -1;
    }
   // g_object_set(avifilesink,"location",AVINAME,NULL);

    /* add depayloading and playback to the pipeline and link */
    gst_bin_add_many (GST_BIN (pipeline), videodepay,tee,videoqueue,videosink, aviqueue,avifmux,NULL);

    res1 = gst_element_link_many (videodepay, tee,videoqueue,videosink, NULL);
    g_assert (res1 == TRUE);
   // res2 = gst_element_link_many (tee,aviqueue,avifmux,avifilesink,NULL);
   // g_assert (res2 == TRUE);

    /* the rtpbin element */
    rtpbin = gst_element_factory_make ("rtpbin", "rtpbin");
    if (!rtpbin){
        g_print("error make the rtpbin\n");
        return -1;
    }

    g_object_set (G_OBJECT (rtpbin),"latency",200,NULL);

    gst_bin_add (GST_BIN (pipeline), rtpbin);

    /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
    srcpad = gst_element_get_static_pad (rtpsrc, "src");
    sinkpad = gst_element_get_request_pad (rtpbin, "recv_rtp_sink_0");
    lres = gst_pad_link (srcpad, sinkpad);
    g_assert (lres == GST_PAD_LINK_OK);
    gst_object_unref (srcpad);

    /* get an RTCP sinkpad in session 0 */
    srcpad = gst_element_get_static_pad (rtcpsrc, "src");
    sinkpad = gst_element_get_request_pad (rtpbin, "recv_rtcp_sink_0");
    lres = gst_pad_link (srcpad, sinkpad);
    g_assert (lres == GST_PAD_LINK_OK);
    gst_object_unref (srcpad);
    gst_object_unref (sinkpad);

    /* get an RTCP srcpad for sending RTCP back to the sender */
    srcpad = gst_element_get_request_pad (rtpbin, "send_rtcp_src_0");
    sinkpad = gst_element_get_static_pad (rtcpsink, "sink");
    lres = gst_pad_link (srcpad, sinkpad);
    g_assert (lres == GST_PAD_LINK_OK);
    gst_object_unref (sinkpad);

    /* the RTP pad that we have to connect to the depayloader will be created
        * dynamically so we connect to the pad-added signal, pass the depayloader as
        * user_data so that we can link to it. */
    g_signal_connect (rtpbin, "pad-added", G_CALLBACK (RemoteClient::pad_added_cb), videodepay);

    /* give some stats when we receive RTCP */
    //g_signal_connect (rtpbin, "on-ssrc-active", G_CALLBACK (on_ssrc_active_cb),videodepay);


    gst_video_overlay_expose(GST_VIDEO_OVERLAY (videosink));
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (videosink), mRCameraPropery.videoWindow);



    /* set the pipeline to playing */
    g_print ("starting receiver pipeline\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    mPipeline = pipeline;

    printf("%s end \n",__func__);
    return 0;
}

int RemoteClient::setFormat(char * format, RemoteClient* pthis)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    printf("====%s %s \n",__func__, format);

    if( pthis->mRemote == 0)
        return 0;

    memcpy(pthis->mRCameraPropery.format, format ,sizeof(pthis->mRCameraPropery.format));
    pthis->mNeedToRestart++;
    return 0;
}

int RemoteClient::setCameraState(char * resPtr, RemoteClient* pthis)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    enum CAMERA_STATE temp;
    printf("====%s %s \n",__func__, resPtr);

    if( pthis->mRemote == 0)
        return 0;

    if (strcmp(resPtr,"play") == 0)
        temp = CAMERA_STATE_START;
    else if (strcmp(resPtr,"pause") == 0)
        temp = CAMERA_STATE_PAUSE;
    else if (strcmp(resPtr,"stop") == 0)
        temp = CAMERA_STATE_STOP;
    else
        return ERROR;

    if(pthis->mRCameraPropery.state == temp)
    {
        return 0;
    }
    pthis->mRCameraPropery.state = temp;

    if(temp == CAMERA_STATE_STOP)
    {
        pthis->stopPipeline();
    }

    if(temp == CAMERA_STATE_START)
    {
        pthis->startClientPipeline();
    }
    return 0;
}

int RemoteClient::setInterlace(char * resPtr, RemoteClient* pthis)
{
    bool mode = false;
    bool modeOld = pthis->mRCameraPropery.interlace_mode;
    printf("remoteClient %s %s \n",__func__, resPtr);

    if( pthis->mRemote == 0)
        return 0;

    if (strcmp(resPtr,"true") == 0 || strcmp(resPtr,"TRUE") == 0)
        mode = true;
    else if (strcmp(resPtr,"false") == 0 || strcmp(resPtr,"FALSE") == 0)
        mode = false;
    else
        return 0;

    if(modeOld == mode)
    {
        printf("the interlaceMode is same with before \n");
        return 0;
    }

    pthis->mRCameraPropery.interlace_mode = mode;
    pthis->mNeedToRestart++;
    return 0;
}

int RemoteClient::setDeInterlaceMethod(char * resPtr, RemoteClient* pthis)
{
    char *deInterlacePtr = resPtr;
    char *deInterlaceOld;

    printf("remoteproxy %s %s \n",__func__, resPtr);
    if( pthis->mRemote == 0)
        return 0;

    deInterlaceOld = pthis->mRCameraPropery.deInterlace;
    printf("deInterlaceOld is %s \n",deInterlaceOld);
    if(0 == strcmp(deInterlaceOld, deInterlacePtr))
    {
        return 0;
    }

    memcpy(pthis->mRCameraPropery.deInterlace, deInterlacePtr, sizeof(pthis->mRCameraPropery.deInterlace));
    pthis->mNeedToRestart++;

    return 0;
}

int RemoteClient::setStreamType(char * reso, RemoteClient* pthis)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    STREAM_TYPE streamType = STREAM_TYPE_NONE;
    printf("====%s %s \n",__func__, reso);

    if(strcmp(reso,"remote") == 0)
    {
        pthis->mRemote = 1;
        streamType = STREAM_TYPE_REMOTE;
        pthis->mNeedToRestart++;
    }

    if(strcmp(reso,"preview") == 0)
    {
        pthis->mRemote = 0;
        streamType = STREAM_TYPE_PREVIEW;
        pthis->stopPipeline();
    }

    if(pthis->mRCameraPropery.streamType != streamType)
    {
        printf("the streamType changed from %d to %d\n", pthis->mRCameraPropery.streamType, streamType);
        pthis->mRCameraPropery.streamType = streamType;
    }
    return 0;
}

int RemoteClient::setResolution(char * reso, RemoteClient* pthis)
{
    int width = 0, height = 0, pos = 0;
    char *token;
    printf("====%s %s \n",__func__, reso);

    if( pthis->mRemote == 0)
        return 0;
    token = strtok((char *)reso, "x");
    if (token != NULL) {
        width = atoi(token);
        memcpy(pthis->mRCameraPropery.width, token ,sizeof(pthis->mRCameraPropery.width));

    }
    token = strtok(NULL, "x");
    if (token != NULL) {
        height = atoi(token);
        memcpy(pthis->mRCameraPropery.height, token ,sizeof(pthis->mRCameraPropery.height));

    }

    printf("the resolution changed to %dx%d\n", width, height);
    pthis->mNeedToRestart++;
    return 0;
}

status_t RemoteClient::setSrcElementType(char *ptrType, RemoteClient *pthis)
{
    printf("RemoteClient %s %s \n",__func__, ptrType);
    pthis->mNeedToRestart++;
    return NO_ERROR;
}

status_t RemoteClient::setConvertElementType(char *ptrType, RemoteClient *pthis)
{
    printf("RemoteClient %s %s \n",__func__, ptrType);
    pthis->mNeedToRestart++;
    strcpy(pthis->mRCameraPropery.convertElementType, ptrType);
    return NO_ERROR;
}

status_t RemoteClient::setSinkElementType(char *ptrType, RemoteClient *pthis)
{
    printf("RemoteClient %s %s \n",__func__, ptrType);
    pthis->mNeedToRestart++;
    strcpy(pthis->mRCameraPropery.sinkElementType, ptrType);
    return NO_ERROR;
}

int RemoteClient::setVideoWindow(guintptr mEmbedXid)
{
    mRCameraPropery.videoWindow = mEmbedXid ;
    return 0;
}

int RemoteClient::setDrawingAreaPosition(GtkWidget* mDrawArea)
{
    mRCameraPropery.drawArea = mDrawArea ;
    return 0;
}

int RemoteClient::testIsServerBusy(int sockfd)
{
    char revbuf[BUFFER_SIZE];
    int num,sendbytes;
    const char *serverBusy = "IsServerBusy?";

    if ((sendbytes = send(sockfd, "IsServerBusy?", (strlen(serverBusy) + 1), 0)) == -1)
    {
        perror("send");
        return -1;
    }

    if((num=recv(sockfd,revbuf,1024,0)) == -1){
        printf("recv() error\n");
        return -1;
    }
    revbuf[num-1]='\0';
    printf("Server Message: %s\n",revbuf);

    if(strstr(revbuf,"busy"))
    {
        close(sockfd);
        printf("remote server is busy \n");
        return 1;
    }
    return 0;
}

int RemoteClient::processCommandInClient(char *buf)
{
    char *s = NULL;
    int i = 0;
    char temp[1024] = {0};
    printf("processCommandInClient \n");

    while(propery[i].keyString != NULL)
    {
        s = strstr(buf, propery[i].keyString);
        if(s != NULL)
        {
            printf("process cmd: find str in command %s\n", s+ strlen(propery[i].keyString)+1);
            strcpy(temp,s+ strlen(propery[i].keyString)+1);
            propery[i].process(strtok(temp,","),this);
        }
        i++;
    }

    if (mNeedToRestart && mRCameraPropery.state != CAMERA_STATE_STOP) {
        stopPipeline();
        usleep(300000);
        startClientPipeline();
        mNeedToRestart = 0;
    }

    return NO_ERROR;
}

status_t RemoteClient::setCmd2RemoteClient(int camId, char* ptr, RCMSGTYPE type)
{
    printf("%s\n",__func__);

    struct rcmsgstru msgs;
    int msg_type;
    char str[256];
    int msqid,ret_value;

    msqid=msgget(RCMSGKEY,IPC_EXCL);/*get the queue id by MSGKEY*/
    if (msqid < 0) {
        return -1;
    }

    printf("sent message %s to %d \n", ptr, msqid);
    msgs.msgtype = type;
    if (NULL != ptr)
    {
        memcpy(msgs.msgtext, ptr, (strlen(ptr) + 1));
    }
    /*send msg to the queue */
    ret_value = msgsnd(msqid,&msgs,sizeof(struct rcmsgstru), 0);
    if (ret_value < 0) {
        printf("msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
        return -1;
    }
    return 0;
}


void* RemoteClient::messageThreadLoop(void *ptr)
{
    struct rcmsgstru msgs;
    int msqid,ret_value, num;
    char revbuf[BUFFER_SIZE];
    printf("RemoteClient @%s: Start\n", __FUNCTION__);
    PipelineStatus *status = NULL;
    RemoteClient* pthis = (RemoteClient*)ptr;

    msqid=msgget(RCMSGKEY,IPC_EXCL);  /*get the queue id by MSGKEY*/
    if (msqid < 0) {
        msqid = msgget(RCMSGKEY,IPC_CREAT|0666);/*create the queue with MSGKEY*/
        if (msqid <0) {
            printf("failed to create msq | errno=%d [%s]\n",errno,strerror(errno));
            return NULL;
        }
        printf("create message que  %d \n",msqid);
    }
    
    bool threadRunning = true; 

    while(threadRunning) {
        /*receive from the queue*/
        ret_value = msgrcv(msqid,&msgs,sizeof(struct rcmsgstru),0,0);
        char *buf = msgs.msgtext;

        switch (msgs.msgtype) {
            case RCMSGTYPE_COMMAND_STRING:
                printf("received RCMSGTYPE_COMMAND_STRING %s \n\r", buf);

                if(pthis->mConnected)
                {
                    pthis->stopPipeline();
                    /*send command buf to server*/
                    if (send(pthis->mSocket, buf, strlen(buf), 0) == -1)
                    {
                        perror("send");
                    }

                    pthis->processCommandInClient(buf);
                    if((num=recv(pthis->mSocket,revbuf,1024,0)) == -1){
                        printf("recv() error\n");
                        return NULL;
                    }
                    revbuf[num-1]='\0';
                    printf("Server Message: %s\n",revbuf);
                }
                break;
            case RCMSGTYPE_EXIT:
                threadRunning = false;
                break;
            default:
                break;
        }
    }

    printf("msq deleting \n\r ");
    if ((msgctl( msqid, IPC_RMID, NULL)) < 0) { /* delete the message queue*/
        perror ("msgctl");
        return NULL;
    }
    return NULL;
}

}

