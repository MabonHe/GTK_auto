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
#include "Defs.h"
#include "LibSimilarityCheck.h"


/******************************************************************************
* Function:         FilterOutIsolatedBlocks
* Description:      Filter out isolated blocks from the artifacts blocks array
* Parameters:       [in]vblocks       -  artifact blocks
* Parameters:       [in]kernel_size   -  SSIM/PSNR method kernel size
* Parameters:       [in]filter_w      -  isolated block in the (filter_w, filter_h) area will be filter out
* Parameters:       [in]filter_h      -  isolated block in the (filter_w, filter_h) area will be filter out
* Parameters:       [in]neighbor_count_th  -  if the neighbor count is less than neighbor_count_th, it will be filter out
*
* Return:           void
******************************************************************************/
void FilterOutIsolatedBlocks(
    V_SSIM_BLOCK &vblocks,
    int kernel_size,
    int filter_w,
    int filter_h,
    int neighbor_count_th);

double CalcBlockSsim(Mat &src1, Mat &src2, int kernel_size, int xs, int ys, int xshift = 0, int yshift = 0)
{
    int x = 0;
    int y = 0;

    uchar *data1 = NULL;
    uchar *data2 = NULL;

    double u1 = 0;
    double u2 = 0;

    double diff1 = 0;
    double diff2 = 0;
    double mse1  = 0;
    double mse2  = 0;
    double mse12 = 0;
    double ssim  = 0;

    int WIN_SIZE = kernel_size;
    int WIN_SIZE_BY_SIZE = WIN_SIZE * WIN_SIZE;
    int WIN_SIZE_BY_SIZE_M1 = WIN_SIZE_BY_SIZE - 1;

    assert(!src1.empty() && !src2.empty());
    assert(src1.channels() == 1 && src2.channels() == 1);
    assert(src1.cols == src2.cols && src1.rows == src2.rows);

    const double C1 = (0.01*255)*(0.01*255);
    const double C2 = (0.03*255)*(0.03*255);

    // calc average
    for (y = ys; y< ys + WIN_SIZE; y++)
    {
        data1 = (uchar *)src1.data + (y + yshift) * src1.step;
        data2 = (uchar *)src2.data + y * src2.step;
        for (x = xs; x < xs + WIN_SIZE; x++)
        {
            u1 += data1[x + xshift] & ~3;
            u2 += data2[x] & ~3;
        }
    }
    u1 /= WIN_SIZE_BY_SIZE;
    u2 /= WIN_SIZE_BY_SIZE;

    // calc mse
    for (y = ys; y < ys + WIN_SIZE; y++)
    {
        data1 = (uchar *)src1.data + (y + yshift) * src1.step;
        data2 = (uchar *)src2.data + y * src2.step;
        for (x = xs; x < xs + WIN_SIZE; x++)
        {
            diff1 = (data1[x + xshift] & ~3) - u1;
            diff2 = (data2[x] & ~3) - u2;
            mse1 += diff1 * diff1;
            mse2 += diff2 * diff2;
            mse12 += abs(diff1 * diff2);
        }
    }

    mse1  /= WIN_SIZE_BY_SIZE_M1;
    mse2  /= WIN_SIZE_BY_SIZE_M1;
    mse12 /= WIN_SIZE_BY_SIZE_M1;

    if (u1 + u2 < 50 && mse1 < 25*25 && mse2 < 25*25)
    {
        ssim = 0.7;
    }
    else
    {
        ssim = (2 * u1 * u2 + C1) * (2 * mse12 + C2) /
               ( (u1*u1 + u2*u2 + C1) * (mse1 + mse2 + C2) );
    }

    return ssim;
}

