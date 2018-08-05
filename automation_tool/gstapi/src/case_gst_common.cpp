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

#include "case_gst_common.h"
#include <gst/video/video.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


struct Plugin g_plugin[MAX_PLUGIN_SIZE];
ltm_tuning_data                            ltm_tuning_data_[LTM_TUNING_TEST_NUM];
struct camera_control_isp_wb_gains_t       isp_wb_gain_data[ISP_CONTROL_TEST_NUM]                    = {3.0,2.0,4.0,3.0,\
                                                                                                        2.0,3.0,3.0,4.0,\
                                                                                                        3.0,4.0,3.0,2.0,\
                                                                                                        9.0,1.0,3.0,5.0,\
                                                                                                        12.0,33.0,9.0,5.0 };

struct camera_control_isp_color_correction_matrix_t isp_color_correction_data[ISP_CONTROL_TEST_NUM]  = {0, 0, 1, 0, 1, 0, 1, 0, 0,\
                                                                                                        1, 0, 0, 0, 1, 0, 0, 0, 1,\
                                                                                                        0, 0, 1, 0, 1, 0, 1, 0, 0,\
                                                                                                        1, 0, 0, 0, 1, 0, 0, 0, 1,\
                                                                                                        0, 0, 1, 0, 1, 0, 1, 0, 0};


struct camera_control_isp_bxt_demosaic_t   isp_bxt_demosaic_data[ISP_CONTROL_TEST_NUM]               = {1, 1, 1, 17, 12, 0, 56,\
                                                                                                        1, 1, 1, 14, 56, 64, 1024,\
                                                                                                        1, 0, 1, 14, 56, 64, 1024,
                                                                                                        1, 1, 1, 16, 120, 1024,56,\
                                                                                                        1, 0, 1, 13, 80, 1200, 70};

struct camera_control_isp_see_t            isp_see_data[ISP_CONTROL_TEST_NUM]                        = {0, 14, 2, 200, 78, 21, 47,\
                                                                                                        0, 4, 0, 100, 20, 5, 10,\
                                                                                                        1, 20, 4, 250, 90, 30, 55,\
                                                                                                        1, 255, 159, 100, 70, 30, 7,\
                                                                                                        0, 6, 60, 90, 120, 80,220 };

struct camera_control_isp_sc_iefd_t        isp_sc_iefd_data[ISP_CONTROL_TEST_NUM]                    = {0};
struct camera_control_isp_bnlm_t           isp_bnlm_data[ISP_CONTROL_TEST_NUM]                       = {0};
struct camera_control_isp_tnr5_21_t        isp_tnr5_21_data[ISP_CONTROL_TEST_NUM]                    = {0};
struct camera_control_isp_xnr_dss_t        isp_xnr_dss_data[ISP_CONTROL_TEST_NUM]                    = {0};
struct camera_control_isp_gamma_tone_map_t isp_gama_tone_map_data[ISP_CONTROL_TEST_NUM]              = {0};
struct camera_control_isp_bxt_csc_t        isp_bxt_csc_data[ISP_CONTROL_TEST_NUM]                    = {3483,11718,1183,8192,-7441,-751,-1877,-6315,8192,\
                                                                                                        3483,11718,1183,-1877,-6153,8192,8192,-7441,-751,\
                                                                                                        3483,11718,1183,8192,-7441,-751,-1877,-6315,8192,\
                                                                                                        3483,11718,1183,-1877,-6153,8192,8192,-7441,-751,\
                                                                                                        3483,11718,1183,8192,-7441,-751,-1877,-6315,8192};

void clear_plugins(void) {

    memset(g_plugin, 0, (MAX_PLUGIN_SIZE * sizeof(struct Plugin)));
    return;
}

int add_plugins(
    const char *plugin_name,
    const char *pro_attrs,
    const char *pro_attrs_test,
    const char *caps_filter)
{
    int i;
    for (i = 0; i < MAX_PLUGIN_SIZE; i++) {
        if (!g_plugin[i].plugin_name) {
            g_plugin[i].plugin_name = plugin_name;
            g_plugin[i].pro_attrs = pro_attrs;
            g_plugin[i].pro_attrs_test = pro_attrs_test;
            g_plugin[i].caps_filter = caps_filter;
            break;
        }
    }
    if (i == MAX_PLUGIN_SIZE) {
        g_print("the plugin queue is full, add failed\n");
        return -1;
    }

    return 0;
}

char *get_input_device_env(void)
{
    char *Input_device = NULL;
    Input_device = getenv("cameraInput");
    if (Input_device == NULL) {
      g_print("ERR: cameraInput is not set,unable to specify property:'device-name'!\n");
      Input_device="";
    }
    return Input_device;
}
static int get_enum_value_from_field(GParamSpec * prop, const char *nick)
{
    int j = 0;
    GEnumValue *values;
    gboolean find_value = false;

    values = G_ENUM_CLASS (g_type_class_ref (prop->value_type))->values;
    while (values[j].value_name) {
        if (strcmp(values[j].value_nick, nick) == 0) {
            find_value = true;
            break;
        }
        j++;
    }
    if (!find_value) {
        g_print("donesn't find the enum nick: %s", nick);
        return -1;
    }

    return values[j].value;
}

static void set_elements_properties(GstElement *element, const char * properties)
{
    char *token;
    char pro_str[1024];
    GObjectClass *oclass;
    GParamSpec *prop;

    strncpy(pro_str, properties, 1024);
    token = strtok( (char*)pro_str, ", ");
    while (token != NULL) {
        char *ptr = strstr(token, "=");
        char *str_value = ptr + 1;
        char *property = token;
        *ptr = '\0';

        g_print("Try to set property %s: %s\n", property, str_value);

        oclass = G_OBJECT_GET_CLASS (element);
        prop = g_object_class_find_property (oclass, property);
        EXPECT_NE(prop, (GParamSpec *)NULL) << "Property " << property << " is not supported";
        if (!prop) {
            //g_print("there isn't the property: %s in element\n", property);
            token = strtok(NULL, ", ");
            continue;
        }
        switch (prop->value_type) {
            gint value_int, value_int_out, value_enum, value_enum_out;
            gboolean value_bool, value_bool_out;
            gchar *str_value_out;
            gfloat value_float, value_float_out;
            case G_TYPE_INT:
                value_int = atoi(str_value);
                g_object_set(G_OBJECT(element), property, value_int, NULL);
                g_print("Set %s (int): %d\n", property, value_int);
                g_object_get(G_OBJECT(element), property, &value_int_out, NULL);
                g_print("Get %s (int): %d\n", property, value_int_out);
                EXPECT_EQ(value_int, value_int_out) << "Property set/get doesn't match";
                break;
            case G_TYPE_FLOAT:
                value_float = atof(str_value);
                g_object_set(G_OBJECT(element), property, (float)value_float, NULL);
                g_print("Set %s (float): %f\n", property, value_float);
                g_object_get(G_OBJECT(element), property, &value_float_out, NULL);
                g_print("Get %s (float): %f\n", property, value_float_out);
                EXPECT_EQ(value_float, value_float_out) << "Property set/get doesn't match";
                break;
            case G_TYPE_BOOLEAN:
                value_bool = ((strcmp(str_value, "true") == 0) ? true : false);
                g_object_set(G_OBJECT(element), property, value_bool, NULL);
                g_print("Set %s (bool): %s\n", property, value_bool?"true":"false");
                g_object_get(G_OBJECT(element), property, &value_bool_out, NULL);
                g_print("Get %s (bool): %s\n", property, value_bool_out?"true":"false");
                EXPECT_EQ(value_int, value_int_out) << "Property set/get doesn't match";
                break;
             case G_TYPE_STRING:
                g_object_set(G_OBJECT(element), property, str_value, NULL);
                g_print("Set %s (string): %s\n", property, str_value);
                g_object_get(G_OBJECT(element), property, &str_value_out, NULL);
                g_print("Get %s (string): %s\n", property, str_value_out);
                if (strcmp(str_value, str_value_out) != 0)
                {
                    ASSERT_TRUE(false) << "Property set/get doesn't match";;
                }
                break;
             default:
                if (G_TYPE_IS_ENUM(prop->value_type)) {
                    value_enum = get_enum_value_from_field(prop, str_value);
                    g_object_set(G_OBJECT(element), property, value_enum, NULL);
                    g_print("Set %s (enum): %d\n", property, value_enum);
                    g_object_get(G_OBJECT(element), property, &value_enum_out, NULL);
                    g_print("Get %s (enum): %d\n", property, value_enum_out);
                    EXPECT_EQ(value_enum, value_enum_out) << "Property set/get doesn't match";
                } else
                    g_print("ERR, this type isn't supported\n");
                break;
        }

        token = strtok(NULL, ", ");
    }
}

