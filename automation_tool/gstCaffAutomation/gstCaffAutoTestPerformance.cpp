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

#include "gstCaffAutoTestPerformance.h"

CPerformanceCheck::CPerformanceCheck() :
    m_TimeInit(0),
    m_TimeConfig(0),
    m_TimeStart(0),
    m_TimeStart2FirstFrame(0),
    m_TimeStop(0),

    m_AverageFPS(0),
    m_MaxInterval(0),
    m_MinInterval(0),
    m_StdInterval(0),

    m_Time2FirstFrameTH(0),
    m_TimeStopTH(0),
    m_ExpectedFPS(0),
    m_AcceptableFpsRatio(0),
    m_MaxIntervalRatio(0),
    m_MinIntervalRatio(0),
    m_StdIntervalRatio(0),

    m_pDataParser(NULL)
{
    DATA_PARSER params [] = {
            { offsetof(CPerformanceCheck, m_Time2FirstFrameTH), "%lf", "time_to_first_frame_TH", 0, 100000, 0 },
            { offsetof(CPerformanceCheck, m_TimeStopTH), "%lf", "time_to_stop_TH", 0, 100000, 0 },
            { offsetof(CPerformanceCheck, m_ExpectedFPS), "%lf", "expected_fps", 0, 100000, 0 },
            { offsetof(CPerformanceCheck, m_AcceptableFpsRatio), "%lf", "acceptable_fps_ratio", 0, 100000, 0 },
            { offsetof(CPerformanceCheck, m_MaxIntervalRatio), "%lf", "max_interval_ratio", 0, 100000, 0 },
            { offsetof(CPerformanceCheck, m_MinIntervalRatio), "%lf", "min_interval_ratio", 0, 100000, 0 },
            { offsetof(CPerformanceCheck, m_StdIntervalRatio), "%lf", "std_interval_ratio", 0, 100000, 0 },
            { 0, NULL, NULL, 0, 0, 0 }
    };

    m_pDataParser = new DATA_PARSER[ARRAYSIZE(params)];

    if (m_pDataParser == NULL)
    {
        EXCEPTION("Failed to new DATA_PARSER");
    }

    memcpy(m_pDataParser, params, ARRAYSIZE(params)*sizeof(DATA_PARSER));

}


CPerformanceCheck::~CPerformanceCheck()
{
    if (m_pDataParser != NULL)
    {
        delete [] m_pDataParser;
        m_pDataParser = NULL;
    }
}

bool CPerformanceCheck::LoadParameters(const char *path)
{
    if (m_pDataParser == NULL || path == NULL)
    {
        return false;
    }

    TiXmlDocument doc;

    if (!doc.LoadFile(path))
    {
        return false;
    }

    if (ReadChildElement(&doc, m_pDataParser, (void*)this, "Performance") == NULL)
    {
        return false;
    }

    return true;
}

#define SMALL_VAL 1e-3

PERFORMANCE_RESULT CPerformanceCheck::CheckResult()
{
    double time2FirstFrame = m_TimeInit + m_TimeConfig + m_TimeStart + m_TimeStart2FirstFrame;

    LOG_INFO(CStringFormat("Time to first frame (%lf); "
        "Time to stop (%lf); "
        "Average FPS (%lf); "
        "Max frame interval (%lf); "
        "Min frame interval (%lf); "
        "Std frame interval (%lf)",
        time2FirstFrame, m_TimeStop, m_AverageFPS,
        m_MaxInterval, m_MinInterval, m_StdInterval).Get(), NULL);

    if (m_Time2FirstFrameTH > SMALL_VAL)
    {
        if (time2FirstFrame > m_Time2FirstFrameTH)
        {
            LOG_ERROR(CStringFormat("Time to first frame is exceeding the threshold (%lf ms)",
                m_Time2FirstFrameTH).Get(), CStringFormat("time2FirstFrame %lf", time2FirstFrame).Get());

            return PERFORMANCE_RESULT_FAIL_TIME_TO_FIRST_FRAME;
        }
    }

    if (m_TimeStopTH > SMALL_VAL)
    {
        if (m_TimeStop > m_TimeStopTH)
        {
            LOG_ERROR(CStringFormat("Time to stop is exceeding the threshold (%lf ms)",
                m_TimeStopTH).Get(), CStringFormat("Time to stop is %lf", m_TimeStop).Get());

            return PERFORMANCE_RESULT_FAIL_TIME_TO_STOP;
        }
    }

    if (m_ExpectedFPS > SMALL_VAL)
    {
        if (m_AcceptableFpsRatio > 10e-6)
        {
            double delta = m_ExpectedFPS * m_AcceptableFpsRatio;

            if (m_AverageFPS > m_ExpectedFPS + delta ||
                m_AverageFPS < m_ExpectedFPS - delta)
            {
                LOG_ERROR(CStringFormat("Average FPS is exceeding the range (%lf+/-%lf)",
                    m_ExpectedFPS, delta).Get(), CStringFormat("Average FPS is %lf", m_AverageFPS).Get());

                return PERFORMANCE_RESULT_FAIL_AVG_FPS;
            }
        }

        if (m_MaxIntervalRatio > SMALL_VAL)
        {
            double maxintervalTH = m_MaxIntervalRatio * 1000 / m_ExpectedFPS;

            if (m_MaxInterval > maxintervalTH)
            {
                LOG_ERROR(CStringFormat("Max frame interval is exceeding the threshold (%lf)", maxintervalTH).Get(),
                    CStringFormat("Max frame interval is %lf", m_MaxInterval).Get());

                return PERFORMANCE_RESULT_FAIL_MAX_INTERVAL;
            }
        }

        if (m_MinIntervalRatio > SMALL_VAL)
        {
            double minintervalTH = m_MinIntervalRatio * 1000 / m_ExpectedFPS;

            if (m_MinInterval < minintervalTH)
            {
                LOG_ERROR(CStringFormat("Min frame interval is exceeding the threshold (%lf)!", minintervalTH).Get(),
                    CStringFormat("Min frame interval is %lf", m_MinInterval).Get());

                return PERFORMANCE_RESULT_FAIL_MIN_INTERVAL;
            }
        }


        if (m_StdIntervalRatio > SMALL_VAL)
        {
            double stdintervalTH = m_StdIntervalRatio * 1000 / m_ExpectedFPS;

            if (m_StdInterval > stdintervalTH)
            {
                LOG_ERROR(CStringFormat("Std frame interval is exceeding the threshold (%lf)!",
                    stdintervalTH).Get(), CStringFormat("Std frame interval is %lf", m_StdInterval).Get());

                return PERFORMANCE_RESULT_FAIL_STD_INTERVAL;
            }
        }
    }

    return PERFORMANCE_RESULT_PASS;
}

