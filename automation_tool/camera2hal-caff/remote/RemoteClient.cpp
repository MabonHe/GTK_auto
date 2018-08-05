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


#define PORT    4321
#define BUFFER_SIZE 1024

using namespace std ;

/* the caps of the sender RTP stream. This is usually negotiated out f band with
 * SDP or RTSP. */
#define VIDEO_CAPS "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:0, depth=(string)8, width=(string)320, height=(string)240, colorimetry=(string)SMPTE240M"
char mVideo_caps[256] = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:0, depth=(string)8, colorimetry=(string)SMPTE240M" ;

int mRemote = 0;
int mNeedToRestart = 0;
GstElement *mPipeline = NULL;


#define AVINAME "camera.avi"
#define UDPPORT 9996
#define VIDEO_SINK  "autovideosink"
/* the destination machine to send RTCP to. This is the address of the sender and
 * is used to send back the RTCP reports of this receiver. If the data is sent
 * from another machine, change this address. */
#define DEST_HOST "127.0.0.1"

enum ERRROR_ID{
    NO_ERROR = 0,
    ERROR = -1
};


enum CAMERA_STATE{
    CAMERA_STATE_STOP, /*fe*/
    CAMERA_STATE_START,
    CAMERA_STATE_PAUSE
};


enum STREAM_TYPE{
    STREAM_TYPE_NONE,
    STREAM_TYPE_PREVIEW,
    STREAM_TYPE_RECORDING,
    STREAM_TYPE_PLAYER,
    STREAM_TYPE_REMOTE  /*send src to remote for display in udp*/
};


struct supportProperty{
   char *keyString;
   int(*process)(char *);
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

    /*below is for player and recording mode*/
    char open_file_path[256];
    char save_file_name[256];
    int remoteIp; /*ip address for remote display*/
    STREAM_TYPE streamType;
    CAMERA_STATE state;
};

struct CameraProperty mRCameraPropery;

int setResolution(char * reso);
int setFormat(char * format);
int setInterlace(char * format);
int setDeInterlaceMethod(char * format);
int setStreamType(char * format);
int setCameraState(char * format);

static const supportProperty propery[] =
{
   // {(char*)"cameraId", RemoteProxy::setCameraId}, //cameraId should be process first
    {(char*)"type", setStreamType},
    {(char*)"resolution", setResolution},
    {(char*)"format", setFormat},
    {(char*)"interlace", setInterlace},
    {(char*)"deinterlace", setDeInterlaceMethod},
    {(char*)"state", setCameraState},

    {NULL,NULL}
};