static bool validate_elements_properties(
    GstElement *element,
    const char * properties,
    PROPERTY_CHECK_TYPE type)
{
    char *token;
    char pro_str[1024];
    GObjectClass *oclass;
    GParamSpec *prop;
    gchar *str_value_out = NULL;

    strncpy(pro_str, properties, 1024);
    token = strtok( (char*)pro_str, ", ");
    while (token != NULL) {
        char *ptr = strstr(token, "=");
        char *str_value = ptr + 1;
        char *property = token;
        *ptr = '\0';

        g_print("Try to set property %s: %s\n", property, str_value);

        oclass = G_OBJECT_GET_CLASS (element);
        prop = g_object_class_find_property (oclass, property);
        EXPECT_NE(prop, (GParamSpec *)NULL) << "Property " << property << " is not supported";
        if (!prop) {
            token = strtok(NULL, ", ");
            continue;
        }
        switch (prop->value_type) {
            gint value_int, value_int_out, value_enum, value_enum_out;
            gfloat value_float, value_float_out;
            gboolean value_bool, value_bool_out;
            case G_TYPE_INT:
                value_int = atoi(str_value);
                switch (type)
                {
                case PROPERTY_CHECK_TYPE_MAX:
                case PROPERTY_CHECK_TYPE_MIN:
                    {
                        /*
                        GValue gval_int;
                        g_value_init(&gval_int, G_TYPE_INT);
                        g_value_set_int(&gval_int, value_int);

                        if (!g_param_value_validate(prop, &gval_int))
                        {
                            g_print("Validate property(%s) value(%s) failed!\n",
                                property, str_value);
                            return false;
                        }
                        */
                        g_object_set(G_OBJECT(element), property, value_int, NULL);
                        g_print("Set %s (int): %d\n", property, value_int);
                        g_object_get(G_OBJECT(element), property, &value_int_out, NULL);
                        g_print("Get %s (int): %d\n", property, value_int_out);

                        if (value_int != value_int_out)
                        {
                            g_print("Property set/get doesn't match\n");
                            return false;
                        }
                    }
                    break;
                case PROPERTY_CHECK_TYPE_DEFAULT:
                    {
                        const GValue *pgval_int = g_param_spec_get_default_value(prop);
                        int val_int_default = g_value_get_int(pgval_int);
                        if (val_int_default != value_int)
                        {
                            g_print("Default value is %d, not as expected (%d)\n",
                                val_int_default, value_int);
                            return false;
                        }

                        g_object_get(G_OBJECT(element), property, &value_int_out, NULL);
                        g_print("Get default %s (int): %d\n", property, value_int_out);

                        if (value_int != value_int_out)
                        {
                            g_print("Default value got is %d, not as expected (%d)\n",
                                value_int_out, value_int);
                            return false;
                        }
                    }
                    break;
                case PROPERTY_CHECK_TYPE_NEGATIVE:
                    {
                        GValue gval_int;
                        g_value_init(&gval_int, G_TYPE_INT);
                        g_value_set_int(&gval_int, value_int);

                        if (g_param_value_validate(prop, &gval_int))
                        {
                            g_print("Validate property(%s) value(%s) passed while it should be failed!\n",
                                property, str_value);
                            return false;
                        }

                        g_object_set(G_OBJECT(element), property, value_int, NULL);
                        g_print("Set %s (int): %d\n", property, value_int);
                        g_object_get(G_OBJECT(element), property, &value_int_out, NULL);
                        g_print("Get %s (int): %d\n", property, value_int_out);

                        if (value_int == value_int_out)
                        {
                            g_print("Property set/get matches each other\n");
                            return false;
                        }
                        /*
                        const GValue *pgval_int = g_param_spec_get_default_value(prop);
                        int val_int_default = g_value_get_int(pgval_int);

                        if (val_int_default != value_int_out)
                        {
                            g_print("In negative case, the property value (%d) got"
                                " from the element should be the default one (%d)\n",
                                value_int_out, val_int_default);
                            return false;
                        }
                        */
                    }
                    break;
                default:
                    g_print("Property check type is not correct\n");
                    return false;
                }
                break;
            case G_TYPE_FLOAT:
                value_float = atof(str_value);
                switch (type)
                {
                case PROPERTY_CHECK_TYPE_MAX:
                case PROPERTY_CHECK_TYPE_MIN:
                    {
                        /*
                        GValue gval_int;
                        g_value_init(&gval_int, G_TYPE_INT);
                        g_value_set_int(&gval_int, value_int);

                        if (!g_param_value_validate(prop, &gval_int))
                        {
                            g_print("Validate property(%s) value(%s) failed!\n",
                                property, str_value);
                            return false;
                        }
                        */
                        g_object_set(G_OBJECT(element), property, value_float, NULL);
                        g_print("Set %s (float): %f\n", property, value_float);
                        g_object_get(G_OBJECT(element), property, &value_float_out, NULL);
                        g_print("Get %s (float): %f\n", property, value_float_out);

                        if (value_float != value_float_out)
                        {
                            g_print("Property set/get doesn't match\n");
                            return false;
                        }
                    }
                    break;
                case PROPERTY_CHECK_TYPE_DEFAULT:
                    {
                        const GValue *pgval_float = g_param_spec_get_default_value(prop);
                        float val_float_default = g_value_get_float(pgval_float);
                        if (val_float_default != value_float)
                        {
                            g_print("Default value is %f, not as expected (%f)\n",
                                val_float_default, value_float);
                            return false;
                        }

                        g_object_get(G_OBJECT(element), property, &value_float_out, NULL);
                        g_print("Get default %s (float): %f\n", property, value_float_out);

                        if (value_float != value_float_out)
                        {
                            g_print("Default value got is %f, not as expected (%f)\n",
                                value_float_out, value_float);
                            return false;
                        }
                    }
                    break;
                case PROPERTY_CHECK_TYPE_NEGATIVE:
                    {
                        GValue gval_float;
                        g_value_init(&gval_float, G_TYPE_FLOAT);
                        g_value_set_float(&gval_float, value_float);

                        if (g_param_value_validate(prop, &gval_float))
                        {
                            g_print("Validate property(%s) value(%s) passed while it should be failed!\n",
                                property, str_value);
                            return false;
                        }

                        g_object_set(G_OBJECT(element), property, value_float, NULL);
                        g_print("Set %s (float): %f\n", property, value_float);
                        g_object_get(G_OBJECT(element), property, &value_float_out, NULL);
                        g_print("Get %s (float): %f\n", property, value_float_out);

                        if (value_float == value_float_out)
                        {
                            g_print("Property set/get matches each other\n");
                            return false;
                        }
                        /*
                        const GValue *pgval_float = g_param_spec_get_default_value(prop);
                        float val_float_default = g_value_get_float(pgval_float);

                        if (val_float_default != value_float_out)
                        {
                            g_print("In negative case, the property value (%f) got"
                                " from the element should be the default one (%f)\n",
                                value_float_out, val_float_default);
                            return false;
                        }
                        */
                    }
                    break;
                default:
                    g_print("Property check type is not correct\n");
                    return false;
                }
                break;
            case G_TYPE_BOOLEAN:
                value_bool = ((strcmp(str_value, "true") == 0) ? true : false);

                switch (type)
                {
                case PROPERTY_CHECK_TYPE_BOOL:
                    {
                        g_object_set(G_OBJECT(element), property, value_bool, NULL);
                        g_print("Set %s (bool): %s\n", property, value_bool?"true":"false");
                        g_object_get(G_OBJECT(element), property, &value_bool_out, NULL);
                        g_print("Get %s (bool): %s\n", property, value_bool_out?"true":"false");

                        if ((!value_bool && value_bool_out) || (value_bool && !value_bool_out))
                        {
                            g_print("Property set/get doesn't match");
                            return false;
                        }
                    }
                    break;
                case PROPERTY_CHECK_TYPE_DEFAULT:
                    {
                        const GValue *pgval_bool = g_param_spec_get_default_value(prop);
                        gboolean val_bool_default = g_value_get_boolean(pgval_bool);

                        if ((!value_bool && val_bool_default) || (value_bool && !val_bool_default))
                        {
                            g_print("Property(%s) default value (%s) is not the same as expected(%s)\n",
                                property, val_bool_default?"true":"false", value_bool?"true":"false");
                            return false;
                        }

                        g_object_get(G_OBJECT(element), property, &value_bool_out, NULL);
                        g_print("Get default %s (bool): %s\n", property, value_bool_out?"true":"false");

                        if ((!value_bool && value_bool_out) || (value_bool && !value_bool_out))
                        {
                            g_print("Property(%s) default value (%s) got is not the same as expected(%s)\n",
                                property, value_bool_out?"true":"false", value_bool?"true":"false");
                            return false;
                        }
                    }
                    break;
                default:
                    g_print("Property check type is not correct\n");
                    return false;
                }
                break;
             case G_TYPE_STRING:
                switch (type)
                {
                case PROPERTY_CHECK_TYPE_STRING:
                    {
                        g_object_set(G_OBJECT(element), property, str_value, NULL);
                        g_print("Set %s (string): %s\n", property, str_value);
                        g_object_get(G_OBJECT(element), property, &str_value_out, NULL);
                        g_print("Get %s (string): %s\n", property, str_value_out);
                        if (strcmp(str_value, str_value_out) != 0)
                        {
                            g_print("Property set/get doesn't match\n");
                            return false;
                        }
                    }
                    break;
                case PROPERTY_CHECK_TYPE_DEFAULT:
                    {
                        const GValue *pgval_string = g_param_spec_get_default_value(prop);
                        const gchar *val_string_default = g_value_get_string(pgval_string);
                        if (val_string_default == NULL)
                        {
                            if (strcmp(str_value, "null") != 0)
                            {
                                g_print("Property(%s) defalut value is null, doesn't match expected (%s)\n",
                                    property, str_value);
                                return false;
                            }
                        }
                        else if (strcmp(str_value, val_string_default) != 0)
                        {
                            g_print("Property(%s) defalut value is %s, doesn't match expected (%s)\n",
                                property, val_string_default, str_value);
                            return false;
                        }
                        g_object_get(G_OBJECT(element), property, &str_value_out, NULL);
                        g_print("Get %s (string): %s\n", property, str_value_out);

                        if (str_value_out == NULL)
                        {
                            if (strcmp(str_value, "null") != 0)
                            {
                                g_print("Property(%s) defalut value is null, doesn't match expected (%s)\n",
                                    property, str_value);
                                return false;
                            }
                        }
                        else if (strcmp(str_value, str_value_out) != 0)
                        {
                            g_print("Property(%s) defalut value got is %s, doesn't match expected (%s)\n",
                                property, val_string_default, str_value);
                            return false;
                        }
                    }
                    break;
                default:
                    g_print("Property check type is not correct\n");
                    return false;
                }
                break;
             default:
                if (G_TYPE_IS_ENUM(prop->value_type))
                {
                    switch (type)
                    {
                    case PROPERTY_CHECK_TYPE_ENUM:
                        {
                            value_enum = get_enum_value_from_field(prop, str_value);
                            if (value_enum < 0)
                            {
                                g_print("Property(%s) value(%s) is not in the enum list.\n",
                                    property, str_value);
                                return false;
                            }
                            g_object_set(G_OBJECT(element), property, value_enum, NULL);
                            g_print("Set %s (enum): %d\n", property, value_enum);
                            g_object_get(G_OBJECT(element), property, &value_enum_out, NULL);
                            g_print("Get %s (enum): %d\n", property, value_enum_out);

                            if (value_enum != value_enum_out)
                            {
                                g_print("Property set/get doesn't match.\n");
                                return false;
                            }
                        }
                        break;
                    case PROPERTY_CHECK_TYPE_DEFAULT:
                        {
                            value_enum = get_enum_value_from_field(prop, str_value);
                            if (value_enum < 0)
                            {
                                g_print("Property(%s) value(%s) is not in the enum list.\n",
                                    property, str_value);
                                return false;
                            }

                            const GValue *pgval_enum = g_param_spec_get_default_value(prop);
                            gint val_enum_default = g_value_get_enum(pgval_enum);

                            if (val_enum_default != value_enum)
                            {
                                g_print("Property(%s) defalut value is %d, doesn't match expected (%d)\n",
                                    property, val_enum_default, value_enum);
                                return false;
                            }

                            g_object_get(G_OBJECT(element), property, &value_enum_out, NULL);
                            g_print("Get %s (enum): %d\n", property, value_enum_out);

                            if (value_enum_out != value_enum)
                            {
                                g_print("Property(%s) defalut value got is %d, doesn't match expected (%d)\n",
                                    property, value_enum_out, value_enum);
                                return false;
                            }
                        }
                        break;
                    case PROPERTY_CHECK_TYPE_NEGATIVE:
                        {
                            value_enum = get_enum_value_from_field(prop, str_value);
                            if (value_enum >= 0)
                            {
                                g_print("Property(%s) value(%s) is in the enum list while it is negative case.\n",
                                    property, str_value);
                                return false;
                            }

                            g_object_set(G_OBJECT(element), property, value_enum, NULL);
                            g_print("Set %s (enum): %d\n", property, value_enum);
                            g_object_get(G_OBJECT(element), property, &value_enum_out, NULL);
                            g_print("Get %s (enum): %d\n", property, value_enum_out);

                            if (value_enum == value_enum_out)
                            {
                                g_print("Property set/get should not match in negative case.\n");
                                return false;
                            }

                            const GValue *pgval_enum = g_param_spec_get_default_value(prop);
                            gint val_enum_default = g_value_get_enum(pgval_enum);

                            if (value_enum_out != val_enum_default)
                            {
                                g_print("Property(%s) defalut value got is %d, doesn't match expected (%d)\n",
                                    property, value_enum_out, val_enum_default);
                                return false;
                            }
                        }
                        break;
                    default:
                        break;
                    }

                }
                else
                {
                    g_print("ERR, this type isn't supported\n");
                    return false;
                }
                break;
        }

        token = strtok(NULL, ", ");
    }
    return true;
}


//calculate current fps
static GstPadProbeReturn callback_have_data(GstPad *padsrc, GstPadProbeInfo *info, gpointer user_data)
{
    double duration;
    float current_fps = 1.0;
    struct timeval current_time;
    CustomData * data = (CustomData *)user_data;

    data->buffer_num++;
    gettimeofday(&current_time, NULL);
    duration = (double)((current_time.tv_sec - data->last_time.tv_sec) * 1000000 +
            (current_time.tv_usec - data->last_time.tv_usec));
    current_fps = (float)(1000000 / duration);

    if (data->buffer_num == FPS_COUNT_START)
        gettimeofday(&(data->start_time), NULL);
    gettimeofday(&(data->last_time), NULL);

    return GST_PAD_PROBE_PASS;
}

static gboolean bus_call(GstBus *bus,GstMessage *msg, gpointer user_data)
{
    CustomData *data = (CustomData *)user_data;

    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(data->main_loop);
            if (data->test_fps && (data->buffer_num > FPS_COUNT_START)) {
                //calculate average fps
                double div = (double)((data->last_time.tv_sec - data->start_time.tv_sec) * 1000000 +
                                                    (data->last_time.tv_usec - data->start_time.tv_usec));
                data->fps = (float) ((data->buffer_num - FPS_COUNT_START)/(div/1000000));
            }
            break;
        case GST_MESSAGE_ERROR:
            {
                gchar *debug;
                GError *error;
                gst_message_parse_error(msg,&error,&debug);
                g_free(debug);
                g_printerr("ERROR:%s\n",error->message);
                g_error_free(error);
                g_main_loop_quit(data->main_loop);
                data->bus_error = true;
                break;
            }
        default:
            break;
    }
    return TRUE;

}

