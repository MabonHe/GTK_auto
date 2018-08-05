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

#include "LibContentAnalyzerTorch.h"

void TorchCompare(Mat &img_low, Mat &img_high, Mat &delta, int deltaL_th, double &dark_ratio, double &bright_ratio);

CContentAnalyzerTorch::CContentAnalyzerTorch() :
CContenAnalyzerBase(ANALYZER_TORCH_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerTorchCompare");

    // Step 2: Initialize algorithm parameters
    th = 0.6;
    deltaL_th = 10;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerTorch, th), "%lf", "parameter_threshold", 0, 1, 0.6 },
        { offsetof(CContentAnalyzerTorch, deltaL_th), "%d", "parameter_deltal_th", -255, 255, 10 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContentAnalyzerTorch::~CContentAnalyzerTorch()
{}

ANALYZER_RETRUN CContentAnalyzerTorch::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    //Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    Mat img1, img2;
    img1 = img_array.at(0).img;
    img2 = img_array.at(1).img;

    //Step 2: Check Input Image
    if (img1.empty() || img2.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Check Input Image Resolution
    if (img1.cols != img2.cols || img1.rows != img2.rows)
    {
        LOG_ERROR("Different Resolution", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 4: Analyzer
    Mat img_delta, img1_grey, img2_grey;

    double dark_ratio = 0;
    double bright_ratio = 0;

#ifndef _WINDOWS_MOBILE
    cvtColor(img1, img1_grey, CV_BGR2GRAY);
    cvtColor(img2, img2_grey, CV_BGR2GRAY);
#else
    cvtColor(img1, img1_grey, COLOR_BGR2GRAY);
    cvtColor(img2, img2_grey, COLOR_BGR2GRAY);
#endif

    TorchCompare(img1_grey, img2_grey, img_delta, deltaL_th, dark_ratio, bright_ratio);


    //Under some circumstance, the 4 corner sides of the whole image might not become darker! (|| ==> &&)
    if (dark_ratio < th && bright_ratio < th)
    {
        LOG_ERROR(CStringFormat("Fail due to dark_ratio and bright_ratio is less than threshold(%lf)", th).Get(), CStringFormat("dark_ratio %lf, bright_ratio %lf", dark_ratio, bright_ratio).Get());

        return ANALYZER_RETRUN_FAIL;
    }
    else if (dark_ratio <= th)
    {
        if (bright_ratio > th * 3) // check the brighter parts with a higher threshold
            return ANALYZER_RETRUN_PASS;
        else
        {
            LOG_ERROR(CStringFormat("Fail due to bright_ratio is less than threshold(%lf)", th).Get(), CStringFormat("bright_ratio %lf",bright_ratio).Get());

            return ANALYZER_RETRUN_FAIL;
        }
    }
    else if (dark_ratio >= th && bright_ratio < th)
    {
        LOG_ERROR(CStringFormat("Fail due to bright_ratio is less than threshold(%lf)", th).Get(), CStringFormat("bright_ratio %lf", bright_ratio).Get());

        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}


void TorchCompare(Mat &img_low, Mat &img_high, Mat &delta, int deltaL_th, double &dark_ratio, double &bright_ratio)
{
    delta.create(img_low.rows, img_low.cols, CV_8UC1);
    uchar *data1 = NULL, *data2 = NULL, *datad = NULL;

    double dark_good = 0;
    double dark_total = 0;
    double bright_good = 0;
    double bright_total = 0;

    int w = img_low.cols;
    int h = img_low.rows;
    int wh2 = w * h / 2;

    for (int y = 0; y < img_low.rows; y++)
    {
        data1 = img_low.data + y * img_low.step;
        data2 = img_high.data + y * img_high.step;
        datad = delta.data + y * delta.step;

        for (int x = 0; x < img_low.cols; x++)
        {
            int delta = (int)data2[x] - (int)data1[x];

            if (delta < -deltaL_th)
            {
                dark_total++;

                /*
                1. h*x - w*y + w*h/2 < 0
                2. h*x + w*y - w*h/2 < 0
                3. h*x - w*y - w*h/2 > 0
                4. h*x + w*y - w*h*3/2 > 0
                */
                int hx = h * x;
                int wy = w * y;


                if (hx - wy + wh2 < 0 ||
                    hx + wy - wh2 < 0 ||
                    hx - wy - wh2 > 0 ||
                    hx + wy - wh2 * 3 > 0)
                {
                    dark_good++;
                }


                datad[x] = 0;
            }
            else if (delta > deltaL_th)
            {
                bright_total++;

                int hx = h * x;
                int wy = w * y;

                if (hx - wy + wh2 > 0 &&
                    hx + wy - wh2 > 0 &&
                    hx - wy - wh2 < 0 &&
                    hx + wy - wh2 * 3 < 0)
                {
                    bright_good++;
                }

                datad[x] = 255;
            }
            else
            {
                datad[x] = 128;
            }
        }
    }

    if (dark_total < 0.1)
    {
        dark_ratio = 0;
    }
    else
    {
        dark_ratio = dark_good / dark_total;
    }

    if (bright_total < 0.1)
    {
        bright_ratio = 0;
    }
    else
    {
        bright_ratio = bright_good / bright_total;
    }
}