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

#include "gstCaffAutoTestScheduler.h"

// Global Var
extern int gWaitTimeMax;
extern int gCheckPoint;

pthread_t CScheduler::m_ThreadID = 0;


void* SchedulerThreadProc(void* lpParam )
{
    CScheduler *pScheduler = (CScheduler*) lpParam;

    int ret = -1;

    try
    {
        ret = pScheduler->ExecuteStress();
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        pScheduler->SetExeResult(-1);
        pScheduler->StopAllCamera();
    }

    pScheduler->SignalExecutionCompleted();

    return NULL;
}


bool CompareFeatureActionTimeStamp(S_FEAUTRE_ACTION *a1, S_FEAUTRE_ACTION *a2)
{
    return a1->time_stamp < a2->time_stamp;
}

bool ComparePerfActionTimeStamp(S_PERF_ACTION a1, S_PERF_ACTION a2)
{
    return a1.time_stamp < a2.time_stamp;
}

bool ComparePhotoActionTimeStamp(S_PHOTO_ACTION a1, S_PHOTO_ACTION a2)
{
    return a1.time_stamp < a2.time_stamp;
}

CScheduler::CScheduler(gstCaff::EventControl *eventControl) :
    m_eventControl(eventControl),
    m_dispatch(NULL),
    m_case_info(NULL),
    m_logpath(NULL),
    m_preview_warm(0)
{
    m_dispatch = new CFrameDispatchQueue(this);

    if (NULL == m_dispatch)
    {
        EXCEPTION("Failed to create CFrameDispatchQueue instance.");
    }

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        m_pContentAnalyzers[i] = NULL;
        m_pGreenCorruptionAnalyzers[i] = NULL;
        m_pOrientationAnalyzers[i] = NULL;
        m_pColorCastAnalyzers[i] = NULL;
    }

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        m_cam_names[i] = NULL;
    }

    pthread_mutex_init(&m_mutex_lock, NULL);

    pthread_cond_init( &m_cond , NULL);
}

CScheduler::~CScheduler()
{
    LOG_VERBOSE("Enter", NULL);
    if (NULL != m_dispatch)
    {
        delete m_dispatch;
        m_dispatch = NULL;
    }

    if (NULL != m_logpath)
    {
        free(m_logpath);
    }

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (m_cam_names[i] != NULL)
        {
            free(m_cam_names[i]);
        }

        if (m_pContentAnalyzers[i] != NULL)
        {
            FactoryDestroyAnalyzer(&m_pContentAnalyzers[i]);
        }

        if (m_pGreenCorruptionAnalyzers[i] != NULL)
        {
            FactoryDestroyAnalyzer(&m_pGreenCorruptionAnalyzers[i]);
        }

        if (m_pOrientationAnalyzers[i] != NULL)
        {
            FactoryDestroyAnalyzer(&m_pOrientationAnalyzers[i]);
        }
    }

    LOG_VERBOSE("Leave", NULL);
}

void CScheduler::SetCaseInfo(CCaseInfo *case_info)
{
    assert(case_info != NULL);
    m_case_info = case_info;
}

CCaseInfo* CScheduler::GetCaseInfo()
{
    return m_case_info;
}

const char* CScheduler::GetSensorName(size_t index)
{
    if (index >= MAX_CONCURRENT_CAM_COUNT)
    {
        return NULL;
    }

    return m_cam_names[index];
}

CContenAnalyzerInterface* CScheduler::GetAnalyzerInterface(int icam)
{
    assert(icam >= 0 && icam < MAX_CONCURRENT_CAM_COUNT);

    return m_pContentAnalyzers[icam];
}

