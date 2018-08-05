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

#include "gstCaffAutoTestQueue.h"
#include "gstCaffAutoTestScheduler.h"

void* ThreadProc(void* lpParam )
{
    CFrameDispatchQueue *pQueue = (CFrameDispatchQueue*) lpParam;
    try
    {

        while (!pQueue->GetQuitStatus())
        {
            pQueue->WaitForQueueEvent();
            if (!pQueue->OnFrameArrival())
            {
                pQueue->ProcessThreadQuitCallback();
                return NULL;
            }
        }
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        pQueue->ProcessThreadQuitCallback();
        return NULL;
    }

    return NULL;
}


CFrameDispatchQueue::CFrameDispatchQueue(CScheduler *parent) :
    m_Parent(parent),
    m_current_config(NULL)
{
    m_bQuit = false;

    for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
    {
        m_prepare_ref_status[i] = PREPARE_REFERENCE_FRAME_NONE;
    }

    pthread_mutex_init(&m_mutex_lock, NULL);

    pthread_cond_init( &m_QueueEvent , NULL);
    if (pthread_create(&m_ThreadID, NULL, ThreadProc, (void *)this) != 0)
    {
        EXCEPTION("Failed to Create Thread");
    }
}

CFrameDispatchQueue::~CFrameDispatchQueue()
{
    LOG_VERBOSE("Enter", NULL);
    QuitProcessThread();
    pthread_cond_destroy(&m_QueueEvent);
    pthread_mutex_destroy(&m_mutex_lock);
    LOG_VERBOSE("Leave", NULL);
}

void CFrameDispatchQueue::DispatchFrame(FRAME_NODE &frame, MULTI_CAM_CONFIG *config)
{
    int icam = frame.camera_index;

    // If process thread quits or queue size reaches max, just return
    if (m_bQuit || m_FrameQueue.size() >= MAX_ANALYZER_INPUT_QUEUE_SIZE)
    {
        LOG_INFO(CStringFormat("Skip one camera %s frame",
            m_Parent->GetSensorName((size_t)icam)).Get(), NULL);

        return;
    }

    if (config != m_current_config)
    {
        for (int i = 0; i < MAX_CONCURRENT_CAM_COUNT; i++)
        {
            m_prepare_ref_status[i] = PREPARE_REFERENCE_FRAME_NONE;
        }
        m_current_config = config;
    }

    if (m_prepare_ref_status[icam] != PREPARE_REFERENCE_FRAME_DONE)
    {
        m_prepare_ref_status[icam] = PrepareReference(frame);

        if (m_prepare_ref_status[icam] == PREPARE_REFERENCE_FRAME_BYPASS)
        {
            pthread_mutex_lock(&m_mutex_lock);
            m_FrameQueue.push_back(frame);
            pthread_cond_signal(&m_QueueEvent);
            pthread_mutex_unlock(&m_mutex_lock);
        }
    }
    else
    {
        pthread_mutex_lock(&m_mutex_lock);
        m_FrameQueue.push_back(frame);
        pthread_cond_signal(&m_QueueEvent);
        pthread_mutex_unlock(&m_mutex_lock);
    }
}

bool CFrameDispatchQueue::OnFrameArrival()
{
    while (m_FrameQueue.size() > 0)
    {
        FRAME_NODE frame = *(m_FrameQueue.begin());
        bool ret = ProcessFrame(frame);

        pthread_mutex_lock(&m_mutex_lock);
        m_FrameQueue.pop_front();
        pthread_mutex_unlock(&m_mutex_lock);

        if (!ret)
        {
            return false;
        }
    }

    return true;
}