/* print the stats of a source */
static void print_source_stats (GObject * source) {
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
static void on_ssrc_active_cb (GstElement * rtpbin, guint sessid, guint ssrc, GstElement * depay)
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
static void
pad_added_cb (GstElement * rtpbin, GstPad * new_pad, GstElement * depay)
{
    GstPad *sinkpad;
    GstPadLinkReturn lres;

    g_print ("new payload on pad: %s\n", GST_PAD_NAME (new_pad));

    sinkpad = gst_element_get_static_pad (depay, "sink");
    g_assert (sinkpad);

    lres = gst_pad_link (new_pad, sinkpad);
    g_assert (lres == GST_PAD_LINK_OK);
    gst_object_unref (sinkpad);
}

int stopPipeline( )
{
    printf("stopPipeline \n");
    if(mPipeline)
    {
        g_print ("stopping receiver pipeline\n");
        gst_element_set_state (mPipeline, GST_STATE_NULL);
        gst_object_unref (mPipeline);
    }
    mPipeline = NULL;
}

int getCapsStringfromProperty(char *VideoCapsTemp)
{
    int pos = 0;

    pos += sprintf(VideoCapsTemp + pos,"%s", mVideo_caps);
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

    if (mRCameraPropery.format[0]!= 0) {
        pos += sprintf(VideoCapsTemp + pos,"%s", mRCameraPropery.format);
    } else {
        pos += sprintf(VideoCapsTemp + pos,"%s", "I420" );
    }
    return 0;
}

int startClientPipeline ()
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

    /* always init first */
    gst_init (0, 0);

    /* the pipeline to hold everything */
    pipeline = gst_pipeline_new (NULL);
    g_assert (pipeline);

    /* the udp src and source we will use for RTP and RTCP */
    rtpsrc = gst_element_factory_make ("udpsrc", "rtpsrc");
    g_assert (rtpsrc);
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
    g_assert (rtcpsrc);
    g_object_set (rtcpsrc, "port", 9997, NULL);

    rtcpsink = gst_element_factory_make ("udpsink", "rtcpsink");
    g_assert (rtcpsink);
    g_object_set (rtcpsink, "port", 9999, "host", DEST_HOST, NULL);
    /* no need for synchronisation or preroll on the RTCP sink */
    g_object_set (rtcpsink, "async", FALSE, "sync", FALSE, NULL);

    gst_bin_add_many (GST_BIN (pipeline), rtpsrc, rtcpsrc, rtcpsink, NULL);

    /* the depayloading and decoding */
    videodepay = gst_element_factory_make ("rtpvrawdepay", "videodepay");
    g_assert (videodepay);
    videoqueue=gst_element_factory_make ("queue","videoqueue");
    g_assert(videoqueue);

    tee = gst_element_factory_make ("tee","tee");
    g_assert(tee);

    aviqueue=gst_element_factory_make ("queue","aviqueue");
    g_assert(aviqueue);

    videosink = gst_element_factory_make (VIDEO_SINK, "videosink");
    g_assert (videosink);

    avifmux = gst_element_factory_make ("avimux","avifmux");
    g_assert (avifmux);

    /* add depayloading and playback to the pipeline and link */
    gst_bin_add_many (GST_BIN (pipeline), videodepay,tee,videoqueue,videosink, aviqueue,avifmux,NULL);

    res1 = gst_element_link_many (videodepay, tee,videoqueue,videosink, NULL);
    g_assert (res1 == TRUE);

    /* the rtpbin element */
    rtpbin = gst_element_factory_make ("rtpbin", "rtpbin");
    g_assert (rtpbin);

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
    g_signal_connect (rtpbin, "pad-added", G_CALLBACK (pad_added_cb), videodepay);

    /* give some stats when we receive RTCP */
    //g_signal_connect (rtpbin, "on-ssrc-active", G_CALLBACK (on_ssrc_active_cb),videodepay);

    /* set the pipeline to playing */
    g_print ("starting receiver pipeline\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    mPipeline = pipeline;

    printf("%s end \n",__func__);
    return 0;
}

int setFormat(char * format)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    printf("====%s %s \n",__func__, format);

    if( mRemote == 0)
        return 0;

    memcpy(mRCameraPropery.format, format ,sizeof(mRCameraPropery.format));
    mNeedToRestart++;
    return 0;
}

int setCameraState(char * resPtr)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    enum CAMERA_STATE temp;
    printf("====%s %s \n",__func__, resPtr);

    if( mRemote == 0)
        return 0;

    if (strcmp(resPtr,"play") == 0)
        temp = CAMERA_STATE_START;
    else if (strcmp(resPtr,"pause") == 0)
        temp = CAMERA_STATE_PAUSE;
    else if (strcmp(resPtr,"stop") == 0)
        temp = CAMERA_STATE_STOP;
    else
        return ERROR;

    if(mRCameraPropery.state == temp)
    {
        return 0;
    }
    mRCameraPropery.state = temp;

    if(temp == CAMERA_STATE_STOP)
    {
        stopPipeline();
    }

    if(temp == CAMERA_STATE_START)
    {
        startClientPipeline();
    }
    return 0;
}

int setInterlace(char * resPtr)
{
    bool mode = false;
    bool modeOld = mRCameraPropery.interlace_mode;
    printf("remoteClient %s %s \n",__func__, resPtr);

    if( mRemote == 0)
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

    mRCameraPropery.interlace_mode = mode;
    mNeedToRestart++;
    return 0;
}

int setDeInterlaceMethod(char * resPtr)
{
    char *deInterlacePtr = resPtr;
    char *deInterlaceOld;

    printf("remoteproxy %s %s \n",__func__, resPtr);
    if( mRemote == 0)
        return 0;

    deInterlaceOld = mRCameraPropery.deInterlace;
    printf("deInterlaceOld is %s \n",deInterlaceOld);
    if(0 == strcmp(deInterlaceOld, deInterlacePtr))
    {
        return 0;
    }

    memcpy(mRCameraPropery.deInterlace, deInterlacePtr, sizeof(mRCameraPropery.deInterlace));
    mNeedToRestart++;

    return 0;
}