void CScheduler::SetCameraSettingByFeature(int cameraID, E_FEATURE_TYPE ft, PARAMETER p)
{
    status_t status = 0;
    switch (ft)
    {
    case E_FEATURE_TYPE_BRIGHTNESS:
    case E_FEATURE_TYPE_CONTRAST:
    case E_FEATURE_TYPE_HUE:
        //m_eventControl->setAeMode(cameraID, "manual");
        //m_eventControl->setAwbMode(cameraID, "manual_gain");
        break;
    case E_FEATURE_TYPE_EXPOSURE_TIME:
    case E_FEATURE_TYPE_EXPOSURE_GAIN:
    case E_FEATURE_TYPE_EXPOSURE_EV:
        //m_eventControl->setAwbMode(cameraID, "manual_gain");
        break;
    case E_FEATURE_TYPE_WB_MODE:
    case E_FEATURE_TYPE_WB_MANUAL_GAIN:
    case E_FEATURE_TYPE_WB_CCT_RANGE:
        //m_eventControl->setAeMode(cameraID, "manual");
        break;
    case E_FEATURE_TYPE_HDR:
    case E_FEATURE_TYPE_SIMILARITY:
    case E_FEATURE_TYPE_CONTENT_CONSISTENT:
    case E_FEATURE_TYPE_3A_AUTO_CONSISTENT:
    case E_FEATURE_TYPE_AE_BRACKETING:
    case E_FEATURE_TYPE_ULL:
        break;
    }
    LOG_INFO(CStringFormat("set manual settings for cam %d", cameraID).Get(), NULL);
    switch (ft)
    {
    case E_FEATURE_TYPE_BRIGHTNESS:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        LOG_INFO(CStringFormat("setBrightness %d", p.val.i).Get(), NULL);
        status = m_eventControl->setBrightness(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_CONTRAST:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        LOG_INFO(CStringFormat("setContrast %d", p.val.i).Get(), NULL);
        status = m_eventControl->setContrast(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_SATURATION:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        LOG_INFO(CStringFormat("setSaturation %d", p.val.i).Get(), NULL);
        status = m_eventControl->setSaturation(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
     case E_FEATURE_TYPE_SHARPNESS:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        LOG_INFO(CStringFormat("setSharpness %d", p.val.i).Get(), NULL);
        status = m_eventControl->setSharpness(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_EXPOSURE_TIME:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        LOG_INFO("setAeMode manual", NULL);
        status = m_eventControl->setAeMode(cameraID, "manual");
        if (status != NO_ERROR)
        {
            EXCEPTION("Set AE mode manual failed");
        }

        LOG_INFO("setExposureGain 10", NULL);
        status = m_eventControl->setExposureGain(cameraID, 10.0f);

        if (status != NO_ERROR)
        {
            EXCEPTION("Failed to set exposure gain");
        }

        LOG_INFO(CStringFormat("setExposureTime %d", p.val.i).Get(), NULL);
        status = m_eventControl->setExposureTime(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_EXPOSURE_GAIN:
        if (p.type != PARAM_FLOAT)
        {
            EXCEPTION("Expect parameter type is FLOAT");
        }

        if (p.val.f < 1.0f || p.val.f > 60.0f)
        {
            EXCEPTION("Expect parameter range [1, 60]");
        }

        LOG_INFO("setAeMode manual", NULL);
        status = m_eventControl->setAeMode(cameraID, "manual");
        if (status != NO_ERROR)
        {
            EXCEPTION("Set AE mode manual failed");
        }

        LOG_INFO("setExposureTime 1000", NULL);
        status = m_eventControl->setExposureTime(cameraID, 1000);

        if (status != NO_ERROR)
        {
            EXCEPTION("Failed to set exposure time");
        }

        LOG_INFO(CStringFormat("setExposureGain %f", p.val.f).Get(), NULL);
        status = m_eventControl->setExposureGain(cameraID, p.val.f);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_EXPOSURE_EV:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        if (p.val.i < -4 || p.val.i > 4)
        {
            EXCEPTION("Expect parameter range [-4, 4]");
        }

        LOG_INFO(CStringFormat("setExposureEv %d", p.val.i).Get(), NULL);
        status = m_eventControl->setExposureEv(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_HUE:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        LOG_INFO(CStringFormat("setHue %d", p.val.i).Get(), NULL);
        status = m_eventControl->setHue(cameraID, p.val.i);

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_WB_MODE:
    case E_FEATURE_TYPE_WB_CCT_RANGE:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }

        if (p.val.i >= GST_CAMERASRC_AWB_MODE_PARTLY_OVERCAST &&
            p.val.i <= GST_CAMERASRC_AWB_MODE_DAYLIGHT)
        {
            GstCamerasrcAwbMode awb_mode = (GstCamerasrcAwbMode)p.val.i;
            switch (awb_mode)
            {
            case GST_CAMERASRC_AWB_MODE_PARTLY_OVERCAST:
                LOG_INFO("setAwbMode partly_overcast", NULL);
                status = m_eventControl->setAwbMode(cameraID, "partly_overcast");
                break;
            case GST_CAMERASRC_AWB_MODE_FULLY_OVERCAST:
                LOG_INFO("setAwbMode fully_overcast", NULL);
                status = m_eventControl->setAwbMode(cameraID, "fully_overcast");
                break;
            case GST_CAMERASRC_AWB_MODE_FLUORESCENT:
                LOG_INFO("setAwbMode fluorescent", NULL);
                status = m_eventControl->setAwbMode(cameraID, "fluorescent");
                break;
            case GST_CAMERASRC_AWB_MODE_INCANDESCENT:
                LOG_INFO("setAwbMode incandescent", NULL);
                status = m_eventControl->setAwbMode(cameraID, "incandescent");
                break;
            case GST_CAMERASRC_AWB_MODE_SUNSET:
                LOG_INFO("setAwbMode sunset", NULL);
                status = m_eventControl->setAwbMode(cameraID, "sunset");
                break;
            case GST_CAMERASRC_AWB_MODE_VIDEO_CONFERENCING:
                LOG_INFO("setAwbMode video_conferencing", NULL);
                status = m_eventControl->setAwbMode(cameraID, "video_conferencing");
                break;
            case GST_CAMERASRC_AWB_MODE_DAYLIGHT:
                LOG_INFO("setAwbMode daylight", NULL);
                status = m_eventControl->setAwbMode(cameraID, "daylight");
                break;
            }
        }
        else if (p.val.i >= 1800 && p.val.i <= 15000)
        {
            LOG_INFO("setAwbMode cct_range", NULL);
            status = m_eventControl->setAwbMode(cameraID, "cct_range");
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }

            LOG_INFO(CStringFormat("setAwbRange %d~%d", p.val.i-100, p.val.i+100).Get(), NULL);
            status = m_eventControl->setAwbRange(cameraID,
                (char*)CStringFormat("%d~%d", p.val.i-100, p.val.i+100).Get());
        }
        else
        {
            EXCEPTION("Error WB setting value");
        }

        if (status != NO_ERROR)
        {
            EXCEPTION("Event control setting failed");
        }
        break;
    case E_FEATURE_TYPE_WB_MANUAL_GAIN:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }
        else
        {
            int bgain = p.val.i & 0xFF;
            int ggain = (p.val.i >> 8) & 0xFF;
            int rgain = (p.val.i >> 16) & 0xFF;

            LOG_INFO("setAwbMode manual_gain", NULL);
            status = m_eventControl->setAwbMode(cameraID, "manual_gain");
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }

            LOG_INFO(CStringFormat("setAwbRGain %d", rgain).Get(), NULL);
            status = m_eventControl->setAwbRGain(cameraID, rgain);
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }

            LOG_INFO(CStringFormat("setAwbGGain %d", ggain).Get(), NULL);
            status = m_eventControl->setAwbGGain(cameraID, ggain);
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }

            LOG_INFO(CStringFormat("setAwbBGain %d", bgain).Get(), NULL);
            status = m_eventControl->setAwbBGain(cameraID, bgain);
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }
        }
        break;
    case E_FEATURE_TYPE_WB_SHIFT:
        if (p.type != PARAM_INT)
        {
            EXCEPTION("Expect parameter type is int");
        }
        else
        {
            int bshift = p.val.i & 0xFF;
            int gshift = (p.val.i >> 8) & 0xFF;
            int rshift = (p.val.i >> 16) & 0xFF;

            LOG_INFO(CStringFormat("setAwbShiftR %d", rshift).Get(), NULL);
            status = m_eventControl->setAwbShiftR(cameraID, rshift);
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }

            LOG_INFO(CStringFormat("setAwbShiftG %d", gshift).Get(), NULL);
            status = m_eventControl->setAwbShiftG(cameraID, gshift);
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }

            LOG_INFO(CStringFormat("setAwbShiftB %d", bshift).Get(), NULL);
            status = m_eventControl->setAwbShiftB(cameraID, bshift);
            if (status != NO_ERROR)
            {
                EXCEPTION("Event control setting failed");
            }
        }
        break;
    case E_FEATURE_TYPE_HDR:
    case E_FEATURE_TYPE_SIMILARITY:
    case E_FEATURE_TYPE_CONTENT_CONSISTENT:
    case E_FEATURE_TYPE_SHUTTER_SPEED:
    case E_FEATURE_TYPE_BIAS:
    case E_FEATURE_TYPE_ISO:
    case E_FEATURE_TYPE_FLASH_MODE:
    case E_FEATURE_TYPE_3A_AUTO_CONSISTENT:
    case E_FEATURE_TYPE_AE_BRACKETING:
    case E_FEATURE_TYPE_ULL:
    case E_FEATURE_TYPE_GREEN_CORRUPTION_DETECTION:
        break;
    }
}

