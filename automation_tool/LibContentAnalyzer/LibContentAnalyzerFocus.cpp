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

#include "LibContentAnalyzerFocus.h"

CContentAnalyzerFocus::CContentAnalyzerFocus() :
    CContenAnalyzerBase(ANALYZER_FOCUS_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerFocusCompare");

    // Step 2: Initialize algorithm parameters
    kernel_size = 5;
    average_threshold = 0.8;
    local_threshold = 0.8;
    scale_threshold = 1.001;

    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerFocus, kernel_size), "%d", "kernel_size", 0, 50, 5 },
        { offsetof(CContentAnalyzerFocus, average_threshold), "%lf", "average_threshold", 0, 1, 0.8 },
        { offsetof(CContentAnalyzerFocus, local_threshold), "%lf", "local_threshold", 0, 1, 0.8 },
        { offsetof(CContentAnalyzerFocus, scale_threshold), "%lf", "scale_threshold", 1, 2, 1.001 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerFocus::~CContentAnalyzerFocus()
{}

ANALYZER_RETRUN CContentAnalyzerFocus::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    Mat image_focus_low, image_focus_high;

    image_focus_low = img_array[0].img;
    image_focus_high = img_array[1].img;

    if (image_focus_low.empty() || image_focus_high.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Check Input Image Resolution
    if (image_focus_low.cols != image_focus_high.cols ||
        image_focus_low.rows != image_focus_high.rows)
    {
        LOG_ERROR("Image resolution doesn't match", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 4: Analyzer
    Mat H;
    bool ret = CalculateHomographyMatrix(image_focus_low, image_focus_high, H);

    if (!ret)
    {
        LOG_ERROR("Failed to CalculateHomographyMatrix", NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    LOG_INFO(CStringFormat("x scale ratio: %lf, y scale ratio: %lf",
        *((double*) H.ptr(0, 0)), *((double*) H.ptr(1, 1))).Get(), NULL);

    if (*((double*) H.ptr(0, 0)) < scale_threshold || *((double*) H.ptr(1, 1)) < scale_threshold)
    {
        LOG_ERROR("x or y scale ratio is less than 1", NULL);

        return ANALYZER_RETRUN_FAIL;
    }


    Mat reference;
    ret = GenerateReferenceFromFullFov(image_focus_low, H, reference, image_focus_high.cols, image_focus_high.rows);
    if (!ret)
    {
        LOG_ERROR("Failed to GenerateReferenceFromFullFov", NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    double ssim_avg = 0;
    double ssim_min = 0;
    V_SSIM_BLOCK blocks;
    CalculateSsimCn3(reference, image_focus_high, kernel_size, local_threshold, ssim_avg, ssim_min, blocks, 2, 2, 2);

    LOG_INFO(CStringFormat("Similarity average: %lf", ssim_avg).Get(), NULL);

    if (ssim_avg < average_threshold)
    {
        DrawArtifactBlocks(image_focus_high, blocks);
        LOG_ERROR(CStringFormat("Similarity check failed, ssim_avg < average_threshold(%lf).",
            average_threshold).Get(), NULL);

        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}
