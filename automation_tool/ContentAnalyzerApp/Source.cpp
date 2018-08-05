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

#ifdef _WIN32
#define _WINDOWS
#endif
#include "opencv2/opencv.hpp"
#include "LibContentAnalyzer.h"
#include "LibToolFunction.h"
#include <stdio.h>
#include <string.h>
#include<iostream>
#include<fstream>
#pragma comment(lib,"LibContentAnalyzer.lib")
#pragma comment(lib,"LibToolFunction.lib")

#ifdef _WINDOWS
extern vector<double> _declspec(dllimport) ratio;
#else
extern vector<double> ratio;
#endif

#ifdef _DEBUG
    #pragma comment(lib,"opencv_core231d.lib")
    #pragma comment(lib,"opencv_highgui231d.lib")
    #pragma comment(lib, "opencv_imgproc231d.lib")
#else
    #pragma comment(lib,"opencv_core231.lib")
    #pragma comment(lib,"opencv_highgui231.lib")
    #pragma comment(lib, "opencv_imgproc231.lib")
#endif

#define MAX_FILE_PATH 512

using namespace std;

CLogger *error_logger = CLogger::GetInstance();


void Loadinputimage(const char *imgpath, const char *paramtypepath, const char *paramvalpath, INPUT_IMAGE_ARRAY &img_array)
{
    INPUT_IMAGE img;
    int img_paramtype = 0;
    if (strcmp(imgpath, "NULL") != 0)
    {
        img.img = imread(imgpath, 1);
        if ((strcmp(paramtypepath, "NULL") != 0) && (strcmp(paramvalpath, "NULL") != 0))
        {
            img_paramtype = atoi(paramtypepath);
            switch (img_paramtype)
            {
            case 0:
                img.param.type = PARAM_INT;
                img.param.val.i = atoi(paramvalpath);
                break;
            case 1:;
                img.param.type = PARAM_FLOAT;
                img.param.val.f = (float)atof(paramvalpath);
                break;
            case 2:
                img.param.type = PARAM_DOUBLE;
                img.param.val.d = atof(paramvalpath);
                break;
            default:
                break;
            }
        }
        img_array.push_back(img);
    }

}

bool Saveoutputimage(const char *outputpath, const char *filename, Mat img)
{
    char outputfile[MAX_FILE_PATH];
    #ifdef _WINDOWS
    sprintf_s(outputfile, MAX_FILE_PATH, "%s/%s", outputpath, filename);
    #else
    snprintf(outputfile, MAX_FILE_PATH, "%s/%s", outputpath, filename);
    #endif
    return imwrite(outputfile, img);
}

bool IsNv12File(char *filename)
{
    if (filename == NULL)
    {
        return false;
    }

    int len = (int) strlen(filename);

    if (len < 6)
    {
        return false;
    }

    if (memcmp(filename + len - 5, ".nv12", sizeof(".nv12")) != 0)
    {
        return false;
    }

    return true;
}

int ReadNV12RawData(const char *filename, uchar **pnv12)
{
    //
    // Read nv12 raw data from file
    //
    FILE *fp = NULL;
#ifdef _WINDOWS
    errno_t err = fopen_s(&fp, filename, "rb");

    if (err != 0 || fp == NULL)
    {
        printf("Failed to open nv12 raw data file %s\n", filename);
        return -1;
    }
#else
    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("Failed to open nv12 raw data file %s\n", filename);
        return -1;
    }
#endif
    fseek(fp, 0L, SEEK_END);
    long datalen = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
#ifdef _WINDOWS
    *pnv12 = (uchar *)_aligned_malloc(datalen, 0x1000);
#else
    *pnv12 = (uchar *)malloc(datalen);
#endif
    size_t readlen = fread(*pnv12, 1, datalen, fp);

    fclose(fp);

    if (readlen != datalen)
    {
        printf("Failed to read nv12 raw data from file %s\n", filename);
        return -2;
    }

    return datalen;
}