void CScheduler::ExecuteFeatureLoop(MULTI_CAM_CONFIG *config)
{
    vector<S_FEAUTRE_ACTION*> action_list;

    long int llTimeMax[MAX_CONCURRENT_CAM_COUNT] = {1};
    long int llTimeAcc = 0;

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        CAM_CONFIG *pCamConfig = &config->cam[i];
        S_FEATURE_SETTING *fs = &config->cam[i].feature;
        S_FEATURE_ANALYZER fa = {};

        if (!pCamConfig->enable)
        {
            continue;
        }

        if (fs->enable)
        {
            if (!fs->GetAnalyzerByFeatureName(fa))
            {
                LOG_VERBOSE(CStringFormat("Feature selection index is %d, name is %lf", fa.feature_type, fa.feature_name).Get(), NULL);
                throw CException("%s %d: Error feature name!", __FILE__, __LINE__);
            }

            if (fs->loop_mode && gCheckPoint >= 2)
            {
                llTimeMax[i] = pCamConfig->duration;
            }

            // Create Analyzers
            m_pContentAnalyzers[i] = FactoryCreateAnalyzer(fa.analyzer);
            if (NULL == m_pContentAnalyzers)
            {
                EXCEPTION("Failed to create content analyzer!");
            }

            if (pCamConfig->analyzer.analyzer_config == NULL ||
                strcmp(pCamConfig->analyzer.analyzer_config, "null") == 0)
            {
                LOG_WARNING("Analyzer config file is NULL", NULL);
            }
            else if (!m_pContentAnalyzers[i]->LoadAlgorithmParameters(pCamConfig->analyzer.analyzer_config))
            {
                EXCEPTION(CStringFormat("Failed to load analyzer config file: %s",
                    pCamConfig->analyzer.analyzer_config).Get());
            }

            // Insert all settings and Qurrey frames
            vector<PARAMETER> parameters;
            if (!fs->GetValue(parameters))
            {
                EXCEPTION("Failed to load feature setting values!");
            }

            for (size_t j = 0; j < parameters.size(); j++)
            {
                S_FEAUTRE_SET *pSet = new S_FEAUTRE_SET;
                pSet->iCamera = i;
                pSet->action_type = E_ACTION_TYPE_SET;
                pSet->feature_type = fa.feature_type;
                pSet->time_stamp = (int)(j+1) * fs->duration_each;
                pSet->parameter = parameters[j];

                // Push Set
                action_list.push_back(pSet);

                S_FEAUTRE_QUERY *pQuery = new S_FEAUTRE_QUERY;
                pQuery->iCamera = i;
                pQuery->action_type = E_ACTION_TYPE_QUERY;
                pQuery->feature_type = fa.feature_type;
                pQuery->time_stamp = (int)(j+1) * fs->duration_each + fs->duration_each / 2;
                pQuery->frame_count = fs->frame_count_each;
                pQuery->parameter = parameters[j];

                if (pCamConfig->analyzer.enable)
                {
                    pQuery->enable = true;
                }
                else
                {
                    pQuery->enable = false;
                }

                // Push Query
                action_list.push_back(pQuery);
            }
        }
    }

    // Sort the action list
    std::sort(action_list.begin(), action_list.end(), CompareFeatureActionTimeStamp);


    timeval time_start = {0};
    timeval time_end = {0};
    while (action_list.size() > 0)
    {
        gettimeofday(&time_start, NULL);

        for (size_t i = 0; i < action_list.size(); i++)
        {
            if (action_list[i]->action_type == E_ACTION_TYPE_SET)
            {
                S_FEAUTRE_SET *pSet = (S_FEAUTRE_SET *) action_list[i];
                SetCameraSettingByFeature(pSet->iCamera, pSet->feature_type, pSet->parameter);
            }
            else if (action_list[i]->action_type == E_ACTION_TYPE_QUERY)
            {
                S_FEAUTRE_QUERY *pQuery = (S_FEAUTRE_QUERY *) action_list[i];

                if (pQuery->enable)
                {
                    StartQueryFrames(&(pQuery->frames), pQuery->frame_count, pQuery->iCamera);

                    WaitQueryFramesDone(pQuery->frame_count, pQuery->iCamera);

                    if (pQuery->frames.size() != pQuery->frame_count)
                    {
						char* e_msg;
						sprintf(e_msg, "Frame count is not same as expected! Frames got: %ld, frames expected:%d",pQuery->frames.size(), pQuery->frame_count);
						EXCEPTION(e_msg);
                    }

                    for (int j = 0; j < pQuery->frame_count; j++)
                    {
                        FRAME_NODE fn;
                        fn.camera_index = pQuery->iCamera;
                        fn.feature_type = pQuery->feature_type;
                        fn.analyzer = m_pContentAnalyzers[fn.camera_index];
                        fn.analyzer_geen_corruption = m_pGreenCorruptionAnalyzers[fn.camera_index];
                        fn.analyzer_orientation = m_pOrientationAnalyzers[fn.camera_index];
                        fn.analyzer_color_cast = m_pColorCastAnalyzers[fn.camera_index];

                        ConvertBufferToMat(pQuery->frames[j], fn.frame);
                        //fn.metadata = pQuery->frames[mode][j].metadata;
                        fn.parameter = pQuery->parameter;

                        // Dispatch frames
                        m_dispatch->DispatchFrame(fn, config);

                        if (NULL != pQuery->frames[j].data_ptr)
                        {
                            free(pQuery->frames[j].data_ptr);
                        }
                    }

                    pQuery->frames.clear();
                }

            }

            gettimeofday(&time_end, NULL);

            int time_stamp = (int) ((time_end.tv_sec - time_start.tv_sec) * 1000 +
                (time_end.tv_usec - time_start.tv_usec) / 1000);

            // For multi-cam cases,if action_type is the same for the next action,we should set all if them.one by one instantly.
            // if action type is not the same, or this is last action, we should wait for the right time before move on.
            // if the time is already over the pre-defined time, just ajust the time.
            if ( i == (action_list.size()-1) || (action_list[i]->action_type !=  action_list[i+1]->action_type))
            {
                if (time_stamp < action_list[i]->time_stamp)
                {
                    LOG_INFO(CStringFormat("wait then move on after %d actions taken", i+1).Get(), NULL);
                    usleep((action_list[i]->time_stamp - time_stamp)*1000);
                    llTimeAcc += action_list[i]->time_stamp;
                }
                else // if action is over expected time, adjust the start point
                {
                    LOG_INFO(CStringFormat("Time over after %d actions taken", i+1).Get(), NULL);
                    int delta_ms = time_stamp - action_list[i]->time_stamp;

                    time_start.tv_sec += delta_ms / 1000;
                    time_start.tv_usec += delta_ms * 1000;

                    if (time_start.tv_usec > 1000000000)
                    {
                        time_start.tv_sec += time_start.tv_usec / 1000000000;
                        time_start.tv_usec = time_start.tv_usec % 1000000000;
                    }

                    llTimeAcc += time_stamp;
                }
            }
        }

        // Remove those out-of-date actions
        for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
        {
            if (llTimeAcc >= llTimeMax[i])
            {
                for (vector<S_FEAUTRE_ACTION*>::iterator it = action_list.begin(); it != action_list.end(); )
                {
                    if ((*it)->iCamera == i)
                    {
                        if ((*it) != NULL)
                        {
                            delete (*it);
                        }
                        it = action_list.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }
        }
    }

}

/*
void CScheduler::CaptureAndDeliverMaxStillIfNeeded(MULTI_CAM_CONFIG *config, const char *path, int preview_warm)
{
    // Take max resolution images for FOV and similarity check
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        S_FEATURE_ANALYZER sFA = {};
        config->cam[i].feature.GetAnalyzerByFeatureName(sFA);
        CMFCamera *pCamera = m_dual_cam->GetCamera(i);

        CAMERA_MODE_CONFIG *CAMERA_MODE_CONFIGs [] =
        {
            &config->cam[i].preview_config,
            &config->cam[i].video_config,
            &config->cam[i].still_config
        };

        if (pCamera != NULL &&
            (sFA.feature_type == E_FEATURE_TYPE_FOV ||
            sFA.feature_type == E_FEATURE_TYPE_SIMILARITY))
        {
            Mat fullfov = pCamera->TakeMaxResPhoto(path, preview_warm);

            for (int mode = 0; mode < 3; mode++)
            {
                if (!CAMERA_MODE_CONFIGs[mode]->analyzer.enable)
                {
                    break;
                }

                FRAME_NODE fn;
                fn.camera_index = i;
                fn.feature_type = sFA.feature_type;
                fn.analyzer = pCamera->GetAnalyzer((CAMERA_MODE) mode);
                fn.analyzer_geen_corruption = pCamera->GetGreenCorruptionAnalyzer((CAMERA_MODE) mode);
                fn.mode = (CAMERA_MODE) mode;
                fn.frame = fullfov;
                fn.parameter.type = PARAM_INT;
                fn.parameter.val.i = 0;

                m_dispatch->DispatchFrame(fn, config);
            }
        }
    }
}
*/

void CScheduler::UpdateCameraCommonSettings(MULTI_CAM_CONFIG *config)
{
    int nEnabledCameraCount = 0;

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable)
        {
            continue;
        }
        nEnabledCameraCount++;
    }

    if (nEnabledCameraCount > 1)
    {
        m_eventControl->updateUICameraNum(nEnabledCameraCount);
    }

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable)
        {
            continue;
        }

        // 4 x virtual channle case
        if (nEnabledCameraCount == 4)
        {

            if (strcmp("dma_import",config->cam[i].common_config.io_mode) == 0)
            {
                m_eventControl->setConvertElementType(i, "vaapipostproc");
                m_eventControl->setSinkElementType(i, "vaapisink");
            }else
            {
                m_eventControl->setConvertElementType(i, "videoconvert");
                m_eventControl->setSinkElementType(i, "ximagesink");

            }

            m_eventControl->setVcEnable(i, true);
        }
        // 2 x cvbs analog camera case
        if (nEnabledCameraCount == 2)
        {
            m_eventControl->setConvertElementType(i, "vaapipostproc");
            m_eventControl->setSinkElementType(i, "vaapisink");
        }

        if (nEnabledCameraCount == 1)
        {
            m_eventControl->setConvertElementType(i, "vaapipostproc");
            m_eventControl->setSinkElementType(i, "vaapisink");
        }
        CAMERA_COMMON_CONFIG *pCommonConfig = &config->cam[i].common_config;
        if (pCommonConfig->enable)
        {
            //m_eventControl->setDeviceName(i, pCommonConfig->device_name);
            if (pCommonConfig->io_mode != NULL)
            {
                LOG_INFO(CStringFormat("setIoMode %s", pCommonConfig->io_mode).Get(), NULL);
                m_eventControl->setIoMode(i, pCommonConfig->io_mode);
            }

            m_eventControl->setResolution(i, pCommonConfig->width, pCommonConfig->height);
            m_eventControl->setInterlaceMode(i, strcmp(pCommonConfig->interlace_mode, "alternate") == 0);

            if (pCommonConfig->deinterlace_method != NULL)
            {
                m_eventControl->setDeInterlace(i, pCommonConfig->deinterlace_method);
            }

            if (pCommonConfig->pixelformat != NULL)
            {
                m_eventControl->setFormat(i, pCommonConfig->pixelformat);
            }

            if (pCommonConfig->framerate != NULL)
            {
                m_eventControl->setFramerate(i, pCommonConfig->framerate);
            }

            if (pCommonConfig->fisheye_mode != NULL)
            {
                printf("set fisheye_mode%s\n",pCommonConfig->fisheye_mode);
                m_eventControl->setFisheyeMode(i,pCommonConfig->fisheye_mode);
            }
            m_eventControl->setNumBuffers(i, pCommonConfig->num_buffers);
        }
     }
}

