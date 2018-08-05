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
#include "LibContentAnalyzerAF.h"
#include <fstream>
void findAccumulatedGradient(Mat frame, double &gradient, double& mitfv, double cutoffTh);
//#define DEBUG_OUTPUT
CContenAnalyzerAutoAF::CContenAnalyzerAutoAF() :
CContenAnalyzerBase(ANALYZER_AUTOAF_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerAutoAFCompare");

    // Step 2: Initialize algorithm parameters

    m_ratio_th = 20;// default pass ratio
    m_MTF_th = 0.70;
    m_Cutoff_th = 0.2;
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerAutoAF, m_ratio_th), "%lf", "ratio_th", 0.0, 100.0, 20.0 },
        { offsetof(CContenAnalyzerAutoAF, m_MTF_th), "%lf", "MTF_th", 0.0, 1.0, 0.70 },
        { offsetof(CContenAnalyzerAutoAF, m_Cutoff_th), "%lf", "Cutoff_th", 0.0, 1.0, 0.20 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerAutoAF::~CContenAnalyzerAutoAF()
{
}

ANALYZER_RETRUN CContenAnalyzerAutoAF::
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
    double gradient = -1.0;
    double mitfv = -1.0;
    try
    {
        findAccumulatedGradient(img_array[0].img, gradient, mitfv, m_Cutoff_th);
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        return ANALYZER_OTHER_EXCEPTION;
    }

    if (mitfv > m_MTF_th)
    {
        LOG_INFO(CStringFormat("average harris corner gradient is %lf and threshold is (%lf)", gradient, m_ratio_th).Get(), NULL);
        LOG_INFO(CStringFormat("mtf value: %lf is LARGER than value threshold(%lf)", mitfv, m_MTF_th).Get(),
            NULL);
        return ANALYZER_RETRUN_PASS;
    }
    else
    {
        LOG_INFO(CStringFormat("average harris corner gradient is %lf and threshold is (%lf)", gradient, m_ratio_th).Get(), NULL);
        LOG_ERROR(CStringFormat("mtf value: %lf is SMALLER than value threshold(%lf)", mitfv, m_MTF_th).Get(),
            NULL);
        return ANALYZER_RETRUN_FAIL;
    }
}

