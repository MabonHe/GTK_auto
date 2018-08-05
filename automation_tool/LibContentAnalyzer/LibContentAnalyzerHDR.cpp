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

#include "LibContentAnalyzerHDR.h"

void CompareHDRbyPixel(Mat &non_hdr, Mat &hdr, Mat &delta, int dth,
    int &ncount_dark, double &sharpness_none_dark, double &sharpness_effect_dark,
    int &ncount_bright, double &sharpness_none_bright, double &sharpness_effect_bright);

void CompareHDRbyBlock(Mat &non_hdr, Mat &hdr, Mat &delta, int dth,
    int &ncount_dark, double &sharpness_none_dark, double &sharpness_effect_dark,
    int &ncount_bright, double &sharpness_none_bright, double &sharpness_effect_bright);

CContenAnalyzerHDR::CContenAnalyzerHDR() :
CContenAnalyzerBase(ANALYZER_HDR_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerHDRCompare");

    // Step 2: Initialize algorithm parameters
    m_luminance_diff_th = 20;
    m_change_area_min_ratio = 1./64;
    m_sharpness_ratio_th = 0.1;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerHDR, m_luminance_diff_th), "%d", "luminance_diff_th", 0, 255, 20 },
        { offsetof(CContenAnalyzerHDR, m_change_area_min_ratio), "%lf", "change_area_min_ratio", 0, 1, 1./64 },
        { offsetof(CContenAnalyzerHDR, m_sharpness_ratio_th), "%lf", "sharpness_ratio_th", 0, 1, 0.1 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerHDR::~CContenAnalyzerHDR()
{

}

ANALYZER_RETRUN CContenAnalyzerHDR::
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
    for (size_t i = 0; i < img_array.size(); i++)
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
        LOG_ERROR("No none HDR image in the inputs", NULL);
        return ANALYZER_PARAMETER_EXCEPTION;
    }

    // Step 5: Analyze
    int ncount_dark = 0;
    int ncount_bright = 0;
    double sharpness_none_dark = 0;
    double sharpness_effect_dark = 0;
    double sharpness_none_bright = 0;
    double sharpness_effect_bright = 0;

   //Comapre pixel by pixel
    /*CompareHDRbyPixel(img_array[iZero].img, img_array[1 - iZero].img, img_debug, m_luminance_diff_th,
        ncount_dark, sharpness_none_dark, sharpness_effect_dark,
        ncount_bright, sharpness_none_bright, sharpness_effect_bright);*/

    //Compare block by block
    CompareHDRbyBlock(img_array[iZero].img, img_array[1 - iZero].img, img_debug, m_luminance_diff_th,
        ncount_dark, sharpness_none_dark, sharpness_effect_dark,
        ncount_bright, sharpness_none_bright, sharpness_effect_bright);

    int ncountTh = (int)(m_change_area_min_ratio *  img_array[iZero].img.cols * img_array[iZero].img.rows);

    if (ncountTh > ncount_dark + ncount_bright)
    {
        LOG_ERROR("The pixel count of luminance changed is smaller than expected", NULL);
        LOG_ERROR(CStringFormat("dark(%d) + bringt(%d) < th(%d)", ncount_dark, ncount_bright, ncountTh).Get(), NULL);
        return ANALYZER_RETRUN_FAIL;
    }

    if (ncount_dark > ncountTh / 2 &&
        sharpness_effect_dark / sharpness_none_dark < 1 + m_sharpness_ratio_th)
    {
        LOG_ERROR("The dark part shapeness is not enhanced as expected", NULL);
        LOG_ERROR(CStringFormat("effect(%lf) / none(%lf) < 1 + %lf ", sharpness_effect_dark,
            sharpness_none_dark, m_sharpness_ratio_th).Get(), NULL);
        return ANALYZER_RETRUN_FAIL;
    }

    if (ncount_bright > ncountTh / 2 &&
        sharpness_effect_bright / sharpness_none_bright < 1 + m_sharpness_ratio_th)
    {
        LOG_ERROR("The bright part shapeness is not enhanced as expected", NULL);
        LOG_ERROR(CStringFormat("effect(%lf) / none(%lf) < 1 + %lf ", sharpness_effect_bright,
            sharpness_none_bright, m_sharpness_ratio_th).Get(), NULL);
        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}