void CScheduler::UpdateCamera3AandIspSettings(MULTI_CAM_CONFIG *config)
{
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable)
        {
            continue;
        }

        CAMERA_3A_CONTROL_CONFIG *p3AConfig = &config->cam[i].cam_3a_control_config;

        if (p3AConfig->enable)
        {
            if (p3AConfig->iris_mode != NULL)
            {
                m_eventControl->setIrisMode(i, p3AConfig->iris_mode);
            }

            m_eventControl->setIrisLevel(i, p3AConfig->iris_level);

            if (p3AConfig->ae_mode != NULL)
            {
                m_eventControl->setAeMode(i, p3AConfig->ae_mode);
            }

            m_eventControl->setExposureTime(i, p3AConfig->exposure_time);
            m_eventControl->setExposureGain(i, p3AConfig->gain);
            m_eventControl->setExposureEv(i, p3AConfig->ev);

            if (p3AConfig->awb_mode != NULL)
            {
                m_eventControl->setAwbMode(i, p3AConfig->awb_mode);
            }

            m_eventControl->setAwbRGain(i, p3AConfig->awb_gain_r);
            m_eventControl->setAwbGGain(i, p3AConfig->awb_gain_g);
            m_eventControl->setAwbBGain(i, p3AConfig->awb_gain_b);
            //m_eventControl->setAwbRange(i, p3AConfig->aw);
            m_eventControl->setAwbShiftR(i, p3AConfig->awb_shift_r);
            m_eventControl->setAwbShiftG(i, p3AConfig->awb_shift_g);
            m_eventControl->setAwbShiftB(i, p3AConfig->awb_shift_b);

            if (p3AConfig->wp_point != NULL)
            {
                m_eventControl->setAwbWp(i, p3AConfig->wp_point);
            }

            /* // Disable it as feature not ready
            if (p3AConfig->day_night_mode != NULL)
            {
                m_eventControl->setDayNightMode(i, p3AConfig->day_night_mode);
            }
            */
            //m_eventControl->setWindowSize(i, p3AConfig->w);
            m_eventControl->setSharpness(i, p3AConfig->sharpness);
            m_eventControl->setBrightness(i, p3AConfig->brightness);
            m_eventControl->setContrast(i, p3AConfig->contrast);
            m_eventControl->setHue(i, p3AConfig->hue);
            m_eventControl->setSaturation(i, p3AConfig->saturation);
            /* wdr-mode is not used any more. use scene-mode
            if (p3AConfig->wdr_mode != NULL)
            {
                m_eventControl->setWdrMode(i, p3AConfig->wdr_mode);
            }
            */
            //m_eventControl->setWdrLevel(i, p3AConfig->wdr_level);

            if (p3AConfig->blc_area_mode != NULL)
            {
                m_eventControl->setBlcMode(i, p3AConfig->blc_area_mode);
            }
            //m_eventControl->setBlcAreaLevel(i, p3AConfig->b);
            if (p3AConfig->nr_mode != NULL)
            {
                m_eventControl->setNoiseReductionMode(i, p3AConfig->nr_mode);
            }

            //m_eventControl->setNoiseFilter(i, p3AConfig->nr_filter_level);
            //m_eventControl->setSpatialLevel(i, p3AConfig->exposure_time);
            //m_eventControl->setTemporalLevel(i, p3AConfig->exposure_time);
            //m_eventControl->setMirrorEffectMode(i, p3AConfig-);

            if (p3AConfig->scene_mode != NULL)
            {
                m_eventControl->setSceneMode(i, p3AConfig->scene_mode);
                if (strcmp(p3AConfig->scene_mode, "hdr") == 0)
				    m_eventControl->setWdrLevel(i, p3AConfig->wdr_level);
            }
            //m_eventControl->setSensorMode(i, p3AConfig->s);
            //m_eventControl->setCaptureFps(i, p3AConfig->);
        }
    }
}