void convert_nv122bgr(uchar *buff, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    Mat ycrcb;
    ycrcb.create(Size(w, h), CV_8UC3);

    uchar *pYCrCb = NULL;
    uchar *pY = NULL;
    uchar *pCbCr = NULL;

    int cbcrstep = (w + 1) / 2 * 2;

    // NV12 to YCrCb
    for (int y = 0; y < h; y++)
    {
        pYCrCb = ycrcb.data + y * ycrcb.step;
        pY = buff + y * w;
        pCbCr = buff + h * w + (y / 2) * cbcrstep;

        for (int x = 0, x3 = 0; x < w; x++, x3 += 3)
        {
            pYCrCb[x3] = pY[x];
            pYCrCb[x3 + 1] = pCbCr[x / 2 * 2 + 1];
            pYCrCb[x3 + 2] = pCbCr[x / 2 * 2];
        }
    }

    // YCrCb to BGR
    cvtColor(ycrcb, dst, CV_YCrCb2BGR);
}

int parseVideoPath(const char *path, const char* flag)
{
    const char* p = path;
    const char* q = flag;
    if (q == NULL || p == NULL)
        return -1;
    int i = 0, j = 0;
    while (i < strlen(path) && j < strlen(flag))
    {
        int k = i;
        while (p[k] == q[j])
        {
            k++;
            j++;
        }
        if (j == strlen(flag))
        {
            return k - strlen(flag) + 1;
        }
        else
        {
            i++;
            j = 0;
        }
    }

    return -1;
}

bool LoadinputVideoBuffer(const char *imgpath, const char *paramtypepath, const char *paramvalpath, INPUT_IMAGE_ARRAY &img_array)
{
    int img_paramtype = 0;
    if (strcmp(imgpath, "NULL") != 0)
    {
        char szVideoPath[MAX_FILE_PATH] = {};
        #ifdef _WINDOWS
        sprintf_s(szVideoPath, MAX_FILE_PATH, "%s", imgpath);
        #else
        snprintf(szVideoPath, MAX_FILE_PATH, "%s", imgpath);
        #endif
        VideoCapture capture(szVideoPath);
        double dRealFPS = capture.get(CV_CAP_PROP_FPS);
        int iRealWidth, iRealHeight;
        iRealWidth = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
        iRealHeight = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
        int iRealEncoder = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
        if (!capture.isOpened())
        {
            LOG_ERROR(CStringFormat("Failed to load video %s", szVideoPath).Get(), NULL);
            return false;
        }
        else
        {
            LOG_INFO(CStringFormat("loading video %s", szVideoPath).Get(), NULL);
        }
        int cCount = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_COUNT));
        for (int i = 0; i < cCount - 1; i++)
        {
            Mat frame;
            INPUT_IMAGE img;
            capture >> frame;
            if (!frame.empty())
            {
                frame.copyTo(img.img);

                if ((strcmp(paramtypepath, "NULL") != 0) && (strcmp(paramvalpath, "NULL") != 0))
                {
                    img.param.type = PARAM_ALL;
                    img.param.val.i = iRealEncoder;
                    img.param.val.f = (iRealWidth << 16) | (iRealHeight);
                    img.param.val.d = dRealFPS;
                    img.param.val.l = atoll(paramvalpath);
                }
                else
                {
                    // This block maybe used for future feature. so just give a default realization here
                    img.param.type = PARAM_ALL;
                    img.param.val.i = 0;
                }
                img_array.push_back(img);
            }
            else
            {
                LOG_ERROR(CStringFormat("Frame count(%d) is not exactly same the value in header file(%d).", cCount, i).Get(), NULL);
                return false;
            }
        }
        LOG_INFO(CStringFormat("Frame count(%d) is exactly same the value in header file(%d).", cCount, cCount).Get(), NULL);
        capture.release();
    }

    return true;
}

ANALYZER_RETRUN runZoomMemSave(const char *imgpath, const char *paramtypepath, const char *paramvalpath, Mat& img_debug, Mat& img_reference, char logfile[]);

