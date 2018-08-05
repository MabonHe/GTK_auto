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

#include "LibContentAnalyzer.h"
#include "LibContentAnalyzerBrightness.h"
#include "LibContentAnalyzerCheckMuteMode.h"
#include "LibContentAnalyzerContrast.h"
#include "LibContentAnalyzerExposure.h"
#include "LibContentAnalyzerHue.h"
#include "LibContentAnalyzerTorch.h"
#include "LibContentAnalyzerWhiteBalance.h"
#include "LibContentAnalyzerZoom.h"
#include "LibContentAnalyzerThumbnail.h"
#include "LibContentAnalyzerJpegQuality.h"
#include "LibContentAnalyzerFocus.h"
#include "LibContentAnalyzerColorEffect.h"
#include "LibContentAnalyzerHDR.h"
#include "LibContentAnalyzerFOV.h"
#include "LibContentAnalyzerSimilarity.h"
#include "LibContentAnalyzerGDC.h"
#include "LibContentAnalyzer3AAutoConsistent.h"
#include "LibContentAnalyzerVideoBasic.h"
#include "LibContentAnalyzerULL.h"
#include "LibContentAnalyzerAutoAWB.h"
#include "LibContentAnalyzerAE.h"
#include "LibContentAnalyzerAF.h"
#include "LibContentAnalyzerFaceDetection.h"
#include "LibContentAnalyzerGreenCorruption.h"
#include "LibContentAnalyzerATE.h"
#include "LibContentAnalyzerCheckChessboard.h"
#include "LibContentAnalyzerCameraPosition.h"
#include "LibContentAnalyzerColorCast.h"

LIBCONTENTANALYZER_API CContenAnalyzerInterface* FactoryCreateAnalyzer(AnalyzerType type)
{
    switch (type)
    {
    case ANALYZER_BRIGHTNESS_COMPARE:
        return new CContenAnalyzerBrightness;
    case ANALYZER_CHECKMUTEMODE:
        return new CContentAnalyzerCheckMuteMode;
    case ANALYZER_CONTRAST_COMPARE:
        return new CContentAnalyzerContrast;
    case ANALYZER_EXPOSURE_COMPARE:
        return new CContentAnalyzerExposure;
    case ANALYZER_HUE_COMPARE:
        return new CContentAnalyzerHue;
    case ANALYZER_TORCH_COMPARE:
        return new CContentAnalyzerTorch;
    case ANALYZER_WHITEBALANCE_COMPARE:
        return new CContentAnalyzerWhiteBalance;
    case ANALYZER_ZOOM_COMPARE:
        return new CContentAnalyzerZoom;
    case ANALYZER_THUMBNAIL_COMPARE:
        return new CContentAnalyzerThumbnail;
    case ANALYZER_JPEGQUALITY_COMPARE:
        return new CContentAnalyzerJpegQuality;
    case ANALYZER_FOCUS_COMPARE:
        return new CContentAnalyzerFocus;
    case ANALYZER_COLOREFFECT_NEGATIVE:
        return new CContenAnalyzerNegative;
    case ANALYZER_COLOREFFECT_GRASSGREEN:
        return new CContenAnalyzerGrassGreen;
    case ANALYZER_COLOREFFECT_MONO:
        return new CContenAnalyzerMono;
    case ANALYZER_COLOREFFECT_SEPIA:
        return new CContenAnalyzerSepia;
    case ANALYZER_COLOREFFECT_SKYBLUE:
        return new CContenAnalyzerSkyBlue;
    case ANALYZER_COLOREFFECT_SKINWHITEN:
        return new CContenAnalyzerSkinWhiten;
    case ANALYZER_HDR_COMPARE:
        return new CContenAnalyzerHDR;
    case ANALYZER_FOV_COMPARE:
        return new CContentAnalyzerFOV;
    case ANALYZER_SIMILARITY_COMPARE:
        return new CContentAnalyzerSimilarity;
    case ANALYZER_GDC_COMPARE:
        return new CContentAnalyzerGDC;
    case ANALYZER_3A_AUTO_CONSISTENT:
        return new CContenAnalyzer3AAutoConsistent;
    case ANALYZER_VIDEO_BASIC:
        return new CContenAnalyzerVideoBasic;
    case ANALYZER_ULL_COMPARE:
        return new CContenAnalyzerULL;
    case ANALYZER_AUTOAWB_COMPARE:
        return new CContenAnalyzerAutoAWB;
    case ANALYZER_AUTOAE_COMPARE:
        return new CContenAnalyzerAutoAE;
    case ANALYZER_AUTOAF_COMPARE:
        return new CContenAnalyzerAutoAF;
    case ANALYZER_FACE_DETECTION:
        return new CContentAnalyzerFaceDetection;
    case ANALYZER_METADATA:
        return NULL;
    case ANALYZER_GREEN_CORRUPTION:
        return new CContenAnalyzerGreenCorruption;
    case ANALYZER_ATE_COMPARE:
        return new CContentAnalyzerATE;
    case ANALYZER_CHESSBOARD_COMPARE:
        return new CContenAnalyzerChessboard;
    case ANALYZER_CAMERA_POSITION_COMPARE:
        return new CContenAnalyzerCameraPosition;
    case ANALYZER_COLORCAST_COMPARE:
        return new CContenAnalyzerColorcast;
    default:
        break;
    }

    return NULL;
}

