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

#include "LibContentAnalyzerZoom.h"
bool getCircleImage(Mat img, double& val);
#define MEDIAN(x, y, z) ((x > y) ? ((x > z) ? (y > z ? y : z): x) : ((x < z) ?(y > z ? z : y): x))
vector<double> ratio;
CContentAnalyzerZoom::CContentAnalyzerZoom() :
    CContenAnalyzerBase(ANALYZER_ZOOM_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerZoomCompare");

    // Step 2: Initialize algorithm parameters
    threshold = 0.8;
    kernel_size = 5;
    lower_bound_coefficient = 0.3;

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerZoom, threshold), "%lf", "parameter_threshold", 0, 1, 0.8 },
        { offsetof(CContentAnalyzerZoom, kernel_size), "%d", "kernel_size", 0, 50, 5 },
        { offsetof(CContentAnalyzerZoom, lower_bound_coefficient), "%lf", "lower_bound_coefficient", 0, 1.0, 0.3 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerZoom::~CContentAnalyzerZoom()
{}

ANALYZER_RETRUN CContentAnalyzerZoom::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() < 2)
    {
        LOG_ERROR("Input image count is expected >= 2", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (img_array.size() <= 10)
    {
        goto image;
    }
    else
    {
        goto video;
    }

image:
    {
        // Step 2: Check Input parameter type
        if (img_array.size() < 3 && img_array[0].param.val.i != 0 && img_array[1].param.val.i != 0)
        {
            int iZero = -1;
            for (size_t i = 0; i < img_array.size(); i++)
            {
                if (img_array[i].param.type != PARAM_INT)
                {
                    LOG_ERROR("Wrong input parameter type", NULL);
                    return ANALYZER_PARAMETER_EXCEPTION;
                }

                if (img_array[i].param.val.i < 0 || // add 0 for smooth zoom check;
                    img_array[i].param.val.i > 61)
                {
                    LOG_ERROR("Input parameter is out of range", NULL);
                    return ANALYZER_PARAMETER_EXCEPTION;
                }

                if (img_array[i].param.val.i == 1)
                {
                    iZero = (int)i;
                }

            }

            if (iZero < 0)
            {
                LOG_ERROR("No Zoom 1 image in the inputs", NULL);
                return ANALYZER_PARAMETER_EXCEPTION;
            }

            Mat src1, src2;
            src1 = img_array[iZero].img;
            src2 = img_array[1 - iZero].img;

            // Step 2: Check Input Image
            if (src1.empty() || src2.empty())
            {
                LOG_ERROR("Load image error", NULL);
                return ANALYZER_IMAGE_EXCEPTION;
            }

            // Step 3: Check Input Image Resolution
            if (src1.cols != src2.cols || src1.rows != src2.rows)
            {
                LOG_ERROR("Resolution of two input images does not match each other", NULL);
                return ANALYZER_IMAGE_EXCEPTION;
            }

            // Step 5: Analyzer
            double zoom_nx = (double)(img_array[1 - iZero].param.val.i);
            double zoom_ratio = (double)(65.0 - zoom_nx) / 64.0;

            // zoom in second image
            Mat src2_zoom_in;
            int zw = (int)(src2.cols * zoom_ratio + 0.5);
            int zh = (int)(src2.rows * zoom_ratio + 0.5);
            int zx = src1.cols / 2 - zw / 2;
            int zy = src1.rows / 2 - zh / 2;

            resize(src2, src2_zoom_in, Size(zw, zh));

            // crop image1
            img_reference = src1(Rect(zx, zy, zw, zh));

            double ssim_avg;
            double ssim_min;
            V_SSIM_BLOCK artifact_blocks;

            CalcDeltaImage(img_reference, src2_zoom_in, img_debug);

            CalculateSsimCn3(img_reference, src2_zoom_in, kernel_size, threshold, ssim_avg, ssim_min, artifact_blocks, 0, 0, 0, true);

            DrawArtifactBlocks(img_debug, artifact_blocks);

            if (ssim_avg < threshold)
            {
                LOG_ERROR(CStringFormat("Fail due to average SSIM value is smaller than the threshold(%lf)",
                    threshold).Get(), CStringFormat("ssim_avg is %lf", ssim_avg).Get());

                return ANALYZER_RETRUN_FAIL;
            }

            if (artifact_blocks.size() > 0 && ssim_min < threshold * lower_bound_coefficient)
            {
                LOG_ERROR(CStringFormat("Fail due to the mimimum SSIM value is smaller than threshold(%lf*%lf)",
                    threshold, lower_bound_coefficient).Get(), CStringFormat("ssim_min is %lf", ssim_min).Get());

                return ANALYZER_RETRUN_FAIL;
            }
            LOG_INFO("Zoom check passed.", NULL);
            return ANALYZER_RETRUN_PASS;
        }
        else
        {
            // check smooth zoom for image pairs
#ifdef DEBUG_OUTPUT
            imwrite("SmoothZoom0.jpg", img_array[0].img);
            imwrite("SmoothZoom1.jpg", img_array[1].img);
            imwrite("SmoothZoom2.jpg", img_array[2].img);
            imwrite("SmoothZoom3.jpg", img_array[img_array.size() - 1].img);
#endif
            Mat Hf, img1, img2;
            img_array[0].img.copyTo(img1);
            img_array[1].img.copyTo(img2);
            bool bMatrix = false;
            double zoom_ratio_cal;
            if (CalculateHomographyMatrix(img1, img2, Hf) ||CalculateHomographyMatrix_sift(img1, img2, Hf))
            {
                bMatrix = true;
            }

            if (bMatrix)
            {
                double dH00 = Hf.at<double>(0, 0);
                double dH01 = Hf.at<double>(0, 1);
                double dH02 = Hf.at<double>(0, 2);
                double dH10 = Hf.at<double>(1, 0);
                double dH11 = Hf.at<double>(1, 1);
                double dH12 = Hf.at<double>(1, 2);
                double dH20 = Hf.at<double>(2, 0);
                double dH21 = Hf.at<double>(2, 1);
                double dH22 = Hf.at<double>(2, 2);
                LOG_INFO(CStringFormat("Homography Matrix: [%lf, %lf, %lf\n %lf, %lf, %lf\n %lf, %lf, %lf]", \
                    dH00, dH01, dH02, dH10, dH11, dH12, dH20, dH21, dH22).Get(), NULL);
                if (abs(dH00 - dH11) < (lower_bound_coefficient / 10))
                {
                    ratio.push_back((dH00 + dH11) / 2);
                    zoom_ratio_cal = (dH00 + dH11) / 2;
                }
            }
            else
            {
                LOG_ERROR("Can not cal HomoMatrix during start and mid frame", NULL);
                return ANALYZER_RETRUN_FAIL;
            }
            // using the current point to check whether "smooth"
            Mat src1, src2;
            Mat src2_zoom_in;
            int zw;
            int zh;
            int zx;
            int zy;
            double ssim_avg;
            double ssim_min;
            V_SSIM_BLOCK artifact_blocks;
            double th;

            if (img_array.size() > 3)
            {
                img_array[1].img.copyTo(img1);
                img_array[2].img.copyTo(img2);

                if (CalculateHomographyMatrix(img1, img2, Hf) || CalculateHomographyMatrix_sift(img1, img2, Hf))
                {
                    bMatrix = true;
                }

                if (bMatrix)
                {
                    double dH00 = Hf.at<double>(0, 0);
                    double dH01 = Hf.at<double>(0, 1);
                    double dH02 = Hf.at<double>(0, 2);
                    double dH10 = Hf.at<double>(1, 0);
                    double dH11 = Hf.at<double>(1, 1);
                    double dH12 = Hf.at<double>(1, 2);
                    double dH20 = Hf.at<double>(2, 0);
                    double dH21 = Hf.at<double>(2, 1);
                    double dH22 = Hf.at<double>(2, 2);
                    LOG_INFO(CStringFormat("Homography Matrix: [%lf, %lf, %lf\n %lf, %lf, %lf\n %lf, %lf, %lf]", \
                        dH00, dH01, dH02, dH10, dH11, dH12, dH20, dH21, dH22).Get(), NULL);
                    if (abs(dH00 - dH11) < (lower_bound_coefficient / 10))
                    {
                        ratio.push_back((dH00 + dH11) / 2);
                        zoom_ratio_cal = (dH00 + dH11) / 2;
                    }
                }
                else
                {
                    LOG_ERROR("Can not cal HomoMatrix during start and mid frame", NULL);
                    return ANALYZER_RETRUN_FAIL;
                }

                if (zoom_ratio_cal > 1 || ratio[0] > img_array[img_array.size() - 1].param.val.i)
                {
                    LOG_INFO("SmoothZoom Interval check passed!", NULL);
                }
            }
            //  for the known end zoom point using cropping SSIM comparison
            src1 = img_array[0].img;
            src2 = img_array[img_array.size() - 1].img;

            // Step 2: Check Input Image
            if (src1.empty() || src2.empty())
            {
                LOG_ERROR("Load image error", NULL);
                return ANALYZER_IMAGE_EXCEPTION;
            }

            // Step 3: Check Input Image Resolution
            if (src1.cols != src2.cols || src1.rows != src2.rows)
            {
                LOG_ERROR("Resolution of two input images does not match each other", NULL);
                return ANALYZER_IMAGE_EXCEPTION;
            }

            // Step 5: Analyzer
            
            int zoom_ratio = img_array[img_array.size() - 1].param.val.i / img_array[0].param.val.i;
            LOG_INFO(CStringFormat("Zoom ratio should be %d / %d = %d", img_array[img_array.size() - 1].param.val.i, img_array[0].param.val.i, zoom_ratio).Get(), NULL);
            // zoom in second image
            src2_zoom_in;
            zw = (int)(src2.cols / zoom_ratio + 0.5);
            zh = (int)(src2.rows / zoom_ratio + 0.5);
            zx = src1.cols / 2 - zw / 2;
            zy = src1.rows / 2 - zh / 2;
            LOG_INFO(CStringFormat("zw %d zh %d zx %d zy %d", zw, zh, zx, zy).Get(), NULL);
            resize(src2, src2_zoom_in, Size(zw, zh));
            // crop image1
            img_reference = src1(Rect(zx, zy, zw, zh));
            ssim_avg;
            ssim_min;
            artifact_blocks;

            CalcDeltaImage(img_reference, src2_zoom_in, img_debug);

            CalculateSsimCn3(img_reference, src2_zoom_in, kernel_size, threshold, ssim_avg, ssim_min, artifact_blocks);

            DrawArtifactBlocks(img_debug, artifact_blocks);
            
            th = img_array[img_array.size() - 1].param.val.i == 16 ? 0.65 : 0.75;
            if (ssim_avg < th)
            {
                LOG_ERROR(CStringFormat("Fail due to average SSIM value is smaller than the threshold(%lf)",
                    th).Get(), CStringFormat("ssim_avg is %lf", ssim_avg).Get());

                return ANALYZER_RETRUN_FAIL;
            }
            LOG_INFO(" Smooth Zoom START and End check passed.", NULL);
            LOG_INFO(" Smooth Zoom check passed.", NULL);
            return ANALYZER_RETRUN_PASS;
        }
    }

video:
    {
        vector<double> vecZoomCoef;
        vecZoomCoef.clear();

        //bypass 5 frame here
        Mat img1, img2;
        LOG_INFO(CStringFormat("Total frame is %d", img_array.size()).Get(), NULL);

        if (img_array.size() < 5)
        {
            LOG_ERROR("Need more frame as input!", NULL);
            return ANALYZER_RETRUN_FAIL;
        }

        img_array[4].img.copyTo(img2);
        for (int i = 5; img_array.size() != i; i++)
        {
            img_array[i].img.copyTo(img1);

            Mat Hf;
            if (CalculateHomographyMatrix_cb(img2, img1, Hf))//CalculateHomographyMatrix_cb(img2, img1, Hf)
            {
                double dH00 = Hf.at<double>(0, 0);
                double dH01 = Hf.at<double>(0, 1);
                double dH02 = Hf.at<double>(0, 2);
                double dH10 = Hf.at<double>(1, 0);
                double dH11 = Hf.at<double>(1, 1);
                double dH12 = Hf.at<double>(1, 2);
                double dH20 = Hf.at<double>(2, 0);
                double dH21 = Hf.at<double>(2, 1);
                double dH22 = Hf.at<double>(2, 2);


                if ((abs(dH01) < (lower_bound_coefficient / 10)) && (abs(dH10) < (lower_bound_coefficient / 10)))
                {
                    if (abs(dH00 - dH11) < (lower_bound_coefficient / 10))
                    {
                        vecZoomCoef.push_back((dH00 + dH11) / 2);
                        LOG_INFO(CStringFormat("Zoom ratio to refernece is %lf", (dH00 + dH11) / 2).Get(), NULL);

                        /*int iIndex = vecZoomCoef.size() - 1;
                        if (iIndex >= 1)
                        {
                            if (abs(vecZoomCoef[iIndex] - vecZoomCoef[iIndex - 1]) < (lower_bound_coefficient / 20))
                            {
                                LOG_INFO(CStringFormat("Valid smooth Zoom from ratio %lf to %lf. Delta is abs(%lf) < TH(%lf).", vecZoomCoef[iIndex - 1], vecZoomCoef[iIndex], \
                                    vecZoomCoef[iIndex] - vecZoomCoef[iIndex - 1], lower_bound_coefficient / 20).Get(), NULL);
                            }
                            else
                            {
                                LOG_INFO(CStringFormat("Invalid smooth Zoom from ratio %lf to %lf. Delta is abs(%lf) >= TH(%lf).", vecZoomCoef[iIndex - 1], vecZoomCoef[iIndex], \
                                    vecZoomCoef[iIndex] - vecZoomCoef[iIndex - 1], lower_bound_coefficient / 20).Get(), NULL);
                                return ANALYZER_RETRUN_FAIL;
                            }
                        }*/
                    }
                    else
                    {
                        LOG_ERROR("Not valid zoom input image pairs", NULL);
                        return ANALYZER_RETRUN_FAIL;
                    }
                }
                else
                {
                    LOG_WARNING("Maybe a bad Homography Matrix", NULL);
                }
                img1.copyTo(img2);
            }
            else
            {
                LOG_ERROR("Not valid zoom input image pairs (pattern not recognized!)", NULL);
                return ANALYZER_RETRUN_FAIL;
            }
        }

        return ANALYZER_RETRUN_PASS;
    }
}

bool getCircleImage(Mat img, double &val)
{
    Mat img_gray, img_smooth, img_scale, img_color_scale;

    cvtColor(img, img_gray, CV_BGR2GRAY);
    //GaussianBlur(img_gray, img_smooth, Size(3,3), 0.5);
    vector<Vec3f> circles;
    circles.clear();
    double d = 1.0;
    for (; d > 0.1; d -= 0.1)
    {
        resize(img, img_color_scale, Size(img.cols*d, img.rows*d));
        resize(img_gray, img_scale, Size(img_gray.cols*d, img_gray.rows*d));
        //double resIndex = 1 / d;
        //GaussianBlur(img_scale, img_smooth, Size(3, 3), 0.5);
        medianBlur(img_scale, img_smooth, 5);

        int threshold0 = 100;
        int threshold1 = 150;
        HoughCircles(img_smooth, circles, CV_HOUGH_GRADIENT, 2, 1, threshold0, threshold1);
        if (circles.size() > 1)
        {
            vector<Vec3f> circles_high;
            circles_high.clear();
            while (true)
            {
                threshold1 += 5;
                HoughCircles(img_smooth, circles_high, CV_HOUGH_GRADIENT, 2, 1, threshold0, threshold1);
                if (circles_high.size() > 1)
                {
                    circles = circles_high;
                }
                else
                {
                    break;
                }
            }
            break;
        }
    }

    if (d == 0.1)
    {
        return false;
    }

    double radius;
    for (size_t i = 0; i < 1; i++)
    {

        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        radius = cvRound(circles[i][2]);
        circle(img_color_scale, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        circle(img_color_scale, center, int(radius), Scalar(155, 50, 255), 3, 8, 0);
        //cout << "radius is " << radius / d << endl;
    }
    imshow("img", img_color_scale);
    waitKey(1);
    system("pause");
    //imwrite("C:/Users/yuboli/My_files/local_test/test_exe/circled.jpg", img_color_scale);
    val = radius / d;
    return true;
}