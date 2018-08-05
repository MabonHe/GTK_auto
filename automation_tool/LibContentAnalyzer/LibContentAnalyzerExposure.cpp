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

#include "LibContentAnalyzerExposure.h"

double CalcDeltaImage_E(Mat &img1, Mat &img2, Mat &delta);

CContentAnalyzerExposure::CContentAnalyzerExposure() :
    CContenAnalyzerBase(ANALYZER_EXPOSURE_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerExposureCompare");

    // Step 2: Initialize algorithm parameters
    th = 0.9;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerExposure, th), "%lf", "parameter_threshold", 0, 1, 0.9 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContentAnalyzerExposure::~CContentAnalyzerExposure()
{}

ANALYZER_RETRUN CContentAnalyzerExposure::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    //Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    Mat img1, img2;
    img1 = img_array[0].img;
    img2 = img_array[1].img;

    // Step 2: Check Input Image
    if (img1.empty() || img2.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 3: Check Input Image Resolution
    if (img1.cols != img2.cols || img1.rows != img2.rows)
    {
        LOG_ERROR("Different Resolution", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 4: Analyzer
    Mat img1_grey, img2_grey;

#ifndef _WINDOWS_MOBILE
    cvtColor(img1, img1_grey, CV_BGR2GRAY);
    cvtColor(img2, img2_grey, CV_BGR2GRAY);
#else
    cvtColor(img1, img1_grey, COLOR_BGR2GRAY);
    cvtColor(img2, img2_grey, COLOR_BGR2GRAY);
#endif

    double var = CalcDeltaImage_E(img1_grey, img2_grey, img_debug);

    if (var < th)
    {
        LOG_ERROR(CStringFormat("Fail: Var < th(%lf)", th).Get(),
            CStringFormat(" Var is %lf", var).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    //WriteErrorMessage("Pass: Var(%lf) > th(%lf)", var, th);

    return ANALYZER_RETRUN_PASS;
}

double CalcDeltaImage_E(Mat &img1, Mat &img2, Mat &delta)
{
    delta.create(img1.rows, img1.cols, CV_8UC1);
    uchar *data1 = NULL, *data2 = NULL, *datad = NULL;
    double error = 0;
    double size = img1.rows * img1.cols;

    for (int y = 0; y < img1.rows; y++)
    {
        data1 = img1.data + y * img1.step;
        data2 = img2.data + y * img2.step;
        datad = delta.data + y * delta.step;

        for (int x = 0; x < img1.cols; x++)
        {
            int delta = (int)data2[x] - (int)data1[x];

            error += delta < 0 ? 0 : 1;

            if (delta < 0)
            {
                delta = 0;
            }

            datad[x] = (uchar)delta;
        }
    }
    return error / size;
}