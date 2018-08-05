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

#include "gstCaffAutoTestSampleCallback.h"
#include <sys/mman.h>
#include <gst/allocators/gstdmabuf.h>

static std::vector<S_RAW_FRAME> *s_pCameraFrames[MAX_CONCURRENT_CAM_COUNT] = {NULL};
static int s_ncount_to_query[MAX_CONCURRENT_CAM_COUNT] = {0};
static pthread_mutex_t s_mutex_lock[MAX_CONCURRENT_CAM_COUNT] = {PTHREAD_MUTEX_INITIALIZER,
														PTHREAD_MUTEX_INITIALIZER,
														PTHREAD_MUTEX_INITIALIZER,
														PTHREAD_MUTEX_INITIALIZER};
static pthread_cond_t  s_QueueEvent[MAX_CONCURRENT_CAM_COUNT] = {PTHREAD_COND_INITIALIZER,
														PTHREAD_COND_INITIALIZER,
														PTHREAD_COND_INITIALIZER,
														PTHREAD_COND_INITIALIZER};
extern int gWaitTimeMax;

GstPadProbeReturn icamerasrc_sample_callback ( 
                    GstPad            *pad,
                    GstPadProbeInfo   *info,
                    gpointer           user_data)
{
    GstBuffer *buffer = NULL;
    unsigned char *data = NULL;
	int camId=*(gint*)user_data;
	//LOG_INFO(CStringFormat("icamerasrc callback for cam %d", camId).Get(), NULL);
    if (s_ncount_to_query[camId] > 0)
    {
		LOG_INFO(CStringFormat("icamerasrc callback for cam %d", camId).Get(), NULL);
		buffer = GST_PAD_PROBE_INFO_BUFFER (info);

        if (buffer == NULL)
        {
            LOG_WARNING("Buffer from sample callback is NULL", NULL);
            return GST_PAD_PROBE_OK;
        }

        GstVideoMeta *videometa = gst_buffer_get_video_meta(buffer);

        if (videometa == NULL)
        {
            LOG_WARNING("Video meta from sample callback is NULL", NULL);
            return GST_PAD_PROBE_OK;
        }
        
        gint nbuffer = gst_buffer_n_memory(buffer);

        if (nbuffer < 1)
        {
            LOG_WARNING("Memory count in the GstBuffer is less than 1", NULL);
            return GST_PAD_PROBE_OK;
        }
        else
        {
            LOG_INFO(CStringFormat("Memory count is %d", nbuffer).Get(), NULL);
        }

        GstMemory *pMemory = gst_buffer_peek_memory(buffer, 0);

        if ( pMemory == NULL )
        {
            LOG_WARNING("Memory pointer is NULL", NULL);
            return GST_PAD_PROBE_OK;
        }

        S_RAW_FRAME frame = {};

        if (gst_is_dmabuf_memory(pMemory))
        {
            //LOG_INFO("DMABUF*********************************************", NULL);
            gint dmafd = gst_dmabuf_memory_get_fd(pMemory);

            if (dmafd <= 0)
            {
                LOG_ERROR("Failed: gst_dmabuf_memory_get_fd", NULL);
                return GST_PAD_PROBE_OK;
            }

            void *dmadata = mmap(NULL, pMemory->size,
                    PROT_READ,
                    MAP_SHARED,
                    dmafd, pMemory->offset);

            if (dmadata == NULL)
            {
                LOG_ERROR("Failed: mmap", NULL);
                return GST_PAD_PROBE_OK;
            }

            data = (unsigned char*)malloc(pMemory->size);

            memcpy(data, dmadata, pMemory->size);

            if (munmap(dmadata, pMemory->size) < 0)
            {
                LOG_ERROR("Failed: munmap", NULL);
            }

            frame.data_size = pMemory->size;
        }
        else
        {
            GstMapInfo mapinfo;
          
            if (!gst_memory_map(pMemory, &mapinfo, GST_MAP_READ))
            {
                LOG_WARNING("Failed to map memory", NULL);
                return GST_PAD_PROBE_OK;
            }
            
            data = (unsigned char*)malloc(mapinfo.size);
            memcpy(data, mapinfo.data, mapinfo.size);
            
            gst_memory_unmap(pMemory, &mapinfo);

            frame.data_size = mapinfo.size;
        }


        frame.data_ptr = data; 
        frame.format = videometa->format;
        frame.w = videometa->width;
        frame.h = videometa->height;
        frame.flag = videometa->flags;

        // For one plan only
        frame.stride = videometa->stride[0];

        s_pCameraFrames[camId]->push_back(frame);

        s_ncount_to_query[camId]--;

        if (s_ncount_to_query[camId] == 0)
        {
            pthread_mutex_lock(&s_mutex_lock[camId]);
            pthread_cond_signal(&s_QueueEvent[camId]);
            pthread_mutex_unlock(&s_mutex_lock[camId]); 
        }
    }
    
    return GST_PAD_PROBE_OK;
}


void StartQueryFrames(vector<S_RAW_FRAME> *pCameraFrames, int ncount, int camId)
{
	LOG_INFO(CStringFormat("StartQueryFrames for cam %d", camId).Get(), NULL);
	s_pCameraFrames[camId] = pCameraFrames;
    s_ncount_to_query[camId] = ncount;
}

void WaitQueryFramesDone(int ncount,int camId)
{
    pthread_mutex_lock(&s_mutex_lock[camId]);
    struct timespec abstime;
    struct timeval now;
    gettimeofday(&now, NULL);

    int timeout_ms = gWaitTimeMax * ncount;
    
    long long nsec = now.tv_usec * 1000 + timeout_ms * 1000000;
    abstime.tv_nsec = nsec % 1000000000;
    abstime.tv_sec = now.tv_sec + nsec / 1000000000;
    
    int ret = pthread_cond_timedwait(&s_QueueEvent[camId], &s_mutex_lock[camId], &abstime);

    pthread_mutex_unlock(&s_mutex_lock[camId]);

    if (ret == ETIMEDOUT)
    {
        EXCEPTION(CStringFormat("Time out to query frame for cam %d", camId).Get());
    }
	else
	{	
		LOG_INFO(CStringFormat("QueryFramesDone for cam %d", camId).Get(), NULL);
	}
}


