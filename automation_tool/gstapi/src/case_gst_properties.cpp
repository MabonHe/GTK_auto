#include "case_gst_properties.h"

int Gst_property_test::m_num_buffers=10;
char Gst_property_test::m_device_name[128]="imx185";
int Gst_property_test::m_width=1920;
int Gst_property_test::m_height=1080;
char Gst_property_test::m_format[16]="NV12";
char Gst_property_test::m_iomode[16]="dma_import";
char Gst_property_test::m_sinktype[16]="vaapi";
char Gst_property_test::m_scene_mode[16]="ull";


#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif


int spilt_parameters(const char *arg, char *key, char *value) {
    char buffer[256];
    strcpy(buffer, arg);
    char *p = strtok(buffer, "=");
    if (!p)
        return -1;
    strcpy(key, p);
    p = strtok(NULL, "=");
    if (!p)
        return -1;
    strcpy(value, p);
    return 0;
}

void Gst_property_test::usage()
{
    g_print("Usage:---------------------------------------------\n");
    g_print("\t--device-name=cameraInput\n");
    g_print("\t--num-buffers=100\n");
    g_print("\t--width=1920\n");
    g_print("\t--height=1080\n");
    g_print("\t--format=NV12\n");
    g_print("\t--iomode=userptr/dma_import\n");
    g_print("\t--sinktype=vaapi/yami\n");
    g_print("\t--scene-mode=auto/ull/hdr/normal/indoor/outdoor/disabled\n");
    g_print("---------------------------------------------------\n");
}

int Gst_property_test::init(int argc, char** argv)
{
    char key[128], value[256];
    for (int i = 1; i < argc; i++) {
        spilt_parameters(argv[i], key, value);
        if ((!strcmp("--help", key)) || (!strcmp("-h", key)))
        {
            usage();
            return -1;
        }
        else if (!strcmp("--device-name", key))
        {
            strncpy(m_device_name, value, 128);
        }
        else if (!strcmp("--format", key))
        {
            strncpy(m_format, value, 16);
        }
        else if (!strcmp("--num-buffers", key))
        {
            m_num_buffers = atoi(value);
        }
        else if (!strcmp("--width", key))
        {
            m_width = atoi(value);
        }
        else if (!strcmp("--height", key))
        {
            m_height = atoi(value);
        }
        else if (!strcmp("--iomode", key))
        {
            strncpy(m_iomode, value, 16);
        }
        else if (!strcmp("--sinktype", key))
        {
            strncpy(m_sinktype, value, 16);
        }
        else if (!strcmp("--scene-mode", key))
        {
            strncpy(m_scene_mode, value, 16);
        }
    }

    g_print("Init parameters------------------------------------\n");
    g_print("\tdevice-name: %s\n", m_device_name);
    g_print("\tnum-buffers: %d\n", m_num_buffers);
    g_print("\twidth: %d\n", m_width);
    g_print("\theight: %d\n", m_height);
    g_print("\tformat: %s\n", m_format);
    g_print("\tiomode: %s\n", m_iomode);
    g_print("\tsinktype: %s\n", m_sinktype);
    g_print("\tscene-mode: %s\n", m_scene_mode);
    g_print("---------------------------------------------------\n");

    return 0;
}