int CScheduler::ExecuteConfig(MULTI_CAM_CONFIG *config, const char *logpath,
    const char *cam_names [], size_t cam_count, int preview_warm)
{
    int retval = EXCUTE_RESULT_PASS;

    vector<S_PERF_ACTION> perf_action_list;

    vector<S_PHOTO_ACTION> photo_action_list;

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable)
        {
            continue;
        }
		m_eventControl->waitSem();
        if (0 != m_eventControl->setDeviceName(i, (char*)cam_names[i]))
        {
            EXCEPTION("Failed to set device name");
        }
		m_eventControl->postSem();
        if (config->cam[i].perf.enable)
        {
            S_PERF_ACTION perf_action = {};
            perf_action.iCamera = i;
            perf_action.time_stamp = config->cam[i].duration;
            perf_action.config = config->cam[i].perf.analyzer_config;
            perf_action_list.push_back(perf_action);
        }

        // Create green corruption analyzers

        if (config->cam[i].green_corruption_analyzer.enable)
        {

            m_pGreenCorruptionAnalyzers[i] = FactoryCreateAnalyzer(ANALYZER_GREEN_CORRUPTION);

            if (NULL == m_pGreenCorruptionAnalyzers[i])
            {
                EXCEPTION("Failed to create color cast analyzer");
            }

            m_pGreenCorruptionAnalyzers[i]->LoadAlgorithmParameters(
                config->cam[i].green_corruption_analyzer.analyzer_config);
        }
        else
        {
            LOG_INFO("after postsem control", NULL);

            FactoryDestroyAnalyzer(&m_pColorCastAnalyzers[i]);
        }

        // Create green corruption analyzers
        if (config->cam[i].orientation_analyzer.enable)
        {
            m_pOrientationAnalyzers[i] = FactoryCreateAnalyzer(ANALYZER_CAMERA_POSITION_COMPARE);

            if (NULL == m_pOrientationAnalyzers[i])
            {
                EXCEPTION("Failed to create green corruption analyzer");
            }

            m_pOrientationAnalyzers[i]->LoadAlgorithmParameters(
                config->cam[i].orientation_analyzer.analyzer_config);
        }
        else
        {
            FactoryDestroyAnalyzer(&m_pOrientationAnalyzers[i]);
        }
        LOG_INFO("after postsem control", NULL);

        if (config->cam[i].color_cast_analyzer.enable)
        {
            LOG_INFO("enter color cast",NULL);
            m_pColorCastAnalyzers[i] = FactoryCreateAnalyzer(ANALYZER_COLORCAST_COMPARE);

            if (NULL == m_pColorCastAnalyzers[i])
            {
                EXCEPTION("Failed to create color cast analyzer");
            }

            m_pColorCastAnalyzers[i]->LoadAlgorithmParameters(
                config->cam[i].color_cast_analyzer.analyzer_config);
        }
        else
        {
            FactoryDestroyAnalyzer(&m_pColorCastAnalyzers[i]);
        }
    }



    sort(perf_action_list.begin(), perf_action_list.end(), ComparePerfActionTimeStamp);

    // Update Camera settings
    UpdateCameraCommonSettings(config);
    UpdateCamera3AandIspSettings(config);

    //CaptureAndDeliverMaxStillIfNeeded(config, logpath, preview_warm);

    // Config all pins
    //m_dual_cam->ConfigAllCameraAllPins(config);

    // Start streaming for all cameras
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable)
        {
            continue;
        }
        m_eventControl->waitSem();
        m_eventControl->setStreamType(i, STREAM_TYPE_PREVIEW);
        m_eventControl->setCameraState(i, CAMERA_STATE_START);
        m_eventControl->postSem();
    }

    if (preview_warm > 0)
    {
        // Wait for AE converage
        usleep(preview_warm * 1000);
    }


    LOG_INFO("Checking 3A convergency time.", NULL);

    // Check 3A cnonvergency status
    /*
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable || !config->cam[i].convergency_3a.enable)
        {
            continue;
        }
    }
    */

    CTimer timer;
    timer.Start();

    // Feature Loop: set, query, dispatch, and analyze
    ExecuteFeatureLoop(config);

    // Wait all the queued tasks processed
    while (!m_dispatch->GetQuitStatus() && m_dispatch->GetAnalyzerQueuePendingCount() > 0)
    {
        LOG_INFO("Waiting analysis in queue completion", NULL);
        usleep(250000);
    }

    deque<ANALYZER_INPUT_OUTPUT>* pErrorOut = m_dispatch->GetErrorAnalyzerOutput();

    char path[PATH_MAX_LEN] = {};

    if (pErrorOut->size() > 0)
    {
        if (!CreateDirectory((char*)logpath))
        {
            EXCEPTION("Failed to CreateDirectory");
        }

        for (deque<ANALYZER_INPUT_OUTPUT>::iterator it = pErrorOut->begin(); it != pErrorOut->end(); it++)
        {
            int icam = it->camera_index;
            CContenAnalyzerInterface *anlyzer = it->analyzer;
            E_FEATURE_TYPE ft = it->feature_type;

            // Output Input images
            for (size_t j = 0; j < it->img_array.size(); j++)
            {
                // image name = SensorName+ModeName+Resolution+Format+FeatureName+AnalyzerName+InputParameter
                int w = it->img_array[j].img.cols;
                int h = it->img_array[j].img.rows;
                PARAMETER p = it->img_array[j].param;

                int n = snprintf(path, PATH_MAX_LEN, "%s/%s_%dx%d_%s_%s_%d", logpath,
                    cam_names[icam], w, h, GetFeatureNameByFeatureType(ft),
                    anlyzer->GetContenAnalyzerName(), j);

                switch (p.type)
                {
                case PARAM_INT:
                    snprintf(path + n, PATH_MAX_LEN - n, "_%d.bmp", p.val.i);
                    break;
                case PARAM_FLOAT:
                    snprintf(path + n, PATH_MAX_LEN - n, "_%f.bmp", p.val.f);
                    break;
                case PARAM_DOUBLE:
                    snprintf(path + n, PATH_MAX_LEN - n, "_%lf.bmp", p.val.d);
                    break;
                }

                imwrite(path, it->img_array[j].img);
            }

            // Ouput debug and reference image if any
            if (!it->img_debug.empty())
            {
                snprintf(path, PATH_MAX_LEN, "%s/%s_%s_%s_debug.jpg", logpath,
                    cam_names[icam], GetFeatureNameByFeatureType(ft),
                    anlyzer->GetContenAnalyzerName());

                imwrite(path, it->img_debug);
            }

            if (!it->img_reference.empty())
            {
                snprintf(path, PATH_MAX_LEN, "%s/%s_%s_%s_reference.jpg", logpath,
                    cam_names[icam], GetFeatureNameByFeatureType(ft),
                    anlyzer->GetContenAnalyzerName());

                imwrite(path, it->img_reference);
            }
        }

        retval |= EXCUTE_RESULT_FAIL_FEATURE;
    }

    // Calculate time and Stop performance track here
    int n = snprintf(path, PATH_MAX_LEN, "%s/../FeatureTestPerformanceRecord.csv", logpath);
    if (n <= 0)
    {
        EXCEPTION("Failed to format path");
    }

    if (perf_action_list.size() > 0)
    {
        FILE *fp = NULL;
        fp = fopen(path, "a+");
        if (fp == NULL)
        {
            EXCEPTION(CStringFormat("Failed to open file %s", path).Get());
        }

        for (size_t i = 0; i < perf_action_list.size(); i++)
        {
            int icam = perf_action_list[i].iCamera;

            if (!m_Performance[icam].LoadParameters(perf_action_list[i].config))
            {
                fclose(fp);
                throw CException("Failed to load performance parameters: %s", perf_action_list[i].config);
            }

            int ts = (int) timer.GetTimeDuration();

            if (ts < perf_action_list[i].time_stamp)
            {
                usleep((perf_action_list[i].time_stamp - ts) * 1000);
            }

            PERFORMANCE_RESULT pefret = m_Performance[icam].CheckResult();

            // If no frame recieved, should be the basic function check failed.
            if (pefret == PERFORMANCE_RESULT_FAIL_NO_FRAME)
            {
                m_case_info->AddBasicErrorMessage(CStringFormat("<%s> No frame recieved in sample callback",
                    cam_names[icam]).Get());
                retval |= EXCUTE_RESULT_FAIL_BASIC;
            }

            CStringFormat msg("<%s>  Performance check return %d",
                cam_names[icam], pefret);

            if (pefret != PERFORMANCE_RESULT_PASS)
            {
                m_case_info->AddPerformanceErrorMessage(msg.Get());

                LOG_ERROR(msg.Get(), NULL);
                retval |= EXCUTE_RESULT_FAIL_PERFORMANCE;
            }
            else
            {
                LOG_INFO(msg.Get(), NULL);
            }

            CAM_CONFIG *cam_config = &config->cam[icam];

            fprintf(fp, "%s, %dx%d, %s, %s, %lf, %lf, %lf, %lf, %lf\n",
                cam_names[icam],
                cam_config->common_config.width,
                cam_config->common_config.height,
                cam_config->common_config.pixelformat,
                cam_config->common_config.framerate,
                m_Performance[icam].m_AverageFPS,
                m_Performance[icam].m_TimeInit + m_Performance[icam].m_TimeConfig +
                m_Performance[icam].m_TimeStart + m_Performance[icam].m_TimeStart2FirstFrame,
                m_Performance[icam].m_MaxInterval,
                m_Performance[icam].m_MinInterval,
                m_Performance[icam].m_StdInterval);
        }

        fprintf(fp, "\n");

        if (fp != NULL)
        {
            fclose(fp);
            fp = NULL;
        }
    }

    // Check duration
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        if (!config->cam[i].enable)
        {
            continue;
        }

        int ts = (int) timer.GetTimeDuration();
        LOG_INFO(CStringFormat("Total duration for camera%d: %dms", i, ts).Get(), NULL);
        if (ts < config->cam[i].duration)
        {
            usleep((config->cam[i].duration - ts) * 1000);
        }

        if (0 != m_eventControl->setCameraState(i, CAMERA_STATE_STOP))
        {
            EXCEPTION("Failed to stop camera.");
        }
    }

    // Check Sample Callback return value
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        // @todo
    }

