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

#ifndef _LIB_CONTENT_ANALYZER_FOV_H_
#define _LIB_CONTENT_ANALYZER_FOV_H_

#include "LibContentAnalyzerBase.h"

class CContentAnalyzerFOV : public CContenAnalyzerBase
{
public:
    CContentAnalyzerFOV();

    ~CContentAnalyzerFOV();

    ANALYZER_RETRUN Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference);

private:
    double m_hFovLossTh;
    double m_vFovLossTh;
    double m_shiftTH;
};
#endif