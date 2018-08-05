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

#include "LibContentAnalyzerBrightness.h"

double CompareBrightnessPixelbyPixel(Mat &bright, Mat &dim, Mat &badpixels, int brightness_lower_bound, int brightness_upper_bound);

CContenAnalyzerBrightness::CContenAnalyzerBrightness() :
    CContenAnalyzerBase(ANALYZER_BRIGHTNESS_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerBrightnessCompare");

    // Step 2: Initialize algorithm parameters

    m_ratio_th = 0.8;// default pass ratio
    m_brightness_lower_bound = 20;
    m_brightness_upper_bound = 240;
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerBrightness, m_ratio_th), "%lf", "ratio_th", 0, 1, 0.8 },
        { offsetof(CContenAnalyzerBrightness, m_brightness_lower_bound), "%d", "brightness_lower_bound", 0, 60, 20 },
        { offsetof(CContenAnalyzerBrightness, m_brightness_upper_bound), "%d", "brightness_upper_bound", 220,255, 240 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerBrightness::~CContenAnalyzerBrightness()
{
}

ANALYZER_RETRUN CContenAnalyzerBrightness::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    if (img_array[0].img.empty() || img_array[1].img.empty())
    {
        LOG_ERROR("Input image is empty", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Check Input Image Resolution
    if (img_array[0].img.cols != img_array[1].img.cols ||
        img_array[0].img.rows != img_array[1].img.rows)
    {
        LOG_ERROR("Input image resolution doesn't match each other", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 4: Check Input parameter type
    int iZero = 0;
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
            LOG_ERROR(CStringFormat("Input parameter is out of range (-128, 127)").Get(),
                CStringFormat("Its value is %d", img_array[i].param.val.i).Get());
            return ANALYZER_PARAMETER_EXCEPTION;
        }
    }

    // Step 5: Analyze
    double ratio = 0;
    try
    {
        // not using gamma_table to fit any longer, just to compare pixel by pixel with boundaries
        if (img_array[iZero].param.val.i > img_array[1-iZero].param.val.i)
            ratio = CompareBrightnessPixelbyPixel(img_array[iZero].img, img_array[1 - iZero].img, img_debug, m_brightness_lower_bound, m_brightness_upper_bound);
        else
            ratio = CompareBrightnessPixelbyPixel(img_array[1 - iZero].img, img_array[iZero].img, img_debug, m_brightness_lower_bound, m_brightness_upper_bound);

    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    if (ratio <m_ratio_th)
    {
        LOG_ERROR(CStringFormat("Ratio value is smaller than Ratio threshold(%lf)", m_ratio_th).Get(),
            CStringFormat("Its value is %lf", ratio).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}

void BrighnessConvertToY(Mat &src, Mat &Y)
{
    Y.create(src.rows, src.cols, CV_8UC1);
    uchar *dataSrc = NULL;
    uchar *dataY = NULL;
    for (int y = 0; y < src.rows; y++)
    {
        dataSrc = src.data + y * src.step;
        dataY = Y.data + y * Y.step;

        for (int x = 0, x3 = 0; x < src.cols; x++, x3 += 3)
        {
            double srcY = 0.114 * dataSrc[x3] + 0.587 * dataSrc[x3 + 1] + 0.299 * dataSrc[x3 + 2];
            dataY[x] = (uchar) srcY;
        }
    }

}
double CompareBrightnessPixelbyPixel(Mat &bright, Mat &dim, Mat &badpixels, int brightness_lower_bound, int brightness_upper_bound)
{
    Mat Yb0, Ybx;
    BrighnessConvertToY(bright, Yb0);
    BrighnessConvertToY(dim, Ybx);
    // to get current ratio of good pixels
    // to get binary image for bad pixels, bad is white and correct is black.
    badpixels.create(bright.rows, bright.cols, CV_8UC1);
    uchar *data1 = NULL, *data2 = NULL, *datadt=NULL;
    int count= 0;
    for (int y = 0; y < bright.rows; y++)
    {
        data1 = Yb0.data + y * Yb0.step;
        data2 = Ybx.data + y * Ybx.step;
        datadt = badpixels.data + y * badpixels.step;
        for (int x = 0; x < bright.cols; x++)
        {
            double Y1 = (double) data1[x];// Y for bright image
            double Y2 = (double) data2[x];// Y for dim image
            if (Y1 > Y2 || Y2 > brightness_upper_bound || Y1 < brightness_lower_bound)
            {
                count++;
                datadt[x] = 0;
            }
            else
                datadt[x] = 255;
        }
    }

    return(count / (double)(bright.rows * bright.cols));
}