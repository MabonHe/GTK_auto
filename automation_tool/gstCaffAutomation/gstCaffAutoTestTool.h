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

#ifndef __GST_CAFF_AUTO_TEST_TOOL_H__
#define __GST_CAFF_AUTO_TEST_TOOL_H__

#include "gstCaffAutoTestConfig.h"
#include <gst/gst.h>
#include <gst/video/gstvideometa.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifndef BYTE
#define BYTE unsigned char
#endif

enum MEDIA_FORMAT
{
    MEDIA_FORMAT_UNKNOWN,
    MEDIA_FORMAT_NV12,
    MEDIA_FORMAT_UYVY,
    MEDIA_FORMAT_RGBA,
    MEDIA_FORMAT_BGRA,
    MEDIA_FORMAT_I420
};

typedef struct _S_RAW_FRAME
{
    unsigned char *data_ptr;
    int data_size;
    int stride;
    int w;
    int h;
    GstVideoFrameFlags flag;
    GstVideoFormat format;
} S_RAW_FRAME;

bool ConvertBufferToMat(S_RAW_FRAME &frame, Mat &dst);

bool CreateDirectory(char *szDirectoryPath, int iDirPermission=0777);

bool GetAppPath(char *path, int maxlen);

class CTimer
{
public:
    CTimer(){}

    void Start();
    double GetTimeDuration();

private:
    timeval m_time_start;
    timeval m_time_end;
};

#endif //__GST_CAFF_AUTO_TEST_SAMPLE_CALLBACK_H__