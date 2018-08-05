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

#include "LibContentAnalyzerATE.h"

int CompareATEPixelbyPixel(Mat &raw, Mat &reference, Mat &debug_yuv, int tolerance);

CContentAnalyzerATE::CContentAnalyzerATE() :
CContenAnalyzerBase(ANALYZER_ATE_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerATECompare");

    // Step 2: Initialize algorithm parameters

    m_ratio_th = 0.01;// default pass ratio
    m_ATE_tolerance = 0;
 
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerATE, m_ratio_th), "%lf", "ratio_th", 0, 1, 0.01 },
        { offsetof(CContentAnalyzerATE, m_ATE_tolerance), "%d", "ATE_tolerance", 0, 255, 0 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContentAnalyzerATE::~CContentAnalyzerATE()
{
}

ANALYZER_RETRUN CContentAnalyzerATE::
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
        // Currently due to known issue, buffer is mismatched in size
        LOG_WARNING("Input image resolution doesn't match each other", NULL);
        // should be failed in future
        //return ANALYZER_IMAGE_EXCEPTION;
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

        if (img_array[i].param.val.i < 0 ||
            img_array[i].param.val.i > 2)
        {
            LOG_ERROR(CStringFormat("Input parameter is out of range [0, 1]").Get(),
                CStringFormat("Its value is %d", img_array[i].param.val.i).Get());
            return ANALYZER_PARAMETER_EXCEPTION;
        }
    }

    // Step 5: Analyze
    int rc = -4;
    try
    {
        if (img_array[iZero].param.val.i < img_array[1 - iZero].param.val.i)
            rc = CompareATEPixelbyPixel(img_array[iZero].img, img_array[1 - iZero].img, img_debug, m_ATE_tolerance);
        else
            rc = CompareATEPixelbyPixel(img_array[1 - iZero].img, img_array[iZero].img, img_debug, m_ATE_tolerance);

    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    if (rc != 0)
    {
        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}
struct rgb_p
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    rgb_p(unsigned char r0, unsigned char g0, unsigned char b0)
    {
        r = r0;
        b = b0;
        g = g0;
    }
};
rgb_p color_table[11] = { rgb_p(255, 0, 0), rgb_p(255, 80, 0), \
rgb_p(255, 165, 0), rgb_p(255, 200, 0), \
rgb_p(255, 255, 0), rgb_p(125, 255, 0), \
rgb_p(0, 255, 0), rgb_p(0, 127, 127), \
rgb_p(0, 127, 255), rgb_p(0, 0, 255), \
rgb_p(139, 0, 255) };
int dif_count[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


rgb_p color_select(int intensity)
{
    switch (intensity)
    {
    case 10:
        return color_table[0];
    case 9:
        return color_table[1];
    case 8:
        return color_table[2];
    case 7:
        return color_table[3];
    case 6:
        return color_table[4];
    case 5:
        return color_table[5];
    case 4:
        return color_table[6];
    case 3:
        return color_table[7];
    case 2:
        return color_table[8];
    case 1:
        return color_table[9];
    case 0:
        return color_table[10];
    default:
        return color_table[10];
    }
}

int CompareATEPixelbyPixel(Mat &raw, Mat &reference, Mat &debug_yuv, int tolerance)
{
    int max_dif = INT_MIN;
    int min_dif = INT_MAX;
    
    if (!raw.empty() && !reference.empty())
    {
        Mat reference_yuv = Mat(reference.rows, reference.cols,CV_8UC3);
        Mat raw_yuv = Mat(raw.rows, raw.cols, CV_8UC3);
        debug_yuv = Mat(raw_yuv.rows, raw_yuv.cols, CV_8UC3);
        cvtColor(reference, reference_yuv, CV_BGR2YUV);
        cvtColor(raw, raw_yuv, CV_BGR2YUV);
        //LOG_INFO("Convert rgb 2 yuv", NULL);
        uchar* pReference = NULL;
        uchar* pRaw = NULL;
        uchar* pDebug = NULL;
        int dif_y = 0, dif_u = 0, dif_v = 0;
        int rows = raw_yuv.rows;
        int cols = raw_yuv.cols;
        long bad_pixel_count = 0;
        for (int i = 0; i < rows; i++)
        {
            pRaw = raw_yuv.data + i * raw_yuv.step;
            pReference = reference_yuv.data + i * reference_yuv.step;
            pDebug = debug_yuv.data + i * debug_yuv.step;

            for (int j = 0; j < cols; j++)
            {
                dif_y = abs(int(pRaw[3 * j + 0]) - int(pReference[3 * j + 0])) <= tolerance ? 0 : 1;
                dif_u = abs(int(pRaw[3 * j + 1]) - int(pReference[3 * j + 1])) <= tolerance ? 0 : 1;
                dif_v = abs(int(pRaw[3 * j + 2]) - int(pReference[3 * j + 2])) <= tolerance ? 0 : 1;
                int sum = abs(int(pRaw[3 * j + 0]) - int(pReference[3 * j + 0])) + abs(int(pRaw[3 * j + 1]) - int(pReference[3 * j + 1])) \
                    + abs(int(pRaw[3 * j + 2]) - int(pReference[3 * j + 2]));
                sum /= 3;
                if (sum > max_dif)
                    max_dif = sum;
                if (sum < min_dif)
                    min_dif = sum;
                if (dif_y & dif_u & dif_v)
                {
                    bad_pixel_count++;
                    if (dif_y)
                        pDebug[3 * j + 0] = sum;
                    else
                        pDebug[3 * j + 0] = 0;
                    if (dif_u)
                        pDebug[3 * j + 1] = sum;
                    else
                        pDebug[3 * j + 1] = 0;
                    if (dif_v)
                        pDebug[3 * j + 2] = sum;
                    else
                        pDebug[3 * j + 2] = 0;
                }
            }
        }

        vector<Mat> vec_debug_yuv(3, Mat());
        split(debug_yuv, vec_debug_yuv);
        //imwrite("./debug_y.bmp", vec_debug_yuv[0]);
        //imwrite("./debug_u.bmp", vec_debug_yuv[1]);
        //imwrite("./debug_v.bmp", vec_debug_yuv[2]);
        LOG_INFO(CStringFormat("debug img %d x %d", cols, rows).Get(), NULL);
        for (int i = 0; i < rows; i++)
        {
            pDebug = debug_yuv.data + i * debug_yuv.step;
            for (int j = 0; j < cols; j++)
            {
                int val = int(pDebug[3 * j + 0]);
                if (val > 0)
                {
                    int intensity = int(val * 1.0 / max_dif * 10);
                    dif_count[intensity] ++;
                    rgb_p pix = color_select(intensity);
                    pDebug[3 * j + 0] = pix.b;
                    pDebug[3 * j + 1] = pix.g;
                    pDebug[3 * j + 2] = pix.r;
                }
                else
                {
                    pDebug[3 * j + 0] = 0;
                    pDebug[3 * j + 1] = 0;
                    pDebug[3 * j + 2] = 0;
                }

            }
        }

        //imwrite("./debug_yuv_bgr.bmp", debug_yuv);

        LOG_INFO(CStringFormat("max difference:%lf", max_dif).Get(), NULL);
        LOG_INFO(CStringFormat("min difference:%lf", min_dif).Get(), NULL);

        for (int i = 0; i < 11; i++)
            LOG_INFO(CStringFormat("diff intensity index(%d): # %d", i, dif_count[i]).Get(), NULL);

        if (bad_pixel_count < 0.01 * rows * cols)
        {
            LOG_INFO(CStringFormat("bad pixel # : %ld", bad_pixel_count).Get(), NULL);
            LOG_INFO(CStringFormat("PASS: different ratio is  %lf", 1.0 * bad_pixel_count / (rows * cols)).Get(), NULL);
            return 0;
        }
        else
        {
            LOG_ERROR(CStringFormat("bad pixel # : %ld", bad_pixel_count).Get(), NULL);
            LOG_ERROR(CStringFormat("PASS: different ratio is  %lf", 1.0 * bad_pixel_count / (rows * cols)).Get(), NULL);
            return -3;
        }
    }
    else
    {
        LOG_ERROR("Images not found!", NULL);
        return -2;
    }

}