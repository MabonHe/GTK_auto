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


#ifndef __LIB_CONTENT_ANALYZER_COLOR_EFFECT_H__
#define __LIB_CONTENT_ANALYZER_COLOR_EFFECT_H__

#include "LibContentAnalyzerBase.h"
#include "LibContentAnalyzerCommonFunction.h"

typedef struct _Pixel
{
    uchar b;
    uchar g;
    uchar r;
} Pixel;

typedef int(*effect_func)(Pixel none, Pixel effect, double var_ratio);

class CContenAnalyzerColorEffect : public CContenAnalyzerBase
{
public:
    CContenAnalyzerColorEffect(AnalyzerType type, effect_func ef);

    ~CContenAnalyzerColorEffect();

    ANALYZER_RETRUN Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference);

protected:
    double m_var_th;
    double m_pass_ratio;

private:
    effect_func m_ef;
};

class CContenAnalyzerNegative : public CContenAnalyzerColorEffect
{
public:
    CContenAnalyzerNegative();
    ~CContenAnalyzerNegative();
};

class CContenAnalyzerGrassGreen : public CContenAnalyzerColorEffect
{
public:
    CContenAnalyzerGrassGreen();
    ~CContenAnalyzerGrassGreen();
};

class CContenAnalyzerMono : public CContenAnalyzerColorEffect
{
public:
    CContenAnalyzerMono();
    ~CContenAnalyzerMono();
};

class CContenAnalyzerSepia : public CContenAnalyzerColorEffect
{
public:
    CContenAnalyzerSepia();
    ~CContenAnalyzerSepia();
};

class CContenAnalyzerSkyBlue : public CContenAnalyzerColorEffect
{
public:
    CContenAnalyzerSkyBlue();
    ~CContenAnalyzerSkyBlue();
};

class CContenAnalyzerSkinWhiten : public CContenAnalyzerColorEffect
{
public:
    CContenAnalyzerSkinWhiten();
    ~CContenAnalyzerSkinWhiten();
};

#endif