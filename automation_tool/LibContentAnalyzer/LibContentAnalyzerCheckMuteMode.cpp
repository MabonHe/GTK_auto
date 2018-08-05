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

#include "LibContentAnalyzerCheckMuteMode.h"

bool check_mute_scene(Mat &src, Mat &pattern);

CContentAnalyzerCheckMuteMode::CContentAnalyzerCheckMuteMode()
:CContenAnalyzerBase(ANALYZER_CHECKMUTEMODE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerCheckMuteMode");
}

CContentAnalyzerCheckMuteMode::~CContentAnalyzerCheckMuteMode()
{}

ANALYZER_RETRUN CContentAnalyzerCheckMuteMode::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    // Step 1: Check Input Image count
    if (img_array.size() != 2)
    {
        LOG_ERROR("Input image count is expected as 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 2: Check Input Image
    if (img_array[0].img.empty() || img_array[1].img.empty())
    {
        LOG_ERROR("Load image error", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    //Step 3: Analyzer
    if (check_mute_scene(img_array[0].img, img_array[1].img))
    {
        LOG_INFO("Muted", NULL);
        return ANALYZER_RETRUN_PASS;
    }
    else
    {
        LOG_INFO("Unmuted", NULL);
        return ANALYZER_RETRUN_FAIL;
    }
}

bool check_mute_scene(Mat &src, Mat &pattern)
{
    if (src.cols < pattern.cols + 32 || src.rows < pattern.rows + 32)
    {
        return false;
    }

    int patternW = pattern.cols;
    int patternH = pattern.rows;
    int PatternX = (src.cols - patternW) / 2;
    int PatternY = (src.rows - patternH) / 2;

    uchar *data = NULL, *pattern_data = NULL;

    int x, y;
    for (y = 0; y < PatternY - 16; y++)
    {
        data = src.data + y * src.step;

        for (x = 0; x < src.cols * 3; x++)
        {
            if (data[x] < MIN_GRAY_LOW || data[x] > MAX_GRAY_LOW)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], MIN_GRAY_LOW, MAX_GRAY_LOW);
                return false;
            }
        }
    }

    for (y = PatternY - 16; y < PatternY; y++)
    {
        data = src.data + y * src.step;

        for (x = 0; x < src.cols * 3; x++)
        {
            if (data[x] < MIN_GRAY || data[x] > MAX_GRAY)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], MIN_GRAY, MAX_GRAY);
                return false;
            }
        }
    }

    for (y = PatternY; y < PatternY + patternH; y++)
    {
        data = src.data + y * src.step;

        for (x = 0; x < PatternX * 3; x++)
        {
            if (data[x] < MIN_GRAY || data[x] > MAX_GRAY)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], MIN_GRAY, MAX_GRAY);
                return false;
            }
        }

        pattern_data = pattern.data + pattern.step * (y - PatternY);
        for (x = PatternX * 3; x < (PatternX + patternW) * 3; x++)
        {
            if (abs(pattern_data[x - PatternX * 3] - data[x]) > 50)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], pattern_data[x - PatternX * 3] - 50, pattern_data[x - PatternX * 3] + 50);
                return false;
            }
        }

        for (x = (PatternX + patternW) * 3; x < src.cols * 3; x++)
        {
            if (data[x] < MIN_GRAY || data[x] > MAX_GRAY)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], MIN_GRAY, MAX_GRAY);
                return false;
            }
        }
    }

    for (y = PatternY + patternH; y < PatternY + patternH + 16; y++)
    {
        data = src.data + y * src.step;

        for (x = 0; x < src.cols * 3; x++)
        {
            if (data[x] < MIN_GRAY_LOW || data[x] > MAX_GRAY_LOW)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], MIN_GRAY_LOW, MAX_GRAY_LOW);
                return false;
            }
        }
    }

    for (y = PatternY + patternH + 16; y < src.rows; y++)
    {
        data = src.data + y * src.step;

        for (x = 0; x < src.cols * 3; x++)
        {
            if (data[x] < MIN_GRAY || data[x] > MAX_GRAY)
            {
                //printf("(%d, %d), %d, (%d:%d)\n", x, y, data[x], MIN_GRAY, MAX_GRAY);
                return false;
            }
        }
    }

    return true;
}

