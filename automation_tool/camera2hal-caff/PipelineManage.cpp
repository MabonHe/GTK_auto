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
#include "PipelineManage.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ENABLE_AUTOMATION_TEST

#ifdef ENABLE_AUTOMATION_TEST
#include "gstCaffAutoTestSampleCallback.h"
#endif

#define HOST_IP "127.0.0.1"
#define UDPPORT 9996
#define Video_Caps "video/x-raw,format=YUY2,width=320,height=240"

namespace gstCaff {

int pluginNum = 0;
struct Plugin g_plugin[MAX_PLUGIN_SIZE];
static GstElement *g_elements[MAX_PLUGIN_SIZE];
PipelineManage * PipelineManage::sInstance = NULL;

PipelineManage* PipelineManage::getInstance()
{
    if (!sInstance) {
        sInstance = new PipelineManage();
    }
    return sInstance;
}

PipelineManage::PipelineManage()
{

}

PipelineManage::~PipelineManage()
{

}

void PipelineManage::clearPlugins(void)
{
    memset(g_plugin, 0, (MAX_PLUGIN_SIZE * sizeof(struct Plugin)));
    return;
}

int PipelineManage::addPlugins(const char *plugin_name, const char *pro_attrs, const char *caps_filter)
{
    int i;
    for (i = 0; i < MAX_PLUGIN_SIZE; i++) {
        if (!g_plugin[i].plugin_name) {
            g_plugin[i].plugin_name = plugin_name;
            g_plugin[i].pro_attrs = pro_attrs;
            g_plugin[i].caps_filter = caps_filter;
            break;
        }
    }
    if (i == MAX_PLUGIN_SIZE) {
        g_print("the plugin queue is full, add failed\n");
        return -1;
    }

    return 0;
}

int PipelineManage::getFileNameFromTime(char *fileName)
{
    time_t t;
    struct tm *local;
    t=time(NULL);
    local=localtime(&t);
    strftime(fileName, 64,"./VID-%F-%H-%M-%S.avi",local);
    printf("fileName is: %s\n", fileName);
    return 0;
}

status_t PipelineManage::init()
{
    int i = 0;
    for(i = 0; i < MAX_CAMERA_NUM; i++) {
        memset((void*)&pipelineStatus[i],0,sizeof(pipelineStatus[i]));
        pipelineStatus[i].activePipeline = NULL;
        pipelineStatus[i].videoWindow = 0;
        pipelineStatus[i].drawArea = NULL;
        printf("sizeof(pipelineStatus) is %lu\n",sizeof(pipelineStatus[i]));
    }

    return NO_ERROR;
}

int PipelineManage::pipelineStart(int camId)
{
    printf("%s\n",__func__);
    PipelineStatus *status = &pipelineStatus[camId];

    if (status->stream_type == STREAM_TYPE_PREVIEW) {
        return pipelinePreviewStart(camId);
    }
    if (status->stream_type == STREAM_TYPE_PLAYER) {
        return pipelinePlayerStart(camId);
    }
    if (status->stream_type == STREAM_TYPE_RECORDING) {
        return pipelineRecordingStart(camId);
    }
    if (status->stream_type == STREAM_TYPE_REMOTE) {
        return pipelineRemoteStart(camId);
    }
}

int PipelineManage::pipelineStop(int camId)
{
    printf("%s camId %d\n",__func__,camId);
    if (pipelineStatus[camId].activePipeline) {
        printf("%s \n", __func__);
        printf("gst_element_set_state %p NULL  \n", pipelineStatus[camId].activePipeline);
        gst_element_set_state (pipelineStatus[camId].activePipeline, GST_STATE_NULL);
        printf("gst_object_unref %p  \n", pipelineStatus[camId].activePipeline);
        gst_object_unref (pipelineStatus[camId].activePipeline);
        pipelineStatus[camId].activePipeline = NULL;
        pipelineStatus[camId].state = CAMERA_STATE_STOP;
    }
    else {
        printf("no pipeline to stop \n");
    }
    return NO_ERROR;
}

int PipelineManage::pipelinePause(int camId)
{
    printf("%s\n",__func__);
    int ret;
    if (pipelineStatus[camId].activePipeline) {
        ret = gst_element_set_state(pipelineStatus[camId].activePipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            g_printerr ("Unable to set the pipeline to the playing state.\n");
            //gst_object_unref (activePipeline[camId]);
            return -1;
        }
        pipelineStatus[camId].state = CAMERA_STATE_PAUSE;
    }
    else {
        printf("no pipeline to pause\n");
        return -1;
    }
    return ret;
}

status_t PipelineManage::pipelineSetState(int camId, CAMERA_STATE state)
{
    int ret;
    switch(state)
    {
        case CAMERA_STATE_START:
            pipelineStart(camId);
            break;
        case CAMERA_STATE_PAUSE:
            pipelinePause(camId);
            break;
        case CAMERA_STATE_STOP:
            pipelineStop(camId);
            break;
        default:
            break;
    }

    return NO_ERROR;
}


status_t PipelineManage::getCameraState(int camId , CAMERA_STATE &state)
{
    state = pipelineStatus[camId].state;
    return NO_ERROR;
}


status_t PipelineManage::setVideoWindow(int camId, guintptr mEmbedXid)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].videoWindow = mEmbedXid;
        return NO_ERROR;
    } else
        return -1;
}

status_t PipelineManage::setDrawingAreaPosition(int camId, GtkWidget* mDrawArea)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].drawArea = mDrawArea ;
        return NO_ERROR;
    }
    else
        return -1;
}

gboolean PipelineManage::bus_call(GstBus *bus,GstMessage *msg, gpointer userData)
{
    PipelineStatus *data = (PipelineStatus *)userData;

    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_print("End of stream, cameraId: %d\n", data->camId);
            gst_element_set_state ((GstElement *)(data->activePipeline), GST_STATE_READY);
            getInstance()->notifyEOF(data->camId);
            break;
        case GST_MESSAGE_ERROR:
            {
                gchar *debug;
                GError *error;
                gst_message_parse_error(msg,&error,&debug);
                g_free(debug);
                g_printerr("ERROR:%s\n",error->message);
                g_error_free(error);
                gst_element_set_state (data->activePipeline, GST_STATE_READY);
                break;
            }
        case GST_MESSAGE_STATE_CHANGED:
            {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->activePipeline)) {
                    g_print ("State set to %s  %lu\n",
                            gst_element_state_get_name (new_state),pthread_self());
                }
            }
        default:
            break;
    }
    return TRUE;
}


void PipelineManage::pipelineConnectWatch(GstElement *pipeline, int camId)
{
    GstBus *bus;
    PipelineStatus *status = &pipelineStatus[camId];
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    g_print ("pipelineConnectWatch reached. %lu\n",pthread_self());
    gst_bus_add_watch(bus, bus_call, status);
    gst_object_unref(bus);
}