PREPARE_REFERENCE_RET CFrameDispatchQueue::PrepareReference(FRAME_NODE &frame)
{
    int icam = frame.camera_index;
    LOG_INFO(CStringFormat("prepare reference for cam %d",icam).Get(), NULL);
    E_FEATURE_TYPE feature_type = frame.feature_type;

    PREPARE_REFERENCE_RET ret = PREPARE_REFERENCE_FRAME_NONE;

    // Check green corrution for reference image only if there is other content check
    if (frame.analyzer_geen_corruption != NULL &&
        frame.feature_type != E_FEATURE_TYPE_GREEN_CORRUPTION_DETECTION)
    {
        LOG_INFO("Pushed one frame for green corruption check",NULL);
        FRAME_NODE frame_for_green_corruption = frame;
        frame_for_green_corruption.feature_type = E_FEATURE_TYPE_GREEN_CORRUPTION_DETECTION;

        pthread_mutex_lock(&m_mutex_lock);
        m_FrameQueue.push_back(frame_for_green_corruption);
        pthread_cond_signal(&m_QueueEvent);
        pthread_mutex_unlock(&m_mutex_lock);
    }
    //check color cast
    if (frame.analyzer_color_cast != NULL &&
        frame.feature_type != E_FEATURE_TYPE_COLOR_CAST)
    {
        LOG_INFO("Pushed one frame for color cast check",NULL);
        FRAME_NODE frame_for_color_cast = frame;
        frame_for_color_cast.feature_type = E_FEATURE_TYPE_COLOR_CAST;

        pthread_mutex_lock(&m_mutex_lock);
        m_FrameQueue.push_back(frame_for_color_cast);
        pthread_cond_signal(&m_QueueEvent);
        pthread_mutex_unlock(&m_mutex_lock);
    }
    // Common analyzer for orientation check
    if (frame.analyzer_orientation!= NULL &&
        frame.feature_type != E_FEATURE_TYPE_ORIENTATION_DETECTION)
    {
        LOG_INFO("Pushed one frame for orientation check", NULL);
        FRAME_NODE frame_for_orientation = frame;
        frame_for_orientation.feature_type = E_FEATURE_TYPE_ORIENTATION_DETECTION;

        pthread_mutex_lock(&m_mutex_lock);
        m_FrameQueue.push_back(frame_for_orientation);
        pthread_cond_signal(&m_QueueEvent);
        pthread_mutex_unlock(&m_mutex_lock);
    }

    switch (feature_type)
    {
    case E_FEATURE_TYPE_BRIGHTNESS:
    case E_FEATURE_TYPE_CONTRAST:
    case E_FEATURE_TYPE_EXPOSURE_TIME:
    case E_FEATURE_TYPE_EXPOSURE_GAIN:
    case E_FEATURE_TYPE_EXPOSURE_EV:
    case E_FEATURE_TYPE_WB_MANUAL_GAIN:
    case E_FEATURE_TYPE_WB_CCT_RANGE:
    case E_FEATURE_TYPE_WB_SHIFT:
        // Keep the latest frame as the reference
        m_ReferenceQueue[icam].clear();
        m_ReferenceQueue[icam].push_back(frame);
        ret = PREPARE_REFERENCE_FRAME_DONE;
        break;
    case E_FEATURE_TYPE_WB_MODE:
        // Keep manaul WB frame as the reference
        if (frame.parameter.val.i > MIN_MANUAL_WITHE_BALANCE_COLOR_TEMP)
        {
            m_ReferenceQueue[icam].clear();
            m_ReferenceQueue[icam].push_back(frame);
            ret = PREPARE_REFERENCE_FRAME_DONE;
        }
        else
        {
            ret = PREPARE_REFERENCE_FRAME_NEED_MORE;
        }
        break;
    case E_FEATURE_TYPE_HUE:
    case E_FEATURE_TYPE_3A_AUTO_CONSISTENT:
    case E_FEATURE_TYPE_HDR:
        // Keep val==0 as the reference
        if (frame.parameter.val.i == 0)
        {
            m_ReferenceQueue[icam].clear();
            m_ReferenceQueue[icam].push_back(frame);
            ret = PREPARE_REFERENCE_FRAME_DONE;
        }
        else
        {
            ret = PREPARE_REFERENCE_FRAME_NEED_MORE;
        }
        break;
    case E_FEATURE_TYPE_ULL:
        // Keep val==2 as the reference
        if (frame.parameter.val.i == 2)
        {
            m_ReferenceQueue[icam].clear();
            m_ReferenceQueue[icam].push_back(frame);
            ret = PREPARE_REFERENCE_FRAME_DONE;
            LOG_INFO("Use auto as reference for ULL mode", NULL);

            CStringFormat path("/home/root/FeatureTest/3A_ULL_REFERENCE_%dx%d.jpg",
                frame.frame.cols,
                frame.frame.rows
                );
            imwrite(path.Get(), frame.frame);
        }
        else
        {
            ret = PREPARE_REFERENCE_FRAME_NEED_MORE;
        }
        break;
    case E_FEATURE_TYPE_SIMILARITY:
        break;
    case E_FEATURE_TYPE_CONTENT_CONSISTENT:
        m_ReferenceQueue[icam].clear();
        m_ReferenceQueue[icam].push_back(frame);
        // Save reference frames to my documents
        {
            CStringFormat path_folder("/home/root/FeatureTest");

            if (!CreateDirectory((char*)path_folder.Get()))
            {
                EXCEPTION("Failed to CreateDirectory");
            }

            CStringFormat path("/home/root/FeatureTest/%s_config%d_cam%d_%dx%d.jpg",
                m_Parent->GetCaseInfo()->GetCaseID(),
                m_current_config->id,
                icam,
                frame.frame.cols,
                frame.frame.rows
                );

            imwrite(path.Get(), frame.frame);

            LOG_INFO(CStringFormat("Frame is saved to %s", path.Get()).Get(), NULL);
        }

        ret = PREPARE_REFERENCE_FRAME_DONE;
        break;
    case E_FEATURE_TYPE_AE_BRACKETING:
        {
            static int index = 0;
            m_ReferenceQueue[icam].clear();
            LOG_INFO(CStringFormat("Exposure bracketing Index : %d is used as refernece", index).Get(), NULL);
            frame.parameter.val.i = index++;
            m_ReferenceQueue[icam].push_back(frame);
            ret = PREPARE_REFERENCE_FRAME_DONE;
        }
        break;
    }

    return ret;
}

