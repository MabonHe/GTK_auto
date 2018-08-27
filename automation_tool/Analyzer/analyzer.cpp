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
#include <libgen.h>
#pragma comment(lib,"LibContentAnalyzer.lib")
#pragma comment(lib,"LibToolFunction.lib")

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

#ifndef BYTE
#define BYTE unsigned char
#endif

#define ULONG unsigned long
int ImageW , ImageH , RawDumpFrameCount, m_frame,result;
char RawDumpPath[64],ReferenceImagePath[128],ConfigFilePath[256],imageformat[64];

enum MEDIA_FORMAT
{
    MEDIA_FORMAT_UNKNOWN,
    MEDIA_FORMAT_NV12,
    MEDIA_FORMAT_NV16,
    MEDIA_FORMAT_UYVY,
    MEDIA_FORMAT_YUY2,
    MEDIA_FORMAT_XRGB,
    MEDIA_FORMAT_XBGR,
    MEDIA_FORMAT_I420,
    MEDIA_FORMAT_ARGB,
    MEDIA_FORMAT_BGRA,
    MEDIA_FORMAT_BGR24,
    MEDIA_FORMAT_RGB565,
    MEDIA_FORMAT_JPG,
    MEDIA_FORMAT_BMP
};
MEDIA_FORMAT RawDumpFormat;
typedef void (*func_convert_format)(uchar *buff, int stride, int w, int h, Mat &dst);

CLogger *error_logger = CLogger::GetInstance();
void Usage(const char *argv0)
{
    printf("Usage: %s \n"
        "         1.  ConfigFilePath:       config XML file path.\n"
        "         2.  RawDumpPath:          raw dump path.\n"
        "         3.  RawDumpFrameCount:    raw dump image count.\n"
        "         4.  RawDumpFormat:        raw dump image format.\n"
        "         5.  ImageW:               image width.\n"
        "         6.  ImageH:               image height.\n"
        "         7.  ReferenceImagePath:   reference image path.\n"
        "         8.  Alignment:            Alignment:0/1\n" , argv0);
}
int Spilt_parameters(const char *arg, char *key, char *value)
{
    char buffer[256];
    strcpy(buffer, arg);
    char *p = strtok(buffer, "=");
    if (!p)
        return -1;
    strcpy(key, p);
    p = strtok(NULL, "=");
    if (!p)
        return -1;
    strcpy(value, p);
    return 0;
}
int Init(int argc,char *argv[])
{
  char key[256], value[256];

  for(int i = 0; i < argc ; i++)
  {
  	   memset(key,0,sizeof(key));
  	   memset(value,0,sizeof(value));
       Spilt_parameters(argv[i],key,value);
       if ((!strcmp("--help", key)) || (!strcmp("-h", key)) || argc <= 1 )
        {
            Usage("analyzer");
            return -1;
        }
       else if (0 == strcmp("--width", key))
       {
           ImageW = atoi(value);
       }
       else if (0 == strcmp("--height", key))
       {
       	    ImageH = atoi(value);
       }
       else if (0 == strcmp("--count", key))
       {
            RawDumpFrameCount = atoi(value);
       }
      else if (0 == strcmp("--frame", key))
       {
            m_frame = atoi(value);
       }
       else if (0 == strcmp("--format", key))
       {
       	    strncpy(imageformat,value,16);
       }
       else if (0 == strcmp("--filename",key))
       {
       	    strncpy(RawDumpPath,value,256);
       }


  }


  return 0;
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

ULONG ReadRawData(const char *filename, uchar **pnv12)
{
    //
    // Read nv12 raw data from file
    //
    FILE *fp = NULL;

    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        LOG_ERROR(CStringFormat("Failed to open raw data file %s\n", filename).Get(), NULL);
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    ULONG datalen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    *pnv12 = (uchar *)malloc(datalen);

    size_t readlen = fread(*pnv12, 1, datalen, fp);

    fclose(fp);

    if (readlen != datalen)
    {
        LOG_ERROR(CStringFormat("Failed to read raw data from file %s\n", filename).Get(), NULL);
        return -2;
    }

    return datalen;
}

inline BYTE Clip(int clr)
{
    return (BYTE) (clr < 0 ? 0 : (clr > 255 ? 255 : clr));
}

inline void ConvertYCrCbToRGB(
    BYTE y,
    BYTE cr,
    BYTE cb,
    BYTE &r,
    BYTE &g,
    BYTE &b
    )
{
    int c = (int)y - 16;
    int d = (int) cb - 128;
    int e = (int) cr - 128;

    r = Clip((298 * c + 409 * e + 128) >> 8);
    g = Clip((298 * c - 100 * d - 208 * e + 128) >> 8);
    b = Clip((298 * c + 516 * d + 128) >> 8);
}

void convert_nv122bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);
    uchar *pY = NULL;
    uchar *pCbCr = NULL;
    uchar *pBGR = NULL;

    BYTE Y = 0, Cr = 0, Cb = 0;
    BYTE R = 0, G = 0, B = 0;

    // NV12 to BGR
    for (int y = 0; y < h; y++)
    {
        pY = buff + y * stride;
        pCbCr = buff + (h + y / 2) * stride;
        pBGR = dst.data + y * dst.step;

        for (int x = 0, x3 = 0; x < w; x++, x3 += 3)
        {
            Y  = pY[x];
            Cr = pCbCr[x / 2 * 2 + 1];
            Cb = pCbCr[x / 2 * 2];

            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);

            pBGR[x3] = B;
            pBGR[x3 + 1] = G;
            pBGR[x3 + 2] = R;
        }
    }
}