void print_ltm_tuning_data( ltm_tuning_data*data)
{
    g_print("algo_mode                                 :%d\n",data->algo_mode                                 );
    g_print("optibright_tuning.GTM_Str                 :%d\n",data->optibright_tuning.GTM_Str                 );
    g_print("optibright_tuning.GF_epspar               :%d\n",data->optibright_tuning.GF_epspar               );
    g_print("optibright_tuning.alpham1                 :%d\n",data->optibright_tuning.alpham1                 );
    g_print("optibright_tuning.alpham                  :%d\n",data->optibright_tuning.alpham                  );
    g_print("optibright_tuning.maskmin                 :%d\n",data->optibright_tuning.maskmin                 );
    g_print("optibright_tuning.maskmax                 :%d\n",data->optibright_tuning.maskmax                 );
    g_print("optibright_tuning.num_iteration           :%d\n",data->optibright_tuning.num_iteration           );
    g_print("optibright_tuning.maskmid                 :%d\n",data->optibright_tuning.maskmid                 );
    g_print("optibright_tuning.hlc_mode                :%d\n",data->optibright_tuning.hlc_mode                );
    g_print("optibright_tuning.max_isp_gain            :%d\n",data->optibright_tuning.max_isp_gain            );
    g_print("optibright_tuning.convergence_speed       :%d\n",data->optibright_tuning.convergence_speed       );
    g_print("optibright_tuning.lm_treatment            :%d\n",data->optibright_tuning.lm_treatment            );
    g_print("optibright_tuning.GTM_mode                :%d\n",data->optibright_tuning.GTM_mode                );
    g_print("optibright_tuning.pre_gamma               :%d\n",data->optibright_tuning.pre_gamma               );
    g_print("optibright_tuning.lav2p_scale             :%d\n",data->optibright_tuning.lav2p_scale             );
    g_print("optibright_tuning.p_max                   :%d\n",data->optibright_tuning.p_max                   );
    g_print("optibright_tuning.p_mode                  :%d\n",data->optibright_tuning.p_mode                  );
    g_print("optibright_tuning.p_value                 :%d\n",data->optibright_tuning.p_value                 );
    g_print("optibright_tuning.filter_size             :%d\n",data->optibright_tuning.filter_size             );
    g_print("optibright_tuning.max_percentile          :%d\n",data->optibright_tuning.max_percentile          );
    g_print("optibright_tuning.ldr_brightness          :%d\n",data->optibright_tuning.ldr_brightness          );
    g_print("optibright_tuning.dr_mid                  :%d\n",data->optibright_tuning.dr_mid                  );
    g_print("optibright_tuning.dr_norm_max             :%d\n",data->optibright_tuning.dr_norm_max             );
    g_print("optibright_tuning.dr_norm_min             :%d\n",data->optibright_tuning.dr_norm_min             );
    g_print("optibright_tuning.convergence_speed_slow  :%d\n",data->optibright_tuning.convergence_speed_slow  );
    g_print("optibright_tuning.convergence_sigma       :%d\n",data->optibright_tuning.convergence_sigma       );
    g_print("optibright_tuning.gainext_mode            :%d\n",data->optibright_tuning.gainext_mode            );
    g_print("optibright_tuning.wdr_scale_max           :%d\n",data->optibright_tuning.wdr_scale_max           );
    g_print("optibright_tuning.wdr_scale_min           :%d\n",data->optibright_tuning.wdr_scale_min           );
    g_print("optibright_tuning.wdr_gain_max            :%d\n",data->optibright_tuning.wdr_gain_max            );
    g_print("optibright_tuning.frame_delay_compensation:%d\n",data->optibright_tuning.frame_delay_compensation);
    g_print("mpgc_tuning.lm_stability                  :%d\n",data->mpgc_tuning.lm_stability                  );
    g_print("mpgc_tuning.lm_sensitivity                :%d\n",data->mpgc_tuning.lm_sensitivity                );
    g_print("mpgc_tuning.blur_size                     :%d\n",data->mpgc_tuning.blur_size                     );
    g_print("mpgc_tuning.tf_str                        :%d\n",data->mpgc_tuning.tf_str                        );
    g_print("drcsw_tuning.blus_sim_sigma               :%d\n",data->drcsw_tuning.blus_sim_sigma               );
}
void print_sc_iefd_t(struct camera_control_isp_sc_iefd_t *data)
{

    for(int j = 0; j < 2; j++)
    {
        g_print("sharpening_power[%d]     %d\t" ,j,data->sharpening_power[j]);

    }
    g_print("\n");
    for(int j = 0; j < 2; j++)
    {

        g_print("cu_ed2_metric_th[%d]     %d\t" ,j,data->cu_ed2_metric_th[j]);
     }

    g_print("\n");
    for(int j = 0; j < 2; j++)
    {


        g_print("cu_nr_power[%d]          %d\t" ,j,data->cu_nr_power[j]);
    }
    g_print("\n");
    for(int j = 0; j < 2; j++)
    {

        g_print("dir_far_sharp_weight[%d] %d\t" ,j,data->dir_far_sharp_weight[j]);
    }
    g_print("\n");
    g_print("sharp_power_edge     %d\n",data->sharp_power_edge     );
    g_print("sharp_power_detail   %d\n",data->sharp_power_detail   );
    g_print("unsharp_weight_edge  %d\n",data->unsharp_weight_edge  );
    g_print("unsharp_weight_detail%d\n",data->unsharp_weight_detail);
    g_print("denoise_power        %d\n",data->denoise_power        );
    g_print("radial_denoise_power %d\n",data->radial_denoise_power );
    g_print("shrpn_nega_lmt_txt   %d\n",data->shrpn_nega_lmt_txt   );
    g_print("shrpn_posi_lmt_txt   %d\n",data->shrpn_posi_lmt_txt   );
    g_print("shrpn_nega_lmt_dir   %d\n",data->shrpn_nega_lmt_dir   );
    g_print("shrpn_posi_lmt_dir   %d\n",data->shrpn_posi_lmt_dir   );
    for(int j = 0; j < 4; j++)
    {
        g_print("cu_var_metric_th[j] %d\t",data->cu_var_metric_th[j]);
    }
    g_print("\n");
    g_print("data->rad_enable%d\n", data->rad_enable);
    g_print("unsharp_filter[15]:");
    for(int j = 0; j < 15; j++)
    {
        g_print("%d\t",data->unsharp_filter[j]);
    }
    g_print("\n configunited_x:");
    for(int j = 0; j < 6; j++)
    {
        g_print("%d\t",data->configunited_x[j]);

    }
    g_print("\n denoise_filter:");
     for(int j = 0; j < 6; j++)
    {
        g_print("%d\t",data->denoise_filter[j]);

    }
    g_print("\n configunitradial_y:");
    for(int j = 0; j < 6; j++)
    {

        g_print("%d\t"  ,data->configunitradial_y[j]);

    }
    g_print("\n configunitradial_x:");
    for(int j = 0; j < 6; j++)
    {

        g_print("%d\t"  ,data->configunitradial_x[j]);
    }
    g_print("\n configunited_y:");
     for(int j = 0; j < 6; j++)
    {

        g_print("%d\t",data->configunited_y[j]);
    }
    g_print("\n");
    g_print("vssnlm_x0%d\n"  ,data->vssnlm_x0);
    g_print("vssnlm_x1%d\n"  ,data->vssnlm_x1);
    g_print("vssnlm_x2%d\n"  ,data->vssnlm_x2);
    g_print("vssnlm_y1%d\n"  ,data->vssnlm_y1);
    g_print("vssnlm_y2%d\n"  ,data->vssnlm_y2);
    g_print("vssnlm_y3%d\n"  ,data->vssnlm_y3);

}
void print_tnr5_21_t(struct camera_control_isp_tnr5_21_t *data)
{

        g_print("bypass %d\n",data->bypass);
        g_print("nm_yy_xcu_b:");
        for(int j = 0;j < 64; j++)
        {
            g_print("%d\t",data->nm_yy_xcu_b[j]);

        }
        g_print("\n nm_yc_xcu_b:");
        for(int j = 0;j < 64; j++)
        {
            g_print("%d\t",data->nm_yc_xcu_b[j]);

        }
        g_print("\n nm_cy_xcu_b:");
        for(int j = 0;j < 64; j++)
        {

            g_print("%d\t",data->nm_cy_xcu_b[j]);

        }
        g_print("\n nm_cc_xcu_b:");
          for(int j = 0;j < 64; j++)
        {

            g_print("%d\t",data->nm_cc_xcu_b[j]);
        }
        g_print("\n");
        g_print("nm_y_log_est_min_b%d\n",    data->nm_y_log_est_min_b);
        g_print("nm_y_log_est_max_b%d\n",    data->nm_y_log_est_max_b);
        g_print("nm_c_log_est_min_b%d\n",    data->nm_c_log_est_min_b);
        g_print("nm_c_log_est_max_b%d\n",    data->nm_c_log_est_max_b);
        g_print("nm_Y_alpha_b      %d\n",    data->nm_Y_alpha_b      );
        g_print("nm_C_alpha_b      %d\n",    data->nm_C_alpha_b      );
        g_print("Tnr_Strength_0    %d\n",    data->Tnr_Strength_0    );
        g_print("Tnr_Strength_1    %d\n",    data->Tnr_Strength_1    );
        g_print("SpNR_Static       %d\n",    data->SpNR_Static       );
        g_print("SpNR_Dynamic      %d\n",    data->SpNR_Dynamic      );
        g_print("Radial_Gain       %d\n",    data->Radial_Gain       );
        g_print("SAD_Gain          %d\n",    data->SAD_Gain          );
        g_print("Pre_Sim_Gain      %d\n",    data->Pre_Sim_Gain      );
        g_print("Weight_In         %d\n",    data->Weight_In         );
        g_print("g_mv_x            %d\n",    data->g_mv_x            );
        g_print("g_mv_y            %d\n",    data->g_mv_y            );
        g_print("tbd_sim_gain      %d\n",    data->tbd_sim_gain      );
        g_print("NS_Gain           %d\n",    data->NS_Gain           );
        g_print("nsw_gain          %d\n",    data->nsw_gain          );
        g_print("nsw_sigma         %d\n",    data->nsw_sigma         );
        g_print("nsw_bias          %d\n",    data->nsw_bias          );
        g_print("ns_clw_bias0      %d\n",    data->ns_clw_bias0      );
        g_print("ns_clw_bias1      %d\n",    data->ns_clw_bias1      );
        g_print("ns_clw_sigma      %d\n",    data->ns_clw_sigma      );
        g_print("ns_clw_center     %d\n",    data->ns_clw_center     );
        g_print("ns_norm_bias      %d\n",    data->ns_norm_bias      );
        g_print("ns_norm_coef      %d\n",    data->ns_norm_coef      );
        g_print("bypass_g_mv       %d\n",    data->bypass_g_mv       );
        g_print("bypass_NS         %d\n",    data->bypass_NS         );

}
void print_xnr_dss_t(struct camera_control_isp_xnr_dss_t *data)
{

        g_print("rad_enable                  %d\n",data->rad_enable               );
        g_print("bypass                      %d\n",data->bypass                   );
        g_print("bypass_mf_y                 %d\n",data->bypass_mf_y              );
        g_print("bypass_mf_c                 %d\n",data->bypass_mf_c              );
        g_print("spatial_sigma_mf_y          %d\n",data->spatial_sigma_mf_y       );
        g_print("spatial_sigma_mf_c          %d\n",data->spatial_sigma_mf_c       );
        g_print("noise_white_mf_y_y          %d\n",data->noise_white_mf_y_y       );
        g_print("noise_white_mf_y_s          %d\n",data->noise_white_mf_y_s       );
        g_print("noise_white_mf_c_y          %d\n",data->noise_white_mf_c_y       );
        g_print("noise_white_mf_c_u          %d\n",data->noise_white_mf_c_u       );
        g_print("noise_white_mf_c_v          %d\n",data->noise_white_mf_c_v       );
        g_print("rad_noise_power_mf_luma     %d\n",data->rad_noise_power_mf_luma  );
        g_print("rad_noise_power_mf_chroma   %d\n",data->rad_noise_power_mf_chroma);
        g_print("range_weight_lut:");
        for(int j = 0; j < 7; j++)
        {
            g_print("%d\t",data->range_weight_lut[j]);
        }
        g_print("\n one_div_64_lut:");
        for(int j = 0; j < 64; j++)
        {
            g_print("%d\t",data->one_div_64_lut[j]);
        }
        g_print("\n");
        g_print("rad_noise_compensation_mf_chroma   %d\n",data->rad_noise_compensation_mf_chroma);
        g_print("rad_noise_compensation_mf_luma     %d\n",data->rad_noise_compensation_mf_luma  );
        g_print("mf_luma_power                      %d\n",data->mf_luma_power                   );
        g_print("rad_mf_luma_power                  %d\n",data->rad_mf_luma_power               );
        g_print("mf_chroma_power                    %d\n",data->mf_chroma_power                 );
        g_print("rad_mf_chroma_power                %d\n",data->rad_mf_chroma_power             );
        g_print("noise_black_mf_y_y                 %d\n",data->noise_black_mf_y_y              );
        g_print("noise_black_mf_c_y                 %d\n",data->noise_black_mf_c_y              );
        g_print("noise_black_mf_c_u                 %d\n",data->noise_black_mf_c_u              );
        g_print("noise_black_mf_c_v                 %d\n",data->noise_black_mf_c_v              );
        g_print("xcu_lcs_x:");
        for(int j = 0; j < 33; j++)
        {
            g_print("%d\t",data->xcu_lcs_x[j]);
        }
        g_print("\n");
        g_print("xcu_lcs_exp         %d\n",data->xcu_lcs_exp      );
        g_print("xcu_lcs_slp_a_res   %d\n",data->xcu_lcs_slp_a_res);
        g_print("xcu_lcs_offset:");
        for(int j = 0; j < 32; j++)
        {
            g_print("%d\t",data->xcu_lcs_offset[j]);
        }
        g_print("\n xcu_lcs_slope:");
         for(int j = 0; j < 32; j++)
        {
            g_print("%d\t",data->xcu_lcs_slope[j] );
        }
        g_print("\n");
        g_print("lcs_th_for_black       %d\n",data->lcs_th_for_black    );
        g_print("lcs_th_for_white       %d\n",data->lcs_th_for_white    );
        g_print("rad_lcs_th_for_black   %d\n",data->rad_lcs_th_for_black);
        g_print("rad_lcs_th_for_white   %d\n",data->rad_lcs_th_for_white);
        g_print("blnd_hf_power_y        %d\n",data->blnd_hf_power_y     );
        g_print("blnd_hf_power_c        %d\n",data->blnd_hf_power_c     );
}