status_t PipelineManage::setReso(int camId ,int width, int height)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].width = width;
        pipelineStatus[camId].height = height;
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::setIoMode(int camId, char *IoModePtr)
{
    if (camId < MAX_PLUGIN_SIZE) {
        strcpy(pipelineStatus[camId].ioMode, IoModePtr);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::setFormat(int camId, char *format)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(pipelineStatus[camId].format,format);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::setFramerate(int camId, char *framerate)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(pipelineStatus[camId].framerate,framerate);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::setInterlaceMode(int camId, bool mode)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].interlace_mode = mode;
        return NO_ERROR;
    }
    return ERROR;
}
status_t PipelineManage::setVcEnable(int camId, bool enable)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].vcEnable = enable;
        return NO_ERROR;
    }
    return ERROR;
}


status_t PipelineManage::setDeInterlace(int camId, char *type)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(pipelineStatus[camId].deInterlace,type);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::setDeviceName(int camId, char *type)
{
    if (camId < MAX_CAMERA_NUM) {
        if(NULL != type)
            strcpy(pipelineStatus[camId].deviceName,type);
        else
            pipelineStatus[camId].deviceName[0] = 0;
        return NO_ERROR;
    }

    return ERROR;
}

status_t PipelineManage::setStreamType(int camId, STREAM_TYPE type)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].stream_type = type;
        return NO_ERROR;
    }
    return ERROR;
}


status_t PipelineManage::setSrcElementType(int camId, char *type)
{
    if (camId < MAX_CAMERA_NUM) {
        if(NULL != type)
            strcpy(pipelineStatus[camId].srcElementType,type);
        else
            pipelineStatus[camId].srcElementType[0] = 0;
        return NO_ERROR;
    }
    return ERROR;

}
status_t PipelineManage::setConvertElementType(int camId, char *type)
{
    if (camId < MAX_CAMERA_NUM) {
        if(NULL != type)
            strcpy(pipelineStatus[camId].convertElementType,type);
        else
            pipelineStatus[camId].convertElementType[0] = 0;
        return NO_ERROR;
    }
    return ERROR;

}
status_t PipelineManage::setSinkElementType(int camId, char *type)
{
    if (camId < MAX_CAMERA_NUM) {
        if(NULL != type)
            strcpy(pipelineStatus[camId].sinkElementType,type);
        else
            pipelineStatus[camId].sinkElementType[0] = 0;
        return NO_ERROR;
    }
    return ERROR;
}