void convert_nv162bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);
    uchar *pY = NULL;
    uchar *pCbCr = NULL;
    uchar *pBGR = NULL;

    BYTE Y = 0, Cr = 0, Cb = 0;
    BYTE R = 0, G = 0, B = 0;

    // NV16 to BGR
    for (int y = 0; y < h; y++)
    {
        pY = buff + y * stride;
        pCbCr = buff + h*stride + y*stride;
        pBGR = dst.data + y * dst.step;

        for (int x = 0, x6 = 0; x < w; x += 2, x6 += 6)
        {
            Y  = pY[x];
            Cr = pCbCr[x+1];
            Cb = pCbCr[x];

            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);

            pBGR[x6] = B;
            pBGR[x6 + 1] = G;
            pBGR[x6 + 2] = R;

            Y  = pY[x + 1];
            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);
            pBGR[x6 + 3] = B;
            pBGR[x6 + 4] = G;
            pBGR[x6 + 5] = R;
        }
    }
}

void convert_yuy22bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR = NULL;
    uchar *pYUY2 = NULL;
    uchar *pCbCr = NULL;

    int yuy2step = w * 2;

    BYTE Y = 0, Cr = 0, Cb = 0;
    BYTE R = 0, G = 0, B = 0;

    // YUY2 to BGR
    for (int y = 0; y < h; y++)
    {
        pBGR = dst.data + y * dst.step;
        pYUY2 = buff + y * stride;

        for (int x4 = 0, x6 = 0; x4 < yuy2step; x4 += 4, x6 += 6)
        {
            Cb = pYUY2[x4 + 1];
            Cr = pYUY2[x4 + 3];

            Y = pYUY2[x4];

            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);

            pBGR[x6] = B;
            pBGR[x6 + 1] = G;
            pBGR[x6 + 2] = R;

            Y = pYUY2[x4 + 2];

            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);

            pBGR[x6 + 3] = B;
            pBGR[x6 + 4] = G;
            pBGR[x6 + 5] = R;
        }
    }
}

