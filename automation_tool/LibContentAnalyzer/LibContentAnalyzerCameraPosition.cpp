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

#include "LibContentAnalyzerCameraPosition.h"
#define PIE 3.1415926

bool checkCameraPositon(Point3f*, int, int, Mat);

CContenAnalyzerCameraPosition::CContenAnalyzerCameraPosition():
CContenAnalyzerBase(ANALYZER_CAMERA_POSITION_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerCameraPositionCompare");
}

CContenAnalyzerCameraPosition::~CContenAnalyzerCameraPosition()
{
}

ANALYZER_RETRUN CContenAnalyzerCameraPosition::
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

    //int rotation_angle_up = img_array[0].param.val.i == -1 ? 46 : (46 + img_array[0].param.val.i);
    //int rotation_angle_right = img_array[1].param.val.i == -1 ? 42 : (42 + img_array[1].param.val.i);
    int rotation_angle_up = 46;
    int rotation_angle_right = 42;

    vector<Point2f> vc = generateChessBoardCornerPoints();
    Mat Hf;
    vector<Point2f> corner;
    if (!findHomoFromGD2IMG(img_array[0].img, vc, Hf, corner))
    {
        LOG_ERROR("Please check if the test pattern is setup correctly with chessboard in it", NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    Point3f *normalized_position = getCorrespondingMarkedPoints(img_array[0].img, vc, Hf);
    if (NULL == normalized_position)
    {
        EXCEPTION("Failed to getCorrespondingMarkedPoints, please make sure the test pattern is correctly set-up.")
    }
    LOG_INFO(CStringFormat("Marked points red(%lf, %lf), blue(%lf, %lf)", normalized_position[0].x, normalized_position[0].y, normalized_position[1].x, normalized_position[1].y).Get(), NULL);
    img_array[0].img.copyTo(img_reference);
    circle(img_reference, Point(normalized_position[0].x, normalized_position[0].y), 2, Scalar(0, 255, 0), 2);
    circle(img_reference, Point(normalized_position[1].x, normalized_position[1].y), 2, Scalar(0, 255, 0), 2);
    line(img_reference, Point(normalized_position[0].x, normalized_position[0].y), Point(normalized_position[1].x, normalized_position[1].y), Scalar(0, 255, 0), 3, 8);
    

    if (NULL == normalized_position)
    {
        LOG_ERROR("can not locate the marked points!", NULL);
        return ANALYZER_RETRUN_FAIL;
    }
    else
    {
        bool bPass = checkCameraPositon(normalized_position, rotation_angle_up, rotation_angle_right, img_reference);

        delete normalized_position;
        
        if (bPass)
            return ANALYZER_RETRUN_PASS;
        else
            return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}

float dist_square(Point a, Point b)
{
    return (pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

float dist(Point a, Point b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

bool checkCameraPositon(Point3f* positions, int rotation_angle_up, int rotation_angle_right, Mat img_reference)
{
    Point red(positions[0].x, positions[0].y);
    Point blue(positions[1].x, positions[1].y);
    
    float current_angle = atan((positions[1].x - positions[0].x) / (positions[1].y - positions[0].y)) / (2 * PIE) * 360;    
    //a^2+b^2-2abCOSc=c^2

    Point up(positions[0].x, 0);
    
    float included_angle_up = acos((dist_square(up, blue) - dist_square(up, red) - dist_square(red, blue)) / (-2 * dist(up, red) * dist(blue, red))) / (2 * PIE) * 360;
    line(img_reference, up, red, Scalar(0, 0, 255), 3, 8);
    line(img_reference, up, blue, Scalar(0, 0, 255), 3, 8);

    Point right(img_reference.cols, positions[0].y);
    float included_angle_right = acos((dist_square(right, blue) - dist_square(right, red) - dist_square(red, blue)) / (-2 * dist(right, red) * dist(blue, red))) / (2 * PIE) * 360;
    line(img_reference, right, red, Scalar(255, 0, 0), 3, 8);
    line(img_reference, right, blue, Scalar(255, 0, 0), 3, 8);

    LOG_INFO(CStringFormat("rotate 2 up: %lf degree, rotate 2 right: %lf degree", included_angle_up, included_angle_right).Get(), NULL);

    if (abs(included_angle_up - rotation_angle_up) > 15 || abs(included_angle_right - rotation_angle_right) > 15)
    {
        // tolerance 15 is hard code 
        LOG_ERROR(CStringFormat("angle difference is too large. UP is %lf degree, RIGHT is %lf degress", abs(included_angle_up - rotation_angle_up), abs(included_angle_right - rotation_angle_right)).Get(), NULL);
        return false;
    }
    else
    {
        LOG_INFO(CStringFormat("angle difference is acceptable. UP is %lf degree, RIGHT is %lf degress", abs(included_angle_up - rotation_angle_up), abs(included_angle_right - rotation_angle_right)).Get(), NULL);
        return true;
    }
}