void Gst_property_test::test_property(
    const char *base_property,
    const char *property,
    PROPERTY_CHECK_TYPE type)
{
    int ret = 0;
    bool status = false;
    CheckField checkdata;
    CheckField *check = &checkdata;
    char property_buffer[512] = "";
    char caps_filter_buffer[512] = "";
    char yammi_caps[512] = "";

    g_print("base property: %s, property: %s\n", base_property, property);

    clear_plugins();
    status = get_checkfield_data(check,type);
    if(status == false)
    {
        check = NULL;
    }
    if (base_property != NULL)
        sprintf(property_buffer, "device-name=%s num-buffers=%d io-mode=%s scene-mode=%s %s",
            m_device_name, m_num_buffers, m_iomode, m_scene_mode, base_property);
    else
        sprintf(property_buffer, "device-name=%s num-buffers=%d  io-mode=%s  scene-mode=%s",
            m_device_name, m_num_buffers, m_iomode, m_scene_mode);

    sprintf(caps_filter_buffer, "video/x-raw, width=%d, height=%d, format=%s, tiled=false",
        m_width, m_height, m_format);

    ret = add_plugins("icamerasrc", property_buffer, property, caps_filter_buffer);
    EXPECT_EQ(ret, 0) << "Failed to add_plugin icamerasrc";

    if (strcmp(m_sinktype, "yami") == 0)
    {
        //gst-launch-1.0 icamerasrc device-name=imx185 wdr-mode=on num-buffers=300 io-mode=3 !
        // video/x-raw,format=NV12,width=1920,height=1080,tiled=false !
        // yamiscale ! video/x-raw,format=xBGR,width=1920,height=1080 ! yamisink
        sprintf(yammi_caps, "video/x-raw, width=%d, height=%d, format=xBGR",
                m_width, m_height);

        ret = add_plugins("yamiscale", NULL, NULL, yammi_caps);
        EXPECT_EQ(ret, 0) << "Failed to add_plugin yamiscale";

        ret = add_plugins("yamisink", NULL, NULL, NULL);
        EXPECT_EQ(ret, 0) << "Failed to add_plugin yamisink";

    }
    else if (strcmp(m_sinktype, "mfx") == 0)
    {

        ret = add_plugins("mfxvpp", NULL, NULL, NULL);
        EXPECT_EQ(ret, 0) << "Failed to add_plugin mfxvpp";

        ret = add_plugins("mfxsink", NULL, NULL, NULL);
        EXPECT_EQ(ret, 0) << "Failed to add_plugin mfxsink";
    }
    else
    {
        ret = add_plugins("vaapipostproc", NULL, NULL, NULL);
        EXPECT_EQ(ret, 0) << "Failed to add_plugin vaapipostproc";

        ret = add_plugins("vaapisink", NULL, NULL, NULL);
        EXPECT_EQ(ret, 0) << "Failed to add_plugin vaapisink";
    }

    ret =  do_pipline(check, type);
    EXPECT_EQ(ret, 0) << "Failed to do_pipline";
}

static gboolean check_field_value(GQuark field, const GValue * value, gpointer pfx)
{
    //static gboolean format_nv12 = false;
    //static gboolean format_yuy2 = false;
    //we can use g_quark_to_string() to get filed name.
    gchar *str_value = gst_value_serialize (value);
    if (!strcmp(str_value, "YUY2"))
      //  format_yuy2 = true;
    if (!strcmp(str_value, "NV12"))
        //format_nv12 = true;

    g_print ("        %s: %s\n", g_quark_to_string (field), str_value);
    g_free (str_value);
    return TRUE;
}

// Enumurate all capbilities
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_ENUM_ALL_CAPBILITIES)
{
    int i = 0;
    const GstCaps * caps;
    const GList *templs;
    GstStructure *structure;
    GstElementFactory * factory;
    GstStaticPadTemplate *padtemplate;
    const char *factoryname = "icamerasrc";

    gst_init (NULL, NULL);
    factory = gst_element_factory_find (factoryname);
    templs = gst_element_factory_get_static_pad_templates (factory);
    while (templs) {
        padtemplate = (GstStaticPadTemplate *) templs->data;
        if (padtemplate->static_caps.string) {
            g_print ("    Capabilities:\n");
            caps = gst_static_caps_get(&padtemplate->static_caps);
            for (i = 0; i < gst_caps_get_size (caps); i++) {
                structure = gst_caps_get_structure (caps, i);
                g_print ("    %s\n", gst_structure_get_name (structure));
                gst_structure_foreach (structure, check_field_value, NULL);
            }
        }
        templs = g_list_next (templs);
    }
}