void convert_uyvy2bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR = NULL;
    uchar *pYUY2 = NULL;
    uchar *pCbCr = NULL;

    int yuy2step = w * 2;

    BYTE Y = 0, Cr = 0, Cb = 0;
    BYTE R = 0, G = 0, B = 0;

    // UYVY to BGR
    for (int y = 0; y < h; y++)
    {
        pBGR = dst.data + y * dst.step;
        pYUY2 = buff + y * stride;

        for (int x4 = 0, x6 = 0; x4 < yuy2step; x4 += 4, x6 += 6)
        {
            Cb = pYUY2[x4];
            Y = pYUY2[x4 + 1];
            Cr = pYUY2[x4 + 2];

            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);

            pBGR[x6] = B;
            pBGR[x6 + 1] = G;
            pBGR[x6 + 2] = R;

            Y = pYUY2[x4 + 3];

            ConvertYCrCbToRGB(Y, Cr, Cb, R, G, B);

            pBGR[x6 + 3] = B;
            pBGR[x6 + 4] = G;
            pBGR[x6 + 5] = R;
        }
    }
}

void convert_i4202bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR0 = NULL;
    uchar *pBGR1 = NULL;
    uchar *pYUV0 = NULL;
    uchar *pYUV1 = NULL;
    uchar *pCbCr = NULL;
    uchar *pV = NULL;
    uchar *pU = NULL;
    uchar *Vorg = NULL;
    uchar *Uorg = NULL;

    BYTE Y00 = 0, Y01 = 0, Y10 = 0, Y11 = 0, Cr = 0, Cb = 0;
    BYTE R = 0, G = 0, B = 0;

    int yuvstep = w;

    Vorg = buff + h * stride;
    Uorg = buff + h * stride + h * stride / 4;

    // I420 to BGR
    for (int y2 = 0; y2 < h; y2 += 2)
    {
        pBGR0 = dst.data + y2 * dst.step;
        pBGR1 = dst.data + (y2 + 1) * dst.step;

        pYUV0 = buff + y2 * stride;
        pYUV1 = buff + (y2 + 1) * stride;

        pV = Vorg + (y2 / 2) * (stride / 2);
        pU = Uorg + (y2 / 2) * (stride / 2);

        for (int x = 0, x2 = 0, x6 = 0; x2 < yuvstep; x++, x2 += 2, x6 += 6)
        {
            Y00 = pYUV0[x2];
            Y01 = pYUV0[x2+1];
            Y10 = pYUV1[x2];
            Y11 = pYUV1[x2+1];

            Cr = pU[x];
            Cb = pV[x];

            ConvertYCrCbToRGB(Y00, Cr, Cb, R, G, B);

            pBGR0[x6] = B;
            pBGR0[x6 + 1] = G;
            pBGR0[x6 + 2] = R;

            ConvertYCrCbToRGB(Y01, Cr, Cb, R, G, B);

            pBGR0[x6 + 3] = B;
            pBGR0[x6 + 4] = G;
            pBGR0[x6 + 5] = R;

            ConvertYCrCbToRGB(Y10, Cr, Cb, R, G, B);

            pBGR1[x6] = B;
            pBGR1[x6 + 1] = G;
            pBGR1[x6 + 2] = R;

            ConvertYCrCbToRGB(Y11, Cr, Cb, R, G, B);

            pBGR1[x6 + 3] = B;
            pBGR1[x6 + 4] = G;
            pBGR1[x6 + 5] = R;
        }
    }
}

void convert_xbgr2bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR = NULL;
    uchar *pXBGR = NULL;

    int xbgrstep = w * 4;

    // XBGR to BGR
    for (int y = 0; y < h; y++)
    {
        pBGR = dst.data + y * dst.step;
        pXBGR = buff + y * stride;

        for (int x4 = 0, x3 = 0; x4 < xbgrstep; x4 += 4, x3 += 3)
        {
            pBGR[x3] = pXBGR[x4+0];
            pBGR[x3 + 1] = pXBGR[x4+1];
            pBGR[x3 + 2] = pXBGR[x4+2];
        }
    }
}

