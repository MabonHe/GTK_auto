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

#ifndef __LIB_CONTENT_ANALYZER_GREEN_CORRUPTION_H__
#define __LIB_CONTENT_ANALYZER_GREEN_CORRUPTION_H__

#include "LibContentAnalyzerBase.h"

class CContenAnalyzerGreenCorruption : public CContenAnalyzerBase
{
public:
    CContenAnalyzerGreenCorruption();

    ~CContenAnalyzerGreenCorruption();

    ANALYZER_RETRUN Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference);

private:
    CContenAnalyzerGreenCorruption(const CContenAnalyzerGreenCorruption &src) : CContenAnalyzerBase(ANALYZER_GREEN_CORRUPTION) {};

    CContenAnalyzerGreenCorruption& operator=(const CContenAnalyzerGreenCorruption &src) { return *this; };
    bool CheckGreenCorruption(Mat &img);
private:
    // threshold for passing ration;
    int m_green_b_max;
    int m_green_g_min;
    int m_green_r_max;
    float m_grenn_line_ratio_th;
    float m_line_pixel_average_lo;
    float m_line_pixel_average_hi;
    float m_line_pixel_var_th;
};

#endif //__LIB_CONTENT_ANALYZER_GREEN_CORRUPTION_H__