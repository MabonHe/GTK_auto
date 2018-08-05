/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2016 Intel Corporation
 * All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */
#ifndef _COMMON_TYPE_H_
#define _COMMON_TYPE_H_

//#define MAX_CAMERA_NUM 4

namespace gstCaff {

enum CamMode {
    CAMERA_MODE_STOP = 0,
    CAMERA_MODE_PREVIEW,
    CAMERA_MODE_RECORDING,
    CAMERA_MODE_PLAYBACK,
    CAMERA_MODE_PREVIEW_PAUSED,
    CAMERA_MODE_RECORDING_PAUSED,
    CAMERA_MODE_PLAYBACK_PAUSED,
};

}
#endif //_COMMON_TYPE_H_

