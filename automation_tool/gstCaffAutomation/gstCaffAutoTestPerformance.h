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

#ifndef __GST_CAFF_AUTO_TEST_PERFORMANCE_H__
#define __GST_CAFF_AUTO_TEST_PERFORMANCE_H__

#include <sys/time.h>
#include "gstCaffAutoTestConfig.h"

#define MAX_PER_ERROR_MSG_LEN 1024

enum PERFORMANCE_RESULT
{
    PERFORMANCE_RESULT_PASS,
    PERFORMANCE_RESULT_FAIL_NO_FRAME,
    PERFORMANCE_RESULT_FAIL_TIME_TO_FIRST_FRAME,
    PERFORMANCE_RESULT_FAIL_TIME_TO_STOP,
    PERFORMANCE_RESULT_FAIL_AVG_FPS,
    PERFORMANCE_RESULT_FAIL_MAX_INTERVAL,
    PERFORMANCE_RESULT_FAIL_MIN_INTERVAL,
    PERFORMANCE_RESULT_FAIL_STD_INTERVAL
};

class CPerformanceCheck
{
public:
    CPerformanceCheck();
    ~CPerformanceCheck();

    bool LoadParameters(const char *path);

    PERFORMANCE_RESULT CheckResult();

public:
    // Lattency
    double m_TimeInit;
    double m_TimeConfig;
    double m_TimeStart;
    double m_TimeStart2FirstFrame;
    double m_TimeStop;

    // FPS
    double m_AverageFPS;
    double m_MaxInterval;
    double m_MinInterval;
    double m_StdInterval;

protected:

private:
    // Threshold
    double m_Time2FirstFrameTH;
    double m_TimeStopTH;
    double m_ExpectedFPS;
    double m_AcceptableFpsRatio;
    double m_MaxIntervalRatio;
    double m_MinIntervalRatio;
    double m_StdIntervalRatio;

    DATA_PARSER *m_pDataParser;

    CPerformanceCheck(const CPerformanceCheck& src){}
    CPerformanceCheck& operator=(const CPerformanceCheck& src){ return *this; }
};

#endif