void print_bnlm_t(struct camera_control_isp_bnlm_t *data)
{

        g_print("bypass            %d\n",data->bypass          );
        g_print("detailix_radgain  %d\n",data->detailix_radgain);
        for(int j = 0; j < 2; j++)
        {
            g_print("data->detailix_x_range[%d]%d\n",j,data->detailix_x_range[j]);
            g_print("data->sad_mu_x_range[%d]  %d\n",j,data->sad_mu_x_range[j]  );
        }
        g_print("sad_radgain  %d\n",data->sad_radgain);
        for(int j = 0; j < 3; j++)
        {
            g_print("detailix_coeffs[%d]              %d\n",j,data->detailix_coeffs[j]);
            g_print("sad_mu_coeffs[%d]                %d\n",j,data->sad_mu_coeffs[j]  );
            g_print("detailth[%d]                     %d\n",j,data->detailth[j]       );
        }
       for(int j = 0; j < 4; j++)
       {
           g_print("data->sad_spatialrad[%d]       %d\n",j,data->sad_spatialrad[j]   );
           g_print("data->sad_detailixlutx[%d]     %d\n",j,data->sad_detailixlutx[j] );
           g_print("data->sad_detailixluty[%d]     %d\n",j,data->sad_detailixluty[j] );
           g_print("data->numcandforavg[%d]        %d\n",j,data->numcandforavg[j]    );
           g_print("data->blend_power[%d]          %d\n",j,data->blend_power[j]      );
           g_print("data->blend_th[%d]             %d\n",j,data->blend_th[j]         );
           g_print("data->blend_texturegain[%d]    %d\n",j,data->blend_texturegain[j]);
           g_print("data->matchqualitycands[%d]    %d\n",j,data->matchqualitycands[j]);
        }
        g_print("data->blend_radgain   %d\n",data->blend_radgain );
        g_print("data->wsumminth       %d\n",data->wsumminth     );
        g_print("data->wmaxminth       %d\n",data->wmaxminth     );
        g_print("data->rad_enable      %d\n",data->rad_enable    );
}

void print_gama_tone_map_t(struct camera_control_isp_gamma_tone_map_t *data)
{

        g_print("gamma:");
        for(int j = 0; j < 1024; j++)
        {
            g_print("%f\t",data->gamma[j]);
        }
        g_print("\n tone_map:");
        for(int j = 0; j < 1024; j++)
        {
            g_print("%f\t",data->tone_map[j]);
        }
        g_print("\n");
        g_print("data->gamma_lut_size    %d\n",   data->gamma_lut_size);
        g_print("data->tone_map_lut_size %d\n",   data->tone_map_lut_size);
}