void convert_xrgb2bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR = NULL;
    uchar *pXRGB = NULL;

    int xbgrstep = w * 4;

    // XRGB to BGR
    for (int y = 0; y < h; y++)
    {
        pBGR = dst.data + y * dst.step;
        pXRGB = buff + y * stride;

        for (int x4 = 0, x3 = 0; x4 < xbgrstep; x4 += 4, x3 += 3)
        {
            pBGR[x3] = pXRGB[x4+0] << 3;
            pBGR[x3 + 1] = pXRGB[x4+1] << 2;
            pBGR[x3 + 2] = pXRGB[x4+2] << 3;
        }
    }
}

void convert_argb2bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR = NULL;
    uchar *pXRGB = NULL;

    int xbgrstep = w * 4;

    // ARGB to BGR
    for (int y = 0; y < h; y++)
    {
        pBGR = dst.data + y * dst.step;
        pXRGB = buff + y * stride;

        for (int x4 = 0, x3 = 0; x4 < xbgrstep; x4 += 4, x3 += 3)
        {
            pBGR[x3] = pXRGB[x4+0]; // B
            pBGR[x3 + 1] = pXRGB[x4+1]; // G
            pBGR[x3 + 2] = pXRGB[x4+2]; // R
        }
    }
}

void convert_bgra2bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pBGR = NULL;
    uchar *pBGRA = NULL;

    int xbgrstep = w * 4;

    for (int y = 0; y < h; y++)
    {
        pBGR = dst.data + y * dst.step;
        pBGRA = buff + y * stride;

        for (int x4 = 0, x3 = 0; x4 < xbgrstep; x4 += 4, x3 += 3)
        {
            pBGR[x3] = pBGRA[x4]; // B
            pBGR[x3 + 1] = pBGRA[x4+1]; // G
            pBGR[x3 + 2] = pBGRA[x4+2]; // R
        }
    }
}

void convert_bgr2bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pSrcBGR = NULL;
    uchar *pDstRGB = NULL;

    int srcstep = w * 3;

    // BGR to BGR
    for (int y = 0; y < h; y++)
    {
        pDstRGB = dst.data + y * dst.step;
        pSrcBGR = buff + y * stride;

        for (int x3 = 0; x3 < srcstep; x3 += 3)
        {
            pDstRGB[x3] = pSrcBGR[x3]; // B
            pDstRGB[x3 + 1] = pSrcBGR[x3+1]; // G
            pDstRGB[x3 + 2] = pSrcBGR[x3+2]; // R
        }
    }
}

