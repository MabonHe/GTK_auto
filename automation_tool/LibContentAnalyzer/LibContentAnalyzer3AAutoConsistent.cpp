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

#include "LibContentAnalyzer3AAutoConsistent.h"

double ExposureDiffRatio(Mat &img1, Mat &img2, Mat &delta, int th);
void BrighnessConvert2Y(Mat &src, Mat &Y);
double BrightnessDiffRatio(Mat &bright, Mat &dim, Mat &badpixels, int th);
double HueDiffRatio(Mat &img_none, Mat &img_effect, Mat &img_debug, double delta_hue, int s_th, int v_th, int h_range);

CContenAnalyzer3AAutoConsistent::CContenAnalyzer3AAutoConsistent() :
CContenAnalyzerBase(ANALYZER_3A_AUTO_CONSISTENT)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("Analyzer3AAutoConsistent");

    // Step 2: Initialize algorithm parameters

    m_cConsistentCount = 10;
    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzer3AAutoConsistent, m_cConsistentCount),   "%d", "consistent_count_threshold", 5, 50, 10},
        { offsetof(CContenAnalyzer3AAutoConsistent, m_iExposure_th),       "%d", "exposure_threshold", 5, 50, 10 },
        { offsetof(CContenAnalyzer3AAutoConsistent, m_iBrightness_th),       "%d", "brightness_threshold", 5, 50, 10 },
        { offsetof(CContenAnalyzer3AAutoConsistent, m_iHueH_th),           "%d", "hue_h_threshold", 5, 50, 15 },
        { offsetof(CContenAnalyzer3AAutoConsistent, m_iHueS_th),           "%d", "hue_s_threshold", 5, 50, 30 },
        { offsetof(CContenAnalyzer3AAutoConsistent, m_iHueV_th),           "%d", "hue_v_threshold", 5, 50, 30 },
        { offsetof(CContenAnalyzer3AAutoConsistent, m_dUniversalRatio_th), "%lf", "ratio_threshold", 0, 1.0, 0.25 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzer3AAutoConsistent::~CContenAnalyzer3AAutoConsistent()
{
}

ANALYZER_RETRUN CContenAnalyzer3AAutoConsistent::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() < 2)
    {
        LOG_ERROR("Input image count is expected >= 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (img_array.size() > 2)
        goto video;
    else
        goto image;

image:
    {

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

            if (img_array[i].param.val.i < 0 ||
                img_array[i].param.val.i > 50)
            {
                LOG_ERROR(CStringFormat("Input parameter is out of range (0, 50)").Get(),
                    CStringFormat("Its value is %d", img_array[i].param.val.i).Get());
                return ANALYZER_PARAMETER_EXCEPTION;
            }
        }

        // Step 5: Analyze
        // 1) Exposure Brightness Hue (Exposure & AWB)
        Mat img1, img2;
        img1 = img_array[0].img;
        img2 = img_array[1].img;

        double dExposureDiffRatio = ExposureDiffRatio(img1, img2, img_debug, m_iExposure_th);
        double dBrightnessDiffRatio = BrightnessDiffRatio(img1, img2, img_debug, m_iBrightness_th);
        double dHueDiffRatio = HueDiffRatio(img1, img2, img_debug, 0, m_iHueS_th, m_iHueV_th, m_iHueH_th);

        if (dExposureDiffRatio >= m_dUniversalRatio_th)
        {
            LOG_ERROR(CStringFormat("Exposure consistency check failed: %lf > threshold %lf", dExposureDiffRatio, m_dUniversalRatio_th).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO("Exposure check passed.", NULL);
        }

        if (dBrightnessDiffRatio >= m_dUniversalRatio_th)
        {
            LOG_ERROR(CStringFormat("Brightness consistency check failed: %lf > threshold %lf", dBrightnessDiffRatio, m_dUniversalRatio_th).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO("Brightness check passed.", NULL);
        }

        if (dHueDiffRatio >= m_dUniversalRatio_th)
        {
            LOG_ERROR(CStringFormat("Hue consistency check failed: %lf > threshold %lf", dHueDiffRatio, m_dUniversalRatio_th).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO("Hue check passed.", NULL);
        }

        // 2) Focus
        Mat Hf;
        if (CalculateHomographyMatrix_cb(img1, img2, Hf))
        {

            CStringFormat primary_msg("Homography matrix: (%lf, %lf, %lf; %lf, %lf, %lf; %lf, %lf, %lf)",
                *((double*)Hf.ptr(0, 0)), *((double*)Hf.ptr(0, 1)), *((double*)Hf.ptr(0, 2)),
                *((double*)Hf.ptr(1, 0)), *((double*)Hf.ptr(1, 1)), *((double*)Hf.ptr(1, 2)),
                *((double*)Hf.ptr(2, 0)), *((double*)Hf.ptr(2, 1)), *((double*)Hf.ptr(2, 2))
                );
            LOG_INFO(primary_msg.Get(), NULL);

            double dXScaleRatio = Hf.at<double>(0, 0);
            double dYScaleRatio = Hf.at<double>(1, 1);


            if (abs(1.0 - dXScaleRatio) <= 0.015 && abs(1.0 - dYScaleRatio) <= 0.015)
            {
                LOG_INFO("Focus check passed.", NULL);
            }
            else
            {
                LOG_ERROR(CStringFormat("Focus consistency check failed: x scale: %lf, y scale: %lf", dXScaleRatio, dYScaleRatio).Get(), NULL);
                return ANALYZER_RETRUN_FAIL;
            }
        }
        else
        {
            LOG_ERROR("Focus consistency check failed: Pattern is not valid.", NULL);
            return ANALYZER_RETRUN_FAIL;
        }

        // 3) Content
        double avg, min;
        V_SSIM_BLOCK vblocks;
        vblocks.clear();
        CalculateSsimCn3(img1, img2, 16, 0.3, avg, min, vblocks, 2, 2, 2);
        if ((1 - m_dUniversalRatio_th) > avg || vblocks.size() > 0)
        {
            LOG_ERROR(CStringFormat("Content check failed. avg is %lf and threshold is %lf. Block size is %d. ", avg, (1 - m_dUniversalRatio_th), vblocks.size()).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO("Content check passed.", NULL);
        }

        LOG_INFO("Consistent check passed one frame.", NULL);
        return ANALYZER_RETRUN_PASS;
    }

video:
    {
        //bitset<32> bsFrameQueue;
        int cCount = 0;
        Mat img1, img2;
        for (int i = 0; i != img_array.size() && !img_array[i].img.empty(); i++)
        {
            LOG_INFO(CStringFormat("Current consistent queue is %d", i).Get(), NULL);
            if (cCount > m_cConsistentCount)
            {
                LOG_INFO(CStringFormat("3A Auto Consistent check passed. Consistent frame count reached threshold: %d", m_cConsistentCount).Get(), NULL);
                return ANALYZER_RETRUN_PASS;
            }

            img_array[i].img.copyTo(img1);
            if (0 < i)
            {
                double dExposureDiffRatio = ExposureDiffRatio(img1, img2, img_debug, m_iExposure_th);
                double dBrightnessDiffRatio = BrightnessDiffRatio(img1, img2, img_debug, m_iBrightness_th);
                double dHueDiffRatio = HueDiffRatio(img1, img2, img_debug, 0, m_iHueS_th, m_iHueV_th, m_iHueH_th);

                if (dExposureDiffRatio >= m_dUniversalRatio_th)
                {
                    cCount = 0;
                    img1.copyTo(img2);
                    LOG_ERROR(CStringFormat("Exposure consistency check failed: %lf > threshold %lf", dExposureDiffRatio, m_dUniversalRatio_th).Get(), NULL);
                    continue;
                }

                if (dBrightnessDiffRatio >= m_dUniversalRatio_th)
                {
                    cCount = 0;
                    img1.copyTo(img2);
                    LOG_ERROR(CStringFormat("Brightness consistency check failed: %lf > threshold %lf", dBrightnessDiffRatio, m_dUniversalRatio_th).Get(), NULL);
                    continue;
                }

                if (dHueDiffRatio >= m_dUniversalRatio_th)
                {
                    cCount = 0;
                    img1.copyTo(img2);
                    LOG_ERROR(CStringFormat("Hue consistency check failed: %lf > threshold %lf", dHueDiffRatio, m_dUniversalRatio_th).Get(), NULL);
                    continue;
                }

                Mat Hf;
                if (CalculateHomographyMatrix_cb(img1, img2, Hf))
                {
                    double dXScaleRatio = Hf.at<double>(0, 0);
                    double dYScaleRatio = Hf.at<double>(1, 1);


                    if (abs(1.0 - dXScaleRatio) > 0.015 || abs(1.0 - dYScaleRatio) > 0.015)
                    {
                        cCount = 0;
                        img1.copyTo(img2);
                        continue;
                    }

                }
                else
                {
                    cCount = 0;
                    img1.copyTo(img2);
                    LOG_ERROR("Focus consistency check failed: Pattern is not valid.", NULL);
                    continue;
                }

                double avg, min;
                V_SSIM_BLOCK vblocks;
                vblocks.clear();
                CalculateSsimCn3(img1, img2, 16, 0.3, avg, min, vblocks, 2, 2, 2);
                if ((1 - m_dUniversalRatio_th) > avg || vblocks.size() > 0)
                {
                    cCount = 0;
                    img1.copyTo(img2);
                    LOG_ERROR(CStringFormat("Content check failed. avg is %lf and threshold is %lf. Block size is %d. ", avg, (1 - m_dUniversalRatio_th), vblocks.size()).Get(), NULL);
                    continue;
                }

                //bsFrameQueue.set(i);
                cCount++;
            }
            else
            {
                img1.copyTo(img2);//prepare reference done
            }
        }

        return ANALYZER_RETRUN_FAIL;
    }

}

double ExposureDiffRatio(Mat &img1src, Mat &img2src, Mat &delta, int th)
{
    Mat img1, img2;

    cvtColor(img1src, img1, CV_BGR2GRAY);
    cvtColor(img2src, img2, CV_BGR2GRAY);

    delta.create(img1.rows, img1.cols, CV_8UC1);
    uchar *data1 = NULL, *data2 = NULL, *datad = NULL;
    double error = 0;
    double size = img1.rows * img1.cols;

    for (int y = 0; y < img1.rows; y++)
    {
        data1 = img1.data + y * img1.step;
        data2 = img2.data + y * img2.step;
        datad = delta.data + y * delta.step;

        for (int x = 0; x < img1.cols; x++)
        {
            int delta = (int)data2[x] - (int)data1[x];

            error += abs(delta < th) ? 0 : 1;

            if (delta < 0)
            {
                delta = 0;
            }

            datad[x] = (uchar)delta;
        }
    }
    return error / size;
}
void BrighnessConvert2Y(Mat &src, Mat &Y)
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
double BrightnessDiffRatio(Mat &bright, Mat &dim, Mat &badpixels, int th)
{
    Mat Yb0, Ybx;
    BrighnessConvert2Y(bright, Yb0);
    BrighnessConvert2Y(dim, Ybx);
    // to get current ratio of good pixels
    // to get binary image for bad pixels, bad is white and correct is black.
    badpixels.create(bright.rows, bright.cols, CV_8UC1);
    uchar *data1 = NULL, *data2 = NULL, *datadt = NULL;
    int count = 0;
    for (int y = 0; y < bright.rows; y++)
    {
        data1 = Yb0.data + y * Yb0.step;
        data2 = Ybx.data + y * Ybx.step;
        datadt = badpixels.data + y * badpixels.step;
        for (int x = 0; x < bright.cols; x++)
        {
            double Y1 = (double)data1[x];
            double Y2 = (double)data2[x];
            if (abs(Y1 - Y2) >= th)
            {
                count++;
                datadt[x] = 0;
            }
            else
                //datadt[x] = 255; keep all the bad pixels in previous check
                ;
        }
    }

    return(count / (double)(bright.rows * bright.cols));
}
double HueDiffRatio(Mat &img_none, Mat &img_effect, Mat &img_debug, double delta_hue, int s_th, int v_th, int h_range)
{
    Mat hsv_none, hsv_effect;

    hsv_none.create(img_none.rows, img_none.cols, CV_8UC3);
    hsv_effect.create(img_none.rows, img_none.cols, CV_8UC3);
    img_debug.create(img_none.rows, img_none.cols, CV_8UC1);

    cvtColor(img_none, hsv_none, CV_BGR2HSV);
    cvtColor(img_effect, hsv_effect, CV_BGR2HSV);

    vector<Mat> hsv_none_chs(3, Mat()), hsv_effect_chs(3, Mat());
    split(hsv_none, hsv_none_chs);
    split(hsv_effect, hsv_effect_chs);

    uchar* datah1 = NULL, *datah2 = NULL, *datadt = NULL;
    uchar* datas1 = NULL, *datas2 = NULL;
    uchar* datav1 = NULL, *datav2 = NULL;
    int count = 0;
    for (int y = 0; y < img_none.rows; y++)
    {
        datah1 = hsv_none_chs.at(0).data + y* hsv_none_chs.at(0).step;
        datah2 = hsv_effect_chs.at(0).data + y* hsv_effect_chs.at(0).step;

        datas1 = hsv_none_chs.at(1).data + y* hsv_none_chs.at(1).step;
        datas2 = hsv_effect_chs.at(1).data + y* hsv_effect_chs.at(1).step;

        datav1 = hsv_none_chs.at(2).data + y* hsv_none_chs.at(2).step;
        datav2 = hsv_effect_chs.at(2).data + y* hsv_effect_chs.at(2).step;

        datadt = img_debug.data + y * img_debug.step;

        for (int x = 0; x < img_none.cols; x++)
        {
            double h1 = (double)datah1[x];
            double h2 = (double)datah2[x];
            double s1 = (double)datas1[x];
            double s2 = (double)datas2[x];
            double v1 = (double)datav1[x];
            double v2 = (double)datav2[x];
            if ((s1 < s_th && s2 < s_th) || (v1 < v_th && v2 < v_th))
            {
                count++;
                datadt[x] = (uchar)(0);
            }
            else if (fabs((h1 + delta_hue) - h2) < h_range || fabs((h1 + delta_hue) - h2 - 180) < h_range || fabs((h1 + delta_hue) - h2 + 180) < h_range)
            {
                count++;
                datadt[x] = (uchar)(0);
            }
            else
            {
                //datadt[x] = (uchar)(255); keep previous check badpixels as well
                ;
            }

        }
    }
    return(1 - count / (double)(img_none.rows * img_none.cols));
}