LIBCONTENTANALYZER_API void FactoryDestroyAnalyzer(CContenAnalyzerInterface **ppAnalyzer)
{
    if (*ppAnalyzer != NULL)
    {
        delete (*ppAnalyzer);
        *ppAnalyzer = NULL;
    }
}

vector<Point2f> generateChessBoardCornerPoints(int row, int col)
{
    int ROW = row;
    int COL = col;
    const Size PATTERN_SIZE(COL, ROW);
    vector<Point2f> virtualCorners(ROW * COL, Point2f(0.0, 0.0));
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            virtualCorners[i * COL + j].x = j * 1;
            virtualCorners[i * COL + j].y = i * 1;
        }
    }
    return virtualCorners;
}

bool findHomoFromGD2IMG(Mat &frame_cn3, vector<Point2f> virtualCorners, Mat& H, vector<Point2f>& corners_frame)
{
    Mat frame;
    cvtColor(frame_cn3, frame, CV_BGR2GRAY);

    if (frame.empty())
    {
        LOG_ERROR("Input frame is empty.\n", NULL);
        return false;
    }

    const int ROW_CORNER = 6;
    const int COL_CORNER = 7;
    const Size PATTERN_SIZE(COL_CORNER, ROW_CORNER);

    int ncorner_count_full = 0;
    int ncorner_count_frame = 0;

    if (!findChessboardCorners(frame, PATTERN_SIZE, corners_frame))
    {
       LOG_ERROR("Can not find chessborad corners", NULL);
       return false;
    }

    cornerSubPix(frame, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(2 | 1, 20, 0.01));

    Mat a;
    frame.copyTo(a);

#ifdef DEBUG_OUTPUT
    drawChessboardCorners(a, Size(7, 6), corners_frame, 1);
    imwrite("a.jpg", a);
#endif

    double ransacReprojThreshold = 5;

    H = findHomography(Mat(virtualCorners), Mat(corners_frame), CV_RANSAC, ransacReprojThreshold);

    if (H.empty())
    {
       LOG_ERROR("Homograph matrix is invalid!\n", NULL);
       return false;
    }

    return true;
}

