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

#include "LibContentAnalyzerAE.h"
#include<fstream>
//#define DEBUG_OUTPUT
void calYHist(Mat img, double &ratio_low, double &ratio_high, double & averageY, double hist[256]);
void findLocalHist(Mat &frame_cn3, double histChessboard[256], double &diff);

CContenAnalyzerAutoAE::CContenAnalyzerAutoAE() :
CContenAnalyzerBase(ANALYZER_AUTOAE_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerAutoAECompare");

    // Step 2: Initialize algorithm parameters

    m_ratio_th = 0.5;// default pass ratio
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerAutoAE, m_ratio_th), "%lf", "ratio_th", 0, 1, 0.5 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerAutoAE::~CContenAnalyzerAutoAE()
{
}

ANALYZER_RETRUN CContenAnalyzerAutoAE::
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
    double ratio_low = 0, ratio_high = 0, averageY = 0, diff = 0;
    double hist[256];
    double histChessboard[256];
    
    for (int i = 0; i < 256; i++)
    {
        hist[i] = 0.0;
        histChessboard[i] = 0.0;
    }

    try
    {
        calYHist(img_array[0].img, ratio_low, ratio_high, averageY, hist);
        findLocalHist(img_array[0].img, histChessboard, diff);
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    if (ratio_low > m_ratio_th)
    {
        LOG_WARNING(CStringFormat("AE: TOO DIM POINT Ratio value is larger than Ratio threshold(%lf)", m_ratio_th).Get(),
            CStringFormat("Its value is %lf", ratio_low).Get());
    }

    if (ratio_high > m_ratio_th)
    {
        LOG_WARNING(CStringFormat("AE: TOO BRIGHT POINT Ratio value is larger than Ratio threshold(%lf)", m_ratio_th).Get(),
            CStringFormat("Its value is %lf", ratio_high).Get());
    }

    if (averageY < 80 || averageY > 200)
    {
        LOG_ERROR(CStringFormat("Average Y value is out of range(80, 200)").Get(),
            CStringFormat("Its value is %lf", averageY).Get());
        return ANALYZER_RETRUN_FAIL;
    }
    else if (diff > m_ratio_th/2)
    {
        LOG_ERROR(CStringFormat("Dim and bright chessboard points is unbalanced. Difference ratio is %lf", diff).Get(), NULL);
        return ANALYZER_RETRUN_FAIL;
    }
    else
    {
        LOG_INFO("AutoAE check passed", NULL);
        return ANALYZER_RETRUN_PASS;
    }
}

void RGBToY(Mat &src, Mat &Y)
{
    Y.create(src.rows, src.cols, CV_8UC1);
    uchar *dataSrc = NULL;
    uchar *dataY = NULL;
    for (int y = 0; y < src.rows; y++)
    {
        dataSrc = src.data + y * src.step;
        dataY = Y.data + y * Y.step;

        for (int x = 0, x3 = 0; x < src.cols; x++, x3 += 3)
        {
            double srcY = 0.114 * dataSrc[x3] + 0.587 * dataSrc[x3 + 1] + 0.299 * dataSrc[x3 + 2];
            dataY[x] = (uchar)srcY;
        }
    }

}

void calYHist(Mat img, double &ratio_low, double &ratio_high, double& averageY, double hist[256])
{
    Mat Y;
    RGBToY(img, Y);
    for (int i = 0; i < Y.rows; i++)
    {
        uchar* data = Y.data + i * Y.step;
        for (int j = 0; j < Y.cols; j++)
        {
            hist[data[j]]+= 1.0 / (Y.cols * Y.rows);
            averageY += double(data[j]) / (Y.cols * Y.rows);
        }
    }

    for (int i = 0; i < 127; i++)
    {
        ratio_low += hist[i];
        ratio_high += hist[255 - i];
    }

    LOG_INFO(CStringFormat("ratio_low (0 - 127) is %lf.", ratio_low).Get(), NULL);
    LOG_INFO(CStringFormat("ratio_high (128 - 255) is %lf.", ratio_high).Get(), NULL);
    LOG_INFO(CStringFormat("averageY is %lf.", averageY).Get(), NULL);
}

bool inRegionAE(int flag, double k, double b, int x, int y)
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

void findLocalHist(Mat &frame_cn3, double histChessboard[256], double &diff)
{
    Mat frame;

    if (frame_cn3.rows > 1000 && frame_cn3.cols > 1000)
    {
        LOG_INFO("Resize image to speed up!", NULL);
        Mat frame_resize;
        resize(frame_cn3, frame_resize, Size(frame_cn3.cols / 2, frame_cn3.rows / 2));
        cvtColor(frame_resize, frame, CV_BGR2GRAY);
    }
    else
    {
        cvtColor(frame_cn3, frame, CV_BGR2GRAY);
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
    LOG_INFO("Finde chessboard and try to locate in subpixel level!", NULL);
    cornerSubPix(frame, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(2 | 1, 30, 0.01));

    //find corner points;
    vector< vector<Point2f> > corner(2, vector<Point2f>(2, Point2f(-1, -1)));
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


    int total = 0;

    Mat debug;
    frame.copyTo(debug);

    for (int i = 0; i < frame.rows; i++)
    {
        uchar* gray = frame.data + i * frame.step;
        uchar *q = debug.data + i*debug.cols;
        for (int j = 0; j < frame.cols; j++)
        {
            if (inRegionAE(0, k[0], b[0], j, i) && inRegionAE(1, k[1], b[1], j, i) && inRegionAE(2, k[2], b[2], j, i) && inRegionAE(3, k[3], b[3], j, i))
            {
                total++;
                histChessboard[gray[j]]++;
                //q[j] = 255;
            }
            else
            {
                q[j] = 0;
            }
        }
    }

    double dim = 0, bright = 0;
    for (int i = 0; i < 256; i++)
    {
        histChessboard[i] /= total;
    }
    for (int i = 0; i < 80; i++)
    {
        dim += histChessboard[i];
        bright += histChessboard[255 - i];
    }

    diff = abs(bright - dim);
    LOG_INFO(CStringFormat("points total: %d", total).Get(), NULL);
    LOG_INFO(CStringFormat("dim of chessboard (0 - 80) is %lf.", dim).Get(), NULL);
    LOG_INFO(CStringFormat("bright (175 - 255) of chessboard is %lf.", bright).Get(), NULL);
    LOG_INFO(CStringFormat("difference ratio is %lf.", diff).Get(), NULL);

#ifdef DEBUG_OUTPUT
    imwrite("regionAE.jpg", debug);

    ofstream myfile("hist_of_chessboard.txt", ios::ate);

    for (int i = 0; i < 256; i++)
    {
        myfile << histChessboard[i] << '\n';
    }
#endif
    return;
}