S_PROPERTY_DEFAULT PropertyTable[] = {
        {"num-buffers", PROPERTY_INT, -1, false, NULL, 0},
        {"capture-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"buffer-count", PROPERTY_INT, 6, false, NULL, 0},
        {"printfps", PROPERTY_BOOL, 0, false, NULL, 0},
        {"interlace-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"interlace-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"deinterlace-method", PROPERTY_ENUM, 0, false, NULL, 0},
        {"device-name", PROPERTY_ENUM, 0, false, NULL, 4},
        {"io-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"sharpness", PROPERTY_INT, 0, false, NULL, 0},
        {"brightness", PROPERTY_INT, 0, false, NULL, 0},
        {"contrast", PROPERTY_INT, 0, false, NULL, 0},
        {"hue", PROPERTY_INT, 0, false, NULL, 0},
        {"saturation", PROPERTY_INT, 0, false, NULL, 0},
        {"iris-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"iris-level", PROPERTY_INT, 0, false, NULL, 0},
        {"exposure-time", PROPERTY_INT, 0, false, NULL, 0},
        {"ev", PROPERTY_INT, 0, false, NULL, 0},
        {"gain", PROPERTY_FLOAT, 0, false, NULL, 0, 0.0f},
        {"ae-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"ae-region", PROPERTY_STRING, 0, false, NULL, 0},
        {"ae-converge-speed", PROPERTY_ENUM, 0, false, NULL, 0},
        {"scene-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"blc-area-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"wdr-level", PROPERTY_INT, 100, false, NULL, 0},
        {"awb-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"awb-region", PROPERTY_STRING, 0, false, NULL, 0},
        {"cct-range", PROPERTY_STRING, 0, false, NULL, 0},
        {"wp-point", PROPERTY_STRING, 0, false, NULL, 0},
        {"awb-gain-r", PROPERTY_INT, 0, false, NULL, 0},
        {"awb-gain-g", PROPERTY_INT, 0, false, NULL, 0},
        {"awb-gain-b", PROPERTY_INT, 0, false, NULL, 0},
        {"awb-shift-r", PROPERTY_INT, 0, false, NULL, 0},
        {"awb-shift-g", PROPERTY_INT, 0, false, NULL, 0},
        {"awb-shift-b", PROPERTY_INT, 0, false, NULL, 0},
        {"color-transform", PROPERTY_STRING, 0, false, NULL, 0},
        {"nr-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"overall", PROPERTY_INT, 0, false, NULL, 0},
        {"spatial", PROPERTY_INT, 0, false, NULL, 0},
        {"temporal", PROPERTY_INT, 0, false, NULL, 0},
        {"scene-mode", PROPERTY_ENUM, 0, false, NULL, 0},
        {"sensor-resolution", PROPERTY_ENUM, 0, false, NULL, 0},
        {"fps", PROPERTY_ENUM, 30, false, NULL, 1},
        {"antibanding-mode", PROPERTY_ENUM, 0, false, NULL, 0}
    };

bool check_property_default(
    const char *property_name,
    PROPERTY_VAL_TYPE type,
    int default_int,
    bool default_bool,
    const char *default_string,
    int default_enum,
    float default_float
    )
{
    int table_size = ARRAYSIZE(PropertyTable);

    for (int i = 0; i < table_size; i++)
    {
        if (strcmp(PropertyTable[i].property_name, property_name) == 0)
        {
            if (type != PropertyTable[i].type)
            {

                g_print("Propery (%s) type is not correct!\n", property_name);
                return false;
            }

            switch (type)
            {
            case PROPERTY_INT:
                if (default_int != PropertyTable[i].val_int)
                {
                    g_print("Propery (%s) default value is not correct!\n", property_name);
                    return false;
                }
                break;
             case PROPERTY_FLOAT:
                if (default_float != PropertyTable[i].val_float)
                {
                    g_print("Propery (%s) default value is not correct!\n", property_name);
                    return false;
                }
            case PROPERTY_BOOL:
                if (!((default_bool && PropertyTable[i].val_bool) ||
                    (!(default_bool) && !(PropertyTable[i].val_bool))))
                {
                    g_print("Propery (%s) default value is not correct!\n", property_name);
                    return false;
                }
                break;
            case PROPERTY_STRING:
                if ((default_string == NULL && PropertyTable[i].val_string != NULL) ||
                    (default_string != NULL && strcmp(default_string, PropertyTable[i].val_string) != 0))
                {

                    g_print("Propery (%s) default value is not correct!\n", property_name);
                    return false;
                }
                break;

            case PROPERTY_ENUM:
                if (default_enum != PropertyTable[i].val_enum)
                {
                    g_print("Propery (%s) default value is not correct!\n", property_name);
                    return false;
                }
                break;
            }
            return true;
        }
    }

    return true;
}

// Enumurate all properties
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_ENUM_ALL_PROPERTIES)
{
    //gst init
    gst_init(NULL, NULL);

    // Factory make element of icamerasrc
    GstElement *source = gst_element_factory_make("icamerasrc", "element-0");

    ASSERT_TRUE(NULL != source) << "Failed to gst_element_factory_make icamerasrc\n";

    // List all properties
    guint num_properties = 0;
    GParamSpec ** properties=g_object_class_list_properties(G_OBJECT_GET_CLASS (source), &num_properties);
    EXPECT_NE(properties, (GParamSpec **)NULL) << "Null properties\n";

    PROPERTY_VAL_TYPE property_type;
    gint val_int_default;
    gboolean val_bool_default;
    const gchar *val_str_default;
    gint val_enum_default;
    gfloat val_float_default;

    g_print ("\tProperties:\n");
    for (guint i = 0; i < num_properties; i++)
    {
        if (properties[i] == NULL)
        {
            continue;
        }

        // property name
        g_print("\t\t%s: flags(", properties[i]->name);

        // property flags
        GParamFlags flags = properties[i]->flags;

        if (flags & G_PARAM_READABLE)
        {
            g_print("READABLE|");
        }
        if (flags & G_PARAM_WRITABLE)
        {
            g_print("WRITABLE|");
        }
        if (flags & G_PARAM_CONSTRUCT)
        {
            g_print("CONSTRUCT|");
        }
        if (flags & G_PARAM_CONSTRUCT_ONLY)
        {
            g_print("CONSTRUCT_ONLY|");
        }
        if (flags & G_PARAM_LAX_VALIDATION)
        {
            g_print("LAX_VALIDATION|");
        }
        if (flags & G_PARAM_PRIVATE)
        {
            g_print("PRIVATE|");
        }
        g_print("); value_type(");

        // property value type
        const gchar *value_type = g_quark_to_string(g_type_qname(properties[i]->value_type));
        g_print("%s); default(", value_type);

        // Property default value
        const GValue *pgval = g_param_spec_get_default_value(properties[i]);
        switch (properties[i]->value_type)
        {
        case G_TYPE_INT:
            {
                property_type = PROPERTY_INT;
                val_int_default = g_value_get_int(pgval);
                g_print("%d)\n", val_int_default);
            }
            break;
        case G_TYPE_BOOLEAN:
            {
                property_type = PROPERTY_BOOL;
                val_bool_default = g_value_get_boolean(pgval);
                g_print("%s)\n", val_bool_default?"true":"false");
            }
            break;
        case G_TYPE_FLOAT:
            {
                property_type = PROPERTY_FLOAT;
                val_float_default = g_value_get_float(pgval);
                g_print("%f)\n",val_float_default);
            }
            break;
        case G_TYPE_STRING:
            {
                property_type = PROPERTY_STRING;
                val_str_default = g_value_get_string(pgval);
                g_print("%s)\n", val_str_default);
            }
            break;
        default:
            if (G_TYPE_IS_ENUM(properties[i]->value_type))
            {
                property_type = PROPERTY_ENUM;
                val_enum_default = g_value_get_enum(pgval);
                g_print("%d)\n", val_enum_default);
            }
            else
            {
                g_print(")\n");
            }
            break;
        }

        ASSERT_TRUE(check_property_default(properties[i]->name, property_type,
            val_int_default, val_bool_default, val_str_default, val_enum_default, val_float_default));
    }
}

/*******************************************************************************
                              Day-Night switch
*******************************************************************************/
// day-night mode
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_DAY_NIGHT_MODE_DAY_MODE)
{
    test_property(NULL, "daynight-mode=day-mode", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_DAY_NIGHT_MODE_NIGHT_MODE)
{
    test_property(NULL, "daynight-mode=night-mode", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_DAY_NIGHT_MODE_AUTO)
{
    test_property(NULL, "daynight-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_DAY_NIGHT_MODE_DEFAULT)
{
    test_property(NULL, "daynight-mode=auto", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_DAY_NIGHT_MODE_NEGATIVE)
{
    test_property(NULL, "daynight-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}


/*******************************************************************************
                              Image  adjust-ment
*******************************************************************************/
// Sharpness
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SHARPNESS_MIN)
{
    test_property(NULL, "sharpness=-128", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SHARPNESS_MAX)
{
    test_property(NULL, "sharpness=127", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SHARPNESS_DEFAULT)
{
    test_property(NULL, "sharpness=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SHARPNESS_NEGATIVE_LOW)
{
    test_property(NULL, "sharpness=-129", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SHARPNESS_NEGATIVE_HIGH)
{
    test_property(NULL, "sharpness=128", PROPERTY_CHECK_TYPE_NEGATIVE);
}


// Brightness
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BRIGHTNESS_MIN)
{
    test_property(NULL, "brightness=-128", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BRIGHTNESS_MAX)
{
    test_property(NULL, "brightness=127", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BRIGHTNESS_DEFAULT)
{
    test_property(NULL, "brightness=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BRIGHTNESS_NEGATIVE_LOW)
{
    test_property(NULL, "brightness=-129", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BRIGHTNESS_NEGATIVE_HIGH)
{
    test_property(NULL, "brightness=128", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Contrast
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CONTRAST_MIN)
{
    test_property(NULL, "contrast=-128", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CONTRAST_MAX)
{
    test_property(NULL, "contrast=127", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CONTRAST_DEFAULT)
{
    test_property(NULL, "contrast=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CONTRAST_NEGATIVE_LOW)
{
    test_property(NULL, "contrast=-129", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CONTRAST_NEGATIVE_HIGH)
{
    test_property(NULL, "contrast=128", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Hue
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_HUE_MIN)
{
    test_property(NULL, "hue=-128", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_HUE_MAX)
{
    test_property(NULL, "hue=127", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_HUE_DEFAULT)
{
    test_property(NULL, "hue=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_HUE_NEGATIVE_LOW)
{
    test_property(NULL, "hue=-129", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_HUE_NEGATIVE_HIGH)
{
    test_property(NULL, "hue=128", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Saturation
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SATURATION_MIN)
{
    test_property(NULL, "saturation=-128", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SATURATION_MAX)
{
    test_property(NULL, "saturation=127", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SATURATION_DEFAULT)
{
    test_property(NULL, "saturation=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SATURATION_NEGATIVE_LOW)
{
    test_property(NULL, "saturation=-129", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SATURATION_NEGATIVE_HIGH)
{
    test_property(NULL, "saturation=128", PROPERTY_CHECK_TYPE_NEGATIVE);
}

/*******************************************************************************
                              Exposure  settings
*******************************************************************************/
// AE mode
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AE_MODE_AUTO)
{
    test_property(NULL, "ae-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AE_MODE_MANUAL)
{
    test_property(NULL, "ae-mode=manual", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AE_MODE_DEFAULT)
{
    test_property(NULL, "ae-mode=auto", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AE_MODE_NEGATIVE)
{
    test_property(NULL, "ae-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}


// Iris mode
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_MODE_AUTO)
{
    test_property(NULL, "iris-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_MODE_MANUAL)
{
    test_property(NULL, "iris-mode=manual", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_MODE_SPECIFIC)
{
    test_property(NULL, "iris-mode=customized", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_MODE_DEFAULT)
{
    test_property(NULL, "iris-mode=auto", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_MODE_NEGATIVE)
{
    test_property(NULL, "iris-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Iris level
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_LEVEL_MIN)
{
    test_property(NULL, "iris-level=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_LEVEL_MAX)
{
    test_property(NULL, "iris-level=100", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_LEVEL_DEFAULT)
{
    test_property(NULL, "iris-level=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_LEVEL_NEGATIVE_LOW)
{
    test_property(NULL, "iris-level=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_IRIS_LEVEL_NEGATIVE_HIGHT)
{
    test_property(NULL, "iris-level=101", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Exposure time for vULL
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_MIN)
{
    test_property("ae-mode=manual", "exposure-time=90", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_MAX)
{
    test_property("ae-mode=manual", "exposure-time=33333", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_DEFAULT)
{
    test_property("ae-mode=manual", "exposure-time=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_NEGATIVE_LOW)
{
    test_property("ae-mode=manual", "exposure-time=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_NEGATIVE_HIGHT)
{
    test_property("ae-mode=manual", "exposure-time=1000001", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Exposure time for vHDR. only short exposure[150-2100] can be set. long exposure is 16times short.
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_MIN_VHDR)
{
    test_property("ae-mode=manual", "exposure-time=2400", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_MAX_VHDR)
{
    test_property("ae-mode=manual", "exposure-time=33328", PROPERTY_CHECK_TYPE_MAX);
}


TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_NEGATIVE_LOW_VHDR)
{
    test_property("ae-mode=manual", "exposure-time=10", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_TIME_NEGATIVE_HIGHT_VHDR)
{
    test_property("ae-mode=manual", "exposure-time=1000001", PROPERTY_CHECK_TYPE_NEGATIVE);
}


// Gain [0, 60] for vULL
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_MIN)
{
    test_property("ae-mode=manual", "gain=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_MAX)
{
    test_property("ae-mode=manual", "gain=60", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_DEFAULT)
{
    test_property("ae-mode=manual", "gain=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_NEGATIVE_LOW)
{
    test_property("ae-mode=manual", "gain=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_NEGATIVE_HIGHT)
{
    test_property("ae-mode=manual", "gain=61", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Gain [4.5, 48] for vHDR
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_MIN_VHDR)
{
    test_property("ae-mode=manual", "gain=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_MAX_VHDR)
{
    test_property("ae-mode=manual", "gain=12", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_NEGATIVE_LOW_VHDR)
{
    test_property("ae-mode=manual", "gain=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_GAIN_NEGATIVE_HIGHT_VHDR)
{
    test_property("ae-mode=manual", "gain=60", PROPERTY_CHECK_TYPE_NEGATIVE);
}


// Exposure ev
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_EV_MIN)
{
    test_property(NULL, "ev=-3", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_EV_MAX)
{
    test_property(NULL, "ev=3", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_EV_DEFAULT)
{
    test_property(NULL, "ev=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_EV_NEGATIVE_LOW)
{
    test_property(NULL, "ev=-5", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_EXPOSURE_EV_NEGATIVE_HIGHT)
{
    test_property(NULL, "ev=5", PROPERTY_CHECK_TYPE_NEGATIVE);
}

/*******************************************************************************
                              Backlight setting
*******************************************************************************/
/*

// WDR mode is not used. Use scene-mode
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_MODE_OFF)
{
    test_property(NULL, "wdr-mode=off", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_MODE_ON)
{
    test_property(NULL, "wdr-mode=on", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_MODE_AUTO)
{
    test_property(NULL, "wdr-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_MODE_DEFAULT)
{
    test_property(NULL, "wdr-mode=off", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_MODE_NEGATIVE)
{
    test_property(NULL, "wdr-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}
*/

// BLC area mode ?
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BLC_AREA_MODE_OFF)
{
    test_property("scene-mode=ull", "blc-area-mode=off", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BLC_AREA_MODE_ON)
{
    test_property("scene-mode=ull", "blc-area-mode=on", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BLC_AREA_MODE_DEFAULT)
{
    test_property("scene-mode=ull", "blc-area-mode=off", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_BLC_AREA_MODE_NEGATIVE)
{
    test_property("scene-mode=ull", "blc-area-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// WDR level
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_LEVEL_MIN)
{
    test_property("scene-mode=hdr", "wdr-level=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_LEVEL_MAX)
{
    test_property("scene-mode=hdr", "wdr-level=200", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_LEVEL_DEFAULT)
{
    test_property("scene-mode=hdr", "wdr-level=100", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_LEVEL_NEGATIVE_LOW)
{
    test_property("scene-mode=hdr", "wdr-level=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WDR_LEVEL_NEGATIVE_HIGH)
{
    test_property("scene-mode=hdr", "wdr-level=201", PROPERTY_CHECK_TYPE_NEGATIVE);
}

/*******************************************************************************
                              White Balance
*******************************************************************************/
// WB mdoe
/*
awb-mode
0 auto (default)
1 partly overcast
2 fully overcase
3 fluorescent
4 incandescent
5 sunset
6 video_conferencing
8 daylight
9 cct_range
10 white_point
11 manual_gain
*/
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_AUTO)
{
    test_property(NULL, "awb-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_PARTLY_OVERCAST)
{
    test_property(NULL, "awb-mode=partly_overcast", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_FULLY_OVERCASE)
{
    test_property(NULL, "awb-mode=fully_overcast", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_FLUORESCENT)
{
    test_property(NULL, "awb-mode=fluorescent", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_INCANDESCENT)
{
    test_property(NULL, "awb-mode=incandescent", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_SUNSET)
{
    test_property(NULL, "awb-mode=sunset", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_VIDEO_CONFERENCING)
{
    test_property(NULL, "awb-mode=video_conferencing", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_MANUAL_DAYLIGHT)
{
    test_property(NULL, "awb-mode=daylight", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_CCT_RANGE)
{
    test_property(NULL, "awb-mode=cct_range", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_WHITE_POINT)
{
    test_property(NULL, "awb-mode=white_point", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_MANUAL_GAIN)
{
    test_property(NULL, "awb-mode=manual_gain", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_DEFAULT)
{
    test_property(NULL, "awb-mode=auto", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WB_MODE_NEGATIVE)
{
    test_property(NULL, "awb-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// CCT Range
// range:[0 100]
// default: 0
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CCT_RANGE_MIN)
{
    test_property("awb-mode=cct_range", "cct-range=1800~2000", PROPERTY_CHECK_TYPE_STRING);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CCT_RANGE_MAX)
{
    test_property("awb-mode=cct_range", "cct-range=12000~15000", PROPERTY_CHECK_TYPE_STRING);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CCT_RANGE_DEFAULT)
{
    test_property("awb-mode=cct_range", "cct-range=null", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CCT_RANGE_NEGATIVE_LOW)
{
    test_property("awb-mode=cct_range", "cct_range=1500~1799", PROPERTY_CHECK_TYPE_STRING);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CCT_RANGE_NEGATIVE_HIGH)
{
    test_property("awb-mode=cct_range", "cct-range=15001~15500", PROPERTY_CHECK_TYPE_STRING);
}

// wp point, remaining questions about wp_point string format???
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WHITE_POINT)
{
    test_property("awb-mode=white_point", "wp_point=\"1920, 1080\"", PROPERTY_CHECK_TYPE_STRING);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_WHITE_POINT_DEFAULT)
{
    test_property("awb-mode=white_point", "wp_point=null", PROPERTY_CHECK_TYPE_DEFAULT);
}

// AWB shift, remainning question about the awb-mode???
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AWB_SHIFT_MAX)
{
    test_property("awb-mode=manual_gain", "awb-shift-r=255 awb-shift-g=255 awb-shift-b=255", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AWB_SHIFT_MIN)
{
    test_property("awb-mode=manual_gain", "awb-shift-r=0 awb-shift-g=0 awb-shift-b=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AWB_SHIFT_DEFAULT)
{
    test_property("awb-mode=manual_gain", "awb-shift-r=0 awb-shift-g=0 awb-shift-b=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AWB_SHIFT_NEGATIVE_LOW)
{
    test_property("awb-mode=manual_gain", "awb-shift-r=-1 awb-shift-g=-1 awb-shift-b=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_AWB_SHIFT_NEGATIVE_HIGH)
{
    test_property("awb-mode=manual_gain", "awb-shift-r=256 awb-shift-g=256 awb-shift-b=256", PROPERTY_CHECK_TYPE_NEGATIVE);
}


// Manual white balance gains
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_MANUAL_WB_GAIN_MAX)
{
    test_property("awb-mode=manual_gain", "awb-gain-r=255 awb-gain-g=255 awb-gain-b=255", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_MANUAL_WB_GAIN_MIN)
{
    test_property("awb-mode=manual_gain", "awb-gain-r=0 awb-gain-g=0 awb-gain-b=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_MANUAL_WB_GAIN_DEFAULT)
{
    test_property("awb-mode=manual_gain", "awb-gain-r=0 awb-gain-g=0 awb-gain-b=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_MANUAL_WB_GAIN_NEGATIVE_LOW)
{
    test_property("awb-mode=manual_gain", "awb-gain-r=-1 awb-gain-g=-1 awb-gain-b=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_MANUAL_WB_GAIN_NEGATIVE_HIGH)
{
    test_property("awb-mode=manual_gain", "awb-gain-r=256 awb-gain-g=256 awb-gain-b=256", PROPERTY_CHECK_TYPE_NEGATIVE);
}


/*******************************************************************************
                              Noise Reduction
*******************************************************************************/
// NR mdoe
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_MODE_AUTO)
{
    test_property(NULL, "nr-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_MODE_OFF)
{
    test_property(NULL, "nr-mode=off", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_MODE_MANUAL_NORMAL)
{
    test_property(NULL, "nr-mode=normal", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_MODE_MANUAL_EXPERT)
{
    test_property(NULL, "nr-mode=expert", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_MODE_DEFAULT)
{
    test_property(NULL, "nr-mode=off", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_MODE_NEGATIVE)
{
    test_property(NULL, "nr-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Noise filter level
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_FILETER_LEVEL_MIN)
{
    test_property("nr-mode=normal", "nr-filter-level=0", PROPERTY_CHECK_TYPE_MIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_FILETER_LEVEL_MAX)
{
    test_property("nr-mode=normal", "nr-filter-level=100", PROPERTY_CHECK_TYPE_MAX);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_FILETER_LEVEL_DEFAULT)
{
    test_property("nr-mode=normal", "nr-filter-level=0", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_FILETER_LEVEL_NEGATIVE_LOW)
{
    test_property("nr-mode=normal", "nr-filter-level=-1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_NR_FILETER_LEVEL_NEGATIVE_HIGH)
{
    test_property("nr-mode=normal", "nr-filter-level=101", PROPERTY_CHECK_TYPE_NEGATIVE);
}

/*******************************************************************************
                              Video adjustment
*******************************************************************************/
// Scene mode
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SCENE_MODE_AUTO)
{
    test_property(NULL, "scene-mode=auto", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SCENE_MODE_INDOOR)
{
    test_property(NULL, "scene-mode=indoor", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SCENE_MODE_OUTDOOR)
{
    test_property(NULL, "scene-mode=outdoor", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SCENE_MODE_DISABLED)
{
    test_property(NULL, "scene-mode=disabled", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SCENE_MODE_DEFAULT)
{
    test_property(NULL, "scene-mode=auto", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SCENE_MODE_NEGATIVE)
{
    test_property(NULL, "scene-mode=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}

/* Removed from document

// Sensor resolution
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SENSOR_RESOLUTION_720P)
{
    test_property(NULL, "sensor-resolution=720p", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SENSOR_RESOLUTION_1080P)
{
    test_property(NULL, "sensor-resolution=1080p", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SENSOR_RESOLUTION_4K)
{
    test_property(NULL, "sensor-resolution=4K", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SENSOR_RESOLUTION_DEFAULT)
{
    test_property(NULL, "sensor-resolution=1080p", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_SENSOR_RESOLUTION_NEGATIVE)
{
    test_property(NULL, "sensor-resolution=VGA", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Capture frame-rate
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CAPTURE_FRAME_RATE_25)
{
    test_property(NULL, "capture-fps=25", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CAPTURE_FRAME_RATE_30)
{
    test_property(NULL, "capture-fps=30", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CAPTURE_FRAME_RATE_50)
{
    test_property(NULL, "capture-fps=50", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CAPTURE_FRAME_RATE_DEFAULT)
{
    test_property(NULL, "capture-fps=25", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CAPTURE_FRAME_RATE_NEGATIVE)
{
    test_property(NULL, "capture-fps=1", PROPERTY_CHECK_TYPE_NEGATIVE);
}

// Video standard
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_VIDEO_STANDARD_PAL)
{
    test_property(NULL, "video-standard=PAL", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_VIDEO_STANDARD_NTSC)
{
    test_property(NULL, "video-standard=NTSC", PROPERTY_CHECK_TYPE_ENUM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_VIDEO_STANDARD_DEFAULT)
{
    test_property(NULL, "video-standard=PAL", PROPERTY_CHECK_TYPE_DEFAULT);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_VIDEO_STANDARD_NEGATIVE)
{
    test_property(NULL, "video-standard=unknown", PROPERTY_CHECK_TYPE_NEGATIVE);
}
*/

/*******************************************************************************
                              Check Ispcontrol interface scene mode=ULL
*******************************************************************************/
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_WB_GAIN_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_WB_GAIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_COLOR_CORRECTION_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_COLOR_CORRECTION);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_BXT_DEMOSAIC_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_BXT_DEMOSAIC);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_SC_IEFD_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_SC_IEFD);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_SEE_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_SEE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_TNR5_21_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_TNR5_21);
}
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_XNR_DSS_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_XNR_DSS);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_GAMMA_TONE_MAP_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_GAMMA_TONE_MAP);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_BNLM_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_BNLM);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_CSC_ULL)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_CSC);
}

/*******************************************************************************
                              Check Ispcontrol interface scene mode=HDR
*******************************************************************************/
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_WB_GAIN_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_WB_GAIN);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_COLOR_CORRECTION_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_COLOR_CORRECTION);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_BXT_DEMOSAIC_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_BXT_DEMOSAIC);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_SC_IEFD_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_SC_IEFD);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_SEE_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_SEE);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_TNR5_21_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_TNR5_21);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_XNR_DSS_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_XNR_DSS);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_GAMMA_TONE_MAP_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_GAMMA_TONE_MAP);
}

TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_BNLM_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_BNLM);
}
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_ISP_CSC_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_CSC);
}
//check ltm tuning
TEST_F(Gst_property_test, CAMERA_API_GST_PROPERTY_CHECK_LTM_TUNING_HDR)
{
    test_property(NULL, NULL, PROPERTY_CHECK_TYPE_LTM_TUNING);
}
