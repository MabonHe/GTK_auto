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

#include "LibContentAnalyzerGreenCorruption.h"

CContenAnalyzerGreenCorruption::CContenAnalyzerGreenCorruption() :
CContenAnalyzerBase(ANALYZER_GREEN_CORRUPTION)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerGreenCorruption");

    // Step 2: Initialize algorithm parameters
    m_green_b_max = 1;
    m_green_g_min = 50;
    m_green_r_max = 1;
    m_grenn_line_ratio_th = 0.2f;
    m_line_pixel_var_th = 0.1;
    m_line_pixel_average_lo = 10.0f;
    m_line_pixel_average_hi = 245.0f;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
            { offsetof(CContenAnalyzerGreenCorruption, m_green_b_max), "%d", "green_b_max", 0, 255, 1 },
            { offsetof(CContenAnalyzerGreenCorruption, m_green_g_min), "%d", "green_g_min", 0, 255, 50 },
            { offsetof(CContenAnalyzerGreenCorruption, m_green_r_max), "%d", "green_r_max", 0, 255, 1 },
            { offsetof(CContenAnalyzerGreenCorruption, m_grenn_line_ratio_th), "%f", "grenn_line_ratio_th", 0, 1, 0.2 },
            { offsetof(CContenAnalyzerGreenCorruption, m_line_pixel_var_th), "%f", "line_pixel_var_th", 0, 255, 0.1 },
            { offsetof(CContenAnalyzerGreenCorruption, m_line_pixel_average_lo), "%f", "line_pixel_average_th_low", 0, 255.0, 10.0 },
            { offsetof(CContenAnalyzerGreenCorruption, m_line_pixel_average_hi), "%f", "line_pixel_average_th_high", 0, 255.0, 245.0 },
            { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerGreenCorruption::~CContenAnalyzerGreenCorruption()
{
}

bool CContenAnalyzerGreenCorruption::CheckGreenCorruption(Mat &img)
{
    int w = img.cols;
    int h = img.rows;

    double var_th = (double) (m_line_pixel_var_th * m_line_pixel_var_th);

    uchar *pdata = NULL;
    uchar b = 0;
    uchar g = 0;
    uchar r = 0;

    unsigned long pix_avg = 0;
    unsigned long pix_avg2 = 0;

    int row_acc = 0;
    vector<int> col_acc_array(w, 0);

    unsigned long row_avg = 0;
    unsigned long row_square_avg = 0;

    vector<unsigned long> col_avg_array(w, 0);
    vector<unsigned long> col_square_avg_array(w, 0);

    for (int y = 0; y < h; y++)
    {
        pdata = img.data + y * img.step;
        row_acc = 0;
        row_avg = 0;
        row_square_avg = 0;

        for (int x = 0, x3 = 0; x < w; x++, x3 += 3)
        {
            b = pdata[x3];
            g = pdata[x3 + 1];
            r = pdata[x3 + 2];
            pix_avg = (r + g + b) / 3;
            pix_avg2 = pix_avg * pix_avg;

            if (b < m_green_b_max && 
                g > m_green_g_min && 
                r < m_green_r_max)
            {
                row_acc++;
                col_acc_array[x]++;
            }

            row_avg += pix_avg;
            row_square_avg += pix_avg2;
            col_avg_array[x] += pix_avg;
            col_square_avg_array[x] += pix_avg2;
        }

        double avg = ((double) row_avg) / w;

        // Exclude under exposure and over exposure situation 
        if (avg > m_line_pixel_average_lo && avg < m_line_pixel_average_hi)
        {
            double square_avg = ((double) row_square_avg) / w;

            // If row variance is too low, it would be artifacts.
            if (square_avg - avg * avg < var_th)
            {
                LOG_ERROR("Image corruption is found as the line variance is too little", 
                    CStringFormat("in Row %d", y).Get());
                return false;
            }
        }

        // If too many critical green pixels in this row, it would be artifacts
        if (row_acc > w * m_grenn_line_ratio_th)
        {
            LOG_ERROR("Green corruption is found", CStringFormat("in Row %d", y).Get());
            return false;
        }
    }

    for (int x = 0; x < w; x++)
    {
        double avg = ((double) col_avg_array[x]) / w;

        // Exclude under exposure and over exposure situation 
        if (avg > m_line_pixel_average_lo && avg < m_line_pixel_average_hi)
        {
            double square_avg = ((double) col_square_avg_array[x]) / w;

            // If column variance is too low, it would be artifacts.
            if (square_avg - avg * avg < var_th)
            {
                LOG_ERROR("Image corruption is found as the line variance is too little",
                    CStringFormat("in Col %d", x).Get());
                return false;
            }
        }
        
        // If too many critical green pixels in this column, it would be artifacts
        if (col_acc_array[x] > h * m_grenn_line_ratio_th)
        {
            LOG_ERROR("Green Corruption is found", CStringFormat("in Col %d", x).Get());
            return false;
        }
    }

    return true;
}

ANALYZER_RETRUN CContenAnalyzerGreenCorruption::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() != 1)
    {
        LOG_ERROR("Input image count is expected as 1", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    if (img_array[0].img.empty())
    {
        LOG_ERROR("Input image is empty", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (!CheckGreenCorruption(img_array[0].img))
    {
        return ANALYZER_RETRUN_FAIL;
    }
    
    return ANALYZER_RETRUN_PASS;
}
