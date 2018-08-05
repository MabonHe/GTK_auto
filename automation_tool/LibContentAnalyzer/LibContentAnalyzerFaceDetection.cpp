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

#include "LibContentAnalyzerFaceDetection.h"

#define Q31_BASE 2147483648

CContentAnalyzerFaceDetection::CContentAnalyzerFaceDetection() :
CContenAnalyzerBase(ANALYZER_FACE_DETECTION)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerFaceDetection");

    // Step 2: Initialize algorithm parameters
    m_face_count = 0;

    DATA_PARSER params[] = {
            { offsetof(CContentAnalyzerFaceDetection, m_face_count), "%d", "face_count", 0, 16, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[0].Region.left),  "%d", "face0_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[0].Region.top),   "%d", "face0_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[0].Region.right), "%d", "face0_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[0].Region.bottom),"%d", "face0_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[1].Region.left),  "%d", "face1_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[1].Region.top),   "%d", "face1_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[1].Region.right), "%d", "face1_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[1].Region.bottom),"%d", "face1_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[2].Region.left),  "%d", "face2_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[2].Region.top),   "%d", "face2_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[2].Region.right), "%d", "face2_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[2].Region.bottom),"%d", "face2_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[3].Region.left),  "%d", "face3_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[3].Region.top),   "%d", "face3_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[3].Region.right), "%d", "face3_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[3].Region.bottom),"%d", "face3_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[4].Region.left),  "%d", "face4_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[4].Region.top),   "%d", "face4_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[4].Region.right), "%d", "face4_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[4].Region.bottom),"%d", "face4_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[5].Region.left),  "%d", "face5_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[5].Region.top),   "%d", "face5_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[5].Region.right), "%d", "face5_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[5].Region.bottom),"%d", "face5_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[6].Region.left),  "%d", "face6_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[6].Region.top),   "%d", "face6_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[6].Region.right), "%d", "face6_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[6].Region.bottom),"%d", "face6_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[7].Region.left),  "%d", "face7_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[7].Region.top),   "%d", "face7_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[7].Region.right), "%d", "face7_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[7].Region.bottom),"%d", "face7_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[8].Region.left), "%d", "face8_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[8].Region.top), "%d", "face8_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[8].Region.right), "%d", "face8_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[8].Region.bottom), "%d", "face8_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[9].Region.left), "%d", "face9_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[9].Region.top), "%d", "face9_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[9].Region.right), "%d", "face9_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[9].Region.bottom), "%d", "face9_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[10].Region.left), "%d", "face10_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[10].Region.top), "%d", "face10_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[10].Region.right), "%d", "face10_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[10].Region.bottom), "%d", "face10_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[11].Region.left), "%d", "face11_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[11].Region.top), "%d", "face11_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[11].Region.right), "%d", "face11_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[11].Region.bottom), "%d", "face11_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[12].Region.left), "%d", "face12_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[12].Region.top), "%d", "face12_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[12].Region.right), "%d", "face12_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[12].Region.bottom), "%d", "face12_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[13].Region.left), "%d", "face13_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[13].Region.top), "%d", "face13_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[13].Region.right), "%d", "face13_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[13].Region.bottom), "%d", "face13_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[14].Region.left), "%d", "face14_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[14].Region.top), "%d", "face14_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[14].Region.right), "%d", "face14_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[14].Region.bottom), "%d", "face14_bottom", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[15].Region.left), "%d", "face15_left", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[15].Region.top), "%d", "face15_top", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[15].Region.right), "%d", "face15_right", -255, 255, 0 },
            { offsetof(CContentAnalyzerFaceDetection, m_faces[15].Region.bottom), "%d", "face15_bottom", -255, 255, 0 },
            { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerFaceDetection::~CContentAnalyzerFaceDetection()
{}

Point2f Transform(Mat &H, Point2f pt)
{
    double H00 = *((double*) H.ptr(0, 0));
    double H01 = *((double*) H.ptr(0, 1));
    double H02 = *((double*) H.ptr(0, 2));
    double H10 = *((double*) H.ptr(1, 0));
    double H11 = *((double*) H.ptr(1, 1));
    double H12 = *((double*) H.ptr(1, 2));
    double H20 = *((double*) H.ptr(2, 0));
    double H21 = *((double*) H.ptr(2, 1));
    double H22 = *((double*) H.ptr(2, 2));

    Point2f pt_ret;
    pt_ret.x = (float)(H00 * pt.x + H01 * pt.y + H02 * 1);
    pt_ret.y = (float)(H10 * pt.x + H11 * pt.y + H12 * 1);

    return pt_ret;
}

float CheckOverlapRatio(Point2f pt0lt, Point2f pt0rb, Point2f pt1lt, Point2f pt1rb)
{
    Point2f pta, ptb;

    pta.x = MAX(pt0lt.x, pt1lt.x);
    pta.y = MAX(pt0lt.y, pt1lt.y);
    ptb.x = MIN(pt0rb.x, pt1rb.x);
    ptb.y = MIN(pt0rb.y, pt1rb.y);

    if (ptb.x <= pta.x || ptb.y <= pta.y)
    {
        return 0;
    }

    return (ptb.y - pta.y) * (ptb.x - pta.x) / (pt0rb.y - pt0lt.y) / (pt0rb.x - pt0lt.x);
}

ANALYZER_RETRUN CContentAnalyzerFaceDetection::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
    if (img_array.size() != 1)
    {
        LOG_ERROR("Input image array size should be 1", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (!img_array[0].metadata.bvalid)
    {
        LOG_ERROR("Meta data is invalid", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    Mat grey_img;
    cvtColor(img_array[0].img, grey_img, CV_BGR2GRAY);

    if (img_array[0].img.empty() || grey_img.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Fixed size according to the chessboard pattern
    const int ROW_CORNER = 7;
    const int COL_CORNER = 6;
    const Size PATTERN_SIZE(COL_CORNER, ROW_CORNER);

    int ncorner_count_frame = 0;
    vector<Point2f> corners_frame;
    vector<Point2f> corners_pattern;

    if (!findChessboardCorners(grey_img, PATTERN_SIZE, corners_frame))
    {
        LOG_ERROR("Corner points do not equal to pattern size", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    cornerSubPix(grey_img, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.01));

    for (int y = ROW_CORNER - 1; y >= 0; y--)
    {
        for (int x = COL_CORNER - 1; x >= 0; x--)
        {
            corners_pattern.push_back(Point2f((float)x, (float)y));
        }
    }

    Mat H = findHomography(Mat(corners_pattern), Mat(corners_frame), 0, 0);

    if (H.empty())
    {
        LOG_ERROR("Failed to generate homography matrix", NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    float imgw = grey_img.cols;
    float imgh = grey_img.rows;

    FaceRectInfoBlobHeader *face_head = &img_array[0].metadata.face_roi_buffer.Header;
    FaceRectInfo *pFace = img_array[0].metadata.face_roi_buffer.FaceInfo;
    int ndetected_faces = 0;

    img_array[0].img.copyTo(img_debug);

    // transform the face positions from pattern to the captured frame
    for (int i = 0; i < m_face_count; i++)
    {
        LONG l = m_faces[i].Region.left;
        LONG t = m_faces[i].Region.top;
        LONG r = m_faces[i].Region.right;
        LONG b = m_faces[i].Region.bottom;

        Point2f pt1 = Transform(H, Point2f((float) (l), (float) (t)));
        Point2f pt2 = Transform(H, Point2f((float) (r), (float) (b)));

        LOG_INFO(CStringFormat("Face reference: (%f, %f) - (%f, %f)", pt1.x, pt1.y, pt2.x, pt2.y).Get(), NULL);

        // Check if it is out of the image FOV range
        if (pt1.x < 0 || pt1.x > imgw || pt1.y < 0 || pt1.y > imgh ||
            pt2.x < 0 || pt2.x > imgw || pt2.y < 0 || pt2.y > imgh)
        {
            LOG_WARNING(CStringFormat("The face (%ld, %ld, %ld, %ld) is out of the Camera FOV",
                l, t, r, b).Get(), NULL);
            continue;
        }

        rectangle(img_debug, pt1, pt2, Scalar(255, 0, 0, 0));

        for (int j = 0; j < face_head->Count && j < MAX_FACE_COUNT; j++)
        {
            l = (int) (double(pFace[j].Region.left) / Q31_BASE * imgw);
            t = (int) (double(pFace[j].Region.top) / Q31_BASE * imgh);
            r = (int) (double(pFace[j].Region.right) / Q31_BASE * imgw);
            b = (int) (double(pFace[j].Region.bottom) / Q31_BASE * imgh);

            float overlap = CheckOverlapRatio(Point2f((float) l, (float) t),
                Point2f((float) r, (float) b), pt1, pt2);

            if (overlap > 0.5)
            {
                ndetected_faces++;
            }
        }
    }

    for (int j = 0; j < face_head->Count && j < MAX_FACE_COUNT; j++)
    {
        LONG l = (int) (double(pFace[j].Region.left) / Q31_BASE * imgw);
        LONG t = (int) (double(pFace[j].Region.top) / Q31_BASE * imgh);
        LONG r = (int) (double(pFace[j].Region.right) / Q31_BASE * imgw);
        LONG b = (int) (double(pFace[j].Region.bottom) / Q31_BASE * imgh);

        rectangle(img_debug, Point(l, t), Point(r, b), Scalar(0, 0, 255, 0));
    }

    if (ndetected_faces <= 0)
    {
        return ANALYZER_RETRUN_FAIL;
    }

    return ANALYZER_RETRUN_PASS;
}

