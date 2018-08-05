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

#include "LibContentAnalyzerVideoBasic.h"

CContenAnalyzerVideoBasic::CContenAnalyzerVideoBasic() :
CContenAnalyzerBase(ANALYZER_VIDEO_BASIC)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerVideoBasic");

    // Step 2: Initialize algorithm parameters

    // Step 3: Algorithm table
    DATA_PARSER params[] = {
        { offsetof(CContenAnalyzerVideoBasic, m_dUniversalRatio_th), "%lf", "ratio_threshold", 0, 1.0, 0.25 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}

CContenAnalyzerVideoBasic::~CContenAnalyzerVideoBasic()
{
}

ANALYZER_RETRUN CContenAnalyzerVideoBasic::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() < 1)
    {
        LOG_ERROR("Frame count is 0 ", NULL);
        return ANALYZER_RETRUN_FAIL;
    }
    else
    {
        LOG_INFO(CStringFormat("Frame count is %d.", img_array.size()).Get(), NULL);
    }

    if (img_array[0].param.type != PARAM_ALL)
    {
        LOG_ERROR(CStringFormat("Invalid input parameter type. Expected type is 4(while %d).", int(img_array[0].param.type)).Get(), NULL);
        return ANALYZER_RETRUN_FAIL;
    }

    Mat img1, img2;

    for (int i = 0; i != img_array.size(); i++)
    {
        double dRealFPS = img_array[i].param.val.d;
        int iRealWidth = (static_cast<int>(img_array[i].param.val.f) >> 16);
        int iRealHeight = ((static_cast<int>(img_array[i].param.val.f) << 16) >> 16);

        int iRealEncoder = img_array[i].param.val.i;
        char codec[5] = {};
        codec[3] = static_cast<char>(img_array[i].param.val.i >> 24);
        codec[2] = static_cast<char>(img_array[i].param.val.i >> 16);
        codec[1] = static_cast<char>(img_array[i].param.val.i >> 8);
        codec[0] = static_cast<char>(img_array[i].param.val.i >> 0);
        codec[4] = '\0';
        LOG_INFO(CStringFormat("Orignal Video Info: %lf fps, %d x %d , coded with %s.", dRealFPS, iRealWidth, iRealHeight, codec).Get(), NULL);

        // decode input parameters:
        int iExpectedfps = img_array[i].param.val.l >> 56;
        int iExpectedWidth = (img_array[i].param.val.l << 8) >> 52;
        int iExpectedHeight = (img_array[i].param.val.l << 20) >> 52;

        // Enable to use for more codec type
        //char szExpectedCodec[5] = {};
        //int iCodec = img_array[i].param.val.l << 32;
        //szExpectedCodec[3] = static_cast<char>(img_array[i].param.val.i >> 24);
        //szExpectedCodec[0] = static_cast<char>(iCodec >> 24);
        //szExpectedCodec[1] = static_cast<char>(iCodec >> 16);
        //szExpectedCodec[2] = static_cast<char>(iCodec >> 8);
        //szExpectedCodec[3] = static_cast<char>(iCodec >> 0);
        //szExpectedCodec[4] = '\0';

        bool bPass = 1;
        LOG_INFO(CStringFormat("Frame %d is in processing......", i).Get(), NULL);
        if (double(iExpectedfps) <= dRealFPS * 0.98 || double(iExpectedfps) >= 1.02 * dRealFPS)
        {
            LOG_ERROR(CStringFormat("The fps check failed. RealFPS is %lf and ExpectedFPS is %lf", dRealFPS, double(iExpectedfps)).Get(), NULL);
            bPass = 0;
        }
        else
        {
            LOG_INFO(CStringFormat("The fps check passed. RealFPS is %lf and ExpectedFPS is %lf", dRealFPS, double(iExpectedfps)).Get(), NULL);
        }

        if (abs(iExpectedWidth - iRealWidth) < 16 && abs(iExpectedHeight - iRealHeight) < 16 && (iRealWidth % 16 == 0) && (iRealHeight % 16 == 0))
        {
            LOG_INFO(CStringFormat("The size check passed. Expected size is %d x %d. Real size is %d(mod 16 == 0) x %d(mod 16 == 0)", iExpectedWidth, iExpectedHeight, iRealWidth, iRealHeight).Get(), NULL);
        }
        else
        {
            LOG_ERROR(CStringFormat("The size check failed. Expected size is %d x %d. Real size is %d x %d", iExpectedWidth, iExpectedHeight, iRealWidth, iRealHeight).Get(), NULL);
            bPass = 0;
        }

        int iExpectedEncoder0 = CV_FOURCC('H', '2', '6', '4');
        int iExpectedEncoder1 = CV_FOURCC('a', 'v', 'c', '1');
        if ((iExpectedEncoder0 != iRealEncoder) && (iExpectedEncoder1 != iRealEncoder))
        {
            LOG_ERROR(CStringFormat("The encode method check failed. Expected is %d(H264) and %d(avc1). Real  is %d(%s)", iExpectedEncoder0, iExpectedEncoder1, iRealEncoder, codec).Get(), NULL);
            bPass = 0;
        }
        else
        {
            LOG_INFO(CStringFormat("The encode method check passed. Expected is %d(H264) and %d(avc1). Real  is %d(%s)", iExpectedEncoder0, iExpectedEncoder1, iRealEncoder, codec).Get(), NULL);
        }


        double avg, min;
        V_SSIM_BLOCK vblocks;
        vblocks.clear();
        img_array[i].img.copyTo(img1);

        if (i > 0)
        {
            CalculateSsimCn3(img1, img2, 16, 0.3, avg, min, vblocks, 2, 2, 2);
            if ((1 - m_dUniversalRatio_th) > avg || vblocks.size() > 0)
            {
                LOG_ERROR(CStringFormat("Content (Focus) check failed. avg is %lf and threshold is %lf. Block size is %d. ", avg, (1 - m_dUniversalRatio_th), vblocks.size()).Get(), NULL);
                bPass = 0;
            }
            else
            {
                LOG_INFO(CStringFormat("Content check passed.avg is %lf and threshold is %lf. Block size is %d. ", avg, (1 - m_dUniversalRatio_th), vblocks.size()).Get(), NULL);
            }
        }
        img1.copyTo(img2);


        if (bPass)
        {
            LOG_INFO(CStringFormat("Frame %d is processed.", i).Get(), NULL);
        }
        else
        {
            LOG_ERROR(CStringFormat("Frame %d failed.", i).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }

    }

    LOG_INFO("Video Basic check passed.", NULL);
    return ANALYZER_RETRUN_PASS;
}
