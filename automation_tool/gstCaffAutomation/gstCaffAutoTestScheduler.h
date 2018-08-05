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

#ifndef __GST_CAFF_AUTO_TEST_SCHEDULER_H__
#define __GST_CAFF_AUTO_TEST_SCHEDULER_H__

#include "gstCaffAutoTestConfig.h"
#include "gstCaffAutoTestQueue.h"
#include "gstCaffAutoTestPerformance.h"
#include "gstCaffAutoTestTool.h"
#include "gstCaffAutoTestSampleCallback.h"
#include "EventControl.h"
#include "unistd.h"

using namespace gstCaff;

#ifndef PATH_MAX_LEN
#define PATH_MAX_LEN 512
#endif

enum E_ACTION_TYPE
{
    E_ACTION_TYPE_SET,
    E_ACTION_TYPE_QUERY,
};

typedef struct _S_FEAUTRE_ACTION
{
    E_FEATURE_TYPE feature_type;
    E_ACTION_TYPE action_type;
    int iCamera;
    int time_stamp;
    PARAMETER parameter;
} S_FEAUTRE_ACTION;

typedef S_FEAUTRE_ACTION S_FEAUTRE_SET;

typedef struct _S_FEAUTRE_QUERY : public _S_FEAUTRE_ACTION
{
    int frame_count;
    bool enable;
    vector<S_RAW_FRAME> frames;
}S_FEAUTRE_QUERY;


typedef struct _S_PHOTO_ACTION
{
    int iCamera;
    int time_stamp;
} S_PHOTO_ACTION;

typedef struct _S_PERF_ACTION
{
    int iCamera;
    char *config;
    int time_stamp;
} S_PERF_ACTION;

#define EXCUTE_RESULT_PASS 0
#define EXCUTE_RESULT_FAIL_BASIC 1
#define EXCUTE_RESULT_FAIL_PERFORMANCE 2
#define EXCUTE_RESULT_FAIL_FEATURE 4
#define EXCUTE_RESULT_FAIL_METADATA 8

class CScheduler
{
public:
    CScheduler(gstCaff::EventControl *eventControl);
    ~CScheduler();

    void SetCaseInfo(CCaseInfo *case_info);
    CCaseInfo* GetCaseInfo();
    const char* GetSensorName(size_t index);

    void UpdateCameraCommonSettings(MULTI_CAM_CONFIG *config);
    void UpdateCamera3AandIspSettings(MULTI_CAM_CONFIG *config);
    void ExecuteFeatureLoop(MULTI_CAM_CONFIG *config);
    int ExecuteConfig(MULTI_CAM_CONFIG *config, const char *logpath, const char *cam_names [], size_t cam_count, int preview_warm);
    int ExecuteAllConfigs(vector<MULTI_CAM_CONFIG> *pAllConfig, const char *logpath, const char *cam_names [], size_t cam_count, int preview_warm);
    void SetExeParams(const char *logpath, const char *cam_names [], int preview_warm);
    int ExecuteStress();
    int GetExeResult();
    int SetExeResult(int result);

    void StopAllCamera();

    CContenAnalyzerInterface* GetAnalyzerInterface(int icam);

    // Set Camera setting according to feature and input paramerter
    void SetCameraSettingByFeature(int cameraID, E_FEATURE_TYPE ft, PARAMETER p);

    static void StartTestExecutionProc(void *pthis);
    void SignalExecutionCompleted();
    bool WaitExecutionCompleted(int time_out_ms);

public:
    gstCaff::EventControl *m_eventControl;
    CPerformanceCheck m_Performance[MAX_CONCURRENT_CAM_COUNT];

    // Content analyzer interfaces
    CContenAnalyzerInterface *m_pContentAnalyzers[MAX_CONCURRENT_CAM_COUNT];
    CContenAnalyzerInterface *m_pGreenCorruptionAnalyzers[MAX_CONCURRENT_CAM_COUNT];
    CContenAnalyzerInterface *m_pOrientationAnalyzers[MAX_CONCURRENT_CAM_COUNT];
    CContenAnalyzerInterface *m_pColorCastAnalyzers[MAX_CONCURRENT_CAM_COUNT];
private:
    CCaseInfo *m_case_info;
    CFrameDispatchQueue *m_dispatch;

    char *m_logpath;
    char *m_cam_names[MAX_CONCURRENT_CAM_COUNT];
    int m_preview_warm;
    int m_result;

    static pthread_t  m_ThreadID;

    pthread_mutex_t m_mutex_lock;
    pthread_cond_t  m_cond;
};

#endif //__GST_CAFF_AUTO_TEST_SCHEDULER_H__