void findAccumulatedGradient(Mat frame_cn3, double &gradient, double& mitfv, double cutoffTh)
{
    Mat frame, frame_resize;
    Mat MTF, MTFDUMP;

    if (frame_cn3.rows > 1000 && frame_cn3.cols > 1000)
    {
        LOG_INFO("Resize image to speed up!", NULL);
        resize(frame_cn3, frame_resize, Size(frame_cn3.cols / 2, frame_cn3.rows / 2));
        cvtColor(frame_resize, frame, CV_BGR2GRAY);
    }
    else
    {
        frame_cn3.copyTo(frame_resize);
        cvtColor(frame_cn3, frame, CV_BGR2GRAY);
    }

    if (frame.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        EXCEPTION("Empty frame!");
    }

    frame_resize.copyTo(MTF);
    frame_resize.copyTo(MTFDUMP);

    const int ROW_CORNER = 6;
    const int COL_CORNER = 7;
    const Size PATTERN_SIZE(COL_CORNER, ROW_CORNER);

    int ncorner_count_full = 0;
    int ncorner_count_frame = 0;
    vector<Point2f> corners_frame;

    if (!findChessboardCorners(frame, PATTERN_SIZE, corners_frame))
    {
        LOG_ERROR("Corner points do not equal to pattern size", NULL);
        EXCEPTION("Corner points do not equal to pattern size. AF function does not work correctly(if full pattern in FOV)");
    }

    cornerSubPix(frame, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(2 | 1, 30, 0.01));

    // calculate accumulation gradient
    int total = 0;
    for (int k = 0; k < corners_frame.size(); k++)
    {
        // choose 10 pixel as radius and calculate accumulative gradient in the direction from p_selected_point to p_corner_point
        for (int i = 0; i < frame.rows; i++)
        {
            uchar* point_0 = frame.data + i * frame.step;
            for (int j = 0; j < frame.cols; j++)
            {
                double dist = sqrt(pow(abs(double(i) - corners_frame[k].y), 2) + pow(abs(double(j) - corners_frame[k].x), 2));
                if (dist < 10)
                {
                    total++;
                    // find the corresponding point 
                    double dx = corners_frame[k].x - j;
                    double dy = corners_frame[k].y - i;
                    double slope = (corners_frame[k].y - double(i)) / (corners_frame[k].x - double(j));
                    int cx = corners_frame[k].x + dx;
                    int cy = dx * slope + corners_frame[k].y;
                    uchar* point_1 = frame.data + cy * frame.step;
                    gradient += sqrt(pow(abs(double(point_0[j]) - point_1[cx]), 2) + pow(abs(double(point_0[j]) - point_1[cx]), 2));
                }
            }
        }
    }

    gradient = gradient / (total * 2);
    LOG_INFO(CStringFormat("points count total: %d", total).Get(), NULL);
    LOG_INFO(CStringFormat("average gradient value: %lf", gradient).Get(), NULL);

    // using MTF Method
    // choose a slanted edge to cal MTF, currently select the line between corner point 17 and 18
    Point2f p0 = corners_frame[17], p1 = corners_frame[18];
    double k, b;
    k = (p0.y - p1.y) / (p0.x - p1.x);
    b = p0.y - k * p0.x;

    // old method to get the slanted edge and keep it as backup
    /*Point2f mid;
    mid.x = double(p0.x + p1.x) / 2;
    mid.y = double(p0.y + p1.y) / 2;
    int xs, xe, ys, ye;
    int edge = max(double(4) / 5 * abs(p0.x - mid.x), double(4) / 5 * abs(p1.y - mid.y));
    xs = int(mid.x) - edge;
    ye = int(mid.y) + edge;
    xe = int(mid.x) + edge;
    ys = int(mid.y) - edge;*/

    // new method to get slanted edge based on new pattern
    vector<Point2f> vc = generateChessBoardCornerPoints();
    Mat Hf;
    vector<Point2f> corner;
    findHomoFromGD2IMG(frame_resize, vc, Hf, corner);
    Point3f dst[7];
    for (int j = 0; j < 7; j++)
    {
        dst[j] = getCorrespondingPoint(frame_resize, corner, verticalSlantedEdge[j], Hf);
        if (abs(dst[j].z - (-1)) < 0.0001)
        {
            LOG_ERROR("Corner flags can not be found", NULL);
            return;
        }
        circle(MTFDUMP, Point(dst[j].x, dst[j].y), 2, Scalar(0, 255, 0), 2);
    }
    Point2f mid;
    mid.x = dst[6].x;
    mid.y = dst[6].y;
    int xs, xe, ys, ye;
    bool bTranversed = (abs(Hf.at<double>(0, 0)) < abs(Hf.at<double>(0, 1))) && (abs(Hf.at<double>(1, 1)) < abs(Hf.at<double>(1, 0)));
    int edge = bTranversed ? abs(dst[0].x - dst[1].x) : abs(dst[0].y - dst[1].y);
    cout << edge << endl;
    /*cout << abs(dst[0].y - dst[1].y) << endl;
    cout << abs(dst[0].x - dst[1].x) << endl;*/
    xs = int(mid.x) - edge;
    ye = int(mid.y) + edge;
    xe = int(mid.x) + edge;
    ys = int(mid.y) - edge;
    //=================================get critical point end

    Point2f lu, rb;
    lu.x = xs;
    lu.y = ys;
    rb.x = xe;
    rb.y = ye;

    // find global Vb and Vw
    uchar Vb = 256, Vw = -1;
    for (int i = ys; i <= ye; i++)
    {
        uchar * y = frame.data + i*frame.cols;
        for (int j = xs; j <= xe; j++)
        {
            if (y[j] < Vb)
                Vb = y[j];
            if (y[j] > Vw)
                Vw = y[j];
        }
    }
    LOG_INFO(CStringFormat("global Vb is %d", int(Vb)).Get(), NULL);
    LOG_INFO(CStringFormat("global Vw is %d", int(Vw)).Get(), NULL);

    //find local SFR in YCbCr region
    // Cal ESF
    double dk = (dst[1].x - dst[4].x) / (dst[1].y - dst[4].y);
    //k = bTranversed ? (1 / dk) : dk;
    k = dk;
    //cout << (dst[1].x - dst[4].x) << " " << (dst[1].y - dst[4].y) << " " << k << endl;
    double theta = abs(atan(k));
    bool bVertical = false;
    LOG_INFO(CStringFormat("theta is %lf (%lf degree)", theta, theta / (2 * 3.14) * 360).Get(), NULL);
    if (theta / (2 * 3.14) * 360 > 10 && theta / (2 * 3.14) * 360 < 80)
        LOG_ERROR("Theta is not ideal[0 ,10] + [80, 90]", NULL);

    if (theta < 45.0 / 360 * 2 * 3.1415 && theta >= 0.0 / 360  * 2 * 3.1415   )
    {
        LOG_INFO("Vertical Slanted Edge", NULL);
        bVertical = true;
    }
    else if (theta < 90.0 / 360 * 3.1416 * 2 && theta >= 45.0 / 360 * 2 * 3.1415)
    {
        LOG_INFO("Horizontal Slanted Edge", NULL);
        bVertical = false;
    }

    int scale = 4;
    int rr = (2 * edge + 1) * scale;
    vector<double> ESF((2 * edge + 1) + 512, 0.0);
    vector<int> ESFCount((2 * edge + 1) + 512, 0.0);
    vector<int> count;
    if (bVertical || !bVertical) // with current pattern, it's tricky that formular is same(two transformation == original)
    {
        for (int i = ys; i <= ye; i++)
        {
            double deltady = double(i - ys) * sin(theta);
            uchar *intensity = frame.data + i * frame.cols;
            for (int j = xs; j <= xe; j++)
            {
                double deltadx = double(j - xs) * cos(theta);
                int index = floor(deltady + deltadx - 0.5) + 1;
                ESF[index] = double(ESF[index] * ESFCount[index]) + intensity[j];
                ESFCount[index] ++;
                ESF[index] /= ESFCount[index];
            }
        }
    }
    /*else
    {
        for (int i = ys; i <= ye; i++)
        {
            double deltady = double(i - ys) * sin(theta);
            uchar *intensity = frame.data + i * frame.cols;
            for (int j = xs; j <= xe; j++)
            {
                double deltadx = double(j - xs) * cos(theta);
                int index = floor(deltady + deltadx - 0.5) + 1;
                ESF[index] = double(ESF[index] * ESFCount[index]) + intensity[j];
                ESFCount[index] ++;
                ESF[index] /= ESFCount[index];
            }
        }
    }*/
    // resize to upsample points
    // remove padding issue
    Mat mESF = Mat(ESF);
    Mat rmESF;
    resize(mESF, rmESF, Size(1, scale * mESF.rows));
    vector<double> vESF(scale * mESF.rows, 0.0);
    for (int i = 0; i < scale * mESF.rows; i++)
    {
        double* esf = (double*)(rmESF.data + i * rmESF.step);
        vESF[i] = esf[0];
    }
    // remove padding issue
    vESF[rr - 1] = vESF[rr - 5];
    vESF[rr - 2] = vESF[rr - 5];
    vESF[rr - 3] = vESF[rr - 5];
    vESF[rr - 4] = vESF[rr - 5];
    vector<double> SmESF(rr, 0.0);
    // N = 5 blur
    SmESF[0] = vESF[0];
    SmESF[1] = vESF[1];
    SmESF[rr - 2] = vESF[rr - 2];
    SmESF[rr - 1] = vESF[rr - 1];
    for (int i = 2; i <= rr - 3; i++)
    {
        SmESF[i] = (vESF[i - 2] + vESF[i - 1] + vESF[i] + vESF[i + 1] + vESF[i + 2]) / 5;
    }

    // Cal LSF
    vector<float>LSF(rr, 0.0);
    for (int i = 1; i < rr - 1; i++)
    {
        LSF[i] = (SmESF[i - 1] - SmESF[i + 1]) / (2 * 1);
    }
    LSF[0] = LSF[1];
    LSF[rr - 1] = LSF[rr - 2];
    // LSF BLUR
    vector<float>SmLSF(rr, 0.0);
    for (int i = 3; i < rr - 2; i++)
    {
        SmLSF[i] = (LSF[i - 2] + LSF[i - 1] + LSF[i] + LSF[i + 1] + LSF[i + 2]) / 5;
    }
    SmLSF[0] = (3 * LSF[0] + 2 * LSF[1] + LSF[2] - LSF[4]) / 5;
    SmLSF[1] = (4 * LSF[1] + 3 * LSF[2] + 2 * LSF[3] + LSF[4]) / 10;
    SmLSF[rr - 2] = (4 * LSF[rr - 4] + 3 * LSF[rr -3] + 2 * LSF[rr - 2] + LSF[rr - 1]) / 10;
    SmLSF[rr - 1] = (3 * LSF[rr - 1] + 2 * LSF[rr - 2] + LSF[rr -3] - LSF[rr - 5]) / 5;
    // Cal MTF
    Mat mLSF = Mat(SmLSF);
    Mat padded;
    int m = getOptimalDFTSize(mLSF.rows);
    int n = getOptimalDFTSize(mLSF.cols);
    copyMakeBorder(mLSF, padded, 0, m - mLSF.rows, 0, n - mLSF.cols, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];

    double uniformCoef = double(Vw - Vb) / (Vw + Vb);
    float *mag = (float*)magI.data;
    int cutoff = 0;
    double MaxMag = -1;
    for (int i = 0; i < rr; i++)
    {
        if (mag[i] > MaxMag)
            MaxMag = mag[i];
    }

    for (int i = 0; i < rr; i++)
    {
        if (mag[i] / MaxMag < cutoffTh)
        {
            cutoff = i - 1;
            LOG_INFO(CStringFormat("Cut off pixel position is %d.", i - 1).Get(), NULL);
            break;
        }
    }

    for (int i = 0; i <= cutoff; i++)
    {
        if (double(cutoff) / 2 == i)
        {
            LOG_INFO(CStringFormat("50%% Cutoff Value: MTF50 = %lf", mag[i] / MaxMag).Get(), NULL);
            mitfv = mag[i] / MaxMag;
        }
        else if (double(cutoff) / 2 > double(i) && double(cutoff) / 2 < double(i + 1))
        {
            {
                mitfv = (mag[i] + mag[i+1]) /(2 * MaxMag);
                LOG_INFO(CStringFormat("50%% Cutoff Value: MTF50 = %lf", mitfv).Get(), NULL);
            }
        }

    }


#ifdef DEBUG_OUTPUT
    //save MTF curve
    ofstream myfile("Modulation_Transfer_Funcion.txt", ios::ate);
    ofstream myfile1("Edge_Spread_Function.txt", ios::ate);
    ofstream myfile2("Line_Spread_Function.txt", ios::ate);
    for (int i = 0; i < rr; i++)
    {
        myfile1 << vESF[i] << endl;
        myfile2 << SmLSF[i] << endl;
    }
    for (int i = 0; i <= cutoff; i++)
    {
        myfile << mag[i] / MaxMag << endl;
    }
    // dump image
    rectangle(MTFDUMP, Rect(lu, rb), Scalar(0, 0, 255), 1);
    imwrite("MTF.jpg", MTFDUMP);
    return;
#endif
}