SIMILARITYCHECK_API int CalcSsim(
    Mat &src1,
    Mat &src2,
    int ichannel,
    int kernel_size,
    double similarity_threshold,
    double &ssim_avg,
    double &min_ssim,
    V_SSIM_BLOCK &artifact_blocks,
    bool ignore_boundary)
{
    uchar *src1Data = NULL;
    uchar *src2Data = NULL;

    int x = 0;
    int y = 0;
    int xs = 0;
    int ys = 0;
    int xe = 0;
    int ye = 0;

    int WIN_SIZE = kernel_size;

    assert(!src1.empty() && !src2.empty());
    assert(src1.channels() == 1 && src2.channels() == 1);
    assert(src1.cols == src2.cols && src1.rows == src2.rows);

    int w = src1.cols;
    int h = src1.rows;
    int new_w = 0;
    int new_h = 0;
    double ssim;
    int num = 0;
    ssim_avg = 0;
    min_ssim = 1;

    int nBlockFail = 0;

    SSIM_BLOCK block = {};

    block.ichannel = ichannel;
    block.kernel_size = WIN_SIZE;
    block.neighbor_count = 0;

    if (ignore_boundary)
    {
        xs = WIN_SIZE;
        ys = WIN_SIZE;
        xe = w - 2 * WIN_SIZE;
        ye = h - 2 * WIN_SIZE;
        new_w = w - WIN_SIZE;
        new_h = h - WIN_SIZE;
    }
    else
    {
        xs = 0;
        ys = 0;
        xe = w - WIN_SIZE;
        ye = h - WIN_SIZE;
        new_w = w;
        new_h = h;
    }

    // divide to WIN_SIZE x WIN_SIZE blocks, calculate SSIM in every block, and average them.
    for (y = ys; y < ye; y += WIN_SIZE)
    {
        src1Data = (uchar *)src1.data + y * src1.step;
        src2Data = (uchar *)src2.data + y * src2.step;

        for (x = xs; x < xe; x += WIN_SIZE)
        {
            ssim = CalcBlockSsim(src1, src2, WIN_SIZE, x, y);
            if (ssim < similarity_threshold)
            {
                nBlockFail++;

                block.x = x;
                block.y = y;

                block.ssim_val = ssim;

                artifact_blocks.push_back(block);

                if (ssim < min_ssim)
                {
                    min_ssim = ssim;
                }
            }
            ssim_avg += ssim;
            num++;
        }

        // last n<WIN_SIZE cols
        if (x < new_w)
        {
            x = new_w - WIN_SIZE;
            ssim = CalcBlockSsim(src1, src2, WIN_SIZE, x, y);
            if (ssim < similarity_threshold)
            {
                nBlockFail++;

                block.x = x;
                block.y = y;
                block.kernel_size = WIN_SIZE;
                block.ssim_val = ssim;

                artifact_blocks.push_back(block);

                if (ssim < min_ssim)
                {
                    min_ssim = ssim;
                }
            }
            ssim_avg += ssim;
            num++;
        }
    }

    // last m < N rows
    if (y < new_h)
    {
        y = new_h - WIN_SIZE;
        src1Data = (uchar*)src1.data + y * src1.step;
        src2Data = (uchar*)src2.data + y * src2.step;

        for (x=0; x<w-WIN_SIZE; x+=WIN_SIZE)
        {
            ssim = CalcBlockSsim(src1, src2, WIN_SIZE, x, y);
            if (ssim < similarity_threshold)
            {
                nBlockFail++;

                block.x = x;
                block.y = y;
                block.kernel_size = WIN_SIZE;
                block.ssim_val = ssim;

                artifact_blocks.push_back(block);

                if (ssim < min_ssim)
                {
                    min_ssim = ssim;
                }
            }
            ssim_avg += ssim;
            num++;
        }

        // last n<N cols
        if (x < new_w)
        {
            x = new_w - WIN_SIZE;
            ssim = CalcBlockSsim(src1, src2, WIN_SIZE, x, y);
            if (ssim < similarity_threshold)
            {
                nBlockFail++;

                block.x = x;
                block.y = y;
                block.kernel_size = WIN_SIZE;
                block.ssim_val = ssim;

                artifact_blocks.push_back(block);

                if (ssim < min_ssim)
                {
                    min_ssim = ssim;
                }
            }
            ssim_avg += ssim;
            num++;
        }
    }

    ssim_avg /= num;

    return nBlockFail;
}