bypass:
    return retval;
}

void CScheduler::StopAllCamera()
{
    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        m_eventControl->setCameraState(i, CAMERA_STATE_STOP);
    }
}


int CScheduler::ExecuteAllConfigs(vector<MULTI_CAM_CONFIG> *pAllConfig, const char *logpath, const char *cam_names [], size_t cam_count, int preview_warm)
{
    int ret = EXCUTE_RESULT_PASS;

    CStringFormat msg;

    for (size_t i = 0; i < pAllConfig->size(); i++)
    {
        LOG_INFO(CStringFormat("Start config %d", i).Get(), NULL);

        // Create Cameras Instances
        //m_dual_cam->CreateCameraInstances(&(*pAllConfig)[i], cam_names, cam_count);

        // Execute single config
        int reti = ExecuteConfig(&(*pAllConfig)[i], logpath, cam_names, cam_count, preview_warm);

        if (EXCUTE_RESULT_PASS != reti)
        {
            LOG_ERROR(CStringFormat("Failed at config %d", i).Get(), NULL);
        }

        ret |= reti;
    }

    return ret;
}

int CScheduler::ExecuteStress()
{
    CASE_CONFIG *case_config = m_case_info->GetCaseConfig();

    assert(case_config != NULL);

    m_result = EXCUTE_RESULT_PASS;
    STRESS_CONFIG *pStressConfig = &case_config->stress_config;

    if (!pStressConfig->enable || pStressConfig->loop_count <= 0)
    {
        pStressConfig->loop_count = 1;
    }

    // nloop
    for (int iloop = 0; iloop < pStressConfig->loop_count; iloop++)
    {
        m_case_info->SetLoopCountCompleted(iloop + 1);
        LOG_INFO(CStringFormat("<Loop Count> %d", iloop + 1).Get(), NULL);

        m_result |= ExecuteAllConfigs(&case_config->multi_cam_configs, (const char *)m_logpath,
            (const char **)m_cam_names, MAX_CONCURRENT_CAM_COUNT, m_preview_warm);
        if (m_result >= (int) (EXCUTE_RESULT_FAIL_BASIC |
                    EXCUTE_RESULT_FAIL_FEATURE |
                    EXCUTE_RESULT_FAIL_PERFORMANCE))
        {
            return m_result;
        }

    }

    return m_result;
}