void CompareHDRbyPixel(Mat &non_hdr, Mat &hdr, Mat &delta, int dth,
    int &ncount_dark, double &sharpness_none_dark, double &sharpness_effect_dark,
    int &ncount_bright, double &sharpness_none_bright, double &sharpness_effect_bright)
{
    int w = non_hdr.cols;
    int h = non_hdr.rows;

    Mat non_hdr_gray, hdr_gray;

#ifndef _WINDOWS_MOBILE
    cvtColor(non_hdr, non_hdr_gray, CV_BGR2GRAY);
    cvtColor(hdr, hdr_gray, CV_BGR2GRAY);
#else
    cvtColor(non_hdr, non_hdr_gray, COLOR_BGR2GRAY);
    cvtColor(hdr, hdr_gray, COLOR_BGR2GRAY);
#endif

    delta.create(Size(w, h), CV_8UC3);
    delta.setTo(0);

    // Compare the brightness
    uchar *nonData = NULL, *hdrData = NULL, *deltaData = NULL;
    uchar *nonData1 = NULL, *nonData2 = NULL, *hdrData1 = NULL, *hdrData2 = NULL;

    sharpness_none_dark = 0;
    sharpness_effect_dark = 0;
    sharpness_none_bright = 0;
    sharpness_effect_bright = 0;

    int ncount1 = 0, ncount2 = 0;

    // Use sobel to calculate the sharpness value
    // [-1, -2, 1; 0, 0, 0; 1, 2, 1]

    for (int y = 1; y < h - 1; y++)
    {
        nonData = non_hdr_gray.data + y * non_hdr_gray.step;
        nonData1 = non_hdr_gray.data + (y - 1) * non_hdr_gray.step;
        nonData2 = non_hdr_gray.data + (y + 1) * non_hdr_gray.step;

        hdrData = hdr_gray.data + y * hdr_gray.step;
        hdrData1 = hdr_gray.data + (y - 1) * hdr_gray.step;
        hdrData2 = hdr_gray.data + (y + 1) * hdr_gray.step;

        deltaData = delta.data + y * delta.step;

        for (int x = 1; x < w - 1; x++)
        {
            int d = (int) (nonData[x]) - (int) (hdrData[x]);
            int x_3 = x * 3;

            if (abs(d) > dth)
            {
                int dx1 = (int) nonData1[x - 1] + 2 * (int) nonData[x - 1] + (int) nonData2[x - 1] -
                    ((int) nonData1[x + 1] + 2 * (int) nonData[x + 1] + (int) nonData2[x + 1]);
                int dy1 = (int) nonData1[x - 1] + 2 * (int) nonData1[x] + (int) nonData1[x + 1] -
                    ((int) nonData2[x - 1] + 2 * (int) nonData2[x] + (int) nonData2[x + 1]);

                double sobel1 = sqrt(dx1 * dx1 + dy1 * dy1);

                int dx2 = (int) hdrData1[x - 1] + 2 * (int) hdrData[x - 1] + (int) hdrData2[x - 1] -
                    ((int) hdrData1[x + 1] + 2 * (int) hdrData[x + 1] + (int) hdrData2[x + 1]);
                int dy2 = (int) hdrData1[x - 1] + 2 * (int) hdrData1[x] + (int) hdrData1[x + 1] -
                    ((int) hdrData2[x - 1] + 2 * (int) hdrData2[x] + (int) hdrData2[x + 1]);

                double sobel2 = sqrt(dx2 * dx2 + dy2 * dy2);

                if (d < 0)
                {
                    sharpness_none_dark += sobel1;
                    sharpness_effect_dark += sobel2;
                    ncount1++;
                    deltaData[x_3] = 255;    //(uchar)(nonData[x] * 0.5) + 128;
                    deltaData[x_3 + 1] = 0;    //(uchar)(nonData[x+1] * 0.5);
                    deltaData[x_3 + 2] = 0;    //(uchar)(nonData[x+2] * 0.5);
                }
                else
                {
                    sharpness_none_bright += sobel1;
                    sharpness_effect_bright += sobel2;
                    ncount2++;
                    deltaData[x_3] = 0;   //(uchar)(nonData[x] * 0.5);
                    deltaData[x_3 + 1] = 0;   //(uchar)(nonData[x+1] * 0.5);
                    deltaData[x_3 + 2] = 255; //(uchar)(nonData[x+2] * 0.5) + 128;
                }
            }
        }
    }

    ncount_dark = ncount1;
    ncount_bright = ncount2;
}