SIMILARITYCHECK_API void CalculateSsimCn3(
    Mat &img_ref,
    Mat &img_test,
    int kernel_size,
    double similarity_threshold,
    double &ssim_avg,
    double &ssim_min,
    V_SSIM_BLOCK &artifact_blocks,
    int filter_w,
    int filter_h,
    int neighbor_count_th,
    bool ignore_boundary)
{
    assert(!img_ref.empty() && !img_test.empty()
        && img_ref.channels() == 3 && img_test.channels() == 3);

    Mat img_test_cn[3], img_ref_cn[3];

    split(img_test, img_test_cn);
    split(img_ref, img_ref_cn);

    double avg_ssim[3] = {};
    double min_ssim[3] = {};

    ssim_avg = 1;
    ssim_min = 1;

    for (int i = 0; i < 3; i++)
    {
        int nblocks = CalcSsim(img_test_cn[i], img_ref_cn[i], i, kernel_size, similarity_threshold,
            avg_ssim[i], min_ssim[i], artifact_blocks, ignore_boundary);

        if (ssim_avg >(float)avg_ssim[i])
            ssim_avg = (float) avg_ssim[i];

        if (nblocks > 0 && ssim_min > (float) min_ssim[i])
        {
            ssim_min = (float) min_ssim[i];
        }
    }

    if (filter_w > 0 && filter_h > 0 && neighbor_count_th > 0)
    {
        FilterOutIsolatedBlocks(artifact_blocks, kernel_size, filter_w, filter_h, neighbor_count_th);
    }

    int imgW = img_test.cols - kernel_size;
    int imgH = img_test.rows - kernel_size;
    int hksize = kernel_size / 2;

    // Check neighboring blocks similarity
    for (V_SSIM_BLOCK::iterator it = artifact_blocks.begin(); it != artifact_blocks.end(); )
    {
        int xs = it->x;
        int ys = it->y;
        int ich = it->ichannel;
        double ssim_val = it->ssim_val;

        int shiftxs = MAX(-hksize, -xs);
        int shiftxe = MIN(hksize + 1, imgW - xs);
        int shiftys = MAX(-hksize, -ys);
        int shiftye = MIN(hksize + 1, imgH - ys);

        for (int y = shiftys; y < shiftye; y++)
        {
            for (int x = shiftxs; x < shiftxe; x++)
            {
                ssim_val = CalcBlockSsim(img_ref_cn[ich], img_test_cn[ich], kernel_size, xs, ys, x, y);
                if (ssim_val >= similarity_threshold)
                {
                    it = artifact_blocks.erase(it);
                    goto CHECK_BLOCK_PASS;
                }
            }
        }

        it++;
    CHECK_BLOCK_PASS:;
    }
}

SIMILARITYCHECK_API void DrawArtifactBlocks(
    Mat src,
    V_SSIM_BLOCK &artifact_blocks)
{
    char str[10] = {};

    for (V_SSIM_BLOCK::iterator it = artifact_blocks.begin(); it != artifact_blocks.end(); it++)
    {
        Scalar color(it->ichannel == 0 ? 255 : 0, it->ichannel == 1 ? 255 : 0, it->ichannel == 2 ? 255 : 0, 0);

        rectangle(src, Rect(it->x, it->y, it->kernel_size, it->kernel_size), color, 1);

#ifdef _WINDOWS
        sprintf_s(str, 10, "%.2lf", it->ssim_val);
#else
        snprintf(str, 10, "%.2lf", it->ssim_val);
#endif

        putText(src, str, Point(it->x + 2, it->y + 9 + it->ichannel * 8), 0, 0.25, color);
    }
}