void Check_LTM_Tuning_data(ltm_tuning_data *setdata,ltm_tuning_data *getdata)
{
    EXPECT_EQ(setdata->algo_mode                                 ,getdata->algo_mode                                 )<<"algo_mode                                 ";
    EXPECT_EQ(setdata->optibright_tuning.GTM_Str                 ,getdata->optibright_tuning.GTM_Str                 )<<"optibright_tuning.GTM_Str                 ";
    EXPECT_EQ(setdata->optibright_tuning.GF_epspar               ,getdata->optibright_tuning.GF_epspar               )<<"optibright_tuning.GF_epspar               ";
    EXPECT_EQ(setdata->optibright_tuning.alpham1                 ,getdata->optibright_tuning.alpham1                 )<<"optibright_tuning.alpham1                 ";
    EXPECT_EQ(setdata->optibright_tuning.alpham                  ,getdata->optibright_tuning.alpham                  )<<"optibright_tuning.alpham                  ";
    EXPECT_EQ(setdata->optibright_tuning.maskmin                 ,getdata->optibright_tuning.maskmin                 )<<"optibright_tuning.maskmin                 ";
    EXPECT_EQ(setdata->optibright_tuning.maskmax                 ,getdata->optibright_tuning.maskmax                 )<<"optibright_tuning.maskmax                 ";
    EXPECT_EQ(setdata->optibright_tuning.num_iteration           ,getdata->optibright_tuning.num_iteration           )<<"optibright_tuning.num_iteration           ";
    EXPECT_EQ(setdata->optibright_tuning.num_iteration           ,getdata->optibright_tuning.num_iteration           )<<"optibright_tuning.num_iteration           ";
    EXPECT_EQ(setdata->optibright_tuning.maskmid                 ,getdata->optibright_tuning.maskmid                 )<<"optibright_tuning.maskmid                 ";
    EXPECT_EQ(setdata->optibright_tuning.hlc_mode                ,getdata->optibright_tuning.hlc_mode                )<<"optibright_tuning.hlc_mode                ";
    EXPECT_EQ(setdata->optibright_tuning.max_isp_gain            ,getdata->optibright_tuning.max_isp_gain            )<<"optibright_tuning.max_isp_gain            ";
    EXPECT_EQ(setdata->optibright_tuning.convergence_speed       ,getdata->optibright_tuning.convergence_speed       )<<"optibright_tuning.convergence_speed       ";
    EXPECT_EQ(setdata->optibright_tuning.lm_treatment            ,getdata->optibright_tuning.lm_treatment            )<<"optibright_tuning.lm_treatment            ";
    EXPECT_EQ(setdata->optibright_tuning.GTM_mode                ,getdata->optibright_tuning.GTM_mode                )<<"optibright_tuning.GTM_mode                ";
    EXPECT_EQ(setdata->optibright_tuning.pre_gamma               ,getdata->optibright_tuning.pre_gamma               )<<"optibright_tuning.pre_gamma               ";
    EXPECT_EQ(setdata->optibright_tuning.lav2p_scale             ,getdata->optibright_tuning.lav2p_scale             )<<"optibright_tuning.lav2p_scale             ";
    EXPECT_EQ(setdata->optibright_tuning.p_max                   ,getdata->optibright_tuning.p_max                   )<<"optibright_tuning.p_max                   ";
    EXPECT_EQ(setdata->optibright_tuning.p_mode                  ,getdata->optibright_tuning.p_mode                  )<<"optibright_tuning.p_mode                  ";
    EXPECT_EQ(setdata->optibright_tuning.p_value                 ,getdata->optibright_tuning.p_value                 )<<"optibright_tuning.p_value                 ";
    EXPECT_EQ(setdata->optibright_tuning.filter_size             ,getdata->optibright_tuning.filter_size             )<<"optibright_tuning.filter_size             ";
    EXPECT_EQ(setdata->optibright_tuning.max_percentile          ,getdata->optibright_tuning.max_percentile          )<<"optibright_tuning.max_percentile          ";
    EXPECT_EQ(setdata->optibright_tuning.ldr_brightness          ,getdata->optibright_tuning.ldr_brightness          )<<"optibright_tuning.ldr_brightness          ";
    EXPECT_EQ(setdata->optibright_tuning.dr_mid                  ,getdata->optibright_tuning.dr_mid                  )<<"optibright_tuning.dr_mid                  ";
    EXPECT_EQ(setdata->optibright_tuning.dr_norm_max             ,getdata->optibright_tuning.dr_norm_max             )<<"optibright_tuning.dr_norm_max             ";
    EXPECT_EQ(setdata->optibright_tuning.dr_norm_min             ,getdata->optibright_tuning.dr_norm_min             )<<"optibright_tuning.dr_norm_min             ";
    EXPECT_EQ(setdata->optibright_tuning.convergence_speed_slow  ,getdata->optibright_tuning.convergence_speed_slow  )<<"optibright_tuning.convergence_speed_slow  ";
    EXPECT_EQ(setdata->optibright_tuning.convergence_sigma       ,getdata->optibright_tuning.convergence_sigma       )<<"optibright_tuning.convergence_sigma       ";
    EXPECT_EQ(setdata->optibright_tuning.gainext_mode            ,getdata->optibright_tuning.gainext_mode            )<<"optibright_tuning.gainext_mode            ";
    EXPECT_EQ(setdata->optibright_tuning.wdr_scale_max           ,getdata->optibright_tuning.wdr_scale_max           )<<"optibright_tuning.wdr_scale_max           ";
    EXPECT_EQ(setdata->optibright_tuning.wdr_scale_min           ,getdata->optibright_tuning.wdr_scale_min           )<<"optibright_tuning.wdr_scale_min           ";
    EXPECT_EQ(setdata->optibright_tuning.wdr_gain_max            ,getdata->optibright_tuning.wdr_gain_max            )<<"optibright_tuning.wdr_gain_max            ";
    EXPECT_EQ(setdata->optibright_tuning.frame_delay_compensation,getdata->optibright_tuning.frame_delay_compensation)<<"optibright_tuning.frame_delay_compensation";
    EXPECT_EQ(setdata->mpgc_tuning.lm_stability                  ,getdata->mpgc_tuning.lm_stability                  )<<"mpgc_tuning.lm_stability                  ";
    EXPECT_EQ(setdata->mpgc_tuning.lm_sensitivity                ,getdata->mpgc_tuning.lm_sensitivity                )<<"mpgc_tuning.lm_sensitivity                ";
    EXPECT_EQ(setdata->mpgc_tuning.blur_size                     ,getdata->mpgc_tuning.blur_size                     )<<"mpgc_tuning.blur_size                     ";
    EXPECT_EQ(setdata->mpgc_tuning.tf_str                        ,getdata->mpgc_tuning.tf_str                        )<<"mpgc_tuning.tf_str                        ";
    EXPECT_EQ(setdata->drcsw_tuning.blus_sim_sigma               ,getdata->drcsw_tuning.blus_sim_sigma               )<<"drcsw_tuning.blus_sim_sigma               ";
}
void CheckWB_Gains_Value(struct camera_control_isp_wb_gains_t *setdata,struct camera_control_isp_wb_gains_t *getdata)
{
    EXPECT_EQ(setdata->gr,getdata->gr)<<"gr";
    EXPECT_EQ(setdata->r ,getdata->r )<<"r ";
    EXPECT_EQ(setdata->b ,getdata->b )<<"b ";
    EXPECT_EQ(setdata->gb,getdata->gb)<<"gb";
}
void CheckCCM_Value(struct camera_control_isp_color_correction_matrix_t *setdata,struct camera_control_isp_color_correction_matrix_t *getdata)
{
    for(int i = 0; i < 9; i++)
    {
        EXPECT_EQ(setdata->ccm_gains[i],getdata->ccm_gains[i]) <<"ccm_gains";
    }
}
void CheckCSC_Value(struct camera_control_isp_bxt_csc_t *setdata,struct camera_control_isp_bxt_csc_t *getdata)
{
    for(int i = 0; i < 9; i++)
    {
        EXPECT_EQ(setdata->rgb2yuv_coef[i],getdata->rgb2yuv_coef[i]) <<"rgb2yuv_coef";
    }
}
void CheckCFA_Value(struct camera_control_isp_bxt_demosaic_t *setdata,struct camera_control_isp_bxt_demosaic_t *getdata)
{
    EXPECT_EQ(setdata->high_frequency_denoise_enable,getdata->high_frequency_denoise_enable)<<"high_frequency_denoise_enable";
    EXPECT_EQ(setdata->false_color_correction_enable,getdata->false_color_correction_enable)<<"false_color_correction_enable";
    EXPECT_EQ(setdata->chroma_artifact_removal_enable,getdata->chroma_artifact_removal_enable)<<"chroma_artifact_removal_enabl";
    EXPECT_EQ(setdata->sharpening_strength          ,getdata->sharpening_strength          )<<"sharpening_strength          ";
    EXPECT_EQ(setdata->false_color_correction_gain  ,getdata->false_color_correction_gain  )<<"false_color_correction_gain  ";
    EXPECT_EQ(setdata->high_frequency_denoise_power ,getdata->high_frequency_denoise_power )<<"high_frequency_denoise_power ";
    EXPECT_EQ(setdata->checkers_removal_w           ,getdata->checkers_removal_w           )<<"checkers_removal_w           ";
}
void CheckIEFD_Value(struct camera_control_isp_sc_iefd_t *setdata,struct camera_control_isp_sc_iefd_t *getdata)
{
 for(int j = 0; j < 2; j++)
 {
     EXPECT_EQ(setdata->sharpening_power[j]    ,getdata->sharpening_power[j]    )<<"sharpening_power    ";
     EXPECT_EQ(setdata->cu_ed2_metric_th[j]    ,getdata->cu_ed2_metric_th[j]    )<<"cu_ed2_metric_th    ";
     EXPECT_EQ(setdata->cu_nr_power[j]         ,getdata->cu_nr_power[j]         )<<"cu_nr_power         ";
     EXPECT_EQ(setdata->dir_far_sharp_weight[j],getdata->dir_far_sharp_weight[j])<<"dir_far_sharp_weight";
 }
EXPECT_EQ(setdata->sharp_power_edge     ,getdata->sharp_power_edge     )<<"sharp_power_edge     ";
EXPECT_EQ(setdata->sharp_power_detail   ,getdata->sharp_power_detail   )<<"sharp_power_detail   ";
EXPECT_EQ(setdata->unsharp_weight_edge  ,getdata->unsharp_weight_edge  )<<"unsharp_weight_edge  ";
EXPECT_EQ(setdata->unsharp_weight_detail,getdata->unsharp_weight_detail)<<"unsharp_weight_detail";
EXPECT_EQ(setdata->denoise_power        ,getdata->denoise_power        )<<"denoise_power        ";
EXPECT_EQ(setdata->radial_denoise_power ,getdata->radial_denoise_power )<<"radial_denoise_power ";
EXPECT_EQ(setdata->shrpn_nega_lmt_txt   ,getdata->shrpn_nega_lmt_txt   )<<"shrpn_nega_lmt_txt   ";
EXPECT_EQ(setdata->shrpn_posi_lmt_txt   ,getdata->shrpn_posi_lmt_txt   )<<"shrpn_posi_lmt_txt   ";
EXPECT_EQ(setdata->shrpn_nega_lmt_dir   ,getdata->shrpn_nega_lmt_dir   )<<"shrpn_nega_lmt_dir   ";
EXPECT_EQ(setdata->shrpn_posi_lmt_dir   ,getdata->shrpn_posi_lmt_dir   )<<"shrpn_posi_lmt_dir   ";
for(int j = 0; j < 4; j++)
{
    EXPECT_EQ(setdata->cu_var_metric_th[j],getdata->cu_var_metric_th[j])<<"data->cu_var_metric_th";
}
EXPECT_EQ(setdata->rad_enable,getdata->rad_enable)<<"rad_enable";
for(int j = 0; j < 15; j++)
{
    EXPECT_EQ(setdata->unsharp_filter[j],getdata->unsharp_filter[j])<<"unsharp_filter";
}
for(int j = 0; j < 6; j++)
{
    EXPECT_EQ(setdata->configunited_x[j]    ,getdata->configunited_x[j]    )<<"configunited_x    ";
    EXPECT_EQ(setdata->denoise_filter[j]    ,getdata->denoise_filter[j]    )<<"denoise_filter    ";
    EXPECT_EQ(setdata->configunitradial_y[j],getdata->configunitradial_y[j])<<"configunitradial_y";
    EXPECT_EQ(setdata->configunitradial_x[j],getdata->configunitradial_x[j])<<"configunitradial_x";
    EXPECT_EQ(setdata->configunited_y[j]    ,getdata->configunited_y[j]    )<<"configunited_y    ";
}
EXPECT_EQ(setdata->vssnlm_x0,getdata->vssnlm_x0)<<"vssnlm_x0";
EXPECT_EQ(setdata->vssnlm_x1,getdata->vssnlm_x1)<<"vssnlm_x1";
EXPECT_EQ(setdata->vssnlm_x2,getdata->vssnlm_x2)<<"vssnlm_x2";
EXPECT_EQ(setdata->vssnlm_y1,getdata->vssnlm_y1)<<"vssnlm_y1";
EXPECT_EQ(setdata->vssnlm_y2,getdata->vssnlm_y2)<<"vssnlm_y2";
EXPECT_EQ(setdata->vssnlm_y3,getdata->vssnlm_y3)<<"vssnlm_y3";
}
void CheckSEE_Value(struct camera_control_isp_see_t *setdata,struct camera_control_isp_see_t *getdata)
{
     EXPECT_EQ(setdata->bypass     ,getdata->bypass     )<<"bypass     ";
     EXPECT_EQ(setdata->edge_max   ,getdata->edge_max   )<<"edge_max   ";
     EXPECT_EQ(setdata->edge_thres ,getdata->edge_thres )<<"edge_thres ";
     EXPECT_EQ(setdata->alpha      ,getdata->alpha      )<<"alpha      ";
     EXPECT_EQ(setdata->alpha_shift,getdata->alpha_shift)<<"alpha_shift";
     EXPECT_EQ(setdata->alpha_width,getdata->alpha_width)<<"alpha_width";
     EXPECT_EQ(setdata->alpha_bias ,getdata->alpha_bias )<<"alpha_bias ";
}
void CheckBNLM_Value(struct camera_control_isp_bnlm_t *setdata,struct camera_control_isp_bnlm_t *getdata)
{
        EXPECT_EQ(setdata->bypass          ,getdata->bypass          )<<"data->bypass          ";
        EXPECT_EQ(setdata->detailix_radgain,getdata->detailix_radgain)<<"data->detailix_radgain";
        for(int j = 0; j < 2; j++)
        {
            EXPECT_EQ(setdata->detailix_x_range[j],getdata->detailix_x_range[j])<<"detailix_x_range";
            EXPECT_EQ(setdata->sad_mu_x_range[j]  ,getdata->sad_mu_x_range[j]  )<<"sad_mu_x_range";
        }
        EXPECT_EQ(setdata->sad_radgain,getdata->sad_radgain)<<"sad_radgain";
        for(int j = 0; j < 3; j++)
        {
            EXPECT_EQ(setdata->detailix_coeffs[j],getdata->detailix_coeffs[j])<<"detailix_coeffs";
            EXPECT_EQ(setdata->sad_mu_coeffs[j]  ,getdata->sad_mu_coeffs[j]  )<<"sad_mu_coeffs";
            EXPECT_EQ(setdata->detailth[j]       ,getdata->detailth[j]       )<<"detailth";
        }
       for(int j = 0; j < 4; j++)
       {
           EXPECT_EQ(setdata->sad_spatialrad[j]   ,getdata->sad_spatialrad[j]   )<<"sad_spatialrad";
           EXPECT_EQ(setdata->sad_detailixlutx[j] ,getdata->sad_detailixlutx[j] )<<"sad_detailixlutx";
           EXPECT_EQ(setdata->sad_detailixluty[j] ,getdata->sad_detailixluty[j] )<<"sad_detailixluty";
           EXPECT_EQ(setdata->numcandforavg[j]    ,getdata->numcandforavg[j]    )<<"numcandforavg ";
           EXPECT_EQ(setdata->blend_power[j]      ,getdata->blend_power[j]      )<<"blend_power   ";
           EXPECT_EQ(setdata->blend_th[j]         ,getdata->blend_th[j]         )<<"blend_th      ";
           EXPECT_EQ(setdata->blend_texturegain[j],getdata->blend_texturegain[j])<<"blend_texturegain";
           EXPECT_EQ(setdata->matchqualitycands[j],getdata->matchqualitycands[j])<<"matchqualitycands";
        }
       EXPECT_EQ(setdata->blend_radgain,getdata->blend_radgain)<<"blend_radgain";
       EXPECT_EQ(setdata->wsumminth    ,getdata->wsumminth    )<<"wsumminth";
       EXPECT_EQ(setdata->wmaxminth    ,getdata->wmaxminth    )<<"wmaxminth";
       EXPECT_EQ(setdata->rad_enable   ,getdata->rad_enable   )<<"rad_enable";
}
void CheckTNR5_Value(struct camera_control_isp_tnr5_21_t *setdata,struct camera_control_isp_tnr5_21_t *getdata)
{
        EXPECT_EQ(setdata->bypass,getdata->bypass)<<"bypass";
        for(int j = 0;j < 64; j++)
        {
            EXPECT_EQ(setdata->nm_yy_xcu_b[j],getdata->nm_yy_xcu_b[j])<<"nm_yy_xcu_b";
            EXPECT_EQ(setdata->nm_yc_xcu_b[j],getdata->nm_yc_xcu_b[j])<<"nm_yc_xcu_b";
            EXPECT_EQ(setdata->nm_cy_xcu_b[j],getdata->nm_cy_xcu_b[j])<<"nm_cy_xcu_b";
            EXPECT_EQ(setdata->nm_cc_xcu_b[j],getdata->nm_cc_xcu_b[j])<<"nm_cc_xcu_b";
        }
        EXPECT_EQ(setdata->nm_y_log_est_min_b,getdata->nm_y_log_est_min_b)<<"nm_y_log_est_min_b";
        EXPECT_EQ(setdata->nm_y_log_est_max_b,getdata->nm_y_log_est_max_b)<<"nm_y_log_est_max_b";
        EXPECT_EQ(setdata->nm_c_log_est_min_b,getdata->nm_c_log_est_min_b)<<"nm_c_log_est_min_b";
        EXPECT_EQ(setdata->nm_c_log_est_max_b,getdata->nm_c_log_est_max_b)<<"nm_c_log_est_max_b";
        EXPECT_EQ(setdata->nm_Y_alpha_b      ,getdata->nm_Y_alpha_b      )<<"nm_Y_alpha_b      ";
        EXPECT_EQ(setdata->nm_C_alpha_b      ,getdata->nm_C_alpha_b      )<<"nm_C_alpha_b      ";
        EXPECT_EQ(setdata->Tnr_Strength_0    ,getdata->Tnr_Strength_0    )<<"Tnr_Strength_0    ";
        EXPECT_EQ(setdata->Tnr_Strength_1    ,getdata->Tnr_Strength_1    )<<"Tnr_Strength_1    ";
        EXPECT_EQ(setdata->SpNR_Static       ,getdata->SpNR_Static       )<<"SpNR_Static       ";
        EXPECT_EQ(setdata->SpNR_Dynamic      ,getdata->SpNR_Dynamic      )<<"SpNR_Dynamic      ";
        EXPECT_EQ(setdata->Radial_Gain       ,getdata->Radial_Gain       )<<"Radial_Gain       ";
        EXPECT_EQ(setdata->SAD_Gain          ,getdata->SAD_Gain          )<<"SAD_Gain          ";
        EXPECT_EQ(setdata->Pre_Sim_Gain      ,getdata->Pre_Sim_Gain      )<<"Pre_Sim_Gain      ";
        EXPECT_EQ(setdata->Weight_In         ,getdata->Weight_In         )<<"Weight_In         ";
        EXPECT_EQ(setdata->g_mv_x            ,getdata->g_mv_x            )<<"g_mv_x            ";
        EXPECT_EQ(setdata->g_mv_y            ,getdata->g_mv_y            )<<"g_mv_y            ";
        EXPECT_EQ(setdata->tbd_sim_gain      ,getdata->tbd_sim_gain      )<<"tbd_sim_gain      ";
        EXPECT_EQ(setdata->NS_Gain           ,getdata->NS_Gain           )<<"NS_Gain           ";
        EXPECT_EQ(setdata->nsw_gain          ,getdata->nsw_gain          )<<"nsw_gain          ";
        EXPECT_EQ(setdata->nsw_sigma         ,getdata->nsw_sigma         )<<"nsw_sigma         ";
        EXPECT_EQ(setdata->nsw_bias          ,getdata->nsw_bias          )<<"nsw_bias          ";
        EXPECT_EQ(setdata->ns_clw_bias0      ,getdata->ns_clw_bias0      )<<"ns_clw_bias0      ";
        EXPECT_EQ(setdata->ns_clw_bias1      ,getdata->ns_clw_bias1      )<<"ns_clw_bias1      ";
        EXPECT_EQ(setdata->ns_clw_sigma      ,getdata->ns_clw_sigma      )<<"ns_clw_sigma      ";
        EXPECT_EQ(setdata->ns_clw_center     ,getdata->ns_clw_center     )<<"ns_clw_center     ";
        EXPECT_EQ(setdata->ns_norm_bias      ,getdata->ns_norm_bias      )<<"ns_norm_bias      ";
        EXPECT_EQ(setdata->ns_norm_coef      ,getdata->ns_norm_coef      )<<"ns_norm_coef      ";
        EXPECT_EQ(setdata->bypass_g_mv       ,getdata->bypass_g_mv       )<<"bypass_g_mv       ";
        EXPECT_EQ(setdata->bypass_NS         ,getdata->bypass_NS         )<<"bypass_NS         ";
}
void CheckXNR_DSS_Value(struct camera_control_isp_xnr_dss_t *setdata,struct camera_control_isp_xnr_dss_t *getdata)
{
        EXPECT_EQ(setdata->rad_enable               ,getdata->rad_enable               )<<"rad_enable               ";
        EXPECT_EQ(setdata->bypass                   ,getdata->bypass                   )<<"bypass                   ";
        EXPECT_EQ(setdata->bypass_mf_y              ,getdata->bypass_mf_y              )<<"bypass_mf_y              ";
        EXPECT_EQ(setdata->bypass_mf_c              ,getdata->bypass_mf_c              )<<"bypass_mf_c              ";
        EXPECT_EQ(setdata->spatial_sigma_mf_y       ,getdata->spatial_sigma_mf_y       )<<"spatial_sigma_mf_y       ";
        EXPECT_EQ(setdata->spatial_sigma_mf_c       ,getdata->spatial_sigma_mf_c       )<<"spatial_sigma_mf_c       ";
        EXPECT_EQ(setdata->noise_white_mf_y_y       ,getdata->noise_white_mf_y_y       )<<"noise_white_mf_y_y       ";
        EXPECT_EQ(setdata->noise_white_mf_y_s       ,getdata->noise_white_mf_y_s       )<<"noise_white_mf_y_s       ";
        EXPECT_EQ(setdata->noise_white_mf_c_y       ,getdata->noise_white_mf_c_y       )<<"noise_white_mf_c_y       ";
        EXPECT_EQ(setdata->noise_white_mf_c_u       ,getdata->noise_white_mf_c_u       )<<"noise_white_mf_c_u       ";
        EXPECT_EQ(setdata->noise_white_mf_c_v       ,getdata->noise_white_mf_c_v       )<<"noise_white_mf_c_v       ";
        EXPECT_EQ(setdata->rad_noise_power_mf_luma  ,getdata->rad_noise_power_mf_luma  )<<"rad_noise_power_mf_luma  ";
        EXPECT_EQ(setdata->rad_noise_power_mf_chroma,getdata->rad_noise_power_mf_chroma)<<"rad_noise_power_mf_chroma";
        for(int j = 0; j < 7; j++)
        {
            EXPECT_EQ(setdata->range_weight_lut[j],getdata->range_weight_lut[j])<<"range_weight_lut";
        }
        for(int j = 0; j < 64; j++)
        {
            EXPECT_EQ(setdata->one_div_64_lut[j],setdata->one_div_64_lut[j])<<"one_div_64_lut";
        }
        EXPECT_EQ(setdata->rad_noise_compensation_mf_chroma,getdata->rad_noise_compensation_mf_chroma)<<"rad_noise_compensation_mf_chroma";
        EXPECT_EQ(setdata->rad_noise_compensation_mf_luma  ,getdata->rad_noise_compensation_mf_luma  )<<"rad_noise_compensation_mf_luma  ";
        EXPECT_EQ(setdata->mf_luma_power                   ,getdata->mf_luma_power                   )<<"mf_luma_power                   ";
        EXPECT_EQ(setdata->rad_mf_luma_power               ,getdata->rad_mf_luma_power               )<<"rad_mf_luma_power               ";
        EXPECT_EQ(setdata->mf_chroma_power                 ,getdata->mf_chroma_power                 )<<"mf_chroma_power                 ";
        EXPECT_EQ(setdata->rad_mf_chroma_power             ,getdata->rad_mf_chroma_power             )<<"rad_mf_chroma_power             ";
        EXPECT_EQ(setdata->noise_black_mf_y_y              ,getdata->noise_black_mf_y_y              )<<"noise_black_mf_y_y              ";
        EXPECT_EQ(setdata->noise_black_mf_c_y              ,getdata->noise_black_mf_c_y              )<<"noise_black_mf_c_y              ";
        EXPECT_EQ(setdata->noise_black_mf_c_u              ,getdata->noise_black_mf_c_u              )<<"noise_black_mf_c_u              ";
        EXPECT_EQ(setdata->noise_black_mf_c_v              ,getdata->noise_black_mf_c_v              )<<"noise_black_mf_c_v              ";
        for(int j = 0; j < 33; j++)
        {
            EXPECT_EQ(setdata->xcu_lcs_x[j],getdata->xcu_lcs_x[j])<<"data->xcu_lcs_x";
        }
        EXPECT_EQ(setdata->xcu_lcs_exp      ,getdata->xcu_lcs_exp      )<<"xcu_lcs_exp      ";
        EXPECT_EQ(setdata->xcu_lcs_slp_a_res,getdata->xcu_lcs_slp_a_res)<<"xcu_lcs_slp_a_res";
         for(int j = 0; j < 32; j++)
        {
            EXPECT_EQ(setdata->xcu_lcs_offset[j]   ,getdata->xcu_lcs_offset[j]   )<<"xcu_lcs_offset[j]   ";
            EXPECT_EQ(setdata->xcu_lcs_slope[j]    ,getdata->xcu_lcs_slope[j]    )<<"xcu_lcs_slope[j]    ";
        }
        EXPECT_EQ(setdata->lcs_th_for_black    ,getdata->lcs_th_for_black    )<<"lcs_th_for_black    ";
        EXPECT_EQ(setdata->lcs_th_for_white    ,getdata->lcs_th_for_white    )<<"lcs_th_for_white    ";
        EXPECT_EQ(setdata->rad_lcs_th_for_black,getdata->rad_lcs_th_for_black)<<"rad_lcs_th_for_black";
        EXPECT_EQ(setdata->rad_lcs_th_for_white,getdata->rad_lcs_th_for_white)<<"rad_lcs_th_for_white";
        EXPECT_EQ(setdata->blnd_hf_power_y     ,getdata->blnd_hf_power_y     )<<"blnd_hf_power_y     ";
        EXPECT_EQ(setdata->blnd_hf_power_c     ,getdata->blnd_hf_power_c     )<<"blnd_hf_power_c     ";
}
void CheckGamma_Tone_Map_Value(struct camera_control_isp_gamma_tone_map_t *setdata,struct camera_control_isp_gamma_tone_map_t *getdata)
{
    for(int i = 0;i < 1024; i++)
    {
         EXPECT_EQ(setdata->gamma[i]   ,getdata->gamma[i])<<"gamma";
         EXPECT_EQ(setdata->tone_map[i],getdata->tone_map[i])<<"tone_map";
    }
    EXPECT_EQ(setdata->gamma_lut_size   ,getdata->gamma_lut_size   )<<"gamma_lut_size   ";
    EXPECT_EQ(setdata->tone_map_lut_size,getdata->tone_map_lut_size)<<"tone_map_lut_size";
}
static GstPadProbeReturn callback_check_isp_interface(GstPad *padsrc, GstPadProbeInfo *info, gpointer user_data)
{
  gboolean status = false;
  CustomData *data = (CustomData *)user_data;
  //switch setting every ten frames

  if((data->buffer_num_isp++ % 100) != 0 || data->buffer_num_isp < 100)
    return GST_PAD_PROBE_PASS;

  data->isp_control_index++;
  data->isp_control_index = data->isp_control_index % ISP_CONTROL_TEST_NUM;
  g_print("line:%d isp_control_index:%d\n",__LINE__,data->isp_control_index);

  if (data->camisp_control.type == camera_control_isp_ctrl_id_wb_gains)
  {
    struct camera_control_isp_wb_gains_t gaindata = {0.0,0.0,0.0,0.0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_wb_gains_t);
    struct camera_control_isp_wb_gains_t *gainPtr =
        (struct camera_control_isp_wb_gains_t *)((char *)data->camisp_control.data + offset);
    g_print("set the wb gain data: (%f, %f, %f, %f)",
                gainPtr->gr, gainPtr->r, gainPtr->b, gainPtr->gb);

    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&gaindata);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get the wb gain data:(%f %f %f %f )\n",gaindata.gr,gaindata.r,gaindata.b,gaindata.gb);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)gainPtr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&gaindata);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get the wb gain data:(%f %f %f %f )\n",gaindata.gr,gaindata.r,gaindata.b,gaindata.gb);
    CheckWB_Gains_Value(gainPtr,&gaindata);
  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_color_correction_matrix)
  {
    g_print("func:%s line:%d testcamera_control_color_correction_matrix\n",__func__,__LINE__);
    struct camera_control_isp_color_correction_matrix_t  color_correctiondata = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_color_correction_matrix_t);
    struct camera_control_isp_color_correction_matrix_t *color_correction_ptr =
           (struct camera_control_isp_color_correction_matrix_t *)((char *) data->camisp_control.data + offset);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&color_correctiondata);
    g_print("get  data:(%f %f %f %f %f %f %f %f %f)\n",color_correctiondata.ccm_gains[0],color_correctiondata.ccm_gains[1],color_correctiondata.ccm_gains[2],\
                                                                  color_correctiondata.ccm_gains[3],color_correctiondata.ccm_gains[4],color_correctiondata.ccm_gains[5],\
                                                                  color_correctiondata.ccm_gains[6],color_correctiondata.ccm_gains[7],color_correctiondata.ccm_gains[8] );
    g_print("get_isp_control_status:%d\n",status);
    status = false;
    g_print("set data:(%f %f %f %f %f %f %f %f %f)\n",color_correction_ptr->ccm_gains[0],color_correction_ptr->ccm_gains[1],color_correction_ptr->ccm_gains[2],\
                                                                  color_correction_ptr->ccm_gains[3],color_correction_ptr->ccm_gains[4],color_correction_ptr->ccm_gains[5],\
                                                                  color_correction_ptr->ccm_gains[6],color_correction_ptr->ccm_gains[7],color_correction_ptr->ccm_gains[8] );

    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)color_correction_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&color_correctiondata);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get data:(%f %f %f %f %f %f %f %f %f)\n",color_correctiondata.ccm_gains[0],color_correctiondata.ccm_gains[1],color_correctiondata.ccm_gains[2],\
                                                                  color_correctiondata.ccm_gains[3],color_correctiondata.ccm_gains[4],color_correctiondata.ccm_gains[5],\
                                                                  color_correctiondata.ccm_gains[6],color_correctiondata.ccm_gains[7],color_correctiondata.ccm_gains[8] );
    CheckCCM_Value(color_correction_ptr,&color_correctiondata);

  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_bxt_demosaic)
  {
    g_print("func:%s line:%d camera_control_isp_ctrl_id_bxt_demosaic\n",__func__,__LINE__);
    struct camera_control_isp_bxt_demosaic_t bxt_demosaic_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_bxt_demosaic_t);
    struct camera_control_isp_bxt_demosaic_t *bxt_demosaic_ptr =
           (struct camera_control_isp_bxt_demosaic_t *)((char *)data->camisp_control.data + offset);
    g_print("set demosaic data(%d %d %d %d %d %d %d )\n",bxt_demosaic_ptr->high_frequency_denoise_enable,bxt_demosaic_ptr->false_color_correction_enable,bxt_demosaic_ptr->chroma_artifact_removal_enable,\
                                                         bxt_demosaic_ptr->sharpening_strength,bxt_demosaic_ptr->false_color_correction_gain,bxt_demosaic_ptr->high_frequency_denoise_power,bxt_demosaic_ptr->checkers_removal_w);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&bxt_demosaic_data);
    g_print("get demosaic data(%d %d %d %d %d %d %d )\n",bxt_demosaic_data.high_frequency_denoise_enable,bxt_demosaic_data.false_color_correction_enable,bxt_demosaic_data.chroma_artifact_removal_enable,\
                                                         bxt_demosaic_data.sharpening_strength,bxt_demosaic_data.false_color_correction_gain,bxt_demosaic_data.high_frequency_denoise_power,bxt_demosaic_data.checkers_removal_w);
    g_print("get_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)bxt_demosaic_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&bxt_demosaic_data);
    g_print("get demosaic data(%d %d %d %d %d %d %d )\n",bxt_demosaic_data.high_frequency_denoise_enable,bxt_demosaic_data.false_color_correction_enable,bxt_demosaic_data.chroma_artifact_removal_enable,\
                                                         bxt_demosaic_data.sharpening_strength,bxt_demosaic_data.false_color_correction_gain,bxt_demosaic_data.high_frequency_denoise_power,bxt_demosaic_data.checkers_removal_w);
    g_print("get_isp_control_status:%d\n",status);
    CheckCFA_Value(bxt_demosaic_ptr,&bxt_demosaic_data);
  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_sc_iefd)
  {
    g_print("func:%s line:%d camera_control_isp_ctrl_id_sc_iefd\n",__func__,__LINE__);
    struct camera_control_isp_sc_iefd_t sc_iefd_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_sc_iefd_t);
    struct camera_control_isp_sc_iefd_t *sc_iefd_ptr =
           (struct camera_control_isp_sc_iefd_t *)((char *)data->camisp_control.data + offset);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&sc_iefd_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get_data*******************************************************************************************************************:\n");
    print_sc_iefd_t(&sc_iefd_data);
    g_print("set_data*******************************************************************************************************************:\n");
    print_sc_iefd_t(sc_iefd_ptr);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)sc_iefd_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&sc_iefd_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get_data*******************************************************************************************************************:\n");
    print_sc_iefd_t(&sc_iefd_data);
    CheckIEFD_Value(sc_iefd_ptr,&sc_iefd_data);

  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_see)
  {
    g_print("func:%s line:%d camera_control_isp_see_t\n",__func__,__LINE__);
    struct camera_control_isp_see_t  see_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_see_t);
    struct camera_control_isp_see_t *see_ptr =
           (struct camera_control_isp_see_t *)((char *)data->camisp_control.data + offset);
    g_print("set see data(%d %d %d %d %d %d %d)\n",see_ptr->bypass,see_ptr->edge_max,see_ptr->edge_thres,see_ptr->alpha,see_ptr->alpha_shift,see_ptr->alpha_width,see_ptr->alpha_bias);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&see_data);
    g_print("get see data(%d %d %d %d %d %d %d)\n",see_data.bypass,see_data.edge_max,see_data.edge_thres,see_data.alpha,see_data.alpha_shift,see_data.alpha_width,see_data.alpha_bias);
    g_print("get_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)see_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&see_data);
    g_print("get see data(%d %d %d %d %d %d %d)\n",see_data.bypass,see_data.edge_max,see_data.edge_thres,see_data.alpha,see_data.alpha_shift,see_data.alpha_width,see_data.alpha_bias);
    g_print("get_isp_control_status:%d\n",status);
    CheckSEE_Value(see_ptr,&see_data);


  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_tnr5_21)
  {
    g_print("func:%s line:%d camera_control_isp_tnr5_21_t\n",__func__,__LINE__);
    struct camera_control_isp_tnr5_21_t tnr5_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_tnr5_21_t);
    struct camera_control_isp_tnr5_21_t * tnr5_ptr =
           (struct camera_control_isp_tnr5_21_t *)((char *)data->camisp_control.data + offset);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&tnr5_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get tnr5_data*******************************************************************************************************************:\n");
    print_tnr5_21_t(&tnr5_data);
    g_print("set tnr5_data*******************************************************************************************************************:\n");
    print_tnr5_21_t(tnr5_ptr);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)tnr5_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&tnr5_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get tnr5_data*******************************************************************************************************************:\n");
    print_tnr5_21_t(&tnr5_data);
    CheckTNR5_Value(tnr5_ptr,&tnr5_data);


  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_xnr_dss)
  {
    g_print("func:%s line:%d camera_control_isp_xnr_dss_t\n",__func__,__LINE__);
    struct camera_control_isp_xnr_dss_t xnr_dss_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_xnr_dss_t);
    struct camera_control_isp_xnr_dss_t *xnr_dss_ptr =
          (struct camera_control_isp_xnr_dss_t *)((char *)data->camisp_control.data + offset);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&xnr_dss_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get xnr_dss_data*******************************************************************************************************************:\n");
    print_xnr_dss_t(&xnr_dss_data);
    g_print("set xnr_dss_data*******************************************************************************************************************:\n");
    print_xnr_dss_t(xnr_dss_ptr);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)xnr_dss_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&xnr_dss_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get xnr_dss_data*******************************************************************************************************************:\n");
    print_xnr_dss_t(&xnr_dss_data);
    CheckXNR_DSS_Value(xnr_dss_ptr,&xnr_dss_data);


  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_gamma_tone_map)
  {
    g_print("func:%s line:%d camera_control_isp_ctrl_id_gamma_tone_map\n",__func__,__LINE__);
    struct camera_control_isp_gamma_tone_map_t gama_tone_map_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_gamma_tone_map_t);
    struct camera_control_isp_gamma_tone_map_t *gama_tone_map_ptr =
            (struct camera_control_isp_gamma_tone_map_t *)((char *)data->camisp_control.data + offset);
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&gama_tone_map_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get gama_tone_map_data*******************************************************************************************************************:\n");
    print_gama_tone_map_t(&gama_tone_map_data);
    g_print("set gama_tone_map_data*******************************************************************************************************************:\n");
    print_gama_tone_map_t(gama_tone_map_ptr);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)gama_tone_map_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&gama_tone_map_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get gama_tone_map_data*******************************************************************************************************************:\n");
    print_gama_tone_map_t(&gama_tone_map_data);
    CheckGamma_Tone_Map_Value(gama_tone_map_ptr,&gama_tone_map_data);


  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_bnlm)
  {
    g_print("func:%s line:%d camera_control_isp_bnlm_t\n",__func__,__LINE__);
    struct camera_control_isp_bnlm_t bnlm_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_bnlm_t);
    struct camera_control_isp_bnlm_t *bnlm_ptr =
          (struct camera_control_isp_bnlm_t *)((char *)data->camisp_control.data + offset);

    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&bnlm_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get bnlm_data*******************************************************************************************************************:\n");
    print_bnlm_t(&bnlm_data);
    g_print("set bnlmdata*******************************************************************************************************************:\n");
    print_bnlm_t(bnlm_ptr);
    status = false;
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)bnlm_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&bnlm_data);
    g_print("get_isp_control_status:%d\n",status);
    g_print("get bnlm_data*******************************************************************************************************************:\n");
    print_bnlm_t(&bnlm_data);
    CheckBNLM_Value(bnlm_ptr,&bnlm_data);

  }
  else if(data->camisp_control.type == camera_control_isp_ctrl_id_bxt_csc)
  {
    g_print("func:%s line:%d camera_control_isp_csc_t\n",__func__,__LINE__);
    struct camera_control_isp_bxt_csc_t csc_data = {0};
    gint offset = data->isp_control_index * sizeof(struct camera_control_isp_bxt_csc_t);
    struct camera_control_isp_bxt_csc_t *csc_ptr =
          (struct camera_control_isp_bxt_csc_t *)((char *)data->camisp_control.data + offset);
    g_print("set csc_data*******************************************************************************************************************:\n");
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&csc_data);
    g_print("get  data:(%d %d %d %d %d %d %d %d %d)\n",csc_data.rgb2yuv_coef[0],csc_data.rgb2yuv_coef[1],csc_data.rgb2yuv_coef[2],\
                                                                  csc_data.rgb2yuv_coef[3],csc_data.rgb2yuv_coef[4],csc_data.rgb2yuv_coef[5],\
                                                                  csc_data.rgb2yuv_coef[6],csc_data.rgb2yuv_coef[7],csc_data.rgb2yuv_coef[8] );
    g_print("get_isp_control_status:%d\n",status);
    status = false;
    g_print("set  data:(%d %d %d %d %d %d %d %d %d)\n",csc_ptr->rgb2yuv_coef[0],csc_ptr->rgb2yuv_coef[1],csc_ptr->rgb2yuv_coef[2],\
                                                                  csc_ptr->rgb2yuv_coef[3],csc_ptr->rgb2yuv_coef[4],csc_ptr->rgb2yuv_coef[5],\
                                                                  csc_ptr->rgb2yuv_coef[6],csc_ptr->rgb2yuv_coef[7],csc_ptr->rgb2yuv_coef[8] );
    status = data->cameraisp_interface->set_isp_control(data->camisp, data->camisp_control.type, (void *)csc_ptr);
    g_print("set_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->apply_isp_control(data->camisp);
    g_print("apply_isp_control_status:%d\n",status);
    status = false;
    status = data->cameraisp_interface->get_isp_control(data->camisp, data->camisp_control.type, (void *)&csc_data);
    g_print("get csc_data*******************************************************************************************************************:\n");
    g_print("get  data:(%d %d %d %d %d %d %d %d %d)\n",csc_data.rgb2yuv_coef[0],csc_data.rgb2yuv_coef[1],csc_data.rgb2yuv_coef[2],\
                                                                  csc_data.rgb2yuv_coef[3],csc_data.rgb2yuv_coef[4],csc_data.rgb2yuv_coef[5],\
                                                                  csc_data.rgb2yuv_coef[6],csc_data.rgb2yuv_coef[7],csc_data.rgb2yuv_coef[8] );
    g_print("get_isp_control_status:%d\n",status);
    CheckCSC_Value(csc_ptr,&csc_data);
  }
  else if(data->camisp_control.type == LTM_TUNING_DATA_TAG)
  {
    g_print("enter ltm_tuning...........................................................................................................\n");
    ltm_tuning_data ltm_data;
    ltm_tuning_data *ltm_temp;
    memset(&ltm_data,0,sizeof(ltm_tuning_data));
    gint offset = data->isp_control_index * sizeof(ltm_tuning_data);
    ltm_tuning_data *ltm_ptr = (ltm_tuning_data *) ((char *)data->camisp_control.data + offset);
    g_print("set ltm_data*******************************************************************************************************************:\n");
    print_ltm_tuning_data(ltm_ptr);
    data->cameraisp_interface->set_ltm_tuning_data(data->camisp, (void *) ltm_ptr);
    if(data->buffer_num_isp > 101 )
    {
        g_print("get ltm_data*******************************************************************************************************************:\n");
        if(offset == 0)
            ltm_temp = (ltm_tuning_data *) ((char *)data->camisp_control.data + 4 * sizeof(ltm_tuning_data));
        else
            ltm_temp = (ltm_tuning_data *) ((char *)data->camisp_control.data + (data->isp_control_index -1) * sizeof(ltm_tuning_data));
        data->cameraisp_interface->get_ltm_tuning_data(data->camisp, (void*) &ltm_data);
        print_ltm_tuning_data(&ltm_data);
        Check_LTM_Tuning_data(ltm_temp,&ltm_data);
    }

  }
  return GST_PAD_PROBE_PASS;
}
int do_pipline(CheckField *check, PROPERTY_CHECK_TYPE type)
{
    int i;
    GstBus *bus;
    CustomData data;
    int plugin_num = 0;
    GstElement *pipeline;
    GstCaps *caps[MAX_PLUGIN_SIZE];
    GstElement *elements[MAX_PLUGIN_SIZE];

    //clear data.
    for (i = 0; i < MAX_PLUGIN_SIZE; i++) {
        caps[i] = NULL;
        elements[i] = NULL;
    }

    //set environment
    if (putenv((char *)("DISPLAY=:0")) != 0) {
        g_print("Failed to set display env\n");
        return -1;
    }
    //gst init
    gst_init(NULL, NULL);

    //initialize the CustomData.
    data.main_loop = g_main_loop_new(NULL,FALSE);
    if (!data.main_loop) {
        g_print("Failed to get new main loop\n");
        return -1;
    }
    if(check && check->check_isp_interface)
    {
        data.check_isp_interface = true;
        data.camisp_control = check->isp_control;
    }
    data.fps = 1.0;
    data.buffer_num = 0;
    data.buffer_num_isp = 0;
    data.isp_control_index = 0;
    data.test_fps = false;
    data.bus_error = false;
    gettimeofday(&(data.start_time), NULL);
    gettimeofday(&(data.last_time), NULL);
    if (check && check->check_fps)
        data.test_fps = true;

    //create pipeline, elements, and set the property
    pipeline = gst_pipeline_new ("pipeline");
    if (!pipeline) {
        g_print("Failed to ctreat pipeline\n");
        return -1;
    }
    for (i = 0; g_plugin[i].plugin_name != NULL; i++) {
        char element_name[20];
        if (i == 0)
        {
            snprintf(element_name, 20, "camerasrc0");
        }
        else
        {
            snprintf(element_name, 20, "element-%d", i);
        }
        elements[i] = gst_element_factory_make(g_plugin[i].plugin_name, element_name);
        if (!elements[i]) {
            gst_object_unref(GST_OBJECT(pipeline));
            g_print("Failed to create source element\n");
            return -1;
        }
        /* Init interface handle for icamerasrc plugin */
        if (strcmp(g_plugin[i].plugin_name, "icamerasrc") == 0) {
            if (check && check->check_isp_interface) {
                data.camisp = GST_CAMERASRC_ISP(elements[i]);
                data.cameraisp_interface = GST_CAMERASRC_ISP_GET_INTERFACE(data.camisp);
                if(NULL == data.cameraisp_interface)
                {
                    g_print("get interface failed!\n");
                    return -1;
                }
            }
        }
        if (g_plugin[i].pro_attrs != NULL)
            set_elements_properties(elements[i], g_plugin[i].pro_attrs);
        if (g_plugin[i].pro_attrs_test != NULL)
            if (!validate_elements_properties(elements[i], g_plugin[i].pro_attrs_test, type))
            {
                g_print("Validate property setting failed\n");
                return -1;
            }
        if (g_plugin[i].caps_filter != NULL)
            caps[i] = gst_caps_from_string(g_plugin[i].caps_filter);

        gst_bin_add_many(GST_BIN(pipeline), elements[i], NULL);
        if (i > 0 && elements[i - 1]) {
            if (caps[i - 1] != NULL) {
                if(!gst_element_link_filtered(elements[i - 1], elements[i], caps[i - 1])) {
                    gst_object_unref(GST_OBJECT(pipeline));
                    g_print("Failed to link element with caps_filter:%s\n", g_plugin[i - 1].caps_filter);
                    return -1;
                }
                gst_caps_unref(caps[i - 1]);
            } else {
                if (!gst_element_link_many(elements[i - 1], elements[i], NULL)) {
                    gst_object_unref(GST_OBJECT(pipeline));
                    g_print("Failed to link many elements\n");
                    return -1;
                }
            }
        }
        plugin_num++;
    }
    g_print("plugin_num: %d\n", plugin_num);

    //add a probe to check fps of the last element.
    if (NULL != check) {
        GstPad * pad;
        pad = gst_element_get_static_pad(elements[plugin_num - 1], "sink");
        if (!pad) {
            gst_object_unref(GST_OBJECT(pipeline));
            g_print("Failed to get the pad\n");
            return -1;
        }
        if(data.test_fps)
        {
            gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER,
                    callback_have_data, &data, NULL);
        }

        if(data.check_isp_interface == true)
        {
            g_print("func:%s line:%d\n",__func__,__LINE__);
            gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER,
                    callback_check_isp_interface, &data, NULL);

        }
        gst_object_unref(pad);
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus,bus_call, &data);
    gst_object_unref(bus);

    g_print("starting sender pipeline\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run(data.main_loop);

    g_print("Returned, stopping the loop\n");
    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref (data.main_loop);
    if (data.bus_error == true) {
        g_print("gstreamer bus error\n");
        return -1;
    }

    if (data.test_fps)
        check->info.fps = data.fps;

    return 0;
}
inline int get_random_value(int min,int max)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    srand((unsigned)tv.tv_usec);
    return (rand()%(max+1 - min)) + min;
}