int CalSobelbyBlock(Mat &src, int y, int x, int block_size)
{
    int value = 0;
    for (int i = y; i < y + block_size; i++)
    {
        uchar *runner = src.data + y*src.step;
        for (int j = x; j < x + block_size; j++)
        {
            value += runner[x];
        }
    }
    return value;
}


void CompareHDRbyBlock(Mat &non_hdr, Mat &hdr, Mat &delta, int dth,
    int &ncount_dark, double &sharpness_none_dark, double &sharpness_effect_dark,
    int &ncount_bright, double &sharpness_none_bright, double &sharpness_effect_bright)
{
    Mat non_hdr_gray, hdr_gray;
    Mat s0, s1;
    cvtColor(non_hdr, non_hdr_gray, CV_BGR2GRAY);
    cvtColor(hdr, hdr_gray, CV_BGR2GRAY);
    medianBlur(non_hdr_gray, non_hdr_gray, 5);
    medianBlur(hdr_gray, hdr_gray, 5);
    Sobel(non_hdr_gray, s0, non_hdr_gray.depth(), 2, 2);
    convertScaleAbs(s0, s0);
    Sobel(hdr_gray, s1, hdr_gray.depth(), 2, 2);
    convertScaleAbs(s1, s1);

    int effect_bright_count = 0;
    int none_bright_count = 0;
    int effect_bright_coef = 0;
    int none_bright_coef = 0;

    int effect_dark_count = 0;
    int none_dark_coef = 0;
    int none_dark_count = 0;
    int effect_dark_coef = 0;

    int kernel_size = 16;
    int h = non_hdr.rows;
    int w = non_hdr.cols;

    double exp_th = 0;
    // to check the HDR under good test condition where HDR effect should be weak
    double ratio = 0.05;

    delta.create(Size(w, h), CV_8UC3);
    delta.setTo(255);

    for (int i = 0; i <= h - kernel_size; i++)
    {
        uchar* eff = hdr.data + i*hdr.step;
        uchar* none = non_hdr.data + i*non_hdr.step;
        uchar* dt = delta.data + i*delta.step;
        for (int j = 0; j <= w - kernel_size; j++)
        {
            exp_th = ratio * eff[j];
            if (eff[j] >= (none[j] + exp_th))
            {
                ncount_bright++;
                effect_bright_coef = CalSobelbyBlock(s1, i, j, kernel_size);
                none_bright_coef = CalSobelbyBlock(s0, i, j, kernel_size);
                if (effect_bright_coef >= none_bright_coef)
                {
                    sharpness_effect_bright += effect_bright_coef;
                    effect_bright_count++;
                }
                else
                {
                    sharpness_none_bright += none_bright_coef;
                    none_bright_count++;
                    dt[3 * j] = 255;
                    dt[3 * j + 1] = 0;
                    dt[3 * j + 2] = 0;
                }
            }
            else if (eff[j] <= (none[j] - exp_th))
            {
                ncount_dark++;
                effect_dark_coef = CalSobelbyBlock(s1, i, j, kernel_size);
                none_dark_coef = CalSobelbyBlock(s0, i, j, kernel_size);
                if (effect_dark_coef >= none_dark_coef)
                {
                    sharpness_effect_dark += (double)effect_dark_coef;
                    effect_dark_count++;
                }
                else
                {
                    sharpness_none_dark += (double)none_dark_coef;
                    none_dark_count++;
                    dt[3 * j + 2] = 255;
                    dt[3 * j + 1] = 0;
                    dt[3 * j] = 0;
                }
            }
            else
            {
                int effect_coef = CalSobelbyBlock(s1, i, j, kernel_size);
                int none_coef = CalSobelbyBlock(s0, i, j, kernel_size);
                if (eff[j] >= none[j])
                {
                    if (effect_coef > (none_coef - effect_coef * ratio))
                    {
                        sharpness_effect_bright += effect_bright_coef;
                        effect_bright_count++;
                    }
                }
                else
                {
                    if (effect_coef >(none_coef - effect_coef * ratio))
                    {
                        sharpness_effect_dark += (double)effect_dark_coef;
                        effect_dark_count++;
                    }
                }
            }
        }
    }

}