void Usage(const char *argv0)
{
    printf("Usage: %s \n"
        "         1.  AnalyzerTypeID:   can be [0, 31], refer to relevant document.\n"
        "         2.  ConfigFilePath:   config XML file path.\n"
        "         3.  Image1Path:       image1 path.\n"
        "         4.  Image1ParamType:  parameter type, can be \"NULL\".\n"
        "         5.  Image1ParamValue: parameter value, can be \"NULL\".\n"
        "         6.  Image2Path:       image2 path, can be \"NULL\".\n"
        "         7.  Image2ParamType:  parameter type, can be \"NULL\".\n"
        "         8.  Image2ParamValue: parameter value, can be \"NULL\".\n"
        "         9.  Image3Path:       image3 path, can be \"NULL\".\n"
        "         10. Image3ParamType:  parameter type, can be \"NULL\".\n"
        "         11. Image3ParamValue: parameter value, can be \"NULL\".\n"
        "         12. OutputFolder:     output folder.\n", argv0);
}

int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    char logfilePath[MAX_FILE_PATH] = {};

    if (argc < 13)
    {
        Usage(argv[0]);
        return -1;
    }

    if (strcmp(argv[12], "NULL") == 0)
    {
        LOG_ERROR("The output folder doesn't exist", NULL);
        return -2;
    }

    #ifdef _WINDOWS
    sprintf_s(logfilePath, MAX_FILE_PATH, "%s/%s", argv[12], "Result.txt");
    #else
    snprintf(logfilePath, MAX_FILE_PATH, "%s/%s", argv[12], "Result.txt");
    #endif

    #ifdef _WINDOWS
    errno_t err = fopen_s(&fp, logfilePath, "a+");
    if (err != 0 || fp == NULL)
    {
        LOG_ERROR(CStringFormat("Failed to open %s", logfilePath).Get(), NULL);
        return -2;
    }
    #else
    fp = fopen(logfilePath, "a+");
    if (fp == NULL)
    {
        LOG_ERROR(CStringFormat("Failed to open %s", logfilePath).Get(), NULL);
        return -2;
    }
    #endif
    else
    {
        error_logger->SetFile(fp);
    }

    if (strcmp(argv[1],"NULL") == 0)
    {
        LOG_ERROR("The Analyzer Type cannot be empty", NULL);
        fclose(fp);
        return -3;
    }
    int typeval = atoi(argv[1]);
    if ((typeval < 0)||(typeval > 31))
    {
        LOG_ERROR("The value of Analyzer Type is out of range", NULL);
        fclose(fp);
        return -3;
    }
    AnalyzerType testType;
    switch (typeval)
    {
    case 0:
        testType = ANALYZER_BRIGHTNESS_COMPARE;
        break;
    case 1:
        testType = ANALYZER_CHECKMUTEMODE;
        break;
    case 2:
        testType = ANALYZER_CONTRAST_COMPARE;
        break;
    case 3:
        testType = ANALYZER_EXPOSURE_COMPARE;
        break;
    case 4:
        testType = ANALYZER_HUE_COMPARE;
        break;
    case 5:
        testType = ANALYZER_TORCH_COMPARE;
        break;
    case 6:
        testType = ANALYZER_WHITEBALANCE_COMPARE;
        break;
    case 7:
        testType = ANALYZER_ZOOM_COMPARE;
        break;
    case 8:
        testType = ANALYZER_THUMBNAIL_COMPARE;
        break;
    case 9:
        testType = ANALYZER_JPEGQUALITY_COMPARE;
        break;
    case 10:
        testType = ANALYZER_FOCUS_COMPARE;
        break;
    case 11:
        testType = ANALYZER_COLOREFFECT_NEGATIVE;
        break;
    case 12:
        testType = ANALYZER_COLOREFFECT_GRASSGREEN;
        break;
    case 13:
        testType = ANALYZER_COLOREFFECT_MONO;
        break;
    case 14:
        testType = ANALYZER_COLOREFFECT_SEPIA;
        break;
    case 15:
        testType = ANALYZER_COLOREFFECT_SKYBLUE;
        break;
    case 16:
        testType = ANALYZER_COLOREFFECT_SKINWHITEN;
        break;
    case 17:
        testType = ANALYZER_HDR_COMPARE;
        break;
    case 18:
        testType = ANALYZER_FOV_COMPARE;
        break;
    case 19:
        testType = ANALYZER_SIMILARITY_COMPARE;
        break;
    case 20:
        testType = ANALYZER_GDC_COMPARE;
        break;
    case 21:
        testType = ANALYZER_3A_AUTO_CONSISTENT;
        break;
    case 22:
        testType = ANALYZER_VIDEO_BASIC;
        break;
    case 23:
        testType = ANALYZER_ULL_COMPARE;
        break;
    case 24:
        testType = ANALYZER_AUTOAWB_COMPARE;
        break;
    case 25:
        testType = ANALYZER_AUTOAE_COMPARE;
        break;
    case 26:
        testType = ANALYZER_AUTOAF_COMPARE;
        break;
    case 29:
        testType = ANALYZER_GREEN_CORRUPTION;
        break;
	case 30:
        testType = ANALYZER_ATE_COMPARE;
        break;
    case 31:
        testType = ANALYZER_CHESSBOARD_COMPARE;
        break;
    case 32:
        testType = ANALYZER_COLORCAST_COMPARE;
        break;
    default:
        break;
    }
    CContenAnalyzerInterface *Test = FactoryCreateAnalyzer(testType);
    if (Test == NULL)
    {
        LOG_ERROR("Cannot create Analyzer", NULL);
        fclose(fp);
        return -3;
    }

    if (strcmp(argv[2],"NULL") != 0)
    {
        if (!Test->LoadAlgorithmParameters(argv[2]))
        {
            LOG_ERROR(CStringFormat("Failed to load analyzer parameters in %s", argv[2]).Get(), NULL);
            fclose(fp);
            return -3;
        }
    }

    Mat img_debug, img_reference;
    INPUT_IMAGE_ARRAY img_array;

    if (typeval == 9)
    {
        INPUT_IMAGE img;
        int filecount = 0;
        int nv12len = 0;
        int nv12w = 0;
        int nv12h = 0;
        uchar *nv12 = NULL;

        for (int i = 1; i <= 3; i++)
        {
            if (strcmp(argv[3*i], "NULL") != 0)
            {
                filecount++;
            }
        }

        if (filecount != 3)
        {
            LOG_ERROR("The number of input image for JpegqualityCompare is not enough", NULL);
            fclose(fp);
            return -3;
        }

        if (!IsNv12File(argv[3]))
        {
            LOG_ERROR("The file type of the first input image is not nv12", NULL);
            fclose(fp);
            return -3;
        }

        nv12len = ReadNV12RawData(argv[3], &nv12);
        nv12w = imread(argv[6], 1).cols;
        nv12h = imread(argv[6], 1).rows;

        if (nv12len < nv12w*nv12h + ((nv12w + 1) / 2)*((nv12h + 1) / 2) * 2)
        {
            LOG_ERROR("Read nv12 raw data length is less than expected", NULL);
            fclose(fp);
            return -3;
        }

        convert_nv122bgr(nv12, nv12w, nv12h, img.img);
#ifdef _WINDOWS
        _aligned_free(nv12);
#else
        free(nv12);
#endif
        img_array.push_back(img);

    }
    else
    {
        Loadinputimage(argv[3], argv[4], argv[5], img_array);
    }

    Loadinputimage(argv[6], argv[7], argv[8], img_array);
    //Loadinputimage(argv[9], argv[10], argv[11], img_array);

    ANALYZER_RETRUN AnalyzerResult;

    switch (testType)
    {
        case ANALYZER_BRIGHTNESS_COMPARE:
        case ANALYZER_CHECKMUTEMODE:
        case ANALYZER_CONTRAST_COMPARE:
        case ANALYZER_EXPOSURE_COMPARE:
        case ANALYZER_HUE_COMPARE:
        case ANALYZER_TORCH_COMPARE:
        case ANALYZER_WHITEBALANCE_COMPARE:
        case ANALYZER_THUMBNAIL_COMPARE:
        case ANALYZER_JPEGQUALITY_COMPARE:
        case ANALYZER_FOCUS_COMPARE:
        case ANALYZER_COLOREFFECT_NEGATIVE:
        case ANALYZER_COLOREFFECT_GRASSGREEN:
        case ANALYZER_COLOREFFECT_MONO:
        case ANALYZER_COLOREFFECT_SEPIA:
        case ANALYZER_COLOREFFECT_SKYBLUE:
        case ANALYZER_COLOREFFECT_SKINWHITEN:
        case ANALYZER_HDR_COMPARE:
        case ANALYZER_FOV_COMPARE:
        case ANALYZER_SIMILARITY_COMPARE:
        case ANALYZER_ULL_COMPARE:
        case ANALYZER_AUTOAWB_COMPARE:
        case ANALYZER_AUTOAE_COMPARE:
        case ANALYZER_AUTOAF_COMPARE:
        case ANALYZER_GREEN_CORRUPTION:
		case ANALYZER_ATE_COMPARE:
        case ANALYZER_CHESSBOARD_COMPARE:
        case ANALYZER_COLORCAST_COMPARE:
        {
            Loadinputimage(argv[9], argv[10], argv[11], img_array);
            break;
        }
        case ANALYZER_GDC_COMPARE:
        case ANALYZER_3A_AUTO_CONSISTENT:
        case ANALYZER_VIDEO_BASIC:
            {
                if(LoadinputVideoBuffer(argv[9], argv[10], argv[11], img_array))
                    break;
                else
                    return ANALYZER_RETRUN_FAIL;
            }
        case ANALYZER_ZOOM_COMPARE:
            {
                if (strcmp(argv[9], "NULL") == 0)
                {
                    LOG_INFO("check digital zoom", NULL);
                    //Loadinputimage(argv[9], argv[10], argv[11], img_array);
                    break;
                }
                else
                {
                    LOG_INFO("check smooth zoom with mem-save mode", NULL);
                    AnalyzerResult = runZoomMemSave(argv[9], argv[10], argv[11], img_debug, img_reference, argv[12]);
                    goto bypassAnalyzer;
                }
            }
        default:
            break;
    }

    AnalyzerResult = Test->Analyze(img_array, img_debug, img_reference);