status_t PipelineManage::setRemoteIp(int camId, int ip)
{
    if (camId < MAX_CAMERA_NUM) {
        pipelineStatus[camId].remoteIp = ip;
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::getRemoteIp(int camId, int &ip)
{
    if (camId < MAX_CAMERA_NUM) {
        ip = pipelineStatus[camId].remoteIp;
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::getReso(int camId, int &width, int &height)
{
    if (camId < MAX_CAMERA_NUM) {
        width = pipelineStatus[camId].width;
        height = pipelineStatus[camId].height;
        printf("PipelineManage::getReso w %d  h%d \n", width, height);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::getIOMode(int camId, char *IoModeOld)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(IoModeOld, pipelineStatus[camId].ioMode);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::getFormat(int camId, char *format)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(format, pipelineStatus[camId].format);
        return NO_ERROR;
    }
    return ERROR;
}


status_t PipelineManage::getInterlaceMode(int camId, bool &mode)
{
    if (camId < MAX_CAMERA_NUM) {
        mode = pipelineStatus[camId].interlace_mode;
        return NO_ERROR;
    }
    return ERROR;
}


status_t PipelineManage::getDeInterlace(int camId, char * type)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(type, pipelineStatus[camId].deInterlace);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::getDeviceName(int camId, char * type)
{
    if (camId < MAX_CAMERA_NUM) {
        strcpy(type, pipelineStatus[camId].deviceName);
        return NO_ERROR;
    }
    return ERROR;
}

status_t PipelineManage::getStreamType(int camId, STREAM_TYPE &type)
{
    if (camId < MAX_CAMERA_NUM) {
        type = pipelineStatus[camId].stream_type;
        return NO_ERROR;
    }
    return ERROR;
}

int PipelineManage::getEnumValueFromField(GParamSpec * prop, const char *nick)
{
    int match_idx = -1, num_of_values = 0;
    GEnumValue *values;

    values = G_ENUM_CLASS (g_type_class_ref (prop->value_type))->values;
    while (values[num_of_values].value_nick) {
        if (strcmp(values[num_of_values].value_nick, nick) == 0) {
            match_idx = num_of_values;
        }
        num_of_values++;
    }

    if (match_idx < 0) {
        g_print("doesn't find the enum nick: %s", nick);
        return -1;
    }

    return values[match_idx].value;
}

void PipelineManage::setProperties(GstElement *element, const char * properties)
{
    char *token;
    char pro_str[1024];
    GObjectClass *oclass;
    GParamSpec *prop;

    strncpy(pro_str, properties, 1024);
    token = strtok( (char*)pro_str, ", ");
    while (token != NULL) {
        char *ptr = strstr(token, "=");
        char *str_value = ptr + 1;
        char *property = token;
        *ptr = '\0';

        oclass = G_OBJECT_GET_CLASS (element);
        prop = g_object_class_find_property (oclass, property);
        if (!prop) {
            g_print("there isn't the property: %s in element\n", property);
            token = strtok(NULL, ", ");
            continue;
        }
        switch (prop->value_type) {
            gint value_int, value_enum;
            gboolean value_bool;
            case G_TYPE_INT:
            value_int = atoi(str_value);
            g_object_set(G_OBJECT(element), property, value_int, NULL);
            break;
            case G_TYPE_BOOLEAN:
            value_bool = ((strcmp(str_value, "true") == 0) ? true : false);
            g_object_set(G_OBJECT(element), property, value_bool, NULL);
            break;
            case G_TYPE_STRING:
            g_object_set(G_OBJECT(element), property, str_value, NULL);
            break;
            default:
            if (G_TYPE_IS_ENUM(prop->value_type)) {
                value_enum = getEnumValueFromField(prop, str_value);
                if (value_enum == -1) {
                    break;
                }
                g_object_set(G_OBJECT(element), property, value_enum, NULL);
            } else
                g_print("ERR, this type isn't supported\n");
            break;
        }

        token = strtok(NULL, ", ");
    }
}

status_t PipelineManage::createAndlinkElement(GstElement *pipeline)
{
    int i = 0;
    pluginNum = 0;
    GstCaps *caps[MAX_PLUGIN_SIZE];

    for (i = 0; i < MAX_PLUGIN_SIZE; i++) {
        caps[i] = NULL;
        g_elements[i] = NULL;
    }

    for (i = 0; g_plugin[i].plugin_name != NULL; i++) {
        char element_name[20];
        if (strcmp(g_plugin[i].plugin_name, "icamerasrc") == 0)
        {
            snprintf(element_name, 20, "camerasrc%d", i);
        }
        else
        {
            snprintf(element_name, 20, "element-%d", i);
        }
        g_elements[i] = gst_element_factory_make(g_plugin[i].plugin_name, element_name);
        if (!g_elements[i]) {
            gst_object_unref(GST_OBJECT(pipeline));
            g_print("Failed to create source element\n");
            return -1;
        }
        if (g_plugin[i].pro_attrs != NULL)
            setProperties(g_elements[i], g_plugin[i].pro_attrs);
        if (g_plugin[i].caps_filter != NULL)
            caps[i] = gst_caps_from_string(g_plugin[i].caps_filter);

        gst_bin_add_many(GST_BIN(pipeline), g_elements[i], NULL);
        if (i > 0 && g_elements[i - 1]) {
            if (caps[i - 1] != NULL) {
                if(!gst_element_link_filtered(g_elements[i - 1], g_elements[i], caps[i - 1])) {
                    gst_object_unref(GST_OBJECT(pipeline));
                    g_print("Failed to link element with caps_filter:%s\n", g_plugin[i - 1].caps_filter);
                    return -1;
                }
                gst_caps_unref(caps[i - 1]);
            } else {
                if (!gst_element_link_many(g_elements[i - 1], g_elements[i], NULL)) {
                    gst_object_unref(GST_OBJECT(pipeline));
                    g_print("Failed to link many elements\n");
                    return -1;
                }
            }
        }
        pluginNum++;
    }
    g_print("plugin_num: %d\n", pluginNum);

}

status_t PipelineManage::setManualProperties(GstElement *src, CamProperties *pro)
{
    int pos = 0;
    char proStr[1024] = {0};

    //ae-region, awb-region and color-transform isn't supported now.
    if (mTestDualMode) {
        if (pro->frameNum != 0)
            g_object_set(G_OBJECT(src), "num-buffers", pro->frameNum, NULL);
        else
            g_object_set(G_OBJECT(src), "num-buffers", 100, NULL);
    }
    g_object_set(G_OBJECT(src), "debug-level", pro->debugLevel, NULL);
    g_object_set(G_OBJECT(src), "iris-level", pro->irisLevel, NULL);
    //g_object_set(G_OBJECT(src), "exposure-time", pro->exposureTime, NULL);
    g_object_set(G_OBJECT(src), "gain", (float)pro->exposureGain, NULL);
    g_object_set(G_OBJECT(src), "ev", pro->exposureEv, NULL);
    g_object_set(G_OBJECT(src), "awb-gain-r", pro->awbRGain, NULL);
    g_object_set(G_OBJECT(src), "awb-gain-g", pro->awbBGain, NULL);
    g_object_set(G_OBJECT(src), "awb-gain-b", pro->awbGGain, NULL);
    g_object_set(G_OBJECT(src), "awb-shift-r", pro->awbRshift, NULL);
    g_object_set(G_OBJECT(src), "awb-shift-g", pro->awbGshift, NULL);
    g_object_set(G_OBJECT(src), "awb-shift-b", pro->awbBshift, NULL);
    g_object_set(G_OBJECT(src), "awb-windowsize", pro->windowSize, NULL);
    g_object_set(G_OBJECT(src), "sharpness", pro->sharpness, NULL);
    g_object_set(G_OBJECT(src), "brightness", pro->brightness, NULL);
    g_object_set(G_OBJECT(src), "contrast", pro->contrast, NULL);
    g_object_set(G_OBJECT(src), "hue", pro->hue, NULL);
    g_object_set(G_OBJECT(src), "saturation", pro->saturation, NULL);
    g_object_set(G_OBJECT(src), "wdr-level", pro->WdrLevel, NULL);
    g_object_set(G_OBJECT(src), "nr-filter-level", pro->noiseFilterLevel, NULL);
    g_object_set(G_OBJECT(src), "spatial-level", pro->spatialLevel, NULL);
    g_object_set(G_OBJECT(src), "temporal-level", pro->temporialLevel, NULL);

    if (pro->IrisMode[0] != 0)
        pos += sprintf(proStr+pos, "iris-mode=%s, ", pro->IrisMode);
    if (pro->aeMode[0] != 0)
        {
        pos += sprintf(proStr+pos, "ae-mode=%s, ", pro->aeMode);
        pos += sprintf(proStr+pos, "exposure-time=%d, ", pro->exposureTime);
        }
    if (pro->convergeSpeedMode[0] != 0)
        pos += sprintf(proStr+pos, "converge-speed-mode=%s, ", pro->convergeSpeedMode);
    if (pro->convergeSpeedLevel[0] != 0)
        pos += sprintf(proStr+pos, "converge-speed=%s, ", pro->convergeSpeedLevel);
    if (pro->awbMode[0] != 0)
        pos += sprintf(proStr+pos, "awb-mode=%s, ", pro->awbMode);
    if (pro->awbRange[0] != 0)
        pos += sprintf(proStr+pos, "cct-range=%s, ", pro->awbRange);
    //if (pro->awbWp[0] != 0)
    //    pos += sprintf(proStr+pos, "wp-point=%s ", pro->awbWp);
    if (pro->daynightMode[0] != 0)
        pos += sprintf(proStr+pos, "daynight-mode=%s, ", pro->daynightMode);
    if (pro->WdrMode[0] != 0)
        pos += sprintf(proStr+pos, "wdr-mode=%s, ", pro->WdrMode);
    if (pro->BlcArea[0] != 0)
        pos += sprintf(proStr+pos, "blc-area-mode=%s, ", pro->BlcArea);
    if (pro->noiseReductionMode[0] != 0)
        pos += sprintf(proStr+pos, "nr-mode=%s, ", pro->noiseReductionMode);
    if (pro->mirrorEffectMode[0] != 0)
        pos += sprintf(proStr+pos, "mirroreffect-mode=%s, ", pro->mirrorEffectMode);
    if (pro->sceneMode[0] != 0)
        {
        pos += sprintf(proStr+pos, "scene-mode=%s, ", pro->sceneMode);
        if (pro->sceneMode == "hdr")
            pos += sprintf(proStr+pos, "wdr-level=%d, ", pro->WdrLevel);
        }
    if (pro->sensorResolution[0] != 0)
        pos += sprintf(proStr+pos, "sensor-resolution=%s, ", pro->sensorResolution);
    if (pro->captureFps[0] != 0)
        pos += sprintf(proStr+pos, "fps=%s, ", pro->captureFps);
    if (pro->videoStandard[0] != 0)
        pos += sprintf(proStr+pos, "video-standard=%s, ", pro->videoStandard);
    if (pro->advanced[0] != 0)
        pos += sprintf(proStr+pos, "advance-feature=%s ", pro->advanced);

    printf("set the manual properties: %s\n", proStr);
    setProperties(src, proStr);

    //g_object_set(G_OBJECT(src), "wdr-mode", 1, NULL);

    //LOG_INFO(CStringFormat("Set wdr-level %d", pro->WdrLevel).Get(), NULL);
    //g_object_set(G_OBJECT(src), "wdr-level", pro->WdrLevel, NULL);

    return NO_ERROR;
}

status_t PipelineManage::pipelinePreviewStart(int camId)
{
    printf("%s camId %d\n",__func__,camId);

    int tryCount = 10;
    int enumId = -1, pos = 0;
    char *scaleCapString = NULL;
    char srcCapString[128]= {0};
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    GstCaps *caps = NULL;
    char srcPro[256] = {0};
    GstElement *pipeline, *source, *sink, *convert, *copy;
    PipelineStatus *status = &pipelineStatus[camId];
    status->camId = camId;
    CamProperties *properties = &(status->properties);

    /*restart from pause state*/
    if(status->state == CAMERA_STATE_PAUSE)
    {
        if(status->activePipeline){
            do {
                ret = gst_element_set_state (status->activePipeline, GST_STATE_PLAYING);
                tryCount--;
                usleep(50000);
            } while((ret == GST_STATE_CHANGE_FAILURE) && (tryCount));

            if (ret == GST_STATE_CHANGE_FAILURE) {
                g_printerr ("Unable to set the pipeline to the playing state.\n");
                return ERROR;
            }
            status->state = CAMERA_STATE_START;
            return NO_ERROR;
        }
    }

    if(status->state == CAMERA_STATE_START)
    {
        printf("the pipeline is already running\n");
        return NO_ERROR;
    }

    /* Initialize GStreamer */
    gst_init (0,0);
    clearPlugins();

    //setup all plugins
    if (status->srcElementType[0] == 0)
        strcpy(status->srcElementType, "icamerasrc");
    getCapsStringfromStatus(status, srcCapString);
    printf("srcCapString is %s\n", srcCapString);
    if (true == status->interlace_mode)
        pos += sprintf(srcPro+pos, "interlace-mode=alternate ");
    else
        pos += sprintf(srcPro+pos, "interlace-mode=any ");
    if (0 != status->deInterlace[0])
        pos += sprintf(srcPro+pos, "deinterlace-method=%s ", status->deInterlace);
    if (0 != status->deviceName[0])
        pos += sprintf(srcPro+pos, "device-name=%s ", status->deviceName);
    if (0 != status->ioMode[0])
        pos += sprintf(srcPro+pos, "io-mode=%s ", status->ioMode);
    if (status->vcEnable)
        pos += sprintf(srcPro+pos, "num-vc=4 ");
    if ( 0 == strcmp(status->fisheyeDewarpingMode,"rearview"))
    {
        char cmd[128];
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"rm  /etc/camera/magna-%s.aiqb",status->fisheyeDewarpingMode);
        printf("cmd:%s\n",cmd);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"cp  /etc/camera/magna-%s-%dx%d.aiqb  /etc/camera/magna-%s.aiqb",status->fisheyeDewarpingMode,status->width,status->height,status->fisheyeDewarpingMode);
        printf("cmd:%s\n",cmd);
        system(cmd);
        pos +=sprintf(srcPro + pos,"fisheye-dewarping-mode=%s  input-width=1280 input-height=720 input-format=UYVY ",status->fisheyeDewarpingMode);
    }
    else if ( 0 == strcmp(status->fisheyeDewarpingMode,"hitchview"))
    {
        char cmd[128];

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"rm  /etc/camera/magna-%s.aiqb",status->fisheyeDewarpingMode);
        printf("cmd:%s\n",cmd);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"cp  /etc/camera/magna-%s-%dx%d.aiqb  /etc/camera/magna-%s.aiqb",status->fisheyeDewarpingMode,status->width,status->height,status->fisheyeDewarpingMode);
        printf("cmd:%s\n",cmd);
        system(cmd);
        pos +=sprintf(srcPro + pos,"fisheye-dewarping-mode=%s  input-width=1280 input-height=720 input-format=UYVY ",status->fisheyeDewarpingMode);
    }

    pos += sprintf(srcPro+pos, "buffer-count=10 ");
    printf("srcPro = %s\n", srcPro);
    addPlugins(status->srcElementType, srcPro, srcCapString);

    //there ins't convert plugin when use fakesink.
    if (status->sinkElementType[0] == 0)
        strcpy(status->sinkElementType, "vaapisink");
    if (strcmp(status->sinkElementType, "fakesink") != 0) {
        if (status->convertElementType[0] == 0)
            strcpy(status->convertElementType, "vaapipostproc");

        if (strcmp(status->convertElementType, "yamiscale") == 0) {
            addPlugins("buffercopy", NULL, NULL);
            addPlugins(status->convertElementType, NULL, "video/x-raw,format=xBGR,width=1920,height=1080");
        } else
            addPlugins(status->convertElementType, NULL, NULL);
    }
    addPlugins(status->sinkElementType, NULL, NULL);

    /* Create the pipeline */
    pipeline = gst_pipeline_new ("preview-pipeline");
    if (!pipeline) {
        g_printerr ("Error to create pipeline.\n");
        return -1;
    }

    createAndlinkElement(pipeline);
    status->sourceElement = g_elements[0];
    if (status->sourceElement == NULL)
    {
        //LOG_ERROR("Failed to create icamerasrc.", NULL);
        return -1;
    }
    if (strcmp(status->srcElementType, "icamerasrc") == 0)
        setManualProperties(status->sourceElement, properties);
    gst_video_overlay_expose(GST_VIDEO_OVERLAY (g_elements[pluginNum - 1]));
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (g_elements[pluginNum - 1]), status->videoWindow);
    printf("pipeline %p set window ok\n", pipeline);

