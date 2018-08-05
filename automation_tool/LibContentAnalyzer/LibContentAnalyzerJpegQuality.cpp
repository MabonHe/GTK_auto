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

#include "LibContentAnalyzerJpegQuality.h"

typedef void(*compare_method)(Mat &src1, Mat &src2, int kernel_size, double threshold, double &avg, double &minval, V_SSIM_BLOCK &artifact_blocks, int filter_w, int filter_h, int neighbor_count_th, bool ignore_boundary);

typedef bool(*compare_threshold)(double val, double th);

bool compare_psnr_ssim(double val, double th);

typedef struct COMPARE_METHOD
{
    const char *method_name;
    const char *measure_unit;
    const char *description;
    compare_method method_func;
    compare_threshold compare;
} COMPARE_METHOD;

const COMPARE_METHOD COMPARE_METHODS[] = {
    { "PSNR", "DB", "PSNR = 10log(MaxErr^2 / MSE)", CalculatePsnrCn3, compare_psnr_ssim },
    { "SSIM", "", "Structural-Similarity-based Image quality Measurement", CalculateSsimCn3, compare_psnr_ssim },
    { NULL, NULL, NULL, NULL }
};


CContentAnalyzerJpegQuality::CContentAnalyzerJpegQuality() :
    CContenAnalyzerBase(ANALYZER_JPEGQUALITY_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerJpegQualityCompare");

    // Step 2: Initialize algorithm parameters
    method = NULL;
    psnr_threshold = 30;
    ssim_threshold = 0.8;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerJpegQuality, method), "%s", "method_name", 0, 0, 0 },
        { offsetof(CContentAnalyzerJpegQuality, psnr_threshold), "%lf", "psnr_threshold", 0, 500, 30 },
        { offsetof(CContentAnalyzerJpegQuality, ssim_threshold), "%lf", "ssim_threshold", 0, 1, 0.8},
        { offsetof(CContentAnalyzerJpegQuality, kernel_size), "%d", "kernel_size", 0, 50, 5 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerJpegQuality::~CContentAnalyzerJpegQuality()
{
    // Free those parameters which are "char*" type
    if (method != NULL)
    {
        free(method);
        method = NULL;
    }
}

ANALYZER_RETRUN CContentAnalyzerJpegQuality::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    double  threshold[2] = { psnr_threshold, ssim_threshold };
    bool    PASS = false;
    V_SSIM_BLOCK artifact_blocks_h, artifact_blocks_l;

    // Step 1: Check Input Image count
    if (img_array.size() != 3)
    {
        LOG_ERROR("Input image count is expected as 3", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    Mat img_nv12, img_hqua, img_lqua;
    img_nv12 = img_array[0].img;
    img_hqua = img_array[1].img;
    img_lqua = img_array[2].img;

    if (img_nv12.empty() || img_hqua.empty() || img_lqua.empty())
    {
        LOG_ERROR("Failed to load image", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Check Input Image Resolution
    if (img_nv12.cols != img_hqua.cols ||
        img_nv12.rows != img_hqua.rows)
    {
        LOG_ERROR("Image resolution cannot match each other", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (img_hqua.cols != img_lqua.cols ||
        img_hqua.rows != img_lqua.rows)
    {
        LOG_ERROR("Image resolution cannot match each other", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 4: Analyzer
    double min_val = 0;
    double avg_val_h = 0;
    double avg_val_l = 0;
    int imethod = 0;
    for (; imethod < 2; imethod++)
    {
        if (strcmp(method, COMPARE_METHODS[imethod].method_name) == 0)
        {
            break;
        }
    }
    if (imethod >= 2)
        imethod = 0;

    PASS = true;

    COMPARE_METHODS[imethod].method_func(img_nv12, img_hqua, kernel_size, threshold[imethod], avg_val_h, min_val, artifact_blocks_h, 0, 0, 0, false);
    COMPARE_METHODS[imethod].method_func(img_nv12, img_lqua, kernel_size, threshold[imethod], avg_val_l, min_val, artifact_blocks_l, 0, 0, 0, false);

    if (!COMPARE_METHODS[imethod].compare(avg_val_h, avg_val_l))
    {
        PASS = false;
        LOG_ERROR("The average value of high quality image is smaller than the low quaility one", NULL);
    }

    return PASS ? ANALYZER_RETRUN_PASS : ANALYZER_RETRUN_FAIL;
}

bool compare_psnr_ssim(double val, double th)
{
    if (val >= th)
    {
        return true;
    }

    return false;
}