void convert_rgb162bgr(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);

    uchar *pSrcBGR = NULL;
    uchar *pDstRGB = NULL;

    int srcstep = w * 2;

    // BGR to BGR
    for (int y = 0; y < h; y++)
    {
        pDstRGB = dst.data + y * dst.step;
        pSrcBGR = buff + y * stride;

        for (int x2 = 0, x3 = 0; x2 < srcstep; x2 += 2, x3 += 3)
        {
            pDstRGB[x3] = (pSrcBGR[x2] & 0x1F) << 3; // B
            pDstRGB[x3 + 1] = ((pSrcBGR[x2] >> 5) + (pSrcBGR[x2+1]&0x7) << 3) << 2; // G
            pDstRGB[x3 + 2] = (pSrcBGR[x2+1]&0xF8); // R
        }
    }
}
void convert_jpgtorgb(uchar *buff, int stride, int w, int h, Mat &dst)
{
    dst.create(Size(w, h), CV_8UC3);


}
bool Loadinputimage(const char *imgpath, int ncount, int w, int h, MEDIA_FORMAT format, vector<Mat> &img_array, bool Alignment)
{
    bool ret = true;
    uchar *pRawData = NULL;
    ULONG nDataLen = ReadRawData(imgpath, &pRawData);
    ULONG one_img_size = 0;
    int stride = 0;

    func_convert_format func = NULL;

    switch (format)
    {
    case MEDIA_FORMAT_NV12:
        one_img_size = w * h * 3 / 2;
        func = convert_nv122bgr;
        stride = w;
        break;
    case MEDIA_FORMAT_NV16:
        one_img_size = w * h * 2;
        func = convert_nv162bgr;
        stride = w;
        break;
    case MEDIA_FORMAT_UYVY:
        one_img_size = w * h * 2;
        func = convert_uyvy2bgr;
        stride = 2*w;
        break;
    case MEDIA_FORMAT_YUY2:
        one_img_size = w * h * 2;
        func = convert_yuy22bgr;
        stride = 2*w;
        break;
    case MEDIA_FORMAT_XRGB:
        one_img_size = w * h * 4;
        func = convert_xrgb2bgr;
        stride = 4*w;
        break;
    case MEDIA_FORMAT_XBGR:
        one_img_size = w * h * 4;
        func = convert_xbgr2bgr;
        stride = 4*w;
        break;
    // Vaapipostproc output format
    case MEDIA_FORMAT_I420:
        one_img_size = w * h * 3 / 2;
        func = convert_i4202bgr;
        stride = w;
        break;
    case MEDIA_FORMAT_ARGB:
        one_img_size = w * h * 4;
        func = convert_argb2bgr;
        stride = 4*w;
        break;
    case MEDIA_FORMAT_BGRA:
        one_img_size = w * h * 4;
        func = convert_bgra2bgr;
        stride = 4*w;
        break;
    case MEDIA_FORMAT_BGR24:
        one_img_size = w * h * 3;
        func = convert_bgr2bgr;
        stride = 3*w;
        break;
    case MEDIA_FORMAT_RGB565:
        one_img_size = w * h * 2;
        func = convert_rgb162bgr;
        stride = 2*w;
        break;
    case MEDIA_FORMAT_JPG:
    case MEDIA_FORMAT_BMP:
        one_img_size = nDataLen;
        break;
    default:
        LOG_ERROR("Unknown format", NULL);
        goto bail;
    }

    if (nDataLen < ncount * one_img_size)
    {
        LOG_ERROR("File size is too small", CStringFormat("actual(%d) vs expected(%d)", nDataLen, ncount * one_img_size).Get());
        ret = false;
        goto bail;
    }

    // ISP limitation, 64 bytes alignment for each line of data
    if( Alignment == true)
    {
         if (stride % 64 != 0)
         {
            stride += 64 - stride % 64;
         }
    }

    one_img_size = nDataLen / ncount;

    for (int i = 0; i < ncount; i++)
    {
        uchar *pdata = pRawData + i * one_img_size;
        Mat img;
        func(pdata, stride, w, h, img);
        img_array.push_back(img);
    }

bail:
    if (pRawData != NULL)
    {
        free(pRawData);
        pRawData = NULL;
    }
    return ret;
}

MEDIA_FORMAT GetFormatFromString(char *strRawDumpFormat)
{
    MEDIA_FORMAT RawDumpFormat = MEDIA_FORMAT_UNKNOWN;

    if (strcmp(strRawDumpFormat, "UYVY") == 0 || strcmp(strRawDumpFormat, "uyvy") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_UYVY;
    }
    else if (strcmp(strRawDumpFormat, "YUY2") == 0 || strcmp(strRawDumpFormat, "yuy2") == 0 || strcmp(strRawDumpFormat, "YUYV") == 0 || strcmp(strRawDumpFormat, "yuyv") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_YUY2;
    }
    else if (strcmp(strRawDumpFormat, "NV12") == 0 || strcmp(strRawDumpFormat, "nv12") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_NV12;
    }
    else if (strcmp(strRawDumpFormat, "NV16") == 0 || strcmp(strRawDumpFormat, "nv16") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_NV16;
    }
    else if (strcmp(strRawDumpFormat, "XRGB") == 0 || strcmp(strRawDumpFormat, "RGBx") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_XRGB;
    }
    else if (strcmp(strRawDumpFormat, "XBGR") == 0 || strcmp(strRawDumpFormat, "BGRx") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_XBGR;
    }
    else if (strcmp(strRawDumpFormat, "I420") == 0 || strcmp(strRawDumpFormat, "i420") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_I420;
    }
    else if (strcmp(strRawDumpFormat, "ARGB") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_ARGB;
    }
    else if (strcmp(strRawDumpFormat, "BGRA") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_BGRA;
    }
    else if (strcmp(strRawDumpFormat, "BGR") == 0 ||
             strcmp(strRawDumpFormat, "BGR888") == 0 ||
             strcmp(strRawDumpFormat, "BGR24") == 0)
    {
        RawDumpFormat = MEDIA_FORMAT_BGR24;
    }
    else if (strcmp(strRawDumpFormat, "RGB16") == 0 ||
             strcmp(strRawDumpFormat, "RGB565") == 0 )
    {
        RawDumpFormat = MEDIA_FORMAT_RGB565;
    }
    return RawDumpFormat;
}