void init_ltm_tuning_data(ltm_tuning_data *data)
{
    for(int i = 0; i < LTM_TUNING_TEST_NUM; i++)
    {
    	memset(&data[i],0,sizeof(ltm_tuning_data));
        data[i].algo_mode = ltm_algo_optibright_gain_map;
        data[i].optibright_tuning.GTM_Str = 120;
        data[i].optibright_tuning.GF_epspar = 2621;
        data[i].optibright_tuning.alpham1 = 32767;
        data[i].optibright_tuning.alpham = 21299;
        data[i].optibright_tuning.maskmin = 0;
        data[i].optibright_tuning.maskmax = 4915;
        if( i % 2 == 0 )
            data[i].optibright_tuning.num_iteration = 255;
        else
            data[i].optibright_tuning.num_iteration = 16;
        data[i].optibright_tuning.maskmid = 4915;
        data[i].optibright_tuning.hlc_mode = 0;
        data[i].optibright_tuning.max_isp_gain = 32;
        data[i].optibright_tuning.convergence_speed = 1229;
        data[i].optibright_tuning.lm_treatment = 22938;
        data[i].optibright_tuning.GTM_mode = 1;
        data[i].optibright_tuning.pre_gamma = 60;
        data[i].optibright_tuning.lav2p_scale = 5;
        data[i].optibright_tuning.p_max = 9830;
        data[i].optibright_tuning.p_mode = 0;
        data[i].optibright_tuning.p_value = 9830;
        data[i].optibright_tuning.filter_size = 0;
        data[i].optibright_tuning.max_percentile = 32604;
        data[i].optibright_tuning.ldr_brightness = 10650;
        data[i].optibright_tuning.dr_mid = 7022;
        data[i].optibright_tuning.dr_norm_max = 7168;
        data[i].optibright_tuning.dr_norm_min = 0;
        data[i].optibright_tuning.convergence_speed_slow = 8192;
        data[i].optibright_tuning.convergence_sigma = 4915;
        data[i].optibright_tuning.gainext_mode = 1;
        data[i].optibright_tuning.wdr_scale_max = 12288;
        data[i].optibright_tuning.wdr_scale_min = 1024;
        data[i].optibright_tuning.wdr_gain_max = 16384;
        data[i].optibright_tuning.frame_delay_compensation = 1;
        data[i].mpgc_tuning.lm_stability = 3277;
        data[i].mpgc_tuning.lm_sensitivity = 16;
        data[i].mpgc_tuning.blur_size = 1;
        data[i].mpgc_tuning.tf_str = 6553;
        data[i].drcsw_tuning.blus_sim_sigma = 8192;
    }

}

