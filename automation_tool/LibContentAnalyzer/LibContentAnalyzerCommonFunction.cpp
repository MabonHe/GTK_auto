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

#include "LibContentAnalyzerCommonFunction.h"

void cvtRGB2HSV(uchar r, uchar g, uchar b, double &H, double &s, double &v)
{
    double vtable[3] = { r, g, b };
    double vmin;

    int imax = 0, imin = 0;
    v = vtable[0]; vmin = vtable[0];

    for (int i = 1; i < 3; i++)
    {
        if (vtable[i] > v)
        {
            imax = i;
            v = vtable[i];
        }
        if (vtable[i] < vmin)
        {
            imin = i;
            vmin = vtable[i];
        }
    }

    s = (v - vmin) / v;

    double delta = v - vmin;

    if (delta < 0.001)
    {
        H = 0;
        return;
    }

    switch (imax)
    {
    case 0:
        H = (vtable[1] - vtable[2]) / delta * 60;
        break;
    case 1:
        H = (vtable[2] - vtable[0]) / delta * 60 + 120;
        break;
    case 2:
        H = (vtable[0] - vtable[1]) / delta * 60 + 240;
        break;
    }

    if (H < 0)
        H += 360;
}

double CalcDeltaImage(Mat &img1, Mat &img2, Mat &delta)
{
    delta.create(img1.rows, img1.cols, CV_8UC3);
    uchar *data1 = NULL, *data2 = NULL, *datad = NULL;
    double error = 0;
    double size = img1.rows * img1.cols * 3;

    for (int y = 0; y < img1.rows; y++)
    {
        data1 = img1.data + y * img1.step;
        data2 = img2.data + y * img2.step;
        datad = delta.data + y * delta.step;

        for (int x = 0; x < img1.cols * 3; x++)
        {
            long delta = ::abs((long)data1[x] - (long)data2[x]);
            datad[x] = (uchar)delta;
            error += delta * delta / size;
        }
    }
    return error;
}