Point3f getCorrespondingPoint(Mat frame, vector<Point2f> corners, Point3f inP, Mat Hf)
{
    // locate the origin, x axis and y axis
    Point3f Ppa[4][4] = { { Point3f(-1.5, -1.5, 1.0), Point3f(1.5, -1.5, 1.0), Point3f(-1.5, 1.5, 1.0), Point3f(1.5, 1.5, 1.0) }, \
    { Point3f(4.5, -1.5, 1.0), Point3f(7.5, -1.5, 1.0), Point3f(4.5, 1.5, 1.0), Point3f(7.5, 1.5, 1.0) }, \
    { Point3f(-1.5, 3.5, 1.0), Point3f(1.5, 3.5, 1.0), Point3f(-1.5, 6.5, 1.0), Point3f(1.5, 6.5, 1.0) }, \
    { Point3f(4.5, 3.5, 1.0), Point3f(7.5, 3.5, 1.0), Point3f(4.5, 6.5, 1.0), Point3f(7.5, 6.5, 1.0) } };
    vector<Mat> bgr;
    split(frame, bgr);
    Mat HSV;
    cvtColor(frame, HSV, CV_BGR2HSV);
    vector<Mat> hsv;
    split(HSV, hsv);

#ifdef DEBUG_OUTPUT
    imshow("b", bgr[0]);
    waitKey(0);
    imshow("r", bgr[2]);
    waitKey(0);
    imshow("s", hsv[1]);
    waitKey(0);
#endif

    // mark the topleft as (0, 0, 255) & bottomright as (255, 0, 0)
    //cal the corresponding points in image plane
    double dH00 = Hf.at<double>(0, 0);
    double dH01 = Hf.at<double>(0, 1);
    double dH02 = Hf.at<double>(0, 2);
    double dH10 = Hf.at<double>(1, 0);
    double dH11 = Hf.at<double>(1, 1);
    double dH12 = Hf.at<double>(1, 2);
    double dH20 = Hf.at<double>(2, 0);
    double dH21 = Hf.at<double>(2, 1);
    double dH22 = Hf.at<double>(2, 2);

#ifdef DEBUG_OUTPUT
    cout << dH00 << " " << dH01 << " " << dH02 << endl;
    cout << dH10 << " " << dH11 << " " << dH12 << endl;
    cout << dH20 << " " << dH21 << " " << dH22 << endl;
#endif

    Point3f dst(0.0, 0.0, 0.0);
    Point3f position[2] = { Point3f(0.0, 0.0, 1.0), Point3f(0.0, 0.0, 1.0) };
    double maxSumr = -1.0, maxSumb = -1.0;
    bool bfindBlue = false, bfindRed = false;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Point3f src = Ppa[i][j];
            dst.x = dH00 * src.x + dH01 * src.y + dH02 * src.z;
            dst.y = dH10 * src.x + dH11 * src.y + dH12 * src.z;
            dst.z = dH20 * src.x + dH21 * src.y + dH22 * src.z;
            // Normalizaion
            dst.x /= dst.z;
            dst.y /= dst.z;
            dst.z /= dst.z;

            double sumr = 0;
            double sumg = 0;
            double sumb = 0;
            double sums = 0;
            for (int p = -15; p < 15; p++)
            {
                if ((int(dst.y + 0.5) + p < 0) || (int(dst.y + 0.5) + p) >= bgr[0].rows)
                    return Point3f(0, 0, -1);
                uchar * b = bgr[0].data + (int(dst.y + 0.5) + p) *bgr[0].step;
                uchar * g = bgr[1].data + (int(dst.y + 0.5) + p) *bgr[1].step;
                uchar * r = bgr[2].data + (int(dst.y + 0.5) + p) *bgr[2].step;
                uchar * s = hsv[1].data + (int(dst.y + 0.5) + p) *hsv[1].step;
                for (int q = -5; q < 5; q++)
                {
                    if ((int(dst.x + 0.5) + q < 0) || (int(dst.x + 0.5) + q) >= bgr[0].cols)
                        return Point3f(0, 0, -1);
                    sumb += b[int(dst.x + 0.5) + q];
                    sumg += g[int(dst.x + 0.5) + q];
                    sumr += r[int(dst.x + 0.5) + q];
                    sums += s[int(dst.x + 0.5) + q];
                }
            }

            sumb /= double(30 * 30);
            sumg /= double(30 * 30);
            sumr /= double(30 * 30);
            sums /= double(30 * 30);

#ifdef DEBUG_OUTPUT
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "sumb" << sumb << endl;
            cout << "sumg" << sumg << endl;
            cout << "sumr" << sumr << endl;
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            Mat cpy;
            frame.copyTo(cpy);
            circle(cpy, Point(dst.x, dst.y), 2, Scalar(0, 0, 0), 2);
            Mat cpy_r;
            resize(cpy, cpy_r, Size(cpy.cols / 2, cpy.rows / 2));
            imshow("cur", cpy_r);
            waitKey(0);
#endif

            if (((sumr - sumg)/2 + (sumr - sumb)/2) > maxSumr)
            {
                position[0].x = Ppa[i][0].x + 1.5;
                position[0].y = Ppa[i][0].y + 1.5;
                maxSumr = (sumr - sumg) / 2 + (sumr - sumb) / 2;
                bfindRed = true;
            }

            if (((sumb - sumg) / 2 + (sumb - sumr) / 2) > maxSumb)
            {
                position[1].x = Ppa[i][0].x + 1.5;
                position[1].y = Ppa[i][0].y + 1.5;
                maxSumb = (sumb - sumg) / 2 + (sumb - sumr) / 2;
                bfindBlue = true;
            }
        }
    }

    if (!bfindBlue || !bfindRed )
     {
         LOG_ERROR("Can not Find red/blue corner flag", NULL);
         return Point3f(0, 0, -1);
     }

    /*for (int i = 0; i < 2; i++)
    {
        Point3f src = position[i];
        dst.x = dH00 * src.x + dH01 * src.y + dH02 * src.z;
        dst.y = dH10 * src.x + dH11 * src.y + dH12 * src.z;
        dst.z = dH20 * src.x + dH21 * src.y + dH22 * src.z;
        // Normalizaion
        dst.x /= dst.z;
        dst.y /= dst.z;
        dst.z /= dst.z;
    }
    */

    bool bxPos = false, byPos = false;
    if (position[1].x - position[0].x > 0)
        bxPos = true;
    if (position[1].y - position[0].y > 0)
        byPos = true;

    inP.x = position[0].x + (bxPos ? inP.x : -1.0 * inP.x);
    inP.y = position[0].y + (byPos ? inP.y : -1.0 * inP.y);
    Point3f outP;
    outP.x = dH00 * inP.x + dH01 * inP.y + dH02 * inP.z;
    outP.y = dH10 * inP.x + dH11 * inP.y + dH12 * inP.z;
    outP.z = dH20 * inP.x + dH21 * inP.y + dH22 * inP.z;
    // Normalizaion
    outP.x /= outP.z;
    outP.y /= outP.z;
    outP.z /= outP.z;
    return outP;
}

