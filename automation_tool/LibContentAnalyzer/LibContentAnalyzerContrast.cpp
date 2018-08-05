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


#include "LibContentAnalyzerContrast.h"

double CalcContrast(Mat &src, Rect roi, Mat &YMat, double &meanY);
double CompareContrastByPixel(Mat &Ysrc, Mat &Ydst, Mat &delta, double meanYsrc, double meanYdst);

CContentAnalyzerContrast::CContentAnalyzerContrast() :
    CContenAnalyzerBase(ANALYZER_CONTRAST_COMPARE)
{
    //Step 1:Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerContrastCompare");

    //Step 2: Initialize algorithm parameters
    th = 0.8;

    //Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerContrast, th), "%lf", "parameter_threshold", 0, 1, 0.8 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);

}

CContentAnalyzerContrast::~CContentAnalyzerContrast()
{}

ANALYZER_RETRUN CContentAnalyzerContrast::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    //Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 2: Check Input parameter type
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
            LOG_ERROR("Input parameter is out of range", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

    }

    if (img_array[0].param.val.i < img_array[1].param.val.i)
    {
        iZero = 0;
    }
    else
    {
        iZero = 1;
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
    Mat Ymat_none, Ymat_effect;
    double mean_none = 0, mean_effect = 0;

    double contast_none = CalcContrast(img_none, Rect(0, 0, img_none.cols, img_none.rows), Ymat_none, mean_none);
    double contast_effect = CalcContrast(img_effect, Rect(0, 0, img_effect.cols, img_effect.rows), Ymat_effect, mean_effect);

    if (contast_effect < contast_none)
    {
        LOG_ERROR("contast_effect < contast_none", NULL);
        return ANALYZER_RETRUN_FAIL;
    }

    double error_rate = 0;

    error_rate = CompareContrastByPixel(Ymat_none, Ymat_effect, img_debug, mean_none, mean_effect);

    if (error_rate > 1 - th)
    {
        LOG_ERROR(CStringFormat("Error rate is exceeding threshold %lf", 1 - th).Get(),
            CStringFormat("Error rate is %lf", error_rate).Get());

        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}

double CalcContrast(Mat &src, Rect roi, Mat &YMat, double &meanY)
{
    int x1 = roi.x;
    int y1 = roi.y;

    x1 = max(0, x1);
    y1 = max(0, y1);

    int x2 = x1 + roi.width;
    int y2 = y1 + roi.height;

    x2 = min(x2, src.cols);
    y2 = min(y2, src.rows);

    double ret = -1;
    double avgY = 0, Y = 0, sigmaY = 0, diffY = 0;
    double R, G, B;

    YMat.create(Size(src.cols, src.rows), CV_64FC1);

    int size = (y2 - y1) * (x2 - x1);

    if (size <= 0)
    {
        return ret;
    }

    uchar *srcData = NULL;
    double *YMatData = NULL;

    //Y = 0.299 * R + 0.587 * G + 0.114 * B
    for (int y = y1; y < y2; y++)
    {
        srcData = src.data + y * src.step;
        YMatData = (double *)(YMat.data + y * YMat.step);

        for (int x = x1, x3 = x1 * 3; x < x2; x++)
        {
            B = srcData[x3++];
            G = srcData[x3++];
            R = srcData[x3++];

            Y = 0.299 * R + 0.587 * G + 0.114 * B;
            avgY += Y;
            YMatData[x] = Y;
        }
    }

    avgY /= size;
    meanY = avgY;

    for (int y = y1; y < y2; y++)
    {
        YMatData = (double *)(YMat.data + y * YMat.step);

        for (int x = x1; x < x2; x++)
        {
            Y = YMatData[x];
            diffY = Y - avgY;
            sigmaY += diffY * diffY;
        }
    }

    sigmaY = sqrt(sigmaY / size);

    ret = sigmaY / avgY;

    return ret;
}

double CompareContrastByPixel(Mat &Ysrc, Mat &Ydst, Mat &delta, double meanYsrc, double meanYdst)
{
    double *YSrcData = NULL, *YDstData = NULL;

    double ratio = 0;

    delta.create(Size(Ysrc.cols, Ysrc.rows), CV_8UC1);

    uchar *deltaData = NULL;

    for (int y = 0; y < Ysrc.rows; y++)
    {
        YSrcData = (double *)(Ysrc.data + y * Ysrc.step);
        YDstData = (double *)(Ydst.data + y * Ydst.step);

        deltaData = delta.data + y * delta.step;

        for (int x = 0; x < Ysrc.cols; x++)
        {
            double contrast1 = abs(YSrcData[x] - meanYsrc) / meanYsrc;
            double contrast2 = abs(YDstData[x] - meanYdst) / meanYdst;

            if (contrast1 > contrast2)
            {
                deltaData[x] = 255 - (uchar)YSrcData[x];
                ratio++;
            }
            else
            {
                deltaData[x] = (uchar)YSrcData[x];
            }
        }
    }

    return ratio / (Ysrc.rows * Ysrc.cols);
}





