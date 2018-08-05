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


void FilterOutIsolatedBlocks(V_SSIM_BLOCK &vblocks, int kernel_size, int filter_w, int filter_h, int neighbor_count_th);

double calc_mse(Mat &src1, Mat &src2, int kernel_size, int xs, int ys, int xshift = 0, int yshift = 0)
{
    uchar *src1Data = NULL, *src2Data = NULL;
    int x, y;
    double MSE = 0, diff;
    assert(!src1.empty() && !src2.empty());
    assert(src1.channels() == 1 && src2.channels() == 1);
    assert(src1.cols == src2.cols && src1.rows == src2.rows);

    int WIN_SIZE = kernel_size;
    int WIN_SIZE_BY_SIZE = WIN_SIZE * WIN_SIZE;

    for (y = ys; y < ys + WIN_SIZE; y++)
    {
        src1Data = (uchar*)src1.data + (y + yshift) * src1.step;
        src2Data = (uchar*)src2.data + y * src2.step;

        for (x = xs; x< xs + WIN_SIZE; x++)
        {
            diff = (double)(src1Data[x + xshift]) - (double)(src2Data[x]);
            MSE += diff * diff;
        }
    }

    MSE /= WIN_SIZE_BY_SIZE;

    return (MSE);
}

SIMILARITYCHECK_API int calc_psnr(
    Mat &src1,
    Mat &src2,
    int ichannel,
    int kernel_size,
    double psnr_threshold,
    double &avg_psnr,
    double &min_pnsr,
    V_SSIM_BLOCK &artifact_blocks,
    bool ignore_boundary)
{
    int WIN_SIZE = kernel_size;

    assert(!src1.empty() && !src2.empty());
    assert(src1.channels() == 1 && src2.channels() == 1);
    assert(src1.cols == src2.cols && src1.rows == src2.rows);

    int h = src1.rows;
    int w = src1.cols;
    int new_w = 0;
    int new_h = 0;

    uchar *src1Data = NULL;
    uchar *src2Data = NULL;

    const double PSNR_CONST = 20 * log10(255.0);
    //bool retval = true;

    int x = 0;
    int y = 0;
    int xs = 0;
    int ys = 0;
    int xe = 0;
    int ye = 0;

    double psnr = 0;
    avg_psnr = 0;
    int nblocks = 0;

    int nFailBlocks = 0;

    SSIM_BLOCK block = {};
    block.ichannel = ichannel;
    block.kernel_size = WIN_SIZE;
    block.neighbor_count = 0;

    min_pnsr = 1000;

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

    // divide to N x N blocks, calculate PSNR in every block, and average them.
    for (y = ys; y < ye; y += WIN_SIZE)
    {
        src1Data = (uchar *)src1.data + y * src1.step;
        src2Data = (uchar *)src2.data + y * src2.step;

        for (x = xs; x < xe; x += WIN_SIZE)
        {
            double mse = calc_mse(src1, src2, kernel_size, x, y);
            if (mse < 1e-6)
            {
                mse = 1e-6;
            }
            psnr = (PSNR_CONST - 10 * log10(mse));

            avg_psnr += psnr;

            if (psnr < psnr_threshold)
            {
                nFailBlocks++;
                if (psnr < min_pnsr)
                {
                    min_pnsr = psnr;
                }
            }

            nblocks++;
        }

        if (x < new_w)
        {
            x = new_w - WIN_SIZE;
            double mse = calc_mse(src1, src2, kernel_size, x, y);
            if (mse < 1e-6)
            {
                mse = 1e-6;
            }
            psnr = (PSNR_CONST - 10 * log10(mse));

            avg_psnr += psnr;

            if (psnr < psnr_threshold)
            {
                nFailBlocks++;

                block.x = x;
                block.y = y;
                block.kernel_size = WIN_SIZE;
                block.ssim_val = psnr;
                artifact_blocks.push_back(block);

                if (psnr < min_pnsr)
                {
                    min_pnsr = psnr;
                }
            }

            nblocks++;

        }
    }

    if (y < new_h)
    {
        y = new_h - WIN_SIZE;

        src1Data = (uchar *)src1.data + y * src1.step;
        src2Data = (uchar *)src2.data + y * src2.step;

        for (x = 0; x<w - WIN_SIZE; x += WIN_SIZE)
        {
            double mse = calc_mse(src1, src2, kernel_size, x, y);
            if (mse < 1e-6)
            {
                mse = 1e-6;
            }
            psnr = (PSNR_CONST - 10 * log10(mse));

            avg_psnr += psnr;

            if (psnr < psnr_threshold)
            {
                nFailBlocks++;
                block.x = x;
                block.y = y;
                block.kernel_size = WIN_SIZE;
                block.ssim_val = psnr;
                artifact_blocks.push_back(block);

                if (psnr < min_pnsr)
                {
                    min_pnsr = psnr;
                }
            }

            nblocks++;
        }

        if (x < new_w)
        {
            x = new_w - WIN_SIZE;
            double mse = calc_mse(src1, src2, kernel_size, x, y);
            if (mse < 1e-6)
            {
                mse = 1e-6;
            }
            psnr = (PSNR_CONST - 10 * log10(mse));

            avg_psnr += psnr;

            if (psnr < psnr_threshold)
            {
                nFailBlocks++;
                block.x = x;
                block.y = y;
                block.kernel_size = WIN_SIZE;
                block.ssim_val = psnr;
                artifact_blocks.push_back(block);

                if (psnr < min_pnsr)
                {
                    min_pnsr = psnr;
                }
            }

            nblocks++;

        }
    }

    avg_psnr /= nblocks;

    return nFailBlocks;
}

SIMILARITYCHECK_API void CalculatePsnrCn3(
    Mat &img_ref,
    Mat &img_test,
    int kernel_size,
    double psnr_threshold,
    double &psnr_avg,
    double &psnr_min,
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

    double avg_psnr[3] = {};
    double min_psnr[3] = {};

    psnr_avg = 1;
    psnr_min = 1;

    for (int i = 0; i < 3; i++)
    {
        int nblocks = calc_psnr(img_test_cn[i], img_ref_cn[i], i, kernel_size, psnr_threshold,
            avg_psnr[i], min_psnr[i], artifact_blocks, ignore_boundary);

        if (psnr_avg >(float)avg_psnr[i])
            psnr_avg = (float)avg_psnr[i];

        if (nblocks > 0 && psnr_min > (float)min_psnr[i])
        {
            psnr_min = (float)min_psnr[i];
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
    for (V_SSIM_BLOCK::iterator it = artifact_blocks.begin(); it != artifact_blocks.end();)
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
                ssim_val = calc_mse(img_ref_cn[ich], img_test_cn[ich], kernel_size, xs, ys, x, y);
                if (ssim_val >= psnr_threshold)
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