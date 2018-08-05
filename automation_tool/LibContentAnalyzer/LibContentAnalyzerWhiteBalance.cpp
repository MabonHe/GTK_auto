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

#include "LibContentAnalyzerWhiteBalance.h"

void WhiteBalanceCompare(Mat &src1, Mat &src2, double &Yerror, double &deltaR, double &deltaG, double &deltaB);

CContentAnalyzerWhiteBalance::CContentAnalyzerWhiteBalance() :
CContenAnalyzerBase(ANALYZER_WHITEBALANCE_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerWhiteBalanceCompare");

    // Step 2: Initialize algorithm parameters
    Y_threshold = 50;
    G_threshold = 0.2;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerWhiteBalance, Y_threshold), "%lf", "Y_threshold", 0, 255 * 255, 50 },
        { offsetof(CContentAnalyzerWhiteBalance, G_threshold), "%lf", "G_threshold", 0, 1, 0.2 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContentAnalyzerWhiteBalance::~CContentAnalyzerWhiteBalance()
{}

ANALYZER_RETRUN CContentAnalyzerWhiteBalance::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    //Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    Mat src1, src2;
    src1 = img_array[0].img;
    src2 = img_array[1].img;

    //Step 2: Check Input Image
    if (src1.empty() || src2.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 3: Check Input Image Resolution
    if (src1.cols != src2.cols || src1.rows != src2.rows)
    {
        LOG_ERROR("Different Resolution", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 4: Analyzer
    double Yerror = 0, deltaR = 0, deltaB = 0, deltaG = 0;
    WhiteBalanceCompare(src1, src2, Yerror, deltaR, deltaG, deltaB);

    if (Yerror < Y_threshold && abs(deltaG) < G_threshold && deltaB > deltaR)
    {
        return ANALYZER_RETRUN_PASS;
    }

    LOG_ERROR(CStringFormat("Criteria: Yerror<%lf, |deltaG|<%lf, deltaB>deltaR",
        Y_threshold, G_threshold).Get(), CStringFormat("Yerror %lf, deltaG %lf, deltaR %lf, deltaB %lf",
        Yerror, deltaG, deltaR, deltaB).Get());

    return ANALYZER_RETRUN_FAIL;
}


void CvtRGB2YCrCb(uchar r, uchar g, uchar b, double &Y, double &Cr, double &Cb)
{
    Y = 0.299 * r + 0.587 * g + 0.114 * b;
    Cb = b - Y;//-0.1687 * r - 0.3313 * g + 0.5 * b;
    Cr = r - Y;//0.5 * r - 0.4187 * g - 0.0813 * b;
}

void WhiteBalanceCompare(Mat &src1, Mat &src2, double &Yerror, double &deltaR, double &deltaG, double &deltaB)
{
    // Y should keep the same
    // Calculate the deltaR sum and deltaB sum
    uchar *src1Data = NULL, *src2Data = NULL;

    Yerror = 0;
    deltaR = 0;
    deltaB = 0;
    deltaG = 0;

    for (int y = 0; y < src1.rows; y++)
    {
        src1Data = src1.data + y * src1.step;
        src2Data = src2.data + y * src2.step;

        for (int x = 0; x < src1.cols * 3; x += 3)
        {
            double Y1 = 0.299 * src1Data[x + 2] + 0.587 * src1Data[x + 1] + 0.114 * src1Data[x];
            double Y2 = 0.299 * src2Data[x + 2] + 0.587 * src2Data[x + 1] + 0.114 * src2Data[x];
            double dY = Y1 - Y2;
            Yerror += dY * dY;

            if (Y1 < 1e-6)
                Y1 = 1;

            if (Y2 < 1e-6)
                Y2 = 1;

            deltaR += ((double)src1Data[x + 2] - (double)src2Data[x + 2]) / 255;
            deltaG += ((double)src1Data[x + 1] - (double)src2Data[x + 1]) / 255;
            deltaB += ((double)src1Data[x] - (double)src2Data[x]) / 255;
        }
    }

    double size = src1.cols * src1.rows;
    Yerror /= size;
    Yerror = sqrt(Yerror);
    deltaR /= size;
    deltaG /= size;
    deltaB /= size;
}