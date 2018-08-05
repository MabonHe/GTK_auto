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

#include "LibContentAnalyzerSimilarity.h"
#include "../LibSimilarityCheck/LibSimilarityCheck.h"

CContentAnalyzerSimilarity::CContentAnalyzerSimilarity() :
CContenAnalyzerBase(ANALYZER_SIMILARITY_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerSimilarityCompare");

    // Step 2: Initialize algorithm parameters
    m_kerenel_size = 32;
    m_local_threshold = 0.3;
    m_global_threshold = 0.7;
    m_filter_w = 2;
    m_filter_h = 2;
    m_neighbor_count_th = 2;

    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerSimilarity, m_kerenel_size), "%d", "kerenel_size", 1, 128, 32 },
        { offsetof(CContentAnalyzerSimilarity, m_local_threshold), "%lf", "local_threshold", 0, 1, 0.3 },
        { offsetof(CContentAnalyzerSimilarity, m_global_threshold), "%lf", "global_threshold", 0, 1, 0.7 },
        { offsetof(CContentAnalyzerSimilarity, m_filter_w), "%d", "filter_width", 1, 100, 2 },
        { offsetof(CContentAnalyzerSimilarity, m_filter_h), "%d", "filter_height", 1, 100, 2 },
        { offsetof(CContentAnalyzerSimilarity, m_neighbor_count_th), "%d", "neighbor_count_th", 1, 100, 2 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerSimilarity::~CContentAnalyzerSimilarity()
{}

ANALYZER_RETRUN CContentAnalyzerSimilarity::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    double ssim_avg = 0;
    double ssim_min = 0;
    V_SSIM_BLOCK vblocks;

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
    if (image_full_fov.cols > image_frame.cols ||
        image_full_fov.rows > image_frame.rows)
    {
        Mat H;
        if (!CalculateHomographyMatrix(image_full_fov, image_frame, H))
        {
            LOG_ERROR("Failed to CalculateHomographyMatrix", NULL);
            return ANALYZER_OTHER_EXCEPTION;
        }

        if (!GenerateReferenceFromFullFov(image_full_fov, H,
            img_reference, image_frame.cols, image_frame.rows))
        {
            LOG_ERROR("Failed to GenerateReferenceFromFullFov", NULL);
            return ANALYZER_OTHER_EXCEPTION;
        }

        CalculateSsimCn3(img_reference, image_frame, m_kerenel_size,
            m_local_threshold, ssim_avg, ssim_min, vblocks, m_filter_w, m_filter_h, m_neighbor_count_th);
    }
    else if (image_full_fov.cols == image_frame.cols
        && image_full_fov.rows == image_frame.rows)
    {
        CalculateSsimCn3(image_full_fov, image_frame, m_kerenel_size,
            m_local_threshold, ssim_avg, ssim_min, vblocks, m_filter_w, m_filter_h, m_neighbor_count_th);
    }
    else
    {
        LOG_ERROR("The full FOV or reference image size is not correct", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (vblocks.size() > 0)
    {
        LOG_ERROR("Failed due to micro block diff",
            CStringFormat("Failed blocks number is %d, min ssim value is %lf.", vblocks.size(), ssim_min).Get());

        image_frame.copyTo(img_debug);

        DrawArtifactBlocks(img_debug, vblocks);

        return ANALYZER_RETRUN_FAIL;
    }

    if (ssim_avg < m_global_threshold)
    {
        LOG_ERROR(CStringFormat("Average SSIM is less than threshold %lf", m_global_threshold).Get(),
            CStringFormat("ssim_avg is %lf", ssim_avg).Get());
        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}

