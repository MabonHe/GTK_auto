/*
 * GStreamer
 * Copyright (C) 2015-2016 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gtest/gtest.h"
#include <sys/time.h>
#include <gst/gst.h>
#include <string.h>
#include <unistd.h>
#include <IspControl.h>
#include <ICamera.h>
#include <Parameters.h>
#include <AlgoTuning.h>
#include <gstcameraispinterface.h>

#define MAX_PLUGIN_SIZE 10
#define FPS_COUNT_START 20 //about 2s.
#define DEFAULT_TARGET_FPS 60 //60 fps.
#define ISP_CONTROL_TEST_NUM 5
#define LTM_TUNING_TEST_NUM 5


#define LTM_TUNING_DATA_TAG 1000

using namespace icamera;
typedef struct _CustomIspControl
{
	unsigned int type;
	void *data;
}CustomIspControl;

typedef struct _CustomData {
    float fps;
    gint buffer_num;
    gint buffer_num_isp;
    gint isp_control_index;
    gboolean bus_error;
    gboolean test_fps;
    gboolean check_isp_interface;
    timeval start_time;
    timeval last_time;
    CustomIspControl camisp_control;
    GstCamerasrcIsp *camisp;
    GstCamerasrcIspInterface *cameraisp_interface;
    GMainLoop *main_loop;  /* GLib's Main Loop */
} CustomData;

struct FrameInfo {
    float fps;
};

typedef struct _CheckField  {
    gboolean check_fps;
    gboolean check_isp_interface;

    struct FrameInfo info;
    CustomIspControl isp_control;
} CheckField;

struct Plugin {
    const char *plugin_name;
    const char *pro_attrs;
    const char *pro_attrs_test; //< add new filed for property to test
    const char *caps_filter;
};

// Property check types
enum PROPERTY_CHECK_TYPE
{
    PROPERTY_CHECK_TYPE_MAX,
    PROPERTY_CHECK_TYPE_MIN,
    PROPERTY_CHECK_TYPE_DEFAULT,
    PROPERTY_CHECK_TYPE_NEGATIVE,
    PROPERTY_CHECK_TYPE_ENUM,
    PROPERTY_CHECK_TYPE_BOOL,
    PROPERTY_CHECK_TYPE_STRING,
    PROPERTY_CHECK_TYPE_WB_GAIN,
    PROPERTY_CHECK_TYPE_COLOR_CORRECTION,
    PROPERTY_CHECK_TYPE_BXT_DEMOSAIC,
    PROPERTY_CHECK_TYPE_SC_IEFD,
    PROPERTY_CHECK_TYPE_SEE,
    PROPERTY_CHECK_TYPE_TNR5_21,
    PROPERTY_CHECK_TYPE_XNR_DSS,
    PROPERTY_CHECK_TYPE_GAMMA_TONE_MAP,
    PROPERTY_CHECK_TYPE_BNLM,
    PROPERTY_CHECK_TYPE_CSC,
    PROPERTY_CHECK_TYPE_LTM_TUNING
};

int do_pipline(CheckField *check, PROPERTY_CHECK_TYPE type);
void clear_plugins(void);
int add_plugins(const char *plugin_name, const char *pro_attrs,
    const char *pro_attrs_test, const char *caps_filter);
char *get_input_device_env(void);
bool get_checkfield_data(CheckField *check,PROPERTY_CHECK_TYPE type);
int  get_random_value(int min,int max);
