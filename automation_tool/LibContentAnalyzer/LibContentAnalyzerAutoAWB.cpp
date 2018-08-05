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

#include "LibContentAnalyzerAutoAWB.h"
#include <fstream>
//#define DEBUG_OUTPUT
void findBlackandWhiteBlock(Mat &frame_cn3, double &ratio, Mat &debug, double &hue, double &value, double& deltaE);

// L = L * 255 \ 100, a = a + 128, b = b + 128
struct sRgb
{
    uchar B;
    uchar G;
    uchar R;
};

void Lab2Rgb(sRgb &rgb, uchar L, uchar A, uchar B)
{
    Mat unitLab;
    unitLab.create(1, 1, CV_8UC3);
    vector<Mat>vunitLab;
    split(unitLab, vunitLab);
    uchar * tt = vunitLab[0].data;
    tt[0] = L;
    tt = vunitLab[1].data;
    tt[0] = A;
    tt = vunitLab[2].data;
    tt[0] = B;
    merge(vunitLab, unitLab);

    Mat unitRgb;
    cvtColor(unitLab, unitRgb, CV_Lab2BGR);
    vector<Mat>vunitRgb;
    split(unitRgb, vunitRgb);
    tt = vunitRgb[0].data;
    rgb.B = tt[0];
    tt = vunitRgb[1].data;
    rgb.G = tt[0];
    tt = vunitRgb[2].data;
    rgb.R = tt[0];
}
void Rgb2Lab(sRgb rgb, uchar& L, uchar& A, uchar& B)
{
    Mat unitRGB;
    unitRGB.create(1, 1, CV_8UC3);
    vector<Mat>vunitRGB;
    split(unitRGB, vunitRGB);
    uchar * tt = vunitRGB[0].data;
    tt[0] = rgb.B;
    tt = vunitRGB[1].data;
    tt[0] = rgb.G;
    tt = vunitRGB[2].data;
    tt[0] = rgb.R;
    merge(vunitRGB, unitRGB);

    Mat unitLAB;
    cvtColor(unitRGB, unitLAB, CV_BGR2Lab);
    vector<Mat>vunitLAB;
    split(unitLAB, vunitLAB);
    tt = vunitLAB[0].data;
    L = tt[0];
    tt = vunitLAB[1].data;
    A = tt[0];
    tt = vunitLAB[2].data;
    B = tt[0];
}

CContenAnalyzerAutoAWB::CContenAnalyzerAutoAWB() :
CContenAnalyzerBase(ANALYZER_AUTOAWB_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerAutoAWBCompare");

    // Step 2: Initialize algorithm parameters

    m_ratio_th = 0.2;// default pass ratio
    m_AutoAWB_upper_bound = 50;
    m_deltaC = 6;
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerAutoAWB, m_ratio_th), "%lf", "ratio_th", 0, 1, 0.2 },
        { offsetof(CContenAnalyzerAutoAWB, m_AutoAWB_upper_bound), "%d", "AutoAWB_upper_bound", 0, 255, 50},
        { offsetof(CContenAnalyzerAutoAWB, m_deltaC), "%d", "LAB_Delta_C", 0, 15, 6 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerAutoAWB::~CContenAnalyzerAutoAWB()
{
}

ANALYZER_RETRUN CContenAnalyzerAutoAWB::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() > 2)
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

    // Step 3: Analyze
    double ratio = 0, deltaC = double(INT_MAX);
    double hue = 0, value = 0;
    try
    {
        findBlackandWhiteBlock(img_array[0].img, ratio, img_debug, hue, value, deltaC);
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    if (deltaC < m_deltaC)
    {
        LOG_INFO("AutoAWB check passed", NULL);
        LOG_INFO(CStringFormat("Color difference: %lf is smaller than threshold %d", deltaC, m_deltaC).Get(), NULL);
        return ANALYZER_RETRUN_PASS;
    }
    else
    {
        LOG_ERROR(CStringFormat("Color difference: %lf is larger than threshold %d", deltaC, m_deltaC).Get(), NULL);
        LOG_ERROR(CStringFormat("AWB ABNORMAL POINT Ratio value is larger than Ratio threshold(%lf)", m_ratio_th).Get(),
            CStringFormat("Bad pixel ratio is %lf, average hue is %lf and average value(luminance) is %lf", ratio, hue, value).Get());
        return ANALYZER_RETRUN_FAIL;
    }
}

