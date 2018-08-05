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


#include "LibContentAnalyzerFOV.h"
#include "../LibSimilarityCheck/LibSimilarityCheck.h"

void CalculateFOVLoss(Mat &H, int maxW, int maxH, int w, int h, double &hfovloss, double &vfovloss);

CContentAnalyzerFOV::CContentAnalyzerFOV() :
CContenAnalyzerBase(ANALYZER_FOV_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerFOVCompare");

    // Step 2: Initialize algorithm parameters
    m_hFovLossTh = 0.1;
    m_vFovLossTh = 0.1;
    m_shiftTH = 0.002;
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerFOV, m_hFovLossTh), "%lf", "hfov_loss_th", 0, 1, 0.1 },
        { offsetof(CContentAnalyzerFOV, m_vFovLossTh), "%lf", "vfov_loss_th", 0, 1, 0.1 },
        { offsetof(CContentAnalyzerFOV, m_shiftTH), "%lf", "shift_th", 0, 1, 0.002 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerFOV::~CContentAnalyzerFOV()
{}

ANALYZER_RETRUN CContentAnalyzerFOV::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    double hfovloss = 0;
    double vfovloss = 0;

    // Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    Mat image_full_fov, image_frame;

    image_full_fov = img_array[0].img;
    image_frame = img_array[1].img;

    if (image_full_fov.empty() || image_frame.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Analyzer
    Mat H;
    if (!CalculateHomographyMatrix(image_full_fov, image_frame, H))
    {
        LOG_ERROR("Failed to CalculateHomographyMatrix", NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    CalculateFOVLoss(H, image_full_fov.cols, image_full_fov.rows,
        image_frame.cols, image_frame.rows, hfovloss, vfovloss);

    if (hfovloss > m_hFovLossTh || vfovloss > m_vFovLossTh)
    {
        LOG_ERROR(CStringFormat("hfovloss or vfovloss is larger "
            "than hfov_loss_th(%lf) or vfov_loss_th(%lf)", m_hFovLossTh, m_vFovLossTh).Get(),
            CStringFormat("hfovloss(%lf), vfovloss(%lf)", hfovloss, vfovloss).Get());

        GenerateReferenceFromFullFov(image_full_fov, H,
            img_reference, image_frame.cols, image_frame.rows);

        return ANALYZER_RETRUN_FAIL;
    }
    // add central shift detection
    else
    {
        LOG_INFO("FOV loss check pass!", NULL);
        double H00 = *(double*)H.ptr(0, 0);
        double H01 = *(double*)H.ptr(0, 1);
        double H02 = *(double*)H.ptr(0, 2);
        double H10 = *(double*)H.ptr(1, 0);
        double H11 = *(double*)H.ptr(1, 1);
        double H12 = *(double*)H.ptr(1, 2);
        double H20 = *(double*)H.ptr(1, 0);
        double H21 = *(double*)H.ptr(2, 1);
        double H22 = *(double*)H.ptr(2, 2);

        LOG_INFO(CStringFormat("Matrix is : [%lf, %lf, %lf\n%lf, %lf, %lf\n%lf, %lf, %lf]", H00, H01, H02, H10, H11, H12, H20, H21, H22).Get(), NULL);

        if (abs(H00 - H11) / H00 < 0.05)
        {
            LOG_INFO(CStringFormat("Zoom ratio in x(%lf) and y(%lf) direction is in same scale", H.at<double>(0, 0), H.at<double>(1, 1)).Get(), NULL);
            double shiftx = (H00 + H11) / 2 * H02;
            double shifty = (H00 + H11) / 2 * H12;
            if ((abs(shiftx) > m_shiftTH  *image_frame.cols) || (abs(shifty) > m_shiftTH * image_frame.rows))
            {
                LOG_ERROR(CStringFormat("abs(shiftx = %lf) > threshold(%lf) or abs(shifty = %lf) > threshold(%lf)",\
                    abs(shiftx), m_shiftTH  *image_frame.cols, abs(shifty), m_shiftTH * image_frame.rows).Get(), NULL);
                GenerateReferenceFromFullFov(image_full_fov, H,
                    img_reference, image_frame.cols, image_frame.rows);
                return ANALYZER_RETRUN_FAIL;
            }
            else
            {
                LOG_INFO(CStringFormat("abs(shiftx = %lf) < threshold(%lf) and abs(shifty = %lf) < threshold(%lf))", \
                    abs(shiftx), m_shiftTH  *image_frame.cols, abs(shifty), m_shiftTH * image_frame.rows).Get(), NULL);
            }
        }
        else
        {
            LOG_ERROR(CStringFormat("Zoom ratio in x(%lf) and y(%lf) direction is not in same scale", H.at<double>(0, 0), H.at<double>(1, 1)).Get(), NULL);
            GenerateReferenceFromFullFov(image_full_fov, H,
                img_reference, image_frame.cols, image_frame.rows);
            return ANALYZER_RETRUN_FAIL;
        }
    }

    return ANALYZER_RETRUN_PASS;
}

void CalculateFOVLoss(Mat &H, int maxW, int maxH, int w, int h, double &hfovloss, double &vfovloss)
{
    double a00 = *(double*) H.ptr(0, 0);
    double b00 = *(double*) H.ptr(1, 1);

    int wnew = (int) (w / a00 + 0.5);
    int hnew = (int) (h / b00 + 0.5);

    double delta = (double) wnew / maxW - (double) hnew / maxH;
    //4:3 - > 16:9
    if (delta > 0.01)
    {
        int hnewmax = (int) ((double) hnew / wnew * maxW + 0.5);
        hfovloss = 1 - (double) hnew / hnewmax;
        vfovloss = 1 - (double) wnew / maxW;
    }
    else if (delta < -0.01)
    {
        int wnewmax = (int) ((double) wnew / hnew * maxH + 0.5);
        hfovloss = 1 - (double) hnew / maxH;
        vfovloss = 1 - (double) wnew / wnewmax;
    }
    else
    {
        hfovloss = 1 - (double) hnew / maxH;
        vfovloss = 1 - (double) wnew / maxW;
    }
}