// to return a vector contains red and blue points

Point3f* getCorrespondingMarkedPoints(Mat frame, vector<Point2f> corners, Mat Hf)
{
    // locate the origin, x axis and y axis
    Point3f Ppa[4][4] = { { Point3f(-1.5, -1.5, 1.0), Point3f(1.5, -1.5, 1.0), Point3f(-1.5, 1.5, 1.0), Point3f(1.5, 1.5, 1.0) }, \
    { Point3f(4.5, -1.5, 1.0), Point3f(7.5, -1.5, 1.0), Point3f(4.5, 1.5, 1.0), Point3f(7.5, 1.5, 1.0) }, \
    { Point3f(-1.5, 3.5, 1.0), Point3f(1.5, 3.5, 1.0), Point3f(-1.5, 6.5, 1.0), Point3f(1.5, 6.5, 1.0) }, \
    { Point3f(4.5, 3.5, 1.0), Point3f(7.5, 3.5, 1.0), Point3f(4.5, 6.5, 1.0), Point3f(7.5, 6.5, 1.0) } };
    vector<Mat> bgr;
    split(frame, bgr);
    Mat HSV;
    cvtColor(frame, HSV, CV_BGR2HSV);
    vector<Mat> hsv;
    split(HSV, hsv);

#ifdef DEBUG_OUTPUT
    imshow("b", bgr[0]);
    waitKey(0);
    imshow("r", bgr[2]);
    waitKey(0);
    imshow("s", hsv[1]);
    waitKey(0);
#endif

    // mark the topleft as (0, 0, 255) & bottomright as (255, 0, 0)
    //cal the corresponding points in image plane
    double dH00 = Hf.at<double>(0, 0);
    double dH01 = Hf.at<double>(0, 1);
    double dH02 = Hf.at<double>(0, 2);
    double dH10 = Hf.at<double>(1, 0);
    double dH11 = Hf.at<double>(1, 1);
    double dH12 = Hf.at<double>(1, 2);
    double dH20 = Hf.at<double>(2, 0);
    double dH21 = Hf.at<double>(2, 1);
    double dH22 = Hf.at<double>(2, 2);

#ifdef DEBUG_OUTPUT
    cout << dH00 << " " << dH01 << " " << dH02 << endl;
    cout << dH10 << " " << dH11 << " " << dH12 << endl;
    cout << dH20 << " " << dH21 << " " << dH22 << endl;
#endif

    Point3f dst(0.0, 0.0, 0.0);
    Point3f position[2] = { Point3f(0.0, 0.0, 1.0), Point3f(0.0, 0.0, 1.0) };
    double maxSumr = -1.0, maxSumb = -1.0;
    bool bfindBlue = false, bfindRed = false;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Point3f src = Ppa[i][j];
            dst.x = dH00 * src.x + dH01 * src.y + dH02 * src.z;
            dst.y = dH10 * src.x + dH11 * src.y + dH12 * src.z;
            dst.z = dH20 * src.x + dH21 * src.y + dH22 * src.z;
            // Normalizaion
            dst.x /= dst.z;
            dst.y /= dst.z;
            dst.z /= dst.z;

            double sumr = 0;
            double sumg = 0;
            double sumb = 0;
            double sums = 0;
            for (int p = -15; p < 15; p++)
            {
                if ((int(dst.y + 0.5) + p < 0) || (int(dst.y + 0.5) + p) >= bgr[0].rows)
                    return NULL;
                uchar * b = bgr[0].data + (int(dst.y + 0.5) + p) *bgr[0].step;
                uchar * g = bgr[1].data + (int(dst.y + 0.5) + p) *bgr[1].step;
                uchar * r = bgr[2].data + (int(dst.y + 0.5) + p) *bgr[2].step;
                uchar * s = hsv[1].data + (int(dst.y + 0.5) + p) *hsv[1].step;
                for (int q = -5; q < 5; q++)
                {
                    if ((int(dst.x + 0.5) + q < 0) || (int(dst.x + 0.5) + q) >= bgr[0].cols)
                        return NULL;
                    sumb += b[int(dst.x + 0.5) + q];
                    sumg += g[int(dst.x + 0.5) + q];
                    sumr += r[int(dst.x + 0.5) + q];
                    sums += s[int(dst.x + 0.5) + q];
                }
            }

            sumb /= double(30 * 30);
            sumg /= double(30 * 30);
            sumr /= double(30 * 30);
            sums /= double(30 * 30);

            if (((sumr - sumg) / 2 + (sumr - sumb) / 2) > maxSumr)
            {
                position[0].x = Ppa[i][0].x + 1.5;
                position[0].y = Ppa[i][0].y + 1.5;
                maxSumr = (sumr - sumg) / 2 + (sumr - sumb) / 2;
                bfindRed = true;
            }

            if (((sumb - sumg) / 2 + (sumb - sumr) / 2) > maxSumb)
            {
                position[1].x = Ppa[i][0].x + 1.5;
                position[1].y = Ppa[i][0].y + 1.5;
                maxSumb = (sumb - sumg) / 2 + (sumb - sumr) / 2;
                bfindBlue = true;
            }
        }
    }

    if (!bfindBlue || !bfindRed)
    {
        LOG_ERROR("Can not Find red/blue corner flag", NULL);
        return NULL;
    }
    else
    {
        Point3f* normalized_position = new Point3f[2];
        for (int i = 0; i < 2; i++)
        {
            Point3f src = position[i];
            Point3f dst;
            dst.x = dH00 * src.x + dH01 * src.y + dH02 * src.z;
            dst.y = dH10 * src.x + dH11 * src.y + dH12 * src.z;
            dst.z = dH20 * src.x + dH21 * src.y + dH22 * src.z;
            // Normalizaion
            dst.x /= dst.z;
            dst.y /= dst.z;
            dst.z /= dst.z;
            normalized_position[i] = dst;
        }
        LOG_INFO(CStringFormat("Marked points red(%lf, %lf), blue(%lf, %lf)", normalized_position[0].x, normalized_position[0].y, normalized_position[1].x, normalized_position[1].y).Get(), NULL);
        return normalized_position;
    }
}