#ifdef ENABLE_AUTOMATION_TEST
    LOG_INFO("gst_pad_add_probe", NULL);
    GstPad *pad = gst_element_get_static_pad (g_elements[0], "src");
    if (pad == NULL)
    {
        LOG_ERROR("Failed to get source pad.", NULL);
        return -1;
    }
	printf("Add pad probe for cam %d, pad address %p, camid address %p\n", camId, pad,&camids[camId]);
	gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
        (GstPadProbeCallback) icamerasrc_sample_callback, (gint*)&camids[camId], NULL);
    gst_object_unref (pad);
#endif
    status->activePipeline = pipeline;
    pipelineConnectWatch(pipeline, camId);

    do {
        ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
        tryCount--;
        usleep(50000);
    } while((ret == GST_STATE_CHANGE_FAILURE) && (tryCount));
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (pipeline);
        return -1;
    }

    printf("pipeline %p set state to playing  ok\n", pipeline);
    status->state = CAMERA_STATE_START;
    return NO_ERROR;

}

status_t PipelineManage::pipelineRecordingStart(int camId)
{
    int pos = 0;
    char fileName[128];
    GstElement *pipeline, *source, *tee;
    GstElement *displayQueue, *displayCvt, *displaySink;
    GstElement *encodeQueue, *bufferCopy, *encoding, *encodeSink;
    GstBus *bus;
    GstMessage *msg;
    GstCaps *srcCaps;
    char srcPro[256] = {0};
    char srcCapString[128] = {0};
    GstPadTemplate *tee_src_pad_template;
    GstPad *tee_display_pad, *tee_encode_pad;
    GstPad *queue_display_pad, *queue_encode_pad;
    PipelineStatus *status = &pipelineStatus[camId];
    CamProperties *properties = &(status->properties);

    printf("recording -----------\n");

    /* Initialize GStreamer */
    gst_init (0,0);
    /* Create the elements */
    if (status->srcElementType[0] == 0)
        strcpy(status->srcElementType, "icamerasrc");
    source = gst_element_factory_make(status->srcElementType, "source");
    status->sourceElement = source;

    tee = gst_element_factory_make ("tee", "tee");
    displayQueue = gst_element_factory_make ("queue", "displayQueue");
    if (status->convertElementType[0] == 0)
        strcpy(status->convertElementType, "vaapipostproc");
    displayCvt = gst_element_factory_make(status->convertElementType, "displayCvt");
    if (status->sinkElementType[0] == 0)
        strcpy(status->sinkElementType, "vaapisink");
    displaySink = gst_element_factory_make(status->sinkElementType, "displaySink");

    encodeQueue = gst_element_factory_make ("queue", "encodeQueue");
    bufferCopy = gst_element_factory_make("buffercopy", "bufferCopy");
    encoding = gst_element_factory_make("yamiencode", "encoding");
    encodeSink = gst_element_factory_make ("filesink", "encodeSink");

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new ("recording-pipeline");
    if (!pipeline || !source || !tee || !displayQueue || !displayCvt || !displaySink ||
        !encodeQueue || !bufferCopy || !encoding || !encodeSink) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    //set the properties.
    if (true == status->interlace_mode)
        pos += sprintf(srcPro+pos, "interlace-mode=alternate ");
    else
        pos += sprintf(srcPro+pos, "interlace-mode=any ");
    if (0 != status->deInterlace[0])
        pos += sprintf(srcPro+pos, "deinterlace-method=%s ", status->deInterlace);
    if (0 != status->deviceName[0])
        pos += sprintf(srcPro+pos, "device-name=%s ", status->deviceName);
    if (0 != status->ioMode[0])
        pos += sprintf(srcPro+pos, "io-mode=%s ", status->ioMode);
    printf("srcPro = %s\n", srcPro);
    setProperties(source, srcPro);
    getFileNameFromTime(fileName);
    if (strcmp(status->srcElementType, "icamerasrc") == 0)
        setManualProperties(status->sourceElement, properties);
    g_object_set(G_OBJECT(encodeSink), "location", fileName, NULL);
    gst_video_overlay_expose(GST_VIDEO_OVERLAY(displaySink));
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY(displaySink), status->videoWindow);

    //Link all elements that can be automatically
    gst_bin_add_many (GST_BIN (pipeline), source, tee, displayQueue, displayCvt, displaySink,
    encodeQueue, bufferCopy, encoding, encodeSink, NULL);

    g_object_set(G_OBJECT (displayQueue),"max-size-buffers", 0, NULL);
    g_object_set(G_OBJECT(displayQueue), "max-size-time", 0, NULL);
    g_object_set(G_OBJECT(displayQueue), "max-size-bytes", 512000000, NULL);
    g_object_set(G_OBJECT (encodeQueue),"max-size-buffers", 0, NULL);
    g_object_set(G_OBJECT(encodeQueue), "max-size-time", 0, NULL);
    g_object_set(G_OBJECT(encodeQueue), "max-size-bytes", 512000000, NULL);

    getCapsStringfromStatus(status, srcCapString);
    printf("srcCapString is %s\n", srcCapString);
    if (srcCapString[0] != 0) {
        srcCaps = gst_caps_from_string(srcCapString);
        if (gst_element_link_filtered(source, tee, srcCaps) != TRUE) {
            g_printerr ("Elements with caps %s could not be linked.\n", srcCapString);
            gst_object_unref (pipeline);
            gst_caps_unref(srcCaps);
            return -1;
        }
        gst_caps_unref(srcCaps);
    } else {
        if (gst_element_link_many (source, tee, NULL) != TRUE) {
            g_printerr ("Elements could not be linked");
            gst_object_unref (pipeline);
            return -1;
        }
    }
    if (gst_element_link_many (displayQueue, displayCvt, displaySink, NULL) != TRUE ||
        gst_element_link_many (encodeQueue, bufferCopy, encoding, encodeSink, NULL) != TRUE) {
        g_printerr ("Elements could not be linked");
        gst_object_unref (pipeline);
        return -1;
    }

    /* Manually link the Tee, which has "Request" pads */
    tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee), "src_%u");
    tee_display_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
    g_print ("Obtained request pad %s for display branch.\n", gst_pad_get_name (tee_display_pad));
    queue_display_pad = gst_element_get_static_pad (displayQueue, "sink");
    tee_encode_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
    g_print ("Obtained request pad %s for encoding branch.\n", gst_pad_get_name (tee_encode_pad));
    queue_encode_pad = gst_element_get_static_pad (encodeQueue, "sink");
    if (gst_pad_link (tee_display_pad, queue_display_pad) != GST_PAD_LINK_OK ||
        gst_pad_link (tee_encode_pad, queue_encode_pad) != GST_PAD_LINK_OK) {
        g_printerr ("Tee could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }
    gst_object_unref (queue_encode_pad);
    gst_object_unref (queue_display_pad);

#ifdef ENABLE_AUTOMATION_TEST
    LOG_INFO("gst_pad_add_probe", NULL);
    GstPad *pad = gst_element_get_static_pad (g_elements[0], "src");
    if (pad == NULL)
    {
        LOG_ERROR("Failed to get source pad.", NULL);
        return -1;
    }
	printf("Add pad probe for cam %d, pad address %p, camid address %p\n", camId, pad,&camids[camId]);
	gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
        (GstPadProbeCallback) icamerasrc_sample_callback, (gint*)&camids[camId], NULL);
    gst_object_unref (pad);
#endif
    pipelineConnectWatch(pipeline, camId);

    /* Start playing the pipeline */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    status->activePipeline = pipeline;
    printf("Start playing the pipeline -----------\n");

    return NO_ERROR;
}

