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


#include "LibContentAnalyzerColorEffect.h"

double color_effect(Mat &img_none, Mat &img_effect, Mat &img_delta, effect_func ef, double var_ratio, double &critical_error_rate);
int effect_negtive(Pixel none, Pixel effect, double var_ratio);
int effect_grass_green(Pixel none, Pixel effect, double var_ratio);
int effect_mono(Pixel none, Pixel effect, double var_ratio);
int effect_sepia(Pixel none, Pixel effect, double var_ratio);
int effect_skyblue(Pixel none, Pixel effect, double var_ratio);
int effect_skin_whiten(Pixel none, Pixel effect, double var_ratio);

CContenAnalyzerColorEffect::CContenAnalyzerColorEffect(AnalyzerType type, effect_func ef) :
CContenAnalyzerBase(type)
{
    m_ef = ef;

    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffect");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.9;
    m_var_th = 0.3;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerColorEffect, m_pass_ratio), "%lf", "pass_ratio", 0, 1, 0.9 },
        { offsetof(CContenAnalyzerColorEffect, m_var_th), "%lf", "var_th", 0, 1, 0.3 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);

}

CContenAnalyzerColorEffect::~CContenAnalyzerColorEffect()
{

}

ANALYZER_RETRUN CContenAnalyzerColorEffect::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    if (m_ef == NULL)
    {
        LOG_ERROR("Effect call back funtion is NULL", NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    // Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    if (img_array[0].img.empty() || img_array[1].img.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Check Input Image Resolution
    if (img_array[0].img.cols != img_array[1].img.cols ||
        img_array[0].img.rows != img_array[1].img.rows)
    {
        LOG_ERROR("Image resolution doesn't match", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 4: Check Input parameter type
    int iZero = -1;
    for (size_t i = 0; i < 2; i++)
    {
        if (img_array[i].param.type != PARAM_INT)
        {
            LOG_ERROR("Wrong input parameter type", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

        if (img_array[i].param.val.i == 0)
        {
            iZero = (int) i;
        }
    }

    if (iZero < 0)
    {
        LOG_ERROR("No non-effect image in the inputs", NULL);
        return ANALYZER_PARAMETER_EXCEPTION;
    }

    // Step 5: Analyze
    double pass_ratio = 0;
    double critical_ratio = 0;
    try
    {
        pass_ratio = color_effect(img_array[iZero].img, img_array[1 - iZero].img, img_debug, m_ef, m_var_th, critical_ratio);
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    if (pass_ratio < 0)
    {
        LOG_ERROR(CStringFormat("Failed because no sufficient valid pixels can be found, maybe image is too dark or the test pattern is not correct.", m_pass_ratio).Get(),
            CStringFormat("Pass ratio is %lf", pass_ratio).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    if (pass_ratio < m_pass_ratio)
    {
        LOG_ERROR(CStringFormat("Failed due to pass ratio is less than threshold %lf", m_pass_ratio).Get(),
            CStringFormat("Pass ratio is %lf", pass_ratio).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    if (critical_ratio > 0.001)
    {
        LOG_ERROR("Failed due to critical error ratio is higher than 0.001",
            CStringFormat("Critical error ratio is %lf", critical_ratio).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}


CContenAnalyzerNegative::CContenAnalyzerNegative() :
CContenAnalyzerColorEffect(ANALYZER_COLOREFFECT_NEGATIVE, effect_negtive)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffectNegative");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.9;
    m_var_th = 0.3;
}

CContenAnalyzerNegative::~CContenAnalyzerNegative()
{
}

CContenAnalyzerGrassGreen::CContenAnalyzerGrassGreen() :
CContenAnalyzerColorEffect(ANALYZER_COLOREFFECT_GRASSGREEN, effect_grass_green)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffectGrassGreen");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.7;
    m_var_th = 0.;
}

CContenAnalyzerGrassGreen::~CContenAnalyzerGrassGreen()
{
}

CContenAnalyzerMono::CContenAnalyzerMono() :
CContenAnalyzerColorEffect(ANALYZER_COLOREFFECT_MONO, effect_mono)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffectMono");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.85;
    m_var_th = 0.2;
}

CContenAnalyzerMono::~CContenAnalyzerMono()
{
}

CContenAnalyzerSepia::CContenAnalyzerSepia() :
CContenAnalyzerColorEffect(ANALYZER_COLOREFFECT_SEPIA, effect_sepia)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffectSepia");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.9;
    m_var_th = 0;
}

CContenAnalyzerSepia::~CContenAnalyzerSepia()
{
}

CContenAnalyzerSkyBlue::CContenAnalyzerSkyBlue() :
CContenAnalyzerColorEffect(ANALYZER_COLOREFFECT_SKYBLUE, effect_skyblue)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffectSkyBlue");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.7;
    m_var_th = 0;
}

CContenAnalyzerSkyBlue::~CContenAnalyzerSkyBlue()
{
}

CContenAnalyzerSkinWhiten::CContenAnalyzerSkinWhiten() :
CContenAnalyzerColorEffect(ANALYZER_COLOREFFECT_SKINWHITEN, effect_skin_whiten)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerColorEffectSkinWhiten");

    // Step 2: Initialize algorithm parameters
    m_pass_ratio = 0.8;
    m_var_th = 0;
}

CContenAnalyzerSkinWhiten::~CContenAnalyzerSkinWhiten()
{
}

#define MINIMAL_PIXEL_RATIO 0.001
double color_effect(Mat &img_none, Mat &img_effect, Mat &img_delta, effect_func ef, double var_ratio, double &critical_error_rate)
{
    // make sure with same resolution
    assert(img_none.cols == img_effect.cols &&
        img_none.rows == img_effect.rows &&
        img_none.channels() == 3 &&
        img_effect.channels() == 3
        );

    critical_error_rate = 0;

    img_delta.create(img_none.rows, img_none.cols, CV_8UC3);
    img_none.copyTo(img_delta);

    // Compare the pixels one by one
    uchar *none_data, *effect_data, *delta_data;
    Pixel pixel_none, pixel_effect;

    long nCount = 0, nCount1 = 0, nCount2 = 0;

    for (int y = 0; y < img_none.rows; y++)
    {
        none_data = img_none.data + y * img_none.step;
        effect_data = img_effect.data + y * img_effect.step;
        delta_data = img_delta.data + y * img_delta.step;

        for (int x = 0; x < img_none.cols; x++)
        {
            int x3 = 3 * x;
            pixel_none.b = none_data[x3];
            pixel_effect.b = effect_data[x3];

            pixel_none.g = none_data[x3 + 1];
            pixel_effect.g = effect_data[x3 + 1];

            pixel_none.r = none_data[x3 + 2];
            pixel_effect.r = effect_data[x3 + 2];

            int rval = ef(pixel_none, pixel_effect, var_ratio);

            if (rval == 1)
            {
                nCount++;
            }
            else if (rval == -1)
            {
                delta_data[x3] = 255 - delta_data[x3];
                delta_data[x3 + 1] = 255 - delta_data[x3 + 1];
                delta_data[x3 + 2] = 255 - delta_data[x3 + 2];
                nCount1++;
            }
            else if (rval == -2)
            {
                delta_data[x3 + 2] = delta_data[x3 + 2] / 2 + 128;
                nCount2++;
            }
            else
            {
                delta_data[x3] = 0;
                delta_data[x3 + 1] = 0;
                delta_data[x3 + 2] = 0;
            }
        }
    }

    double ret = -1;
    double total_count = nCount + nCount1 + nCount2;

    LOG_INFO(CStringFormat("color effect: total valid pixel count is %lf, "
        "pass pixel count is %lf, fail pixel count is %lf, critical fail pixel count is %lf",
        total_count, nCount, nCount1, nCount2).Get(), NULL);

    if (total_count > MINIMAL_PIXEL_RATIO * img_none.rows * img_none.cols)
    {
        ret = nCount / total_count;
        critical_error_rate = nCount2 / total_count;
    }

    return ret;
}

int effect_negtive(Pixel none, Pixel effect, double var_ratio)
{
    double negb = 255 - none.b;
    double negg = 255 - none.g;
    double negr = 255 - none.r;

    double effb = effect.b;
    double effg = effect.g;
    double effr = effect.r;

    double th = var_ratio * 255;


    if (abs(negb - effb) > 120 || abs(negg - effg) > 120 || abs(negr - effr) > 120)
    {
        return -2;
    }

    if (abs(negb - effb) > th || abs(negg - effg) > th || abs(negr - effr) > th)
    {
        return -1;
    }

    return 1;
}

int effect_grass_green(Pixel none, Pixel effect, double var_ratio)
{
    // Judge the pixel if it is blue
    double H_none = 0, S_none = 0, V_none = 0;
    double H_effect = 0, S_effect = 0, V_effect = 0;

    cvtRGB2HSV(none.r, none.g, none.b, H_none, S_none, V_none);
    cvtRGB2HSV(effect.r, effect.g, effect.b, H_effect, S_effect, V_effect);

    // avoid dark pixel
    if (V_none < 50 || V_effect < 50 || S_none < 0.2 || S_effect < 0.2)
    {
        return 0;
    }
#define H_GREEN_MIN 70
#define H_GREEN_MAX 130
    bool none_green = H_none > H_GREEN_MIN && H_none < H_GREEN_MAX;
    bool effect_green = H_effect > H_GREEN_MIN && H_effect < H_GREEN_MAX;

    if (!none_green)
    {
        if (fabs(S_effect - S_none) > 0.3)
        {
            return -1;
        }

        return 0;
    }
    else if (!effect_green)
    {
        return -1;
    }

    if (S_effect < 0.95 && S_effect <= S_none)
    {
        return -1;
    }

    return 1;
}

int effect_mono(Pixel none, Pixel effect, double var_ratio)
{
    double brightness_none = none.b   * 0.114 + none.g   * 0.587 + none.r   * 0.299;
    double brightness_effect = effect.b * 0.114 + effect.g * 0.587 + effect.r * 0.299;

    if (abs(brightness_none - brightness_effect)  > var_ratio * 255)
    {
        return -1;
    }

    if (abs(effect.b - brightness_effect) > 0.1 * 255 ||
        abs(effect.g - brightness_effect) > 0.1 * 255 ||
        abs(effect.r - brightness_effect) > 0.1 * 255)
    {
        return -1;
    }

    return 1;
}

int effect_sepia(Pixel none, Pixel effect, double var_ratio)
{
    // Judge the pixel if it is blue
    double H_none = 0, S_none = 0, V_none = 0;
    double H_effect = 0, S_effect = 0, V_effect = 0;

    cvtRGB2HSV(none.r, none.g, none.b, H_none, S_none, V_none);
    cvtRGB2HSV(effect.r, effect.g, effect.b, H_effect, S_effect, V_effect);

    // avoid dark pixel
    if (V_none < 50)
    {
        return 0;
    }

    if (fabs(V_none - V_effect) > 0.2 * 255 || H_effect < 25 || H_effect > 60)
    {
        return -1;
    }

    return 1;
}

int effect_skyblue(Pixel none, Pixel effect, double var_ratio)
{
    // Judge the pixel if it is blue
    double H_none = 0, S_none = 0, V_none = 0;
    double H_effect = 0, S_effect = 0, V_effect = 0;

    cvtRGB2HSV(none.r, none.g, none.b, H_none, S_none, V_none);
    cvtRGB2HSV(effect.r, effect.g, effect.b, H_effect, S_effect, V_effect);

    // avoid dark pixel
    if (V_none < 25 || V_effect < 25 || S_none < 0.2 || S_effect < 0.2)
    {
        return 0;
    }
#define H_BLUE_MIN 200
#define H_BLUE_MAX 240
    bool none_blue = H_none > H_BLUE_MIN && H_none < H_BLUE_MAX;
    bool effect_blue = H_effect > H_BLUE_MIN && H_effect < H_BLUE_MAX;

    if (!none_blue)
    {
        if (fabs(S_effect - S_none) > 0.3)
        {
            return -1;
        }
        return 0;
    }
    else if (!effect_blue)
    {
        return -1;
    }

    if (S_effect < 0.95 && S_effect <= S_none)
    {
        return -1;
    }

    return 1;
}

bool isSkinColor(Pixel pix)
{
    double Cg, Cr, Cg1, Cr1, Y;
    Y = pix.b * 0.114 + pix.g * 0.587 + pix.r * 0.299;//16 + 0.2568 * pix.r + 0.5041 * pix.g + 0.0979 * pix.b;

    if (Y < 25)
    {
        return false;
    }

    Cg = 128 - 0.318 * pix.r + 0.4392 * pix.g - 0.1212 * pix.b;
    Cr = 128 + 0.4392 * pix.r - 0.3677 * pix.g - 0.0714 * pix.b;

    Cg1 = 0.866 * Cg + 0.5 * Cr - 48;
    Cr1 = 0.866 * Cr - 0.5 * Cg + 80;

    /* 143, 129, 129
    Cg'∈[125.9238,152.0492]
    Cr'∈[138.7329,184.1046]
    */

    if (Cg1 >= 124 && Cg1 <= 160 && Cr1 >= 130 && Cr1 <= 210)
    {
        return true;
    }

    return false;
}

int effect_skin_whiten(Pixel none, Pixel effect, double var_ratio)
{
    double H_none = 0, S_none = 0, V_none = 0;
    double H_effect = 0, S_effect = 0, V_effect = 0;

    cvtRGB2HSV(none.r, none.g, none.b, H_none, S_none, V_none);
    cvtRGB2HSV(effect.r, effect.g, effect.b, H_effect, S_effect, V_effect);

    if (S_none < 0.3 || V_effect < 0.3)
    {
        return 0;
    }

    bool isNoneSkin = isSkinColor(none);
    bool isEffectSkin = isSkinColor(effect);

    if (!(isNoneSkin && isEffectSkin))
    {
        //if (fabs(H_effect-H_none) > 50)
        //    return -1;
        //if (S_none - S_effect > 0.4)
        //    return -1;
        //else
        return 0;
    }

    if ((fabs(H_effect - H_none) > 30 && fabs(H_effect - H_none) < 330) || S_effect - S_none > 0.02)
    {
        return -1;
    }

    return 1;
}
