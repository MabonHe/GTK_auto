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

#ifndef __GST_CAFF_AUTO_TEST_QUEUE_H__
#define __GST_CAFF_AUTO_TEST_QUEUE_H__

#include "gstCaffAutoTestConfig.h"
#include <pthread.h>

#define MAX_ANALYZER_INPUT_QUEUE_SIZE 16

typedef struct _FRAME_NODE
{
    _FRAME_NODE():
        camera_index(0)
    {}
    int camera_index;
    E_FEATURE_TYPE feature_type;
    PARAMETER parameter;
    Mat frame;
    METADATA_INFO metadata;
    CContenAnalyzerInterface *analyzer;
    CContenAnalyzerInterface *analyzer_geen_corruption;
    CContenAnalyzerInterface *analyzer_orientation;
    CContenAnalyzerInterface *analyzer_color_cast;
}FRAME_NODE;

typedef struct _ANALYZER_INPUT_OUTPUT
{
    _ANALYZER_INPUT_OUTPUT():
        camera_index(0),
        ret(ANALYZER_RETRUN_FAIL),
        expected_ret(ANALYZER_RETRUN_PASS),
        feature_type(E_FEATURE_TYPE_NONE),
        analyzer(NULL)
    {}
    int camera_index;
    Mat img_debug;
    Mat img_reference;
    ANALYZER_RETRUN ret;
    ANALYZER_RETRUN expected_ret;
    E_FEATURE_TYPE feature_type;
    INPUT_IMAGE_ARRAY img_array;
    CContenAnalyzerInterface *analyzer;
}ANALYZER_INPUT_OUTPUT;

enum PREPARE_REFERENCE_RET
{
    PREPARE_REFERENCE_FRAME_DONE,
    PREPARE_REFERENCE_FRAME_NONE,
    PREPARE_REFERENCE_FRAME_NEED_MORE,
    PREPARE_REFERENCE_FRAME_BYPASS,
    PREPARE_REFERENCE_FRAME_NOT_MATCH
};

class CScheduler;

class CFrameDispatchQueue
{
public:
    CFrameDispatchQueue(CScheduler *parent);

    ~CFrameDispatchQueue();

    void DispatchFrame(FRAME_NODE &frame, MULTI_CAM_CONFIG *config);

    bool OnFrameArrival();

    bool ProcessFrame(FRAME_NODE &frame);

    void WaitForQueueEvent();

    bool GetQuitStatus()
    {
        return m_bQuit;
    }

    void ProcessThreadQuitCallback();

    void QuitProcessThread();

    deque<ANALYZER_INPUT_OUTPUT>* GetErrorAnalyzerOutput()
    {
        return &m_AnalyzerFailQueue;
    }

    int GetAnalyzerQueuePendingCount()
    {
        return (int) m_FrameQueue.size();
    }

protected:
    PREPARE_REFERENCE_RET PrepareReference(FRAME_NODE &frame);

private:
    deque<FRAME_NODE> m_ReferenceQueue[MAX_CONCURRENT_CAM_COUNT];
    deque<FRAME_NODE> m_FrameQueue;
    deque<ANALYZER_INPUT_OUTPUT> m_AnalyzerFailQueue;

    pthread_mutex_t m_mutex_lock;
    pthread_cond_t  m_QueueEvent;

    bool m_bQuit;

    pthread_t  m_ThreadID;

    CScheduler *m_Parent;

    Mat m_MutePattern;

    MULTI_CAM_CONFIG *m_current_config;
    PREPARE_REFERENCE_RET m_prepare_ref_status[MAX_CONCURRENT_CAM_COUNT];
};

#endif //__GST_CAFF_AUTO_TEST_QUEUE_H__