status_t PipelineManage::pipelinePlayerStart(int camId)
{
    /* Create the elements */
    GstElement * playbin2;
    PipelineStatus *status = &pipelineStatus[camId];
    playbin2 = gst_element_factory_make ("playbin2", "playbin2");

    if (!playbin2) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    g_print ("create_ui reached. %lu\n",pthread_self());
    pipelineConnectWatch(playbin2, camId);

    /* Set the URI to play */
    g_object_set (playbin2, "uri", "http://docs.gstreamer.com/media/sintel_trailer-480p.webm", NULL);
    pipelineStatus[camId].activePipeline = playbin2;
    return gst_element_set_state (playbin2, GST_STATE_PLAYING);
}

int PipelineManage::pipelineRemoteStart(int camId)
{
    GstElement *vsource,*vrate,*vscale,*vconvert;
    GstElement *vrtpbin,*vrtpsink,*vrtppay;
    GstElement *pipeline;
    GMainLoop *loop;
    GstCaps *caps;
    char capsString[1024]= {0};
    GstPad *srcpad,*sinkpad;
    int ret, enumId = -1;
    char ipaddress[128]= {0};
    PipelineStatus *status = &pipelineStatus[camId];

    printf("start %s\n", __func__);

    gst_init(0,0);

    pipeline=gst_pipeline_new(NULL);
    if(NULL == pipeline)
        return -1;

    if (status->srcElementType[0] != 0) {
        vsource = gst_element_factory_make (status->srcElementType, "vsource");
    } else {
        vsource = gst_element_factory_make ("icamerasrc", "vsource");
    }
    if(NULL == vsource)
        return -1;
    status->sourceElement = vsource;

    if (status->convertElementType[0] != 0) {
        vconvert = gst_element_factory_make (status->convertElementType, "vconvert");
    } else {
        vconvert = gst_element_factory_make ("vaapipostproc", "vconvert");
    }
    if(NULL == vsource)
        return -1;

    //set the convert format property to UYVY
    g_object_set(G_OBJECT(vconvert), "format", 5, NULL);

    vrtppay=gst_element_factory_make("rtpvrawpay","vrtppay");
    if(NULL == vrtppay)
        return -1;

    gst_bin_add_many(GST_BIN(pipeline),vsource,vconvert,vrtppay,NULL);

    if (true == status->interlace_mode) {
        printf("interlace_mode is set \n");
        g_object_set(G_OBJECT(vsource), "interlace_mode", true, NULL);
        if (0 != status->deInterlace[0]) {
            printf("the deInterlace: %s\n", status->deInterlace);
            enumId = getEnumValueFromNick(vsource, "deinterlace_method", status->deviceName);
            g_object_set(G_OBJECT(vsource), "deinterlace_method", enumId, NULL);
        }
    }

    /* set the cameraInput  */
    if (status->deviceName[0] != 0) {
        printf("the deviceName: %s\n", status->deviceName);
        enumId = getEnumValueFromNick(vsource, "device-name", status->deviceName);
        g_object_set(G_OBJECT(vsource), "device-name", enumId, NULL);
    } else {
         printf("waring: env cameraInput is NULL, Caff set it to tpg now\n");
         g_object_set(G_OBJECT(vsource), "device-name", 0, NULL);
    }

    caps=gst_caps_from_string(Video_Caps);
    getCapsStringfromStatus(status, capsString);

    if (0 != capsString[0]) {
        printf("capsString is %s\n",capsString);
        caps = gst_caps_from_string(capsString);
    }

    if(!gst_element_link_filtered(vsource,vconvert,caps)) {
        g_error("Failed to link caps");
        return -1;
    }

    if(!gst_element_link_many(vconvert,vrtppay,NULL)){
        g_error("Failed to link ");
        return -1;
    }
    gst_caps_unref(caps);

    vrtpbin=gst_element_factory_make("rtpbin","vrtpbin");
    gst_bin_add(GST_BIN(pipeline),vrtpbin);

    vrtpsink=gst_element_factory_make("udpsink","vrtpsink");
    {
        int p = 0;
        p += sprintf(ipaddress+p, "%d",status->remoteIp & 0xFF);
        p += sprintf(ipaddress+p,".");
        p += sprintf(ipaddress+p,"%d",status->remoteIp>>8 & 0xFF);
        p += sprintf(ipaddress+p,".");
        p += sprintf(ipaddress+p,"%d",status->remoteIp>>16 & 0xFF);
        p += sprintf(ipaddress+p,".");
        p += sprintf(ipaddress+p,"%d",status->remoteIp>>24 & 0xFF);

        printf("ipaddress %s \n",ipaddress);
    }

    g_object_set(vrtpsink,"port",UDPPORT,"host",ipaddress,NULL);
    gst_bin_add_many(GST_BIN(pipeline),vrtpsink,NULL);

    sinkpad=gst_element_get_request_pad(vrtpbin,"send_rtp_sink_0");
    srcpad=gst_element_get_static_pad(vrtppay,"src");
    if(gst_pad_link(srcpad,sinkpad)!=GST_PAD_LINK_OK)
        g_error("Failed to link video payloader to vrtpbin");
    gst_object_unref(srcpad);
    srcpad=gst_element_get_static_pad(vrtpbin,"send_rtp_src_0");
    sinkpad=gst_element_get_static_pad(vrtpsink,"sink");
    if(gst_pad_link(srcpad,sinkpad)!=GST_PAD_LINK_OK)
        g_error("Failed to link vrtpbin to vrtpsink");
    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);

    g_print("starting sender pipeline\n");
    pipelineConnectWatch(pipeline, camId);

    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (pipeline);
        return -1;
    }

    printf("pipeline %p set state to playing  ok\n", pipeline);
    status->activePipeline = pipeline;
    status->state = CAMERA_STATE_START;
    return NO_ERROR;
}