bool inRegionAWB(int flag, double k, double b, int x, int y)
{
    switch (flag)
    {
    case 0:
        return (y - k *x - b > 0);
    case 1:
        if (k > 0)
        {
            return (y - k *x - b > 0);
        }
        else if (k < 0)
        {
            return (y - k *x - b < 0);
        }
        else return false;
    case 2:
        return (y - k *x - b < 0);
    case 3:
        if (k < 0)
        {
            return (y - k *x - b > 0);
        }
        else if (k > 0)
        {
            return (y - k *x - b < 0);
        }
        else return false;
    default:
        return false;
    }
}

void findBlackandWhiteBlock(Mat &frame_cn3, double &ratio, Mat& debug, double &hue, double &value, double &deltaE)
{
    Mat frame, dot, hsv, rgb, lab;
    vector<Mat> vHSV;
    vector<Mat> vLab;
    Mat frame_resize;

    if (frame_cn3.rows > 1000 && frame_cn3.cols > 1000)
    {
        LOG_INFO("Resize image to speed up!", NULL);
        resize(frame_cn3, frame_resize, Size(frame_cn3.cols / 2, frame_cn3.rows / 2));
        cvtColor(frame_resize, frame, CV_BGR2GRAY);
        // convert to hsv
        cvtColor(frame_resize, hsv, CV_BGR2HSV);
        cvtColor(frame_resize, lab, CV_BGR2Lab);
        frame_resize.copyTo(dot);
        split(hsv, vHSV);
        split(lab, vLab);
    }
    else
    {
        frame_cn3.copyTo(frame_resize);
        cvtColor(frame_cn3, frame, CV_BGR2GRAY);
        frame_resize.copyTo(dot);
        // convert to hsv
        cvtColor(frame_cn3, hsv, CV_BGR2HSV);
        cvtColor(frame_cn3, lab, CV_BGR2Lab);
        split(hsv, vHSV);
        split(lab, vLab);
    }

    if (frame.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        EXCEPTION("Empty frame!");
    }

    const int ROW_CORNER = 6;
    const int COL_CORNER = 7;
    const Size PATTERN_SIZE(COL_CORNER, ROW_CORNER);

    int ncorner_count_full = 0;
    int ncorner_count_frame = 0;
    vector<Point2f> corners_frame, temp(4, Point2f(-1, -1));
    temp.clear();

    if (!findChessboardCorners(frame, PATTERN_SIZE, corners_frame))
    {
        LOG_ERROR("Corner points do not equal to pattern size", NULL);
        EXCEPTION("Corner points do not equal to pattern size");
    }

    cornerSubPix(frame, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(2 | 1, 30, 0.01));

#ifdef DEBUG_OUTPUT
    //----------------------reference output, to see chessboard result, enable this part---------------//
    drawChessboardCorners(dot, PATTERN_SIZE, Mat(corners_frame), true);
    imwrite("dot.jpg", dot);
#endif

    // find corner points;
    vector<vector<Point2f> > corner(2, vector<Point2f>(2, Point2f()));
    //get four corner points from the vector
    temp.push_back(corners_frame[0]);
    temp.push_back(corners_frame[6]);
    temp.push_back(corners_frame[35]);
    temp.push_back(corners_frame[41]);

    // rerange to a rectangle
    // if not meet the criteria, the pattern is invalid
    for (int i = 0; i < temp.size() - 1; i++)
    {
        for (int j = i + 1; j < temp.size(); j++)
        {
            if (temp[j].x < temp[i].x)
            {
                Point2f tmp;
                tmp = temp[j];
                temp[j] = temp[i];
                temp[i] = tmp;
            }
        }
    }

    if (temp[0].y <= temp[1].y)
    {
        corner[0][0] = temp[0];
        corner[1][0] = temp[1];
    }
    else
    {
        corner[1][0] = temp[0];
        corner[0][0] = temp[1];
    }

    if (temp[2].y <= temp[3].y)
    {
        corner[0][1] = temp[2];
        corner[1][1] = temp[3];
    }
    else
    {
        corner[1][1] = temp[2];
        corner[0][1] = temp[3];
    }

    LOG_INFO(CStringFormat("left up point is: %lf %lf", corner[0][0].x, corner[0][0].y).Get(), NULL);
    LOG_INFO(CStringFormat("right up point is: %lf %lf", corner[0][1].x, corner[0][1].y).Get(), NULL);
    LOG_INFO(CStringFormat("left bottom point is: %lf %lf", corner[1][0].x, corner[1][0].y).Get(), NULL);
    LOG_INFO(CStringFormat("right bottom point is: %lf %lf", corner[1][1].x, corner[1][1].y).Get(), NULL);

    double k[4], b[4];
    k[0] = (corner[0][1].y - corner[0][0].y) / (corner[0][1].x - corner[0][0].x);
    k[1] = (corner[1][1].y - corner[0][1].y) / (corner[1][1].x - corner[0][1].x);
    k[2] = (corner[1][0].y - corner[1][1].y) / (corner[1][0].x - corner[1][1].x);
    k[3] = (corner[0][0].y - corner[1][0].y) / (corner[0][0].x - corner[1][0].x);

    b[0] = corner[0][0].y - k[0] * corner[0][0].x;
    b[1] = corner[0][1].y - k[1] * corner[0][1].x;
    b[2] = corner[1][1].y - k[2] * corner[1][1].x;
    b[3] = corner[1][0].y - k[3] * corner[1][0].x;

    LOG_INFO(CStringFormat("k0 b0: %lf %lf", k[0], b[0]).Get(), NULL);
    LOG_INFO(CStringFormat("k1 b1: %lf %lf", k[1], b[1]).Get(), NULL);
    LOG_INFO(CStringFormat("k2 b2: %lf %lf", k[2], b[2]).Get(), NULL);
    LOG_INFO(CStringFormat("k3 b3: %lf %lf", k[3], b[3]).Get(), NULL);

    // generate LAB region at L = 100;
    Mat regionLab, regionRGB;
    regionLab.create(256, 256, CV_8UC3);
    regionRGB.create(256, 256, CV_8UC3);
    vector<Mat> vRGB;
    split(regionRGB, vRGB);
    for (int i = 0; i < 256; i++)
    {
        uchar * l = vRGB[0].data + i*vRGB[0].cols;
        uchar * a = vRGB[1].data + i*vRGB[1].cols;
        uchar * b = vRGB[2].data + i*vRGB[2].cols;
        for (int j = 0; j < 256; j++)
        {
            l[j] = 100/100*255;
            a[j] = i;
            b[j] = j;
        }
    }

    merge(vRGB, regionRGB);
    cvtColor(regionRGB, regionLab, CV_Lab2BGR);

    vHSV[1].copyTo(debug);
    uchar* debugd = debug.data;
    Mat raw;
    frame.copyTo(raw);
    uchar* rawd = raw.data;

    int total = 0;
    double sat = 0.0;
    int subtotal = 0;
    int count = 0;
    int countLab = 0;
    double sumLab = 0.0;
    double maxE = -1.0;
    double averageHue = -1, averageValue = -1;
    int histhue[360];
    for (int i = 0; i < 360; i++)
        histhue[i] = 0;
    double average_a = 0.0, average_b = 0.0;

    for (int i = 0; i < vHSV[1].rows; i++)
    {
        uchar *q = debugd + i*debug.cols;
        uchar *r = rawd + i*frame.cols; 
        uchar *h = vHSV[0].data + i*vHSV[0].cols;
        uchar *v = vHSV[2].data + i*vHSV[2].cols;
        uchar *s = vHSV[1].data + i*vHSV[1].cols;
        uchar *pl = vLab[0].data + i*vLab[0].cols;
        uchar *pa = vLab[1].data + i*vLab[1].cols;
        uchar *pb = vLab[2].data + i*vLab[2].cols;

        for (int j = 0; j < vHSV[1].cols; j++)
        {
            if (inRegionAWB(0, k[0], b[0], j, i) && inRegionAWB(1, k[1], b[1], j, i) && inRegionAWB(2, k[2], b[2], j, i) && inRegionAWB(3, k[3], b[3], j, i))
            {
                total++;
                // furtherly discover the white balance deviation from white blocks
                if (v[j] > 150)
                {
                    averageHue += h[j]*2;
                    averageValue += (double(v[j]) / 255);
                    histhue[h[j]*2]++;
                    subtotal++;
                    sat += s[j];
                }

                if (q[j] > (-1.9 * v[j] / 255 + 1.1) && v[j] > 80)
                {
                    q[j] = 255;
                    count++;
                }
                else
                    q[j] = 0;
                //////////////////LAB REGION ERROR////////////////
                //circle(regionLab, Point(pb[j],pa[j] ), 2, Scalar(0, 0, 0), 1);
                if (pl[j]>100)
                {
                    sumLab += sqrt(pow(pa[j] - 128, 2) + pow(pb[j] - 128, 2));
                    countLab++;
                    if (sqrt(pow(pa[j] - 128, 2) + pow(pb[j] - 128, 2)) > maxE)
                        maxE = sqrt(pow(pa[j] - 128, 2) + pow(pb[j] - 128, 2));
                    average_a += pa[j];
                    average_b += pb[j];
                }
                continue;
            }
            else
            {
                q[j] = 0;
                r[j] = 0;
            }
        }
    }

    ratio = double(count) / total;
    averageHue /= subtotal;
    averageValue /= subtotal;

    LOG_INFO(CStringFormat("points count: %d", count).Get(), NULL);
    LOG_INFO(CStringFormat("points total: %d", total).Get(), NULL);
    LOG_INFO(CStringFormat("points average hue : %lf", averageHue).Get(), NULL);
    LOG_INFO(CStringFormat("points average saturation : %lf", sat / subtotal).Get(), NULL);
    LOG_INFO(CStringFormat("points average value(luminance): %lf", averageValue).Get(), NULL);
    hue = averageHue;
    value = averageValue;
    LOG_INFO(CStringFormat("max Error: %lf", maxE).Get(), NULL);
    LOG_INFO(CStringFormat("delta C %lf", sumLab / countLab).Get(), NULL);
    // delta C cal here
    deltaE = sumLab / countLab;
    average_a /= countLab;
    average_b /= countLab;

    // Enable color difference check for each part of the 24 color chart but currentlt not used as check point
    Mat Hf;
    vector<Point2f> vc = generateChessBoardCornerPoints();
    findHomoFromGD2IMG(frame_resize, vc, Hf, corners_frame);
    for(int j = 0; j < 24; j++)
    {
        Point3f src;
        src.x = (colorChart[0][j].x + colorChart[1][j].x) / 2;
        src.y = (colorChart[0][j].y + colorChart[1][j].y) / 2;
        src.z = 1.0;
        Point3f dst = getCorrespondingPoint(frame_resize, corners_frame, src, Hf);
        if (abs(dst.z - (-1)) < 0.0001)
        {
            LOG_ERROR("Corner flags can not be found", NULL);
            return;
        }
#ifdef DEBUG_OUTPUT
        circle(dot, Point2f(dst.x, dst.y), 2, Scalar(0, 0, 255), 2);
#endif
        double deltaCab = 0.0;
        int dis = 3;
        sRgb crgb;
        crgb.R = ::r[j];
        crgb.G = ::g[j];
        crgb.B = ::b[j];
        uchar L, A, B;
        Rgb2Lab(crgb, L, A, B);
        double aveL = 0.0, aveA = 0.0, aveB = 0.0;
        for(int i = dst.y - dis; i <= dst.y + dis; i++)
        {
            uchar * pl = vLab[0].data + i * vLab[0].step;
            uchar * pa = vLab[1].data + i * vLab[1].step;
            uchar * pb = vLab[2].data + i * vLab[2].step;
            for(int j = dst.x - dis; j <= dst.x + dis; j++)
            {
                deltaCab += sqrt(pow(pa[j] - A, 2) + pow(pb[j] - B, 2)) / pow((2 * dis + 1), 2);
                aveL += double(pl[j]) / pow((2 * dis + 1), 2);
                aveA += double(pa[j]) / pow((2 * dis + 1), 2);
                aveB += double(pb[j]) / pow((2 * dis + 1), 2);
            }
        }
        LOG_INFO(CStringFormat("Average( L, A, B ) : (%lf, %lf, %lf); Expected ( L, A, B ) : (%d, %d, %d)", aveL, aveA, aveB, L, A, B).Get(), NULL);
        LOG_INFO(CStringFormat("Color Difference in LAB for index %d is %lf", j, deltaCab).Get(), NULL);
    }
#ifdef DEBUG_OUTPUT
    imwrite("dot.jpg",dot);
    imwrite("croppedregion.jpg", raw);
    ofstream myfile("hist_of_hue.txt", ios::ate);

    for (int i = 0; i < 360; i++)
    {
        myfile << histhue[i] << '\n';
    }
    circle(regionLab, Point(average_b, average_a), 2, Scalar(0,0,0), 2);
    rectangle(regionLab, Rect(Point(125, 125), Point(131, 131)), Scalar(0, 0, 0), 1);
    imwrite("lab.jpg", regionLab);
#endif
    return;
}