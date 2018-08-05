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

#include "LibContentAnalyzerThumbnail.h"

CContentAnalyzerThumbnail::CContentAnalyzerThumbnail() :
    CContenAnalyzerBase(ANALYZER_THUMBNAIL_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerThumbnailCompare");

    // Step 2: Initialize algorithm parameters
    threshold = 0.8;
    kernel_size = 5;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerThumbnail, threshold), "%lf", "parameter_threshold", 0, 1, 0.8 },
        { offsetof(CContentAnalyzerThumbnail, kernel_size), "%d", "kernel_size", 0, 50, 5 },
        { offsetof(CContentAnalyzerThumbnail, lower_bound_coefficient), "%lf", "lower_bound_coefficient", 0, 1.0, 0.3 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContentAnalyzerThumbnail::~CContentAnalyzerThumbnail()
{}


void CalculateThumbnailSize(
    int iSrcWidth,
    int iSrcHeight,
    int iRatio,  // 2, 4, 8 or 16.
    int* piThmbWidth,
    int* piThmbHeight
)
{
    if (max(iSrcWidth, iSrcHeight) < MINIMUM_THUMBNAIL_SIZE)
    {
        *piThmbWidth = iSrcWidth;
        *piThmbHeight = iSrcHeight;
    }
    else if (iSrcWidth > iSrcHeight)
    {
        *piThmbWidth = (iSrcWidth + iRatio - 1) / iRatio;
        *piThmbWidth = max(*piThmbWidth, MINIMUM_THUMBNAIL_SIZE);
        *piThmbHeight = (int)(((double) iSrcHeight / iSrcWidth) * (*piThmbWidth));
        *piThmbHeight += (*piThmbHeight % 2);
    }
    else
    {
        *piThmbHeight = (iSrcHeight + iRatio - 1) / iRatio;
        *piThmbHeight = max(*piThmbHeight, MINIMUM_THUMBNAIL_SIZE);
        *piThmbWidth = (int)(((double) iSrcWidth / iSrcHeight) * (*piThmbHeight));
        *piThmbWidth += (*piThmbWidth % 2);
    }
}

ANALYZER_RETRUN CContentAnalyzerThumbnail::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input parameter type
    int iZero = -1;
    for (size_t i = 0; i < img_array.size(); i++)
    {
        if (img_array[i].param.type != PARAM_DOUBLE)
        {
            LOG_ERROR("Wrong input parameter type", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

        if (img_array[i].param.val.d < 1 ||
            img_array[i].param.val.d > 16)
        {
            LOG_ERROR("Input parameter is out of range", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

        if (img_array[i].param.val.d == 1)
        {
            iZero = (int) i;
        }
    }

    if (iZero < 0)
    {
        LOG_ERROR("No scale 1 image in the inputs", NULL);
        return ANALYZER_PARAMETER_EXCEPTION;
    }

    // Step 3: Check Input Image
    Mat src1, src2;
    src1 = img_array[iZero].img;
    src2 = img_array[1 - iZero].img;

    if (src1.empty() || src2.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 4: Check the scale
    int iratio = int(img_array[1 - iZero].param.val.d + 0.5);
    int thumbw = 0;
    int thumbh = 0;

    CalculateThumbnailSize(src1.cols, src1.rows, iratio, &thumbw, &thumbh);

    if (src2.cols != thumbw || src2.rows != thumbh)
    {
        LOG_ERROR("The size of the Thumbnai image doesn't match the scale number", NULL);
        return ANALYZER_PARAMETER_EXCEPTION;
    }

    // Step 5: Analyzer
    resize(src1, img_reference, Size(thumbw, thumbh), 0, 0, CV_INTER_AREA);

    double ssim_avg;
    double ssim_min;
    V_SSIM_BLOCK artifact_blocks;

    CalculateSsimCn3(img_reference, src2, kernel_size, threshold, ssim_avg, ssim_min, artifact_blocks, 0, 0);

    if (ssim_avg < threshold)
    {
        LOG_ERROR(CStringFormat("Fail due to average SSIM value is smaller than the threshold(%lf)",
            threshold).Get(), CStringFormat("ssim_avg is %lf", ssim_avg).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    if (artifact_blocks.size() > 0 && ssim_min < threshold * lower_bound_coefficient)
    {
        LOG_ERROR(CStringFormat("Fail due to the mimimum SSIM value is smaller than threshold(%lf*%lf)",
            threshold, lower_bound_coefficient).Get(), CStringFormat("ssim_min is %lf", ssim_min).Get());

        src2.copyTo(img_debug);

        DrawArtifactBlocks(img_debug, artifact_blocks);

        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}