int PipelineManage::getEnumValueFromNick(GstElement *element, const char * property, const char *nick)
{
    int j = 0;
    GEnumValue *values;
    gboolean find_value = false;
    GObjectClass *oclass;
    GParamSpec *prop;

    if(NULL == element)
    {
       // LOG_ERROR(CStringFormat("NULL element: %s", property).Get(), NULL);
        return 0;
    }
    oclass = G_OBJECT_GET_CLASS (element);
    if(NULL == oclass)
    {
        //LOG_ERROR("NULL oclass", NULL);
        return 0;
    }
    prop = g_object_class_find_property (oclass, property);
    if(NULL == prop)
    {
        //LOG_ERROR("NULL prop", NULL);
        printf("can not find property!");
        return 0;
    }

    values = G_ENUM_CLASS (g_type_class_ref (prop->value_type))->values;
    while (values[j].value_name) {
        //LOG_INFO(CStringFormat("getEnumValueFromNick: %s", values[j].value_name).Get(), NULL);
        if (strcmp(values[j].value_nick, nick) == 0) {
            find_value = true;
            break;
        }
        j++;
    }
    if (!find_value) {
        g_print("donesn't find the enum nick: %s", nick);
        return -1;
    }

    return values[j].value;
}

int PipelineManage::getNickFromEnumValue(GstElement *element, const char * property, const int value, char* output)
{
    int j = 0;
    GEnumValue *values;
    gboolean find_value = false;
    GObjectClass *oclass;
    GParamSpec *prop;

    if(NULL == element)
        return 0;
    oclass = G_OBJECT_GET_CLASS (element);
    if(NULL == oclass)
        return 0;
    prop = g_object_class_find_property (oclass, property);
    if(NULL == prop)
        return 0;

    values = G_ENUM_CLASS (g_type_class_ref (prop->value_type))->values;
    while (values[j].value_name) {
        if (values[j].value == value) {
            find_value = true;
            break;
        }
        j++;
    }
    if (!find_value) {
        g_print("donesn't find the enum : %d", value);
        return -1;
    }

    memcpy(output,(char*)values[j].value_nick,strlen((char*)values[j].value_nick));
    return 0;
}

