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

#include "LibContentAnalyzerULL.h"

vector<double> SNR;
vector<double> brightnessDif;
Mat Hf;
vector<Point3f> dst;
double calPSNR(Mat& img_signal, Mat& img_noise);
double calBriDif(Mat& img_signal, Mat& img_noise);

CContenAnalyzerULL::CContenAnalyzerULL():
CContenAnalyzerBase(ANALYZER_ULL_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerULLCompare");

    // Step 2: Initialize algorithm parameters

    // default pass ratio
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerULL::~CContenAnalyzerULL()
{
}

ANALYZER_RETRUN CContenAnalyzerULL::
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
        LOG_ERROR("Input image is empty", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 3: Check Input Image Resolution
    if (img_array[0].img.cols != img_array[1].img.cols ||
        img_array[0].img.rows != img_array[1].img.rows)
    {
        LOG_ERROR("Input image resolution doesn't match each other", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    // Step 4: Check Input parameter type
    int iZero = 0;
    for (size_t i = 0; i < img_array.size(); i++)
    {
        if (img_array[i].param.type != PARAM_INT)
        {
            LOG_ERROR("Wrong input parameter type", NULL);
            return ANALYZER_PARAMETER_EXCEPTION;
        }

        // 0: disabled; 1: on; 2: auto;
        if (img_array[i].param.val.i <0 ||
            img_array[i].param.val.i > 3)
        {
            LOG_ERROR(CStringFormat("Input parameter is out of range [0, 2]").Get(),
                CStringFormat("Its value is %d", img_array[i].param.val.i).Get());
            return ANALYZER_PARAMETER_EXCEPTION;
        }
    }

    // Step 5: Analyze
    // This part is code for modified psnr and currently the content check is implemented via snr
    /*double dSNR = calPSNR(img_array[0].img, img_array[1].img);
    double dDif = calBriDif(img_array[0].img, img_array[1].img);
    SNR.push_back(dSNR);
    brightnessDif.push_back(dDif);

    if (SNR.size() == 2 && brightnessDif.size() == 2)
    {
        if (brightnessDif[0] < brightnessDif[1])
        {
            LOG_INFO(CStringFormat("BD(%lf) with ULL enabled is smaller than BD(%lf) with ULL disabled", brightnessDif[0], brightnessDif[1]).Get(), NULL);
        }
        else
        {
            LOG_ERROR(CStringFormat("BD(%lf) with ULL enabled is smaller than BD(%lf) with ULL disabled", brightnessDif[0], brightnessDif[1]).Get(), NULL);
            img_array[0].img.copyTo(img_debug);
            img_array[1].img.copyTo(img_reference);
            return ANALYZER_RETRUN_FAIL;
        }

        if (SNR[0] >= SNR[1])
        {
            LOG_ERROR(CStringFormat("SNR(%lf) with ULL disabled is larger than or equal to SNR(%lf) with ULL enabled", SNR[0], SNR[1]).Get(), NULL);
            img_array[0].img.copyTo(img_debug);
            img_array[1].img.copyTo(img_reference);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO(CStringFormat("SNR(%lf) with ULL disabled is smaller than SNR(%lf) with ULL enabled", SNR[0], SNR[1]).Get(), NULL);
            return ANALYZER_RETRUN_PASS;
        }
    }*/

    vector<Point2f> vc = generateChessBoardCornerPoints();
    Mat Hf;
    vector<Point2f> corner;
    Mat frame_resize, frame;
    Mat cpy[2];
    dst.clear();
    for (int i = 0; i < img_array.size(); i++)
    {
        double bw0 = 0.0, bw1 = 0.0, mn = 0.0;
        if (img_array[i].img.rows > 1000 && img_array[i].img.cols > 1000)
        {
            LOG_INFO("Resize image to speed up!", NULL);
            resize(img_array[i].img, frame_resize, Size(img_array[i].img.cols / 2, img_array[i].img.rows / 2));
            cvtColor(frame_resize, frame, CV_BGR2GRAY);
        }
        else
        {
            img_array[i].img.copyTo(frame_resize);
            cvtColor(img_array[i].img, frame, CV_BGR2GRAY);
        }

        frame_resize.copyTo(cpy[i]);

     try
     {
            if (i == 0)
            {
                findHomoFromGD2IMG(frame_resize, vc, Hf, corner);

            }

            for (int cc = 0; cc < 7; cc++)
            {

                Point3f src;
                src.x = ((::grayscale[0][cc].x) + (::grayscale[1][cc].x)) / 2;
                src.y = ((::grayscale[0][cc].y) + (::grayscale[1][cc].y)) / 2;
                src.z = 1.0;
                if (i == 0)
                {
                    try{
                            Point3f cp = getCorrespondingPoint(frame_resize, corner, src, Hf);
                            if (abs(cp.x - 0.0) < 0.001 && abs(cp.y - 0.0) < 0.001 && abs(cp.z + 1.0) < 0.001)
                            {
                                LOG_WARNING("Invalid pattern!", NULL);
                                return ANALYZER_RETRUN_FAIL;
                            }
                            dst.push_back(cp);
                        }
                    catch (CException e)
                    {
                        LOG_ERROR(e.GetException(), NULL);
                        return ANALYZER_RETRUN_FAIL;
                    }
                }

                if (cc == 0)
                {
                    // assuem the block is at least 21 X 21. If too small, it is meaningless.
                    for (int i = dst[cc].y - 10; i <= dst[cc].y + 10; i++)
                    {
                        uchar* y = frame.data + i * frame.step;
                        for (int j = dst[cc].x - 10; j <= dst[cc].x + 10; j++)
                        {
                            bw0 += double(y[j]) / (21 * 21);
                        }
                    }
#ifdef DEBUG_OUTPUT
                    circle(cpy[i], Point(dst[cc].x, dst[cc].y), 2, Scalar(0, 0, 255), 1);
#endif
                }
                if (cc == 6)
                {
                    // assuem the block is at least 21 X 21. If too small, it is meaningless.
                    for (int i = dst[cc].y - 10; i <= dst[cc].y + 10; i++)
                    {
                        uchar* y = frame.data + i * frame.step;
                        for (int j = dst[cc].x - 10; j <= dst[cc].x + 10; j++)
                        {
                            bw1 += double(y[j]) / (21 * 21);
                        }
                    }
#ifdef DEBUG_OUTPUT
                    circle(cpy[i], Point(dst[cc].x, dst[cc].y), 2, Scalar(0, 0, 255), 1);
#endif
                }
                if (cc == 3)
                {
                    // assuem the block is at least 21 X 21. If too small, it is meaningless.
                    for (int i = dst[cc].y - 10; i <= dst[cc].y + 10; i++)
                    {
                        uchar* y = frame.data + i * frame.step;
                        for (int j = dst[cc].x - 10; j <= dst[cc].x + 10; j++)
                        {
                            // rms noise
                            mn += double(1) / (21 * 21) * pow((255 / double(2)) - y[j], 2);
                        }
                    }
                    mn = sqrt(mn);
#ifdef DEBUG_OUTPUT
                    circle(cpy[i], Point(dst[cc].x, dst[cc].y), 2, Scalar(0, 0, 255), 1);
#endif
                }
            }
        }
     catch (CException e)
     {
         LOG_ERROR(e.GetException(), NULL);
         return ANALYZER_RETRUN_FAIL;
     }
        LOG_INFO(CStringFormat("%d image intensity range [bw0, bw1]: [%lf, %lf].", i, bw0, bw1).Get(), NULL);
        double dSNR = 20 * log10(abs(bw0 - bw1) / mn);
        SNR.push_back(dSNR);
        LOG_INFO(CStringFormat("%d image medium intensiy level Noise(RMS) =  %lf, SNR = %lf.", i, mn, dSNR).Get(), NULL)
#ifdef DEBUG_OUTPUT
        imwrite(CStringFormat("%d.jpg", i).Get(), cpy[i]);
#endif
    }

    if (SNR.size() == 4)
    {
        if (SNR[1] - SNR[0] < SNR[3] - SNR[2])
        {
            LOG_INFO(CStringFormat("Normal Light SNR: %lf; Low Light without ULL: %lf < Low Light with ULL: %lf", SNR[0], SNR[1], SNR[3]).Get(), NULL);
            return ANALYZER_RETRUN_PASS;
        }
        else
        {
            LOG_ERROR(CStringFormat("Normal Light SNR: %lf; Low Light without ULL: %lf > Low Light with ULL: %lf", SNR[0], SNR[1], SNR[3]).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
    }
    return ANALYZER_RETRUN_PASS;
}

double calPSNR(Mat& img_signal, Mat& img_noise)
{
    Mat img_signal_gray, img_noise_gray;
    cvtColor(img_signal, img_signal_gray, CV_BGR2GRAY);
    cvtColor(img_noise, img_noise_gray, CV_BGR2GRAY);

    uchar *pSi, *pNo;
    pSi = NULL;
    pNo = NULL;
    double dMSE = 0.0, dPSNR = 0.0;
    int iPulse = INT_MIN;

    for (int i = 0; i < img_noise_gray.rows; i++)
    {
        pSi = img_signal_gray.data + i * img_signal_gray.step;
        pNo = img_noise_gray.data + i * img_noise_gray.step;
        for (int j = 0; j < img_noise_gray.cols; j++)
        {
            uchar ucSi = pSi[j];
            uchar ucNo = pNo[j];
            dMSE += pow((abs(ucSi - ucNo)), 2);
            if (static_cast<int>(ucNo) > iPulse)
            {
                iPulse = static_cast<int>(ucNo);
            }
        }
    }

    dMSE /= (img_noise_gray.rows * img_noise_gray.cols * iPulse);

    LOG_INFO(CStringFormat("current pixel pulse is %d", iPulse).Get(), NULL);
    dPSNR = 10 * log10(pow(255, 2) / dMSE);
    return dPSNR;
}
double calBriDif(Mat& img_signal, Mat& img_noise)
{
    double dDiff = 0.0;
    Mat img_signal_gray, img_noise_gray;
    cvtColor(img_signal, img_signal_gray, CV_BGR2GRAY);
    cvtColor(img_noise, img_noise_gray, CV_BGR2GRAY);

    uchar *pSi, *pNo;
    pSi = NULL;
    pNo = NULL;

    for (int i = 0; i < img_noise_gray.rows; i++)
    {
        pSi = img_signal_gray.data + i * img_signal_gray.step;
        pNo = img_noise_gray.data + i * img_noise_gray.step;
        for (int j = 0; j < img_noise_gray.cols; j++)
        {
            uchar ucSi = pSi[j];
            uchar ucNo = pNo[j];
            dDiff += static_cast<double>(ucNo - ucSi);
        }
    }

    dDiff /= (img_noise_gray.rows * img_noise_gray.cols);

    LOG_INFO(CStringFormat("current brightness difference is %lf", dDiff).Get(), NULL);
    return dDiff;
}