void init_sc_iefd_t(struct camera_control_isp_sc_iefd_t *data_t,int num)
{

    for(int i = 0;i < num; i++)
    {
        struct camera_control_isp_sc_iefd_t *data = (struct camera_control_isp_sc_iefd_t*)(data_t + i);
        for(int j = 0; j < 2; j++)
        {
            data->sharpening_power[j]     = get_random_value(0,127);
            data->cu_ed2_metric_th[j]     = get_random_value(0,511);
            data->cu_nr_power[j]          = get_random_value(0,64);
            data->dir_far_sharp_weight[j] = get_random_value(0,64);
        }
        data->sharp_power_edge       = get_random_value(0,64);
        data->sharp_power_detail     = get_random_value(0,64);
        data->unsharp_weight_edge    = get_random_value(0,64);
        data->unsharp_weight_detail  = get_random_value(0,64);
        data->denoise_power          = get_random_value(0,64);
        data->radial_denoise_power   = get_random_value(0,64);
        data->shrpn_nega_lmt_txt     = get_random_value(0,8191);
        data->shrpn_posi_lmt_txt     = get_random_value(0,8191);
        data->shrpn_nega_lmt_dir     = get_random_value(0,8191);
        data->shrpn_posi_lmt_dir     = get_random_value(0,8191);
        for(int j = 0; j < 4; j++)
        {
            data->cu_var_metric_th[j] = get_random_value(0,511);
        }
        data->rad_enable = get_random_value(0,1);
        for(int j = 0; j < 15; j++)
        {
            data->unsharp_filter[j] = get_random_value(-256,255);
        }
        for(int j = 0; j < 6; j++)
        {
            data->configunited_x[j] = get_random_value(0,511);
            data->denoise_filter[j] = get_random_value(-255,255);
            data->configunitradial_y[j] = get_random_value(0,255);
            data->configunitradial_x[j] = get_random_value(0,255);
            data->configunited_y[j] = get_random_value(0,511);
        }
        data->vssnlm_x0 = get_random_value(0,255);
        data->vssnlm_x1 = get_random_value(0,255);
        data->vssnlm_x2 = get_random_value(0,255);
        data->vssnlm_y1 = get_random_value(0,8);
        data->vssnlm_y2 = get_random_value(0,8);
        data->vssnlm_y3 = get_random_value(0,8);
    }
}
void init_tnr5_21_t(struct camera_control_isp_tnr5_21_t *data_t,int num)
{
    for(int i = 0; i < num; i++)
    {
        struct camera_control_isp_tnr5_21_t *data = (struct camera_control_isp_tnr5_21_t*)(data_t +i);
        data->bypass = get_random_value(0,1);
        for(int j = 0;j < 64; j++)
        {
            data->nm_yy_xcu_b[j] = get_random_value(-32768,32767);
            data->nm_yc_xcu_b[j] = get_random_value(-32768,32767);
            data->nm_cy_xcu_b[j] = get_random_value(-32768,32767);
            data->nm_cc_xcu_b[j] = get_random_value(-32768,32767);
        }
        data->nm_y_log_est_min_b = get_random_value(-32768,32767);
        data->nm_y_log_est_max_b = get_random_value(-32768,32767);
        data->nm_c_log_est_min_b = get_random_value(-32768,32767);
        data->nm_c_log_est_max_b = get_random_value(-32768,32767);
        data->nm_Y_alpha_b       = get_random_value(-32768,32767);
        data->nm_C_alpha_b       = get_random_value(-32768,32767);
        data->Tnr_Strength_0     = get_random_value(0,100);
        data->Tnr_Strength_1     = get_random_value(0,100);
        data->SpNR_Static        = get_random_value(0,100);
        data->SpNR_Dynamic       = get_random_value(0,100);
        data->Radial_Gain        = get_random_value(0,100);
        data->SAD_Gain           = get_random_value(0,100);
        data->Pre_Sim_Gain       = get_random_value(0,100);
        data->Weight_In          = get_random_value(0,100);
        data->g_mv_x             = get_random_value(-128,127);
        data->g_mv_y             = get_random_value(-128,127);
        data->tbd_sim_gain       = get_random_value(5,128);
        data->NS_Gain            = get_random_value(0,100);
        data->nsw_gain           = get_random_value(0,100);
        data->nsw_sigma          = get_random_value(0,100);
        data->nsw_bias           = get_random_value(0,100);
        data->ns_clw_bias0       = get_random_value(0,100);
        data->ns_clw_bias1       = get_random_value(0,100);
        data->ns_clw_sigma       = get_random_value(0,100);
        data->ns_clw_center      = get_random_value(0,100);
        data->ns_norm_bias       = get_random_value(0,32767);
        data->ns_norm_coef       = get_random_value(-32768,32767);
        data->bypass_g_mv        = get_random_value(0,1);
        data->bypass_NS          = get_random_value(1,1);
}
}
void init_xnr_dss_t(struct camera_control_isp_xnr_dss_t *data_t,int num)
{
    for(int i = 0; i < num; i++)
    {
        struct camera_control_isp_xnr_dss_t *data=(struct camera_control_isp_xnr_dss_t *)(data_t+i);
        data->rad_enable                 = get_random_value(0,1);
        data->bypass                     = get_random_value(0,1);
        data->bypass_mf_y                = get_random_value(0,1);
        data->bypass_mf_c                = get_random_value(0,1);
        data->spatial_sigma_mf_y         = get_random_value(0,32767);
        data->spatial_sigma_mf_c         = get_random_value(0,32767);
        data->noise_white_mf_y_y         = get_random_value(0,4194304);
        data->noise_white_mf_y_s         = get_random_value(0,4194304);
        data->noise_white_mf_c_y         = get_random_value(0,4194304);
        data->noise_white_mf_c_u         = get_random_value(0,4194304);
        data->noise_white_mf_c_v         = get_random_value(0,4194304);
        data->rad_noise_power_mf_luma    = get_random_value(0,32768);
        data->rad_noise_power_mf_chroma  = get_random_value(0,32767);
        for(int j = 0; j < 7; j++)
        {
            data->range_weight_lut[j] = get_random_value(0,32767);
        }
        for(int j = 0; j < 64; j++)
        {
            data->one_div_64_lut[j] = get_random_value(0,256);
        }
        data->rad_noise_compensation_mf_chroma  = get_random_value(0,32767);
        data->rad_noise_compensation_mf_luma    = get_random_value(0,32767);
        data->mf_luma_power                     = get_random_value(0,32767);
        data->rad_mf_luma_power                 = get_random_value(0,32767);
        data->mf_chroma_power                   = get_random_value(0,32767);
        data->rad_mf_chroma_power               = get_random_value(0,32767);
        data->noise_black_mf_y_y                = get_random_value(0,4194304);
        data->noise_black_mf_c_y                = get_random_value(0,4194304);
        data->noise_black_mf_c_u                = get_random_value(0,4194304);
        data->noise_black_mf_c_v                = get_random_value(0,4194304);
        for(int j = 0; j < 33; j++)
        {
            data->xcu_lcs_x[j]  = get_random_value(0,32767);
        }
        data->xcu_lcs_exp       = get_random_value(0,15);
        data->xcu_lcs_slp_a_res = get_random_value(0,31);
         for(int j = 0; j < 32; j++)
        {
            data->xcu_lcs_offset[j] = get_random_value(0,32767);
            data->xcu_lcs_slope[j]  = get_random_value(0,32767);
        }
        data->lcs_th_for_black      = get_random_value(0,32767);
        data->lcs_th_for_white      = get_random_value(0,32767);
        data->rad_lcs_th_for_black  = get_random_value(0,32767);
        data->rad_lcs_th_for_white  = get_random_value(0,32767);
        data->blnd_hf_power_y       = get_random_value(0,32767);
        data->blnd_hf_power_c       = get_random_value(0,32767);
}
}
void init_bnlm_t(struct camera_control_isp_bnlm_t *data_t,int num)
{
    for(int i = 0;i < num; i++)
    {
        struct camera_control_isp_bnlm_t *data = (struct camera_control_isp_bnlm_t*)(data_t+i);
        data->bypass           = get_random_value(1,1);
        data->detailix_radgain = get_random_value(0,131071);
        for(int j = 0; j < 2; j++)
        {
            data->detailix_x_range[j] = get_random_value(0,16383);
            data->sad_mu_x_range[j]   = get_random_value(0,16383);
        }
        data->sad_radgain = get_random_value(0,131071);
        for(int j = 0; j < 3; j++)
        {
            data->detailix_coeffs[j] = get_random_value(0,1073741824);
            data->sad_mu_coeffs[j]   = get_random_value(0,1073741824);
            data->detailth[j]        = get_random_value(0,16383);
        }
       for(int j = 0; j < 4; j++)
       {
            data->sad_spatialrad[j]      = get_random_value(0,255);
            data->sad_detailixlutx[j]    = get_random_value(0,16383);
            data->sad_detailixluty[j]    = get_random_value(0,16383);
            data->numcandforavg[j]       = get_random_value(0,16);
            data->blend_power[j]         = get_random_value(0,65535);
            data->blend_th[j]            = get_random_value(0,1023);
            data->blend_texturegain[j]   = get_random_value(0,16777216);
            data->matchqualitycands[j]   = get_random_value(0,65535);
        }
        data->blend_radgain = get_random_value(0,131071);
        data->wsumminth     = get_random_value(0,65535);
        data->wmaxminth     = get_random_value(0,16383);
        data->rad_enable    = get_random_value(0,1);
}
}
void init_gama_tone_map_t(struct camera_control_isp_gamma_tone_map_t *data_t,int num)
{
    for(int i = 0; i < num; i++)
    {
        struct camera_control_isp_gamma_tone_map_t *data = (struct camera_control_isp_gamma_tone_map_t*)(data_t + i);
        for(int j = 0; j < 1024; j++)
        {
            data->gamma[j]    = get_random_value(0,1); //0 1
            data->tone_map[j] = get_random_value(0,4); //0 4
        }
        data->gamma_lut_size    = get_random_value(0,1024);  //0 1024
        data->tone_map_lut_size = get_random_value(513,1024);  //513 1024
    }
}
bool get_checkfield_data(CheckField *check,PROPERTY_CHECK_TYPE type)
{
    g_print("func:%s line:%d\n",__func__,__LINE__);
    bool ret = false;
    switch(type)
    {
       case PROPERTY_CHECK_TYPE_WB_GAIN:
            {
                check->isp_control.data = (void *)&isp_wb_gain_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_wb_gains;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_COLOR_CORRECTION:
            {
                check->isp_control.data = (void *)&isp_color_correction_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_color_correction_matrix;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_BXT_DEMOSAIC:
            {
                check->isp_control.data = (void *)&isp_bxt_demosaic_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_bxt_demosaic;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_SC_IEFD:
            {
                init_sc_iefd_t(isp_sc_iefd_data,ISP_CONTROL_TEST_NUM);
                check->isp_control.data = (void *)&isp_sc_iefd_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_sc_iefd;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_SEE:
            {
                check->isp_control.data = (void *)&isp_see_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_see;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_TNR5_21:
            {
                init_tnr5_21_t(isp_tnr5_21_data,ISP_CONTROL_TEST_NUM);
                check->isp_control.data = (void *)&isp_tnr5_21_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_tnr5_21;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_XNR_DSS:
            {
                init_xnr_dss_t(isp_xnr_dss_data,ISP_CONTROL_TEST_NUM);
                check->isp_control.data = (void *)&isp_xnr_dss_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_xnr_dss;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_GAMMA_TONE_MAP:
            {
                init_gama_tone_map_t(isp_gama_tone_map_data,ISP_CONTROL_TEST_NUM);
                check->isp_control.data = (void *)&isp_gama_tone_map_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_gamma_tone_map;
                ret = true;
            }
            break;
       case PROPERTY_CHECK_TYPE_BNLM:
            {
                init_bnlm_t(isp_bnlm_data,ISP_CONTROL_TEST_NUM);
                check->isp_control.data = (void *)&isp_bnlm_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_bnlm;
                ret = true;
            }
            break;
        case PROPERTY_CHECK_TYPE_CSC:
            {
                check->isp_control.data = (void *)&isp_bxt_csc_data;
                check->check_isp_interface = true;
                check->isp_control.type = camera_control_isp_ctrl_id_bxt_csc;
                ret = true;
            }
            break;
        case PROPERTY_CHECK_TYPE_LTM_TUNING:
            {
                init_ltm_tuning_data(ltm_tuning_data_);
                check->isp_control.data = (void *)ltm_tuning_data_;
                check->check_isp_interface = true;
                check->isp_control.type = LTM_TUNING_DATA_TAG;
                ret = true;
            }
            break;
        default:
            check = NULL;
            ret = false;
            break;
    }
    return ret;
}