void CScheduler::SetExeParams(const char *logpath, const char *cam_names [], int preview_warm)
{
    m_logpath = strdup(logpath);
    if (m_logpath == NULL)
    {
        EXCEPTION("Failed to strdup");
    }

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        m_cam_names[i] = strdup(cam_names[i]);
        if (m_cam_names[i] == NULL)
        {
            EXCEPTION("Failed to strdup");
        }
    }
    m_preview_warm = preview_warm;
}

int CScheduler::GetExeResult()
{
    return m_result;
}

int CScheduler::SetExeResult(int result)
{
    m_result = result;
}

void CScheduler::StartTestExecutionProc(void *pthis)
{
    if (pthread_create(&CScheduler::m_ThreadID, NULL, SchedulerThreadProc, (void *)pthis) != 0)
    {
        EXCEPTION("Failed to Create Thread");
    }
}

void CScheduler::SignalExecutionCompleted()
{
    pthread_mutex_lock(&m_mutex_lock);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex_lock);
}

bool CScheduler::WaitExecutionCompleted(int time_out_ms)
{
    pthread_mutex_lock(&m_mutex_lock);
    struct timespec abstime;
    struct timeval now;
    gettimeofday(&now, NULL);

    long long nsec = now.tv_usec * 1000 + time_out_ms * 1000000;
    abstime.tv_nsec = nsec % 1000000000;
    abstime.tv_sec = now.tv_sec + nsec / 1000000000;

    int ret = pthread_cond_timedwait(&m_cond, &m_mutex_lock, &abstime);

    pthread_mutex_unlock(&m_mutex_lock);

    if (ret == ETIMEDOUT)
    {
        return false;
    }

    pthread_join(m_ThreadID, NULL);

    return true;
}



