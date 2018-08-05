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

#include "LibContentAnalyzerHue.h"

void cvtHSV2RGB(double H, double s, double v, uchar &r, uchar &g, uchar &b);
void GenReferenceImage(Mat &src, Mat &dst, double delta_hue);
double HueComparePixelbyPixel(Mat &img_none, Mat &img_effect, Mat &img_debug, double delta_hue, int s_th, int v_th, int h_range);
CContentAnalyzerHue::CContentAnalyzerHue() :
    CContenAnalyzerBase(ANALYZER_HUE_COMPARE)
{
    //Step 1:Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerHueCompare");

    //Step 2: Initialize algorithm parameters
    m_s_th = 30;
    m_v_th = 30;
    m_h_range = 10;
    m_ratio_th = 0.8;

    //Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerHue, m_s_th), "%d", "s_th", 0, 50, 30 },
        { offsetof(CContentAnalyzerHue, m_v_th), "%d", "v_th", 0, 50, 30 },
        { offsetof(CContentAnalyzerHue, m_h_range), "%d", "h_range", 0, 50, 10 },
        { offsetof(CContentAnalyzerHue, m_ratio_th), "%lf", "ratio_th", 0.0, 1.0,0.8 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContentAnalyzerHue::~CContentAnalyzerHue()
{}

ANALYZER_RETRUN CContentAnalyzerHue::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    //Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 4: Check Input parameter type
    int iZero = -1;
    for (size_t i = 0; i < img_array.size(); i++)
    {
        if (img_array[i].param.type != PARAM_INT)
        {
            LOG_ERROR("Wrong input parameter type", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

        if (img_array[i].param.val.i < -128 ||
            img_array[i].param.val.i > 127)
        {
            LOG_ERROR("Input parameter is out of range", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

        if (img_array[i].param.val.i == 0)
        {
            iZero = (int) i;
        }
    }

    if (iZero < 0)
    {
        LOG_ERROR("No Hue_0 image in the inputs", NULL);
        return ANALYZER_PARAMETER_EXCEPTION;
    }

    Mat img_none, img_effect;
    img_none = img_array[iZero].img;
    img_effect = img_array[1 - iZero].img;

    // Step 3: Check Input Image
    if (img_none.empty() || img_effect.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 4: Check Input Image Resolution
    if (img_none.cols != img_effect.cols || img_effect.rows != img_none.rows)
    {
        LOG_ERROR("Different Resolution", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 5: Analyzer
    int x = img_array[1 - iZero].param.val.i;
    double delta_hue = x / 256.0 * 180;

    double ratio = HueComparePixelbyPixel(img_none, img_effect, img_debug, delta_hue, m_s_th, m_v_th, m_h_range);

    if (ratio < m_ratio_th)
    {
        LOG_ERROR(CStringFormat("Failed due to ratio <ratio_th %lf", m_ratio_th).Get(),
            CStringFormat("ratio %lf", ratio).Get());
        return ANALYZER_RETRUN_FAIL;
    }
    LOG_INFO(CStringFormat("Passed: ratio >ratio_th %lf", m_ratio_th).Get(),CStringFormat("ratio %lf", ratio).Get())
    return ANALYZER_RETRUN_PASS;
}


double HueComparePixelbyPixel(Mat &img_none, Mat &img_effect, Mat &img_debug, double delta_hue, int s_th, int v_th, int h_range)
{
    Mat hsv_none, hsv_effect;

    hsv_none.create(img_none.rows, img_none.cols, CV_8UC3);
    hsv_effect.create(img_none.rows, img_none.cols, CV_8UC3);
    img_debug.create(img_none.rows, img_none.cols, CV_8UC1);
#ifndef _WINDOWS_MOBILE
    cvtColor(img_none, hsv_none, CV_BGR2HSV);
    cvtColor(img_effect, hsv_effect, CV_BGR2HSV);
#else
    cvtColor(img_none, hsv_none, COLOR_BGR2HSV);
    cvtColor(img_effect, hsv_effect, COLOR_BGR2HSV);
#endif
    vector<Mat> hsv_none_chs(3,Mat()), hsv_effect_chs(3,Mat());
    split(hsv_none, hsv_none_chs);
    split(hsv_effect, hsv_effect_chs);

    uchar* datah1 = NULL, *datah2 = NULL, *datadt = NULL;
    uchar* datas1 = NULL, *datas2 = NULL;
    uchar* datav1 = NULL, *datav2 = NULL;
    int count = 0;
    for (int y = 0; y < img_none.rows; y++)
    {
        datah1 = hsv_none_chs.at(0).data + y* hsv_none_chs.at(0).step;
        datah2 = hsv_effect_chs.at(0).data + y* hsv_effect_chs.at(0).step;

        datas1 = hsv_none_chs.at(1).data + y* hsv_none_chs.at(1).step;
        datas2 = hsv_effect_chs.at(1).data + y* hsv_effect_chs.at(1).step;

        datav1 = hsv_none_chs.at(2).data + y* hsv_none_chs.at(2).step;
        datav2 = hsv_effect_chs.at(2).data + y* hsv_effect_chs.at(2).step;

        datadt = img_debug.data + y * img_debug.step;

        for (int x = 0; x < img_none.cols ; x++)
        {
            double h1 = (double)datah1[x];
            double h2 = (double)datah2[x];
            double s1 = (double)datas1[x];
            double s2 = (double)datas2[x];
            double v1 = (double)datav1[x];
            double v2 = (double)datav2[x];
            if ((s1 < s_th && s2 < s_th) || (v1 < v_th && v2 < v_th))
            {
                count++;
                datadt[x] = (uchar)(0);
            }
            else if (fabs((h1 + delta_hue) - h2) < h_range || fabs((h1 + delta_hue) - h2 - 180) < h_range || fabs((h1 + delta_hue) - h2 + 180) < h_range)
            {
                count++;
                datadt[x] = (uchar)(0);
            }
            else datadt[x] = (uchar)(255);

        }
    }
    return(count / (double)(img_none.rows * img_none.cols));
}
