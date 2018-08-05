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

#include "gstCaffAutoTestConfig.h"

DATA_PARSER sub_result_settings[] = {
    { offsetof(SUB_RESULT, result), "%s", "result", 0, 0, 0 },
    { offsetof(SUB_RESULT, error_message), "%s", "error_message", 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER case_result_settings[] = {
    { offsetof(CASE_RESULT, status),                   "%d", "status", 0, 2, 0 },
    { offsetof(CASE_RESULT, loop_count_completed),     "%d", "loop_count_completed", 0, 1000000, 0 },
    { offsetof(CASE_RESULT, start_time),               "%s", "start_time", 0, 0, 0 },
    { offsetof(CASE_RESULT, end_time),                 "%s", "end_time", 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER case_info_settings[] = {
    { offsetof(CASE_INFO, case_name),       "%s", "case_name", 0, 0, 0 },
    { offsetof(CASE_INFO, description),     "%s", "description", 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER stress_config_settings[] = {
    { offsetof(STRESS_CONFIG, loop_count),           "%d", "loop_count", 0, 1000000, 1 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER camera_settings[] = {
    { offsetof(CAM_CONFIG, duration),  "%d", "duration", 0, 1000000, 1 },
    {0, NULL, NULL, 0, 0, 0}
};

DATA_PARSER camera_common_config_settings[] = {
    { offsetof(CAMERA_COMMON_CONFIG, width), "%d", "width", 0, 10000, 1920 },
    { offsetof(CAMERA_COMMON_CONFIG, height), "%d", "height", 0, 10000, 1080 },
    { offsetof(CAMERA_COMMON_CONFIG, pixelformat), "%s", "pixelformat", 0, 0, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, num_buffers), "%d", "num-buffers", -1, 2147483647, 100 },
    { offsetof(CAMERA_COMMON_CONFIG, printfps), "%d", "printfps", 0, 1, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, capture_mode), "%s", "capture-mode", 0, 0, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, io_mode), "%s", "io-mode", 0, 0, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, interlace_mode), "%s", "interlace-mode", 0, 0, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, deinterlace_method), "%s", "deinterlace-method", 0, 0, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, framerate), "%s", "framerate", 0, 0, 0 },
    { offsetof(CAMERA_COMMON_CONFIG, fisheye_mode), "%s", "fisheye-dewarping-mode", 0, 0, 0 },
    //{ offsetof(CAMERA_COMMON_CONFIG, device_name), "%s", "device-name", 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER camera_3a_control_config_settings[] = {
    { offsetof(CAMERA_3A_CONTROL_CONFIG, sharpness), "%d", "sharpness", -1, 2147483647, 100 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, brightness), "%d", "brightness", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, contrast), "%d", "contrast", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, hue), "%d", "hue", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, saturation), "%d", "saturation", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, iris_level), "%d", "iris-level", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, exposure_time), "%d", "exposure-time", 0, 1000000, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, gain), "%f", "gain", 0.0f, 60.0f, 0.0f },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, ev), "%d", "ev", -4, 4, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, wdr_level), "%d", "wdr-level", 0, 200, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, cct_range_min), "%d", "cct-range-min", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, cct_range_max), "%d", "cct-range-max", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_shift_r), "%d", "awb-shift-r", 0, 255, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_shift_g), "%d", "awb-shift-g", 0, 255, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_shift_b), "%d", "awb-shift-b", 0, 255, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_gain_r), "%d", "awb-gain-r", 0, 255, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_gain_g), "%d", "awb-gain-g", 0, 255, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_gain_b), "%d", "awb-gain-b", 0, 255, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, nr_filter_level), "%d", "nr-filter-level", 0, 100, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, day_night_mode), "%s", "day-night-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, iris_mode), "%s", "iris-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, ae_mode), "%s", "ae-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, wdr_mode), "%s", "wdr-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, blc_area_mode), "%s", "blc-area-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, awb_mode), "%s", "awb-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, wp_point), "%s", "wp-point", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, nr_mode), "%s", "nr-mode", 0, 0, 0 },
    { offsetof(CAMERA_3A_CONTROL_CONFIG, scene_mode), "%s", "scene-mode", 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER S_FEATURE_SETTINGs[] = {
    { offsetof(S_FEATURE_SETTING, feature_name),    "%s", "feature_name", 0, 0, 0 },
    { offsetof(S_FEATURE_SETTING, values),          "%s", "values", 0, 0, 0 },
    { offsetof(S_FEATURE_SETTING, value_format),    "%s", "value_type", 0, 0, 0 },
    { offsetof(S_FEATURE_SETTING, value_count),     "%d", "value_count", 0, 32, 5 },
    { offsetof(S_FEATURE_SETTING, duration_each),   "%d", "duration_each", 0, 1000000000, 500 },
    { offsetof(S_FEATURE_SETTING, frame_count_each),"%d", "frame_count_each", 1, 1000000, 1 },
    { offsetof(S_FEATURE_SETTING, loop_mode),       "%d", "loop_mode", 0, 1, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_PARSER analyzer_settings[] = {
    { offsetof(ANALYZER_CONFIG, analyzer_config),        "%s", "config_path", 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0 }
};

DATA_TAG cam_common_tag = { "common_settings", offsetof(CAM_CONFIG, common_config), offsetof(CAMERA_COMMON_CONFIG, enable), camera_common_config_settings, NULL };

DATA_TAG cam_3a_control_tag = { "manual_3a_control", offsetof(CAM_CONFIG, cam_3a_control_config), offsetof(CAMERA_3A_CONTROL_CONFIG, enable), camera_3a_control_config_settings, NULL };

DATA_TAG analyzer_tag = { "analyzer", offsetof(CAM_CONFIG, analyzer), offsetof(ANALYZER_CONFIG, enable), analyzer_settings, NULL };

DATA_TAG performence_tag = { "performance", offsetof(CAM_CONFIG, perf), offsetof(ANALYZER_CONFIG, enable), analyzer_settings, NULL };

DATA_TAG convergency_tag = { "convergency_3a", offsetof(CAM_CONFIG, convergency_3a), offsetof(ANALYZER_CONFIG, enable), analyzer_settings, NULL };

DATA_TAG green_corruption_tag = { "green_corruption", offsetof(CAM_CONFIG, green_corruption_analyzer), offsetof(ANALYZER_CONFIG, enable), analyzer_settings, NULL };

DATA_TAG color_cast_tag = { "color_cast", offsetof(CAM_CONFIG, color_cast_analyzer), offsetof(ANALYZER_CONFIG, enable), analyzer_settings, NULL };

DATA_TAG orientation_analyzer_tag = { "orientation_analyzer", offsetof(CAM_CONFIG, orientation_analyzer), offsetof(ANALYZER_CONFIG, enable), analyzer_settings, NULL };

DATA_TAG feature_tag = { "feature", offsetof(CAM_CONFIG, feature), offsetof(S_FEATURE_SETTING, enable), S_FEATURE_SETTINGs, NULL };

DATA_TAG camera_child_tags[] = { cam_common_tag, cam_3a_control_tag, feature_tag, performence_tag, convergency_tag, analyzer_tag, green_corruption_tag, orientation_analyzer_tag, color_cast_tag, { NULL, 0, -1, NULL, NULL } };

DATA_TAG stress_tag = { "stress", offsetof(CASE_CONFIG, stress_config), offsetof(STRESS_CONFIG, enable), stress_config_settings, NULL };

DATA_TAG case_info_tag = { "case_info", offsetof(CASE_CONFIG, case_info), offsetof(CASE_INFO, enable), case_info_settings, NULL };

DATA_TAG basic_result_tag = { "basic", offsetof(CASE_RESULT, basic_result), offsetof(SUB_RESULT, enable), sub_result_settings, NULL };

DATA_TAG performance_result_tag = { "performance", offsetof(CASE_RESULT, performance_result), offsetof(SUB_RESULT, enable), sub_result_settings, NULL };

DATA_TAG feature_result_tag = { "feature", offsetof(CASE_RESULT, feature_result), offsetof(SUB_RESULT, enable), sub_result_settings, NULL };

DATA_TAG result_child_tags [] = {
    basic_result_tag,
    performance_result_tag,
    feature_result_tag,
    { NULL, 0, -1, NULL, NULL } };

DATA_TAG case_reslut_tag = { "result", offsetof(CASE_CONFIG, case_result), offsetof(CASE_RESULT, enable), case_result_settings, result_child_tags };

DATA_TAG case_config_childs[] = { case_reslut_tag, stress_tag, case_info_tag, { NULL, 0, -1, NULL, NULL } };

CCaseInfo::CCaseInfo(
    const char *case_id)
{
    m_case_id = NULL;
    SetCaseID(case_id);
    m_comment_index = 0;
}

CCaseInfo::~CCaseInfo()
{
    LOG_VERBOSE("Enter", NULL);

    // Create a stack
    vector<DATA_TAG_NODE> vtag_nodes;
    int i = 0;
    while (case_config_childs != NULL && case_config_childs[i].tag_name != NULL)
    {
        DATA_TAG_NODE node = { &case_config_childs[i], NULL, &m_case_config };
        vtag_nodes.push_back(node);
        i++;
    }
    Depth_first_delete(vtag_nodes);

    for (size_t q = 0; q < m_case_config.multi_cam_configs.size(); q++)
    {
        for (int p = 0; p < MAX_CONCURRENT_CAM_COUNT; p++)
        {
            string camera_name(CAMERA_NAME);
            stringstream camera_index;
            string camera_postfix;
            camera_index << p;
            camera_index >> camera_postfix;
            camera_name += camera_postfix;
            DATA_TAG camera_tag = { camera_name.c_str(), 0, offsetof(CAM_CONFIG, enable), camera_settings, camera_child_tags };
            DATA_TAG_NODE local_node = { &camera_tag, NULL, &m_case_config.multi_cam_configs[q].cam[p] };
            vector<DATA_TAG_NODE> vlocal_nodes;
            vlocal_nodes.push_back(local_node);
            Depth_first_delete(vlocal_nodes);
        }
    }


    if (m_case_id != NULL)
    {
        free(m_case_id);
    }
    LOG_VERBOSE("Leave", NULL);
}

void CCaseInfo::Depth_first_delete(vector<DATA_TAG_NODE> nodes)
{
    size_t vsize = 0;
    while ((vsize = nodes.size()) > 0)
    {
        // pop up first elment in the stack
        DATA_TAG_NODE node = nodes[vsize - 1];
        nodes.pop_back();

        // struct pointer
        void *pdata = (char*)node.pdata + node.tag->offset_org;

        for (DATA_PARSER *p = node.tag->pdata_parser; p->description != NULL; p++)
        {
            if (p->format[1] == 's')
            {
                char **pptr = (char **)((char*)pdata + p->offset);
                if (*pptr != NULL)
                {
                    //LOG_INFO(CStringFormat("Free data: %s @0x%x", p->description, *pptr).Get(), NULL);
                    free(*pptr);
                    *pptr = NULL;
                }
            }
        }

        int i = 0;

        //Push back all the childs
        while (node.tag->childs != NULL && node.tag->childs[i].tag_name != NULL)
        {
            DATA_TAG_NODE temp = { &node.tag->childs[i], NULL, pdata };
            nodes.push_back(temp);
            i++;
        }
    }
}
size_t CCaseInfo::GetMultiCamConfigCount()
{
    return m_case_config.multi_cam_configs.size();
}

MULTI_CAM_CONFIG* CCaseInfo::GetMultiCamConfigByIndex(size_t index)
{
    if (index >= m_case_config.multi_cam_configs.size())
    {
        return NULL;
    }

    return &(m_case_config.multi_cam_configs[index]);
}

CASE_CONFIG* CCaseInfo::GetCaseConfig()
{
    return &m_case_config;
}

const char* CCaseInfo::GetCaseID()
{
    return m_case_id;
}

bool CCaseInfo::IsResultEnabled()
{
    if (m_case_config.case_result.enable)
    {
        return true;
    }
    return false;
}

void CCaseInfo::SetCaseID(
    const char *case_id)
{
    assert(case_id != NULL);

    if (m_case_id != NULL)
    {
        free(m_case_id);
    }

    m_case_id = _strdup(case_id);
}

void CCaseInfo::SetStatus(
    int status)
{
    m_case_config.case_result.status = status;
}

int CCaseInfo::GetStatus()
{
    return m_case_config.case_result.status;
}

const char* CCaseInfo::GetExeStartTime()
{
    return m_case_config.case_result.start_time;
}

const char* CCaseInfo::GetExeStopTime()
{
    return m_case_config.case_result.end_time;
}

void CCaseInfo::SetExeStartTime(const char *time)
{
    assert(time != NULL);

    if (m_case_config.case_result.start_time != NULL)
    {
        free(m_case_config.case_result.start_time);
        m_case_config.case_result.start_time = NULL;
    }

    if (time != NULL)
    {
        m_case_config.case_result.start_time = _strdup(time);
    }
}

void CCaseInfo::SetExeStopTime(const char *time)
{
    assert(time != NULL);

    if (m_case_config.case_result.end_time != NULL)
    {
        free(m_case_config.case_result.end_time);
    }

    if (time != NULL)
    {
        m_case_config.case_result.end_time = _strdup(time);
    }
}

const char* CCaseInfo::GetBasicErrorMessage()
{
    return m_case_config.case_result.basic_result.error_message;
}
const char* CCaseInfo::GetPerformanceErrorMessage()
{
    return m_case_config.case_result.performance_result.error_message;
}
const char* CCaseInfo::GetFeatureErrorMessage()
{
    return m_case_config.case_result.feature_result.error_message;
}

int CCaseInfo::GetExecutionBasicResult()
{
    if (m_case_config.case_result.basic_result.result == NULL ||
        strcmp(m_case_config.case_result.basic_result.result, "null") == 0)
    {
        return 0;
    }
    else if (strcmp(m_case_config.case_result.basic_result.result, "pass") == 0)
    {
        return 1;
    }
    else if (strcmp(m_case_config.case_result.basic_result.result, "fail") == 0)
    {
        return -1;
    }

    return 0;
}
int CCaseInfo::GetExecutionPerformanceResult()
{
    if (m_case_config.case_result.performance_result.result == NULL ||
        strcmp(m_case_config.case_result.performance_result.result, "null") == 0)
    {
        return 0;
    }
    else if (strcmp(m_case_config.case_result.performance_result.result, "pass") == 0)
    {
        return 1;
    }
    else if (strcmp(m_case_config.case_result.performance_result.result, "fail") == 0)
    {
        return -1;
    }

    return 0;
}

int CCaseInfo::GetExecutionFeatureResult()
{
    if (m_case_config.case_result.feature_result.result == NULL ||
        strcmp(m_case_config.case_result.feature_result.result, "null") == 0)
    {
        return 0;
    }
    else if (strcmp(m_case_config.case_result.feature_result.result, "pass") == 0)
    {
        return 1;
    }
    else if (strcmp(m_case_config.case_result.feature_result.result, "fail") == 0)
    {
        return -1;
    }

    return 0;
}

void CCaseInfo::SetExecutionBasicResult(bool bpass)
{
    if (m_case_config.case_result.basic_result.result != NULL)
    {
        free(m_case_config.case_result.basic_result.result);
        m_case_config.case_result.basic_result.result = NULL;
    }

    m_case_config.case_result.basic_result.result = _strdup(bpass ? "pass" : "fail");
    SetBasicErrorMessage(m_basic_error.c_str());
}
void CCaseInfo::SetExecutionPerformanceResult(bool bpass)
{
    if (m_case_config.case_result.performance_result.result != NULL)
    {
        free(m_case_config.case_result.performance_result.result);
        m_case_config.case_result.performance_result.result = NULL;
    }

    m_case_config.case_result.performance_result.result = _strdup(bpass ? "pass" : "fail");
    SetPerformanceErrorMessage(m_performance_error.c_str());
}
void CCaseInfo::SetExecutionFeatureResult(bool bpass)
{
    if (m_case_config.case_result.feature_result.result != NULL)
    {
        free(m_case_config.case_result.feature_result.result);
        m_case_config.case_result.feature_result.result = NULL;
    }

    m_case_config.case_result.feature_result.result = _strdup(bpass ? "pass" : "fail");
    SetFeatureErrorMessage(m_feature_error.c_str());
}

void CCaseInfo::AddBasicErrorMessage(const char* message)
{
    assert(message != NULL);
    if (m_basic_error.size() >= MAX_ERROR_MESSAGE_LEN)
    {
        return;
    }
    m_basic_error += message;
    m_basic_error += "; ";
}

void CCaseInfo::AddPerformanceErrorMessage(const char* message)
{
    assert(message != NULL);
    if (m_performance_error.size() >= MAX_ERROR_MESSAGE_LEN)
    {
        return;
    }
    m_performance_error += message;
    m_performance_error += "; ";
}

void CCaseInfo::AddFeatureErrorMessage(const char* message)
{
    assert(message != NULL);
    if (m_feature_error.size() >= MAX_ERROR_MESSAGE_LEN)
    {
        return;
    }
    m_feature_error += message;
    m_feature_error += "; ";
}

void CCaseInfo::SetBasicErrorMessage(const char* message)
{
    assert(message != NULL);

    if (m_case_config.case_result.basic_result.error_message != NULL)
    {
        free(m_case_config.case_result.basic_result.error_message);
        m_case_config.case_result.basic_result.error_message = NULL;
    }

    if (strlen(message) > 0)
    {
        m_case_config.case_result.basic_result.error_message = _strdup(message);
    }
}

void CCaseInfo::SetPerformanceErrorMessage(const char* message)
{
    assert(message != NULL);

    if (m_case_config.case_result.performance_result.error_message != NULL)
    {
        free(m_case_config.case_result.performance_result.error_message);
        m_case_config.case_result.performance_result.error_message = NULL;
    }

    if (strlen(message) > 0)
    {
        m_case_config.case_result.performance_result.error_message = _strdup(message);
    }
}

void CCaseInfo::SetFeatureErrorMessage(const char* message)
{
    assert(message != NULL);

    if (m_case_config.case_result.feature_result.error_message != NULL)
    {
        free(m_case_config.case_result.feature_result.error_message);
        m_case_config.case_result.feature_result.error_message = NULL;
    }

    if (strlen(message) > 0)
    {
        m_case_config.case_result.feature_result.error_message = _strdup(message);
    }
}

void CCaseInfo::Depth_first_read(vector<DATA_TAG_NODE> nodes)
{
    size_t vsize = 0;
    while ((vsize = nodes.size()) != 0)
    {
        // pop up first elment in the stack
        DATA_TAG_NODE node = nodes[vsize - 1];
        nodes.pop_back();

        // struct pointer
        void *pdata = (char*)node.pdata + node.tag->offset_org;


        // Read data
        TiXmlElement *pNextParent = ReadChildElement(node.node,
            node.tag->pdata_parser, pdata, node.tag->tag_name);

        if (pNextParent == NULL)
        {
            *((bool *)pdata + node.tag->offset_enable) = false;
        }
        else
        {
            *((bool *)pdata + node.tag->offset_enable) = true;
            int i = 0;

            //Push back all the childs
            while (node.tag->childs != NULL && node.tag->childs[i].tag_name != NULL)
            {
                DATA_TAG_NODE temp = { &node.tag->childs[i], pNextParent, pdata };
                nodes.push_back(temp);
                i++;
            }
        }

    }
}
bool CCaseInfo::Read(
    TiXmlDocument *doc)
{
    assert(doc != NULL && m_case_id != NULL);

    // Find case by the case ID in the xml config file
    TiXmlElement *pcase_element = doc->FirstChildElement(m_case_id);

    if (pcase_element == NULL)
    {
        throw CException("%s %d: Cannot find the case ID in the config file",
            __FILE__, __LINE__);
    }

    vector<DATA_TAG_NODE> vtag_nodes;
    int i = 0;
    while (case_config_childs != NULL && case_config_childs[i].tag_name != NULL)
    {
        DATA_TAG_NODE node = { &case_config_childs[i], pcase_element, &m_case_config };
        vtag_nodes.push_back(node);
        i++;
    }
    Depth_first_read(vtag_nodes);

    for (int q = 0; q < MAX_CONCURRENT_CONFIG_COUNT; q++)
    {
        string name(CONFIG_NAME);
        stringstream index;
        string postfix;
        MULTI_CAM_CONFIG temp;

        index << q;
        index >> postfix;
        name += postfix;

        TiXmlElement *pCameraParent = pcase_element->FirstChildElement(name.c_str());
        if (NULL == pCameraParent)
        {
            break;
        }

        for (int p = 0; p < MAX_CONCURRENT_CAM_COUNT; p++)
        {
            string camera_name(CAMERA_NAME);
            stringstream camera_index;
            string camera_postfix;
            camera_index << p;
            camera_index >> camera_postfix;
            camera_name += camera_postfix;
            DATA_TAG camera_tag = { camera_name.c_str(), 0, offsetof(CAM_CONFIG, enable), camera_settings, camera_child_tags };
            DATA_TAG_NODE local_node = { &camera_tag, pCameraParent, &temp.cam[p] };
            vector<DATA_TAG_NODE> vlocal_nodes;
            vlocal_nodes.push_back(local_node);
            Depth_first_read(vlocal_nodes);
        }
        temp.enable = true;
        temp.id = q;
        m_case_config.multi_cam_configs.push_back(temp);
    }

    return true;
}

void CCaseInfo::Depth_first_write(vector<DATA_TAG_NODE> nodes)
{
    size_t vsize = 0;

       // Depth-first traveral
    while ((vsize = nodes.size()) != 0)
    {
        // pop up first elment in the stack
        DATA_TAG_NODE  node = nodes[vsize-1];
        nodes.pop_back();

           // struct pointer
        void *pdata = (char *)node.pdata + node.tag->offset_org;

        if (*((bool *)pdata+node.tag->offset_enable) == true)
        {
            // Write data
            TiXmlElement *pNextParent = WriteChildElement(node.node,
                node.tag->pdata_parser, pdata, node.tag->tag_name);
            int i = 0;

            //Push back all the childs
            while (node.tag->childs != NULL && node.tag->childs[i].tag_name != NULL)
            {
                DATA_TAG_NODE temp = { &node.tag->childs[i], pNextParent, pdata };
                nodes.push_back(temp);
                i++;
            }
        }


    }
}

void CCaseInfo::Write(
    TiXmlDocument *doc)
{
    assert(doc != NULL && m_case_id != NULL);

    m_case_config.case_result.enable = true;

    TiXmlElement *pcase = WriteChildElement(doc, NULL, NULL, m_case_id);

    vector<DATA_TAG_NODE> vtag_nodes;
    int i = 0;
    while (case_config_childs != NULL && case_config_childs[i].tag_name != NULL)
    {
        DATA_TAG_NODE node = { &case_config_childs[i], pcase, &m_case_config };
        vtag_nodes.push_back(node);
        i++;
    }
    Depth_first_write(vtag_nodes);


    for (size_t j = 0; j < m_case_config.multi_cam_configs.size(); j++)
    {
        string name(CONFIG_NAME);
        stringstream index;
        string postfix;
        index << j;
        index >> postfix;
        name += postfix;
        if (m_case_config.multi_cam_configs[j].enable == true)
        {
            TiXmlElement *pconfig = pcase->FirstChildElement(name.c_str());
            if (pconfig == NULL)
            {
                pconfig = (TiXmlElement *)pcase->InsertEndChild(TiXmlElement(name.c_str()));
                if (pconfig == NULL)
                {
                    throw CException("%s %d: Failed to create node: %s", __FILE__, __LINE__, name.c_str());
                }
            }
            for (int p = 0; p < MAX_CONCURRENT_CAM_COUNT; p++)
            {
                string camera_name(CAMERA_NAME);
                stringstream camera_index;
                string camera_postfix;
                camera_index << p;
                camera_index >> camera_postfix;
                camera_name += camera_postfix;
                DATA_TAG camera_tag = { camera_name.c_str(), 0, offsetof(CAM_CONFIG, enable), camera_settings, camera_child_tags };
                DATA_TAG_NODE local_node = { &camera_tag, pconfig, &m_case_config.multi_cam_configs[j].cam[p] };
                vector<DATA_TAG_NODE> vlocal_nodes;
                vlocal_nodes.push_back(local_node);
                Depth_first_write(vlocal_nodes);
            }
        }
    }
}

int CCaseInfo::GetLoopCount()
{
    return m_case_config.stress_config.loop_count;
}

void CCaseInfo::SetLoopCountCompleted(int nloops)
{
    m_case_config.case_result.loop_count_completed = nloops;
}


/***********************************************************************************************************/

const static S_FEATURE_ANALYZER feature_analyzer [] =
{
    { E_FEATURE_TYPE_BRIGHTNESS, "brightness", ANALYZER_BRIGHTNESS_COMPARE },
    { E_FEATURE_TYPE_CONTRAST, "contrast", ANALYZER_CONTRAST_COMPARE },
    { E_FEATURE_TYPE_EXPOSURE_TIME, "exposure_time", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_EXPOSURE_GAIN, "exposure_gain", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_EXPOSURE_EV, "exposure_ev", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_HUE, "hue", ANALYZER_HUE_COMPARE },
    { E_FEATURE_TYPE_WB_MODE, "wb_mode", ANALYZER_WHITEBALANCE_COMPARE },
    { E_FEATURE_TYPE_WB_MANUAL_GAIN, "wb_manual_gain", ANALYZER_WHITEBALANCE_COMPARE },
    { E_FEATURE_TYPE_WB_CCT_RANGE, "wb_cct_range", ANALYZER_WHITEBALANCE_COMPARE },
    { E_FEATURE_TYPE_WB_SHIFT, "wb_shift", ANALYZER_WHITEBALANCE_COMPARE },
    { E_FEATURE_TYPE_HDR, "hdr", ANALYZER_HDR_COMPARE },
    { E_FEATURE_TYPE_SIMILARITY, "similarity", ANALYZER_SIMILARITY_COMPARE },
    { E_FEATURE_TYPE_CONTENT_CONSISTENT, "content_consistent", ANALYZER_SIMILARITY_COMPARE },
    { E_FEATURE_TYPE_SHUTTER_SPEED, "shutter_speed", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_BIAS, "bias", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_ISO, "iso", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_FLASH_MODE, "flash_mode", ANALYZER_BRIGHTNESS_COMPARE},
    { E_FEATURE_TYPE_3A_AUTO_CONSISTENT, "3A_auto_consistent", ANALYZER_3A_AUTO_CONSISTENT},
    { E_FEATURE_TYPE_AE_BRACKETING, "ae_bracketing", ANALYZER_EXPOSURE_COMPARE },
    { E_FEATURE_TYPE_ULL, "ull", ANALYZER_ULL_COMPARE },
    { E_FEATURE_TYPE_GREEN_CORRUPTION_DETECTION, "green_corruption_detection", ANALYZER_GREEN_CORRUPTION},
    { E_FEATURE_TYPE_ORIENTATION_DETECTION, "orientation_detection", ANALYZER_CAMERA_POSITION_COMPARE},
    { E_FEATURE_TYPE_COLOR_CAST, "color_cast", ANALYZER_COLORCAST_COMPARE},
    { E_FEATURE_TYPE_NONE, "none" }
};

const char* GetFeatureNameByFeatureType(E_FEATURE_TYPE feature_type)
{
    for (int i = 0; i < ARRAYSIZE(feature_analyzer); i++)
    {
        if (feature_type == feature_analyzer[i].feature_type)
        {
            return feature_analyzer[i].feature_name;
        }
    }
    return NULL;
}

bool _S_FEATURE_SETTING::GetAnalyzerByFeatureName(S_FEATURE_ANALYZER &fa)
{
    if (feature_name == NULL)
    {
        return false;
    }

    for (int i = 0; i < ARRAYSIZE(feature_analyzer); i++)
    {
        if (strcmp(feature_analyzer[i].feature_name, feature_name) == 0)
        {
            fa = feature_analyzer[i];

            return true;
        }
    }

    return false;
}

int GetFirstValueString(const char *src, char *dst, int maxlen)
{
    assert(src != NULL && dst != NULL);

    //Skip space
    int ispace = 0;
    while (src[ispace] == ' ')
    {
        ispace++;
    }

    int inum = 0;
    for (inum = ispace; src[inum] != 0; inum++)
    {
        if (src[inum] == ',')
        {
            if (maxlen > inum - ispace)
            {
                strncpy_s(dst, maxlen, src + ispace, inum - ispace);
                dst[inum - ispace] = 0;
                return inum + 1;
            }
            else
            {
                EXCEPTION("The dst buffer size is not engugh!");
            }
        }
    }

    if (inum > ispace)
    {
        if (maxlen > inum - ispace)
        {
            strncpy_s(dst, maxlen, src + ispace, inum - ispace);
            dst[inum - ispace] = 0;
            return inum;
        }
        else
        {
            EXCEPTION("The dst buffer size is not engugh!");
        }
    }

    return 0;
}

bool _S_FEATURE_SETTING::GetValue(vector<PARAMETER> &parameters)
{
    AnalyzerParamType pt = PARAM_INT;

    if (strcmp(value_format, "%d") == 0)
    {
        pt = PARAM_INT;
    }
    else if (strcmp(value_format, "%f") == 0)
    {
        pt = PARAM_FLOAT;
    }
    else if (strcmp(value_format, "%lf") == 0)
    {
        pt = PARAM_DOUBLE;
    }
    else
    {
        return false;
    }

    int nstart = 0;
    char *ptr = values;
    char pValue[32] = {};

    PARAMETER p;
    p.type = pt;

    for (int i = 0; i < value_count; i++)
    {
        ptr += nstart;
        nstart = GetFirstValueString(ptr, pValue, 32);

        if (nstart < 0)
        {
            EXCEPTION("Cannot find enough values");
        }

        switch (pt)
        {
        case PARAM_INT:
            p.val.i = atoi(pValue);
            p.cCount = p.val.i;
            break;
        case PARAM_FLOAT:
            p.val.f = (float)atof(pValue);
            break;
        case PARAM_DOUBLE:
            p.val.d = atof(pValue);
            break;
        default:
            break;
        }

        parameters.push_back(p);
    }
    return true;
}

/*************************************
Mode              |  Min   |  Max
-------------------------------------
Daylight          |  5000  |  7000
Partly_overcast   |  5500  |  9000
Fully_overcast    |  6000  |  7000
Fluorescent       |  2700  |  5500
Incandescent      |  2700  |  3100
Sunset            |  2700  |  3000
Video_conference  |  2700  |  6000
Production_test   |  2700  |  6500
Candlelight       |  1800  |  2000
Snow              |  5000  |  12000
Beach             |  5000  |  10000
**************************************/
const static S_AWB_MODE_RANGE awb_mode_temp_range [] = {
        { GST_CAMERASRC_AWB_MODE_AUTO, 0, 0},
        { GST_CAMERASRC_AWB_MODE_PARTLY_OVERCAST, 4500, 12000},
        { GST_CAMERASRC_AWB_MODE_FULLY_OVERCAST, 4500, 12000},
        { GST_CAMERASRC_AWB_MODE_FLUORESCENT, 2700, 5500 },
        { GST_CAMERASRC_AWB_MODE_INCANDESCENT, 2700, 5500 },
        { GST_CAMERASRC_AWB_MODE_SUNSET, 2700, 5500 },
        { GST_CAMERASRC_AWB_MODE_VIDEO_CONFERENCING, 1800, 6000 },
        { GST_CAMERASRC_AWB_MODE_DAYLIGHT, 4000, 7000 }
};

S_AWB_MODE_RANGE GetAwbModeColorTempRange(GstCamerasrcAwbMode mode)
{
    assert(mode > GST_CAMERASRC_AWB_MODE_AUTO && mode <= GST_CAMERASRC_AWB_MODE_DAYLIGHT);
    return awb_mode_temp_range[mode];
}