int main(int argc, char *argv[])
{

    Init(argc,argv);
    RawDumpFormat = GetFormatFromString(argv[4]);
    if (RawDumpFormat == MEDIA_FORMAT_UNKNOWN)
    {
        LOG_ERROR("Unknown raw dump format!", NULL);
        return -1;
    }

    vector<Mat> RawDumpImages;
    if (!Loadinputimage(RawDumpPath, RawDumpFrameCount, ImageW, ImageH, RawDumpFormat, RawDumpImages, 0))
    {
        LOG_ERROR("Load raw dump file failed", NULL);
        return -1;
    }

    Mat ReferenceImage = imread(ReferenceImagePath);
    if (ReferenceImage.empty())
    {
        LOG_ERROR("Load reference image file failed", NULL);
        return -1;
    }

    if (ReferenceImage.cols != ImageW || ReferenceImage.rows != ImageH)
    {
        LOG_ERROR("Reference resolution does not match", NULL);
        return -1;
    }

    CContenAnalyzerInterface *analyzer = FactoryCreateAnalyzer(ANALYZER_SIMILARITY_COMPARE);
    if (!analyzer->LoadAlgorithmParameters(ConfigFilePath))
    {
        LOG_ERROR("Load analyzer config file failed", NULL);
        FactoryDestroyAnalyzer(&analyzer);
        return -1;
    }
    //workaround: since the first filed has known issue and if weaving is used, the second frame will use the first field.
    //so if the name of file contains weaving, we check the image from the third frame.

    for (int i = 0; i < RawDumpFrameCount; i++)
    {
        INPUT_IMAGE img1;
        INPUT_IMAGE img2;
        img1.img = RawDumpImages[i];
        img2.img = ReferenceImage;
        INPUT_IMAGE_ARRAY img_array;
        img_array.push_back(img1);
        img_array.push_back(img2);
        Mat img_debug;
        Mat img_reference;
        LOG_INFO(CStringFormat("Analyzing frame %d", i+1).Get(), NULL);
        ANALYZER_RETRUN analyzer_ret = analyzer->Analyze(img_array, img_debug, img_reference);
        if (ANALYZER_RETRUN_PASS != analyzer_ret)
        {
            LOG_ERROR("Analyzer result is failed", NULL);
            if (!img_reference.empty())
            {
                Saveoutputimage(".", CStringFormat("%s_%d_img_reference.jpg", RawDumpPath,i+1).Get(), img_reference);
            }
            Saveoutputimage(".", CStringFormat("%s_%d_raw_imge.bmp", RawDumpPath,i+1).Get(), img1.img);
            if (!img_debug.empty())
            {
                Saveoutputimage(".", CStringFormat("%s_%d_debug_imge.jpg", RawDumpPath,i+1).Get(), img_debug);
            }
            //We want to see more wrong images instead of just the first one. So below changes are made.
            //FactoryDestroyAnalyzer(&analyzer);
            //return -1;
            result += 1;
            break;
        }
        else
        {
            LOG_INFO("Analyzer result is passed", NULL);
        }
    }
    FactoryDestroyAnalyzer(&analyzer);
    if ( result != 0)
        LOG_INFO(CStringFormat("Total fail frames: %d, please check the dumped images.", result).Get(), NULL);
    return result;
}