#define MAX_ANALYZER_COUNT_ONCE 2
bool CFrameDispatchQueue::ProcessFrame(FRAME_NODE &frame)
{
    int icam = frame.camera_index;
    LOG_INFO(CStringFormat("Process frame from cam %d", icam).Get(), NULL);
    E_FEATURE_TYPE feature_type = frame.feature_type;
    INPUT_IMAGE image;
    ANALYZER_INPUT_OUTPUT analyzer[MAX_ANALYZER_COUNT_ONCE];

    switch (feature_type)
    {
    case E_FEATURE_TYPE_BRIGHTNESS:
    case E_FEATURE_TYPE_CONTRAST:
    case E_FEATURE_TYPE_WB_MANUAL_GAIN:
    case E_FEATURE_TYPE_WB_CCT_RANGE:
    case E_FEATURE_TYPE_WB_SHIFT:
        if (m_ReferenceQueue[icam].size() > 0)
        {
            FRAME_NODE fn = m_ReferenceQueue[icam][0];
            image.img = fn.frame;
            image.param = fn.parameter;
            analyzer[0].img_array.push_back(image);
            image.img = frame.frame;
            image.param = frame.parameter;
            analyzer[0].img_array.push_back(image);
            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;

            // Replaced the reference image everytime
            m_ReferenceQueue[icam].clear();
            m_ReferenceQueue[icam].push_back(frame);
        }
        break;
    case E_FEATURE_TYPE_HUE:
    case E_FEATURE_TYPE_HDR:
        if (m_ReferenceQueue[icam].size() > 0)
        {
            FRAME_NODE fn = m_ReferenceQueue[icam][0];
            image.img = fn.frame;
            image.param = fn.parameter;
            analyzer[0].img_array.push_back(image);
            image.img = frame.frame;
            image.param = frame.parameter;
            analyzer[0].img_array.push_back(image);
            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
        }
        break;
    case E_FEATURE_TYPE_ULL:
        if (m_ReferenceQueue[icam].size() > 0)
        {
            // @todo
            //if (m_Parent->m_dual_cam->GetCamera(icam)->IsConverged(CAMERA_MODE_PREVIEW, E_3A_AE))
            {
                LOG_INFO("AE is already converged!", NULL);
                FRAME_NODE fn = m_ReferenceQueue[icam][0];
                image.img = fn.frame;
                image.param = fn.parameter;
                analyzer[0].img_array.push_back(image);
                image.img = frame.frame;
                image.param = frame.parameter;
                analyzer[0].img_array.push_back(image);
                analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;

                static int i = 0;
                CStringFormat path("/home/root/FeatureTest/3A_ULL_IMAGE_%d_%dx%d.jpg",
                    i++,
                    frame.frame.cols,
                    frame.frame.rows
                    );
                imwrite(path.Get(), frame.frame);
                break;
            }
            /*
            else
            {
                LOG_ERROR("Auto Exposure does not coverge in preview warm time", NULL);
                return false;
            }*/
        }
    case E_FEATURE_TYPE_EXPOSURE_TIME:
    case E_FEATURE_TYPE_EXPOSURE_GAIN:
    case E_FEATURE_TYPE_EXPOSURE_EV:
        if (m_ReferenceQueue[icam].size() > 0)
        {
            FRAME_NODE fn = m_ReferenceQueue[icam][0];

            image.img = fn.frame;
            image.param = fn.parameter;

            INPUT_IMAGE image1;
            image1.img = frame.frame;
            image1.param = frame.parameter;

            double paramval1 = 0;
            double paramval = 0;

            switch (image.param.type)
            {
            case PARAM_INT:
                paramval1 = (double)image1.param.val.i;
                paramval = (double) image.param.val.i;
                break;
            case PARAM_FLOAT:
                paramval1 = (double) image1.param.val.f;
                paramval = (double) image.param.val.f;
                break;
            case PARAM_DOUBLE:
                paramval1 = (double) image1.param.val.d;
                paramval = (double) image.param.val.d;
                break;
            }

            if (paramval1 > paramval)
            {
                analyzer[0].img_array.push_back(image);
                analyzer[0].img_array.push_back(image1);
            }
            else
            {
                analyzer[0].img_array.push_back(image1);
                analyzer[0].img_array.push_back(image);
            }

            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;

            m_ReferenceQueue[icam].clear();
            m_ReferenceQueue[icam].push_back(frame);
        }
        break;
    case E_FEATURE_TYPE_WB_MODE:
        if (m_ReferenceQueue[icam].size() > 0)
        {
            // Manual AWB
            if (frame.parameter.val.i >= MIN_MANUAL_WITHE_BALANCE_COLOR_TEMP)
            {
                // Compare with last one
                size_t size = m_ReferenceQueue[icam].size();
                FRAME_NODE fn = m_ReferenceQueue[icam][size-1];
                image.img = fn.frame;
                image.param = fn.parameter;

                INPUT_IMAGE image1;
                image1.img = frame.frame;
                image1.param = frame.parameter;

                if (image1.param.val.i > image.param.val.i)
                {
                    analyzer[0].img_array.push_back(image);
                    analyzer[0].img_array.push_back(image1);
                }
                else
                {
                    analyzer[0].img_array.push_back(image1);
                    analyzer[0].img_array.push_back(image);
                }

                analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
                m_ReferenceQueue[icam].push_back(frame);
            }
            else if (frame.parameter.val.i > GST_CAMERASRC_AWB_MODE_AUTO &&
                frame.parameter.val.i <= GST_CAMERASRC_AWB_MODE_DAYLIGHT)
            {
                S_AWB_MODE_RANGE awb_range = GetAwbModeColorTempRange((GstCamerasrcAwbMode) frame.parameter.val.i);

                deque<FRAME_NODE>::iterator iLbound = m_ReferenceQueue[icam].end();
                deque<FRAME_NODE>::iterator iUbound = m_ReferenceQueue[icam].end();
                int lbound_delta = 100000;
                int ubound_delta = 100000;
                int delta = 0;

                // Find suitable reference for analyzer
                for (deque<FRAME_NODE>::iterator it = m_ReferenceQueue[icam].begin();
                    it != m_ReferenceQueue[icam].end(); it++)
                {
                    if (it->parameter.val.i <= awb_range.min)
                    {
                        delta = awb_range.min - it->parameter.val.i;
                        if (lbound_delta > delta)
                        {
                            lbound_delta = delta;
                            iLbound = it;
                        }
                    }
                    else if (it->parameter.val.i >= awb_range.max)
                    {
                        delta = it->parameter.val.i - awb_range.max;
                        if (ubound_delta > delta)
                        {
                            ubound_delta = delta;
                            iUbound = it;
                        }
                    }
                }

                if (iLbound != m_ReferenceQueue[icam].end())
                {
                    image.img = iLbound->frame;
                    image.param = iLbound->parameter;

                    INPUT_IMAGE image1;
                    image1.img = frame.frame;
                    image1.param = frame.parameter;

                    analyzer[0].img_array.push_back(image);
                    analyzer[0].img_array.push_back(image1);

                    analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
                }

                if (iUbound != m_ReferenceQueue[icam].end())
                {
                    image.img = iUbound->frame;
                    image.param = iUbound->parameter;

                    INPUT_IMAGE image1;
                    image1.img = frame.frame;
                    image1.param = frame.parameter;

                    analyzer[1].img_array.push_back(image1);
                    analyzer[1].img_array.push_back(image);

                    analyzer[1].expected_ret = ANALYZER_RETRUN_PASS;
                }
            }
        }
        break;
    case E_FEATURE_TYPE_SIMILARITY:
        break;
    case E_FEATURE_TYPE_CONTENT_CONSISTENT:
        if (m_ReferenceQueue[icam].size() > 0)
        {
            FRAME_NODE fn = m_ReferenceQueue[icam][0];
            image.img = fn.frame;
            image.param = fn.parameter;
            image.metadata = fn.metadata;

            INPUT_IMAGE image1;
            image1.img = frame.frame;
            image1.param = frame.parameter;
            image1.metadata = frame.metadata;

            analyzer[0].img_array.push_back(image);
            analyzer[0].img_array.push_back(image1);

            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
        }
        break;
    case E_FEATURE_TYPE_3A_AUTO_CONSISTENT:
        /*{
            int iReferenceIndex = 0;
            int iCurrentIndex = 0;
            static bitset<32> sbsConsistentQueue;// to show the queue of 3A Auto consistency
            ANALYZER_INPUT_OUTPUT analyzer;
            size_t size = m_ReferenceQueue[icam].size();
            FRAME_NODE fn = m_ReferenceQueue[icam][size - 1];
            image.img = fn.frame;
            image.param = fn.parameter;
            analyzer.img_array.push_back(image);
            image.img = frame.frame;
            image.param = frame.parameter;
            analyzer.img_array.push_back(image);

            analyzer.expected_ret = ANALYZER_RETRUN_PASS;
            iCurrentIndex = frame.parameter.val.i;
            iReferenceIndex = fn.parameter.val.i;

            analyzer.camera_index = icam;
            analyzer.feature_type = feature_type;
            analyzer.analyzer = frame.analyzer;
            analyzer.ret = ANALYZER_RETURN_NONE;

            CStringFormat path("/home/root/FeatureTest/3A_AUTO_REFERENCE_%s_config%d_cam%d(%s)_%dx%d.jpg",
                m_Parent->GetCaseInfo()->GetCaseID(),
                m_current_config->id,
                icam,
                m_Parent->GetSensorName((size_t)icam),
                fn.frame.cols,
                fn.frame.rows
                );

            LOG_INFO(CStringFormat("%d is reference and %d is frame under process", iReferenceIndex, iCurrentIndex).Get(), NULL);
            analyzer.ret = analyzer.analyzer->Analyze(analyzer.img_array,
            analyzer.img_debug, analyzer.img_reference);

            if (analyzer.ret != analyzer.expected_ret)
            {
                m_ReferenceQueue[icam].push_back(frame);
                iReferenceIndex = iCurrentIndex;

                // suppose that after 10(?) queried frames(real time elapsed: 4 * duration ms), the 3A should converge.
                if (iCurrentIndex > 10)
                {
                    string szConsistentQueue = sbsConsistentQueue.to_string();
                    LOG_ERROR(CStringFormat("Queue is %s", szConsistentQueue).Get(), NULL);
                    CStringFormat msg("Analyzer{%s} return %d, while exected is %d",
                        analyzer.analyzer->GetContenAnalyzerName(), analyzer.ret, analyzer.expected_ret);
                    LOG_ERROR(msg.Get(), NULL);
                    CCaseInfo *pInfo = m_Parent->GetCaseInfo();
                    if (pInfo != NULL)
                    {
                        pInfo->AddFeatureErrorMessage(msg.Get());
                    }

                    m_AnalyzerFailQueue.push_back(analyzer);

                    imwrite(path.Get(), fn.frame);

                    return false;
                }
                else
                {
                    sbsConsistentQueue.reset(iCurrentIndex);
                    string szConsistentQueue = sbsConsistentQueue.to_string();
                    LOG_INFO(CStringFormat("Queue is %s", szConsistentQueue).Get(), NULL);
                }
            }
            else
            {
                sbsConsistentQueue.set(iCurrentIndex);
                string szConsistentQueue = sbsConsistentQueue.to_string();
                LOG_INFO(CStringFormat("Queue is %s", szConsistentQueue).Get(), NULL);
            }

            imwrite(path.Get(), fn.frame);
        }*/
        break;
    case E_FEATURE_TYPE_GREEN_CORRUPTION_DETECTION:
        {
            INPUT_IMAGE image1;
            image1.img = frame.frame;
            image1.param = frame.parameter;
            image1.metadata = frame.metadata;
            analyzer[0].analyzer = frame.analyzer_geen_corruption;
            analyzer[0].img_array.push_back(image1);
            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
        }
        break;
    case E_FEATURE_TYPE_COLOR_CAST:
         {
            INPUT_IMAGE image1;
            image1.img = frame.frame;
            image1.param = frame.parameter;
            image1.metadata = frame.metadata;

            analyzer[0].analyzer = frame.analyzer_color_cast;
            analyzer[0].img_array.push_back(image1);
            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
        }

        break;
    case E_FEATURE_TYPE_ORIENTATION_DETECTION:
        {
            INPUT_IMAGE image1;
            image1.img = frame.frame;
            image1.param = frame.parameter;
            image1.metadata = frame.metadata;

            analyzer[0].analyzer = frame.analyzer_orientation;
            analyzer[0].img_array.push_back(image1);
            analyzer[0].expected_ret = ANALYZER_RETRUN_PASS;
        }

        break;

    }

    for (int i = 0; i < MAX_ANALYZER_COUNT_ONCE; i++)
    {
        if (analyzer[i].img_array.size() != 0)
        {
            analyzer[i].camera_index = icam;
            analyzer[i].feature_type = feature_type;

            if (analyzer[i].analyzer == NULL)
            {
                analyzer[i].analyzer = frame.analyzer;
            }
            analyzer[i].ret = ANALYZER_RETURN_NONE;

            LOG_INFO(CStringFormat("{%s}: Analyze", analyzer[i].analyzer->GetContenAnalyzerName()).Get(), NULL);
            analyzer[i].ret = analyzer[i].analyzer->Analyze(analyzer[i].img_array,
                            analyzer[i].img_debug, analyzer[i].img_reference);

            if (analyzer[i].ret != analyzer[i].expected_ret)
            {
                CStringFormat msg("Analyzer{%s} return %d, while exected is %d",
                    analyzer[i].analyzer->GetContenAnalyzerName(), analyzer[i].ret, analyzer[i].expected_ret);

                LOG_ERROR(msg.Get(), NULL);

                CCaseInfo *pInfo = m_Parent->GetCaseInfo();
                if (pInfo != NULL)
                {
                    pInfo->AddFeatureErrorMessage(msg.Get());
                }

                m_AnalyzerFailQueue.push_back(analyzer[i]);
                return false;
            }
            else
            {
                LOG_INFO(CStringFormat("Analyzer{%s} successfully processed one frame with expected result",
                    analyzer[i].analyzer->GetContenAnalyzerName()).Get(), NULL);
            }
        }
    }

    return true;
}

void CFrameDispatchQueue::WaitForQueueEvent()
{
    LOG_VERBOSE("Enter CFrameDispatchQueue::WaitForQueueEvent", NULL);
    pthread_mutex_lock(&m_mutex_lock);
    pthread_cond_wait(&m_QueueEvent, &m_mutex_lock);
    pthread_mutex_unlock(&m_mutex_lock);
    LOG_VERBOSE("Leave CFrameDispatchQueue::WaitForQueueEvent", NULL);
}

void CFrameDispatchQueue::QuitProcessThread()
{
    LOG_VERBOSE("Enter QuitProcessThread", NULL);

    m_bQuit = true;
    pthread_mutex_lock(&m_mutex_lock);
    pthread_cond_signal(&m_QueueEvent);
    pthread_mutex_unlock(&m_mutex_lock);

    pthread_join(m_ThreadID, NULL);

    LOG_VERBOSE("Leave QuitProcessThread", NULL);
}

void CFrameDispatchQueue::ProcessThreadQuitCallback()
{
    m_bQuit = true;
}