bypassAnalyzer:

    int finalresult = (int)AnalyzerResult;

    if (ANALYZER_RETRUN_PASS == AnalyzerResult)
    {

        LOG_INFO(CStringFormat("{%s}>> pass", Test->GetContenAnalyzerName()).Get(), NULL);
    }
    else
    {
        LOG_ERROR(CStringFormat("{%s}>> fail", Test->GetContenAnalyzerName()).Get(), NULL);
    }

    if (!img_debug.empty())
    {
        if (!Saveoutputimage(argv[12],"deta.jpg",img_debug))
        {
            LOG_ERROR("Failed to save delta image", NULL);
        }
    }

    if (!img_reference.empty())
    {
        if (!Saveoutputimage(argv[12], "reference.jpg", img_reference))
        {
            LOG_ERROR("Failed to save reference image", NULL);
        }
    }

    if (fp != NULL)
        fclose(fp);

    FactoryDestroyAnalyzer(&Test);

    return finalresult;
}

ANALYZER_RETRUN runZoomMemSave(const char *imgpath, const char *paramtypepath, const char *paramvalpath, Mat& img_debug, Mat& img_reference,char logfile[])
{
    int img_paramtype = 0;
    if (strcmp(imgpath, "NULL") != 0)
    {
        char szVideoPath[MAX_FILE_PATH] = {};
        #ifdef _WINDOWS
        sprintf_s(szVideoPath, MAX_FILE_PATH, "%s", imgpath);
        #else
        snprintf(szVideoPath, MAX_FILE_PATH, "%s", imgpath);
        #endif
        VideoCapture capture(szVideoPath);
        double dRealFPS = capture.get(CV_CAP_PROP_FPS);
        int iRealWidth, iRealHeight;
        iRealWidth = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
        iRealHeight = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
        int iRealEncoder = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
        if (!capture.isOpened())
        {
            LOG_ERROR(CStringFormat("Failed to load video %s", szVideoPath).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO(CStringFormat("loading video %s", szVideoPath).Get(), NULL);
        }
        int cCount = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_COUNT));
        INPUT_IMAGE_ARRAY img_array;
        img_array.clear();
        LOG_INFO(CStringFormat("%d frame in total", cCount).Get(), NULL);
        for (int i = 0; i < cCount - 1; i++)
        {
            Mat frame;
            INPUT_IMAGE img;
            capture >> frame;
            if (!frame.empty())
            {
                frame.copyTo(img.img);

                if ((strcmp(paramtypepath, "NULL") != 0) && (strcmp(paramvalpath, "NULL") != 0))
                {
                    img.param.type = PARAM_INT;
                    img.param.val.i = atoi(paramvalpath);
                }
                else
                {
                    // This block maybe used for future feature. so just give a default realization here
                    img.param.type = PARAM_ALL;
                    img.param.val.i = 0;
                }

                // choose # = 3 check points in video to evaluate smooth zoom quality
                if ((parseVideoPath(szVideoPath, "2X") > 0 && parseVideoPath(szVideoPath, "16X") > 0 && parseVideoPath(szVideoPath, "2X") < parseVideoPath(szVideoPath, "16X")) ||
                    (parseVideoPath(szVideoPath, "1X") > 0 && parseVideoPath(szVideoPath, "16X") > 0 && parseVideoPath(szVideoPath, "1X") < parseVideoPath(szVideoPath, "16X")) ||
                    (parseVideoPath(szVideoPath, "1X") > 0 && parseVideoPath(szVideoPath, "2X") > 0 && parseVideoPath(szVideoPath, "1X") < parseVideoPath(szVideoPath, "2X")))
                {
                    if (i == 0 * (cCount - 2) / 5 || i == 1 * (cCount - 2) / 5 || i == 2 * (cCount - 2) / 5  || i == 5 * (cCount - 2) / 5)
                    {
                        if (i == 0)
                        {
                            img.param.val.i = parseVideoPath(szVideoPath, "1X") > 0 ? 1 : 2;
                            LOG_INFO(CStringFormat("Zoom start point from %d X", img.param.val.i).Get(), NULL);
                        }
                        if (i == 5 * (cCount - 2) / 5)
                        {
                            img.param.val.i = parseVideoPath(szVideoPath, "16X") > 0 ? 16 : 2;
                            LOG_INFO(CStringFormat("Zoom end point to %d X", img.param.val.i).Get(), NULL);
                        }
                        img_array.push_back(img);
                    }
                }
                else if ((parseVideoPath(szVideoPath, "1X") > 0 && parseVideoPath(szVideoPath, "16X") > 0 && parseVideoPath(szVideoPath, "1X") > parseVideoPath(szVideoPath, "16X")))
                {
                    if (i == 0 * (cCount - 2) / 5 || i == 2 * (cCount - 2) / 5 || i == 5 * (cCount - 2) / 5)
                    {
                        if (i == 0)
                        {
                            img.param.val.i = 16;
                            LOG_INFO(CStringFormat("Zoom start point from %d X", img.param.val.i).Get(), NULL);
                        }
                        if (i == 5 * (cCount - 2) / 5)
                        {
                            img.param.val.i = 1;
                            LOG_INFO(CStringFormat("Zoom end point to %d X", img.param.val.i).Get(), NULL);
                        }
                        img_array.push_back(img);
                    }
                }
            }
            else
            {
                LOG_ERROR(CStringFormat("Frame count(%d) is not exactly same the value in header file(%d).", i, cCount).Get(), NULL);
                return ANALYZER_RETRUN_PASS;
            }
        }
        if ((parseVideoPath(szVideoPath, "1X") > 0 && parseVideoPath(szVideoPath, "16X") > 0 && parseVideoPath(szVideoPath, "1X") > parseVideoPath(szVideoPath, "16X")))
        {
            reverse(img_array.begin(), img_array.end());
        }
        LOG_INFO(CStringFormat("Frame count(%d) is exactly same the value in header file(%d).", cCount, cCount).Get(), NULL);
        capture.release();

        LOG_INFO(CStringFormat("check point # = (%d).", img_array.size()).Get(), NULL);
        CContenAnalyzerInterface *Test = FactoryCreateAnalyzer(ANALYZER_ZOOM_COMPARE);
        ANALYZER_RETRUN temp_result = Test->Analyze(img_array, img_debug, img_reference);
        img_array.erase(img_array.begin());

        if (temp_result != ANALYZER_RETRUN_PASS)
            return temp_result;
    }

    ofstream fout(CStringFormat("%s/%s", logfile, "zoom_data.txt").Get());
    for (size_t i = 0; i < ratio.size(); i++)
    {
        double zoom_ratio = 1.0;
        for (size_t j = 0; j <= i; j++)
        {
            zoom_ratio *= ratio[j];
        }

        fout << zoom_ratio << endl;
    }
    return ANALYZER_RETRUN_PASS;
}