int setStreamType(char * reso)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    STREAM_TYPE streamType;
    printf("====%s %s \n",__func__, reso);

    if(strcmp(reso,"remote") == 0)
    {
        mRemote = 1;
        streamType = STREAM_TYPE_REMOTE;
        mNeedToRestart++;
    }

    if(strcmp(reso,"preview") == 0)
    {
        mRemote = 0;
        streamType = STREAM_TYPE_PREVIEW;
        stopPipeline();
    }

    if(mRCameraPropery.streamType != streamType)
    {
        printf("the streamType changed from %d to %d\n", mRCameraPropery.streamType, streamType);
        mRCameraPropery.streamType = streamType;
    }
    return 0;
}

int setResolution(char * reso)
{
    int width, height, pos = 0;
    int widthOld,heightOld;
    char *token;
    printf("====%s %s \n",__func__, reso);

    if( mRemote == 0)
        return 0;
    token = strtok((char *)reso, "x");
    if (token != NULL) {
        width = atoi(token);
        memcpy(mRCameraPropery.width, token ,sizeof(mRCameraPropery.width));

    }
    token = strtok(NULL, "x");
    if (token != NULL) {
        height = atoi(token);
        memcpy(mRCameraPropery.height, token ,sizeof(mRCameraPropery.height));

    }

    printf("the resolution changed to %dx%d\n", width, height);
    mNeedToRestart++;
    return 0;
}

int testIsServerBusy(int sockfd)
{
    char revbuf[BUFFER_SIZE];
    int num,sendbytes;

    if ((sendbytes = send(sockfd, "IsServerBusy?", 64, 0)) == -1)
    {
        perror("send");
        exit(1);
    }

    if((num=recv(sockfd,revbuf,1024,0)) == -1){
        printf("recv() error\n");
        exit(1);
    }
    revbuf[num-1]='\0';
    printf("Server Message: %s\n",revbuf);

    if(strstr(revbuf,"busy"))
    {
        close(sockfd);
        exit(0);
    }
}

int processCommandInClient(char *buf)
{
    char *s = NULL;
    int i = 0;
    char temp[1024] = {0};
    printf("processCommandInClient \n");

    while(propery[i].keyString != NULL)
    {
        printf("process keystring %s \n",propery[i].keyString );
        s = strstr(buf, propery[i].keyString);
        if(s != NULL)
        {
            printf("find str in command %s\n", s+ strlen(propery[i].keyString)+1);
            strcpy(temp,s+ strlen(propery[i].keyString)+1);
            propery[i].process(strtok(temp,","));
        }
        i++;
    }

    if (mNeedToRestart)
    {
        stopPipeline();
        startClientPipeline();
        mNeedToRestart = 0;
    }
}

int main(int argc, char *argv[])
{
    int sockfd, sendbytes, num;
    char buf[BUFFER_SIZE];
    struct hostent *host;
    struct sockaddr_in serv_addr;

    if(argc < 2) {
        cout<<"please specify server's ip address"<<endl;
        return -1;
    }
    /*get ip from input*/
    if ((host = gethostbyname(argv[1])) == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
    perror("socket");
    exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr))== -1)
    {
        perror("connect");
        exit(1);
    }

    /*if server has been connect with another client ,quit the connection now*/
    testIsServerBusy(sockfd);

    if(argc < 3)
    {
        /*type in ./tcpclient mode */
        printf("setting the caff in remote, input '--help' for help \n");

        memset(buf, 0, sizeof(buf));
        while(scanf("%s", (char*)&buf) != EOF){
            if(strcmp(buf,"--help") == 0)
            {
                int x = 0;
                cout<<"properties support lists as below, can input them in one line,use ',' to seperate, no space needed\n";
                cout<<"for example: cameraId=0;state=play,resolution=320x240\n";
                cout<<"\n ";
                cout<<"cameraId  \n ";
                cout<<"state \n ";
                cout<<"resolution  \n ";
                cout<<"format  \n ";
                cout<<"interlace \n ";
                cout<<"deinterlace \n ";
                cout<<"state"<<"\n";
                cin>>x;
                cout<<"input is"<<x;
            }
            printf("send buf to server %s \n",buf);
            if ((sendbytes = send(sockfd, buf, strlen(buf), 0)) == -1)
            {
                perror("send");
                exit(1);
            }

            processCommandInClient(buf);
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", argv[2]);

        /*send command buf to server*/
        if ((sendbytes = send(sockfd, buf, strlen(buf), 0)) == -1)
        {
            perror("send");
            exit(1);
        }

        processCommandInClient(buf);
        /*sleep 1 second for holding on the pipeline in the client , when the cmd is finished,
                the pipeline will be destroyed .*/
        if(mPipeline)
            sleep(1);
    }

    close(sockfd);
    exit(0);
}

