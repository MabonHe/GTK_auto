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

#include "LibSimilarityCheck.h"

void FilterOutIsolatedBlocks(V_SSIM_BLOCK &vblocks, int kernel_size, int filter_w, int filter_h, int neighbor_count_th)
{
    double wth = filter_w * kernel_size;
    double hth = filter_h * kernel_size;

    for (V_SSIM_BLOCK::iterator it = vblocks.begin(); it != vblocks.end(); it++)
    {
        it->neighbor_count = 0;
    }

    for (V_SSIM_BLOCK::iterator itx = vblocks.begin(); itx != vblocks.end(); itx++)
    {
        int xx = itx->x;
        int xy = itx->y;
        int xchanel = itx->ichannel;

        for (V_SSIM_BLOCK::iterator ity = itx + 1; ity != vblocks.end(); ity++)
        {
            if (xchanel != ity->ichannel)
            {
                break;
            }

            int dx = ity->x - xx;
            int dy = ity->y - xy;

            if ((dx < wth && dy < kernel_size) || (dx < kernel_size && dy < hth))
            {
                itx->neighbor_count++;
                ity->neighbor_count++;
            }
        }
    }

    for (V_SSIM_BLOCK::iterator it = vblocks.begin(); it != vblocks.end();)
    {
        if (it->neighbor_count < neighbor_count_th)
        {
            it = vblocks.erase(it);
        }
        else
        {
            it++;
        }
    }
}