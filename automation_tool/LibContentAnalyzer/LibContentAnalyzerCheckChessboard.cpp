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

#include "LibContentAnalyzerCheckChessboard.h"
bool findPattern(Mat &frame_cn3, Mat &debug, int max_count, double epsilon);


CContenAnalyzerChessboard::CContenAnalyzerChessboard() :
CContenAnalyzerBase(ANALYZER_CHESSBOARD_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerChessboardCompare");

    // Step 2: Initialize algorithm parameters
    m_max_count = 30;
    m_epsilon = 0.01;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerChessboard, m_max_count), "%d", "max_count", 0, 100, 30 },
        { offsetof(CContenAnalyzerChessboard, m_epsilon), "%lf", "epsilon", 0, 1, 0.01 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerChessboard::~CContenAnalyzerChessboard()
{
}

ANALYZER_RETRUN CContenAnalyzerChessboard::
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

    // Step 3: Check pattern in view
    if (findPattern(img_array[0].img, img_debug, m_max_count, m_epsilon))
    {
        LOG_INFO("Find Chessboard Pattern in View", NULL);
        return ANALYZER_RETRUN_PASS;
    }
    else
    {
        LOG_ERROR("Can not find Chessboard Pattern in View", NULL);
        return ANALYZER_RETRUN_FAIL;
    }
}

bool findPattern(
    Mat &frame_cn3,
    Mat &debug,
    int max_count,
    double epsilon)
{
    Mat frame;
    cvtColor(frame_cn3, frame, CV_BGR2GRAY);

    if (frame.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        return false;
    }

    const int ROW_CORNER = 6;
    const int COL_CORNER = 7;
    const Size PATTERN_SIZE(COL_CORNER, ROW_CORNER);

    int ncorner_count_full = 0;
    int ncorner_count_frame = 0;
    vector<Point2f> corners_full;
    vector<Point2f> corners_frame;

    if (!findChessboardCorners(frame, PATTERN_SIZE, corners_frame))
    {
        LOG_ERROR("Corner points do not equal to pattern size", NULL);
        return false;
    }

    cornerSubPix(frame, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, max_count, epsilon));

    //----------------------reference output, to see chessboard result, enable this part---------------//
    frame_cn3.copyTo(debug);
    drawChessboardCorners(debug, PATTERN_SIZE, Mat(corners_frame), true);
    return true;
}