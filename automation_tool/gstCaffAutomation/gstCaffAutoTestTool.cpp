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

#include "gstCaffAutoTestTool.h"

typedef void (*func_convert_format)(uchar *buff, int stride, int w, int h, Mat &dst);

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
        pCbCr = buff + (h + y) * stride;
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

    // YUY2 to BGR
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
            pBGR[x3] = pXRGB[x4+2];  
            pBGR[x3 + 1] = pXRGB[x4+1];  
            pBGR[x3 + 2] = pXRGB[x4+0];  
        }
    }
}

bool ConvertBufferToMat(S_RAW_FRAME &frame, Mat &dst)
{
    func_convert_format func = NULL;
    int one_img_size = 0;
    int stride = frame.stride;
    int h = frame.h;
    int w = frame.w;
    
    switch (frame.format)
    {
    case GST_VIDEO_FORMAT_NV12:
        one_img_size = stride * h * 3 / 2;
        func = convert_nv122bgr;
        break;
    case GST_VIDEO_FORMAT_NV16:
        one_img_size = stride * h * 2;
        func = convert_nv162bgr;
        break;
    case GST_VIDEO_FORMAT_YUY2:
        one_img_size = stride * h;
        func = convert_yuy22bgr;
        break;
    case GST_VIDEO_FORMAT_UYVY:
        one_img_size = stride * h;
        func = convert_uyvy2bgr;
        break;
    case GST_VIDEO_FORMAT_RGBx:
        one_img_size = stride * h;
        func = convert_xrgb2bgr;
        break;
    case GST_VIDEO_FORMAT_BGRx:
        one_img_size = stride * h;
        func = convert_xbgr2bgr;
        break;
    default:
        LOG_ERROR(CStringFormat("Unknown format: %d", frame.format).Get(), NULL);
        return false;
    }

    if (frame.data_size < one_img_size)
    {
        LOG_ERROR("File size is too small", CStringFormat("actual(%d) vs expected(%d)", frame.data_size, one_img_size).Get());
        return false;
    }

    func(frame.data_ptr, stride, w, h, dst);
       
    return true;
}

bool CreateDirectory(char *szDirectoryPath , int iDirPermission)
{
    if ( NULL == szDirectoryPath )
    {
        LOG_ERROR("Null path to create directory", NULL);
        return false;
    }

    int ret = access(szDirectoryPath, F_OK);

    if ( 0 == ret )
    {
        return true;
    }
    
    ret = mkdir(szDirectoryPath , iDirPermission);
    if ( 0 != ret )
    {
        LOG_ERROR("Failed to create directory", NULL);
        return false;
    }

    return true;
}

void CTimer::Start()
{
    gettimeofday(&m_time_start, NULL);
}

double CTimer::GetTimeDuration()
{
    gettimeofday(&m_time_end, NULL);
    double time_stamp = (double) ((m_time_end.tv_sec - m_time_start.tv_sec) * 1000 + 
                (m_time_end.tv_usec - m_time_start.tv_usec) / 1000.0);

    return time_stamp;
}


bool GetAppPath(char *path, int maxlen)
{  
    int count = readlink( "/proc/self/exe", path, maxlen); 
    
    if ( count < 0 || count >= maxlen ) 
    
    { 
        return false;
    } 
    
    path[count] = '\0'; 

    for (int i = count-1; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            path[i] = '\0';
            break;
        }
    }
    
    return true;
}