void PipelineManage::getCapsStringfromStatus(PipelineStatus *status,char *string)
{
    int pos = 0;

    if (0 == status->format[0]
        && (0 == status->width || 0 == status->height)) {
        /*no setting ,do noting, use the defalut pad caps between elements*/
        return;
    }
    pos += sprintf(string,"video/x-raw");
    if (0 != status->format[0]) {
        pos += sprintf(string+pos,",format=");
        pos += sprintf(string+pos,"%s",status->format);
    }
    if (0 != status->width && 0 != status->height) {
        pos += sprintf(string+pos,",width=");
        pos += sprintf(string+pos,"%d",status->width);
        pos += sprintf(string+pos,",height=");
        pos += sprintf(string+pos,"%d",status->height);
    }
    if (0 != status->framerate[0]) {
        pos += sprintf(string+pos,",framerate=");
        pos += sprintf(string+pos,"%s",status->framerate);
    }
}

status_t PipelineManage::setCaptureNum(int camId, int num)
{
    return NO_ERROR;
}
status_t PipelineManage::getIrisMode(int camId, char *irisModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "iris-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"iris-mode", valueOld, irisModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setIrisMode(int camId, char *irisModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).IrisMode, irisModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement, "iris-mode", irisModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "iris-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getIrisLevel(int camId,  int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "iris-level", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setIrisLevel(int camId, int value)
{
    (pipelineStatus[camId].properties).irisLevel = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "iris-level", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAeMode(int camId, char *value)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "ae-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"ae-mode", valueOld, value);
    return NO_ERROR;
}
status_t PipelineManage::setAeMode(int camId, char *mode)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).aeMode, mode);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement, "ae-mode", mode);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "ae-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::setFisheyeMode(int camId, char *mode)
{
    int value = 0;
    printf("mode:%s\n",mode);
    printf("sourceElement:%s\n",pipelineStatus[camId].sourceElement);
    strcpy(pipelineStatus[camId].properties.fisheyemode,mode);
    strcpy(pipelineStatus[camId].fisheyeDewarpingMode,mode);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement, "fisheye-dewarping-mode", mode);
        printf("value:%d\n",value);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "fisheye-dewarping-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getFisheyeMode(int camId, char *value)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "fisheye-dewarping-mode",&valueOld, NULL);
    printf("fisheyemodeold:%d\n",valueOld);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"fisheye-dewarping-mode", valueOld, value);

    return NO_ERROR;
}
status_t PipelineManage::getConvergeSpeedMode(int camId, char *value)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "converge-speed-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"converge-speed-mode", valueOld, value);
    return NO_ERROR;
}
status_t PipelineManage::setConvergeSpeedMode(int camId, char *speed)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).convergeSpeedMode, speed);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement, "converge-speed-mode", speed);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "converge-speed-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getConvergeSpeedLevel(int camId, char *value)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "converge-speed", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"converge-speed", valueOld, value);
    return NO_ERROR;
}
status_t PipelineManage::setConvergeSpeedLevel(int camId, char *speed)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).convergeSpeedLevel, speed);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement, "converge-speed", speed);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "converge-speed", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getExposureTime(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "exposure-time", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setExposureTime(int camId, int value)
{
    (pipelineStatus[camId].properties).exposureTime = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "exposure-time", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getExposureGain(int camId, float &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "gain", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setExposureGain(int camId, float value)
{
    (pipelineStatus[camId].properties).exposureGain = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "gain", (float)value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getExposureEv(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "ev", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setExposureEv(int camId, int value)
{
    (pipelineStatus[camId].properties).exposureEv = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "ev", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbMode(int camId, char *awbModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"awb-mode", valueOld, awbModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setAwbMode(int camId, char *awbModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).awbMode, awbModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement, "awb-mode", awbModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbRGain(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-gain-r", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbRGain(int camId, int value)
{
    (pipelineStatus[camId].properties).awbRGain = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-gain-r", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbGGain(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-gain-g", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbGGain(int camId, int value)
{
    (pipelineStatus[camId].properties).awbGGain = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-gain-g", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbBGain(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-gain-b", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbBGain(int camId, int value)
{
    (pipelineStatus[camId].properties).awbBGain = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-gain-b", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbRange(int camId, char *awbRangeOld)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "cct-range", awbRangeOld, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbRange(int camId, char *awbRangePtr)
{
    strcpy((pipelineStatus[camId].properties).awbRange, awbRangePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "cct-range", awbRangePtr, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbWp(int camId, char *value)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "wp-point", value, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbWp(int camId, char *value)
{
    strcpy((pipelineStatus[camId].properties).awbWp, value);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "wp-point", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::setAeRegion(int camId, char *region)
{
    g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "ae-region", region, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbRegion(int camId, char *region)
{
    g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-region", region, NULL);
    return NO_ERROR;
}
status_t PipelineManage::getWindowSize(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-windowsize", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setWindowSize(int camId, int value)
{
    (pipelineStatus[camId].properties).windowSize = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-windowsize", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::setColorTransform(int camId, char *value)
{
    g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "color-transform", value, NULL);
    return NO_ERROR;
}
status_t PipelineManage::getColorTransform(int camId, char *value)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "color-transform", value, NULL);
    return NO_ERROR;
}
status_t PipelineManage::getAwbShiftR(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-shift-r", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbShiftR(int camId, int value)
{
    (pipelineStatus[camId].properties).awbRshift = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-shift-r", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbShiftG(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-shift-g", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbShiftG(int camId, int value)
{
    (pipelineStatus[camId].properties).awbGshift = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-shift-g", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAwbShiftB(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-shift-b", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setAwbShiftB(int camId, int value)
{
    (pipelineStatus[camId].properties).awbBshift = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "awb-shift-b", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getDayNightMode(int camId, char *dayNightModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "daynight-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"daynight-mode",valueOld,dayNightModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setDayNightMode(int camId, char *dayNightModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).daynightMode, dayNightModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"daynight-mode",dayNightModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "daynight-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getSharpness(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "sharpness", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setSharpness(int camId, int value)
{
    (pipelineStatus[camId].properties).sharpness = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "sharpness", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getBrightness(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "brightness", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setBrightness(int camId, int value)
{
    (pipelineStatus[camId].properties).brightness = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "brightness", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getContrast(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "contrast", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setContrast(int camId, int value)
{
    (pipelineStatus[camId].properties).contrast = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "contrast", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getHue(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "hue", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setHue(int camId, int value)
{
    (pipelineStatus[camId].properties).hue = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "hue", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getSaturation(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "saturation", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setSaturation(int camId, int value)
{
    (pipelineStatus[camId].properties).saturation = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "saturation", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getWdrMode(int camId, char *wdrModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "wdr-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"wdr-mode", valueOld, wdrModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setWdrMode(int camId, char *wdrModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).WdrMode, wdrModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"wdr-mode", wdrModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "wdr-mode", value, NULL);
    }
    return NO_ERROR;
}

status_t PipelineManage::getWdrLevel(int camId, int &valueOld)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "wdr-level", &valueOld, NULL);
    return NO_ERROR;
}

status_t PipelineManage::setWdrLevel(int camId, int value)
{
    pipelineStatus[camId].properties.WdrLevel = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "wdr-level", value, NULL);
    }
    return NO_ERROR;
}

status_t PipelineManage::getBlcMode(int camId, char *wdrModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "blc-area-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"blc-area-mode", valueOld, wdrModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setBlcMode(int camId, char *enableBlcPtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).BlcArea, enableBlcPtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"blc-area-mode",enableBlcPtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "blc-area-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getBlcAreaLevel(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "wdr-level", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setBlcAreaLevel(int camId, int value)
{
    (pipelineStatus[camId].properties).WdrLevel = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "wdr-level", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getNoiseReductionMode(int camId, char *noiseReductionModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "nr-mode", &valueOld, NULL);

    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"nr-mode", valueOld, noiseReductionModeOld);
          printf("%s %s %d\n", __func__, noiseReductionModeOld , valueOld);
    return NO_ERROR;
}
status_t PipelineManage::setNoiseReductionMode(int camId, char *nrModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).noiseReductionMode, nrModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"nr-mode",nrModePtr);
        printf("%s %s %d\n", __func__, nrModePtr , value);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "nr-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getNoiseFilter(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "nr-filter-level", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setNoiseFilter(int camId, int value)
{
    (pipelineStatus[camId].properties).noiseFilterLevel = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "nr-filter-level", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getSpatialLevel(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "spatial-level", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setSpatialLevel(int camId, int value)
{
    (pipelineStatus[camId].properties).spatialLevel = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "spatial-level", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getTemporalLevel(int camId, int &oldValue)
{
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "temporal-level", &oldValue, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setTemporalLevel(int camId, int value)
{
    (pipelineStatus[camId].properties).temporialLevel = value;
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "temporal-level", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getMirrorEffectMode(int camId, char *mirrorEffectModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "mirroreffect-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"mirroreffect-mode", valueOld, mirrorEffectModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setMirrorEffectMode(int camId, char *mirrorEffectModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).mirrorEffectMode, mirrorEffectModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"mirroreffect-mode",mirrorEffectModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "mirroreffect-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getSceneMode(int camId, char *sceneModePtrOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "scene-mode", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"scene-mode", valueOld, sceneModePtrOld);
    return NO_ERROR;
}
status_t PipelineManage::setSceneMode(int camId, char *sceneModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).sceneMode, sceneModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"scene-mode",sceneModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "scene-mode", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getSensorMode(int camId, char *sensorModeOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "sensor-resolution", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"sensor-resolution", valueOld, sensorModeOld);
    return NO_ERROR;
}
status_t PipelineManage::setSensorMode(int camId, char *sensorModePtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).sensorResolution, sensorModePtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"sensor-resolution",sensorModePtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "sensor-resolution", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getCaptureFps(int camId, char *captureFpsOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "fps", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"fps", valueOld, captureFpsOld);
    return NO_ERROR;
}
status_t PipelineManage::setCaptureFps(int camId, char *captureFpsPtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).captureFps, captureFpsPtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"fps",captureFpsPtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "fps", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getVideoStandard(int camId, char *videoStandardOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "video-standard", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"video-standard", valueOld, videoStandardOld);
    return NO_ERROR;
}
status_t PipelineManage::setVideoStandard(int camId, char *videoStandardPtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).videoStandard, videoStandardPtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"video-standard", videoStandardPtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "video-standard", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::getAdvanceFeature(int camId, char *advancedOld)
{
    int valueOld = 0;
    g_object_get(G_OBJECT(pipelineStatus[camId].sourceElement), "advance-feature", &valueOld, NULL);
    getNickFromEnumValue(pipelineStatus[camId].sourceElement,"advance-feature", valueOld, advancedOld);
    return NO_ERROR;
}
status_t PipelineManage::setAdvanceFeature(int camId, char *advancedPtr)
{
    int value = 0;
    strcpy((pipelineStatus[camId].properties).advanced, advancedPtr);
    if (NULL != pipelineStatus[camId].sourceElement)
    {
        value = getEnumValueFromNick(pipelineStatus[camId].sourceElement,"advance-feature", advancedPtr);
        g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "advance-feature", value, NULL);
    }
    return NO_ERROR;
}
status_t PipelineManage::setCustomAicParam(int camId, char *aicParam)
{
    g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "custom-aic-param", aicParam, NULL);
    return NO_ERROR;
}
status_t PipelineManage::setDebugLevel(int camId, int value)
{
    (pipelineStatus[camId].properties).debugLevel = value;
    g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "debug-level", value, NULL);
    return NO_ERROR;
}

status_t PipelineManage::setNumBuffers(int camId, int value)
{
    (pipelineStatus[camId].properties).frameNum = value;
    g_object_set(G_OBJECT(pipelineStatus[camId].sourceElement), "num-buffers", value, NULL);
    return NO_ERROR;
}

status_t PipelineManage::setTestDualMode(bool enable)
{
    mTestDualMode = enable;
    return NO_ERROR;
}

}

