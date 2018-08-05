#include "video_capture.h"
#include <stdio.h>

// gst-launch-1.0 -e icamerasrc printfps=true num-buffers=$NUM_BUFFERS ! "video/x-raw, width=$PREVIEW_WIDTH, height=$PREVIEW_HEIGHT" ! xvimagesink
/* Signal handler for ctrl+c */
static gboolean bus_call(GstBus     *bus,
                             GstMessage *msg,
                             gpointer    data)
{
    GMainLoop *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE (msg)) {

        case GST_MESSAGE_EOS:
            g_print ("End of stream\n");
            g_main_loop_quit (loop);
            break;

        case GST_MESSAGE_ERROR: 
            {
                gchar  *debug;
                GError *error;

                gst_message_parse_error (msg, &error, &debug);
                g_free (debug);

                g_printerr ("Error: %s\n", error->message);
                g_error_free (error);

                g_main_loop_quit (loop);
            }
            break;
        
        default:
            break;
    }

    return TRUE;
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
        g_printerr("donesn't find the enum nick: %s", nick);
        return -1;
    }

    return j;
}

CCamera::CCamera()
{
    loop = g_main_loop_new (NULL, FALSE);
    pipeline = NULL;
    source = NULL;
    sink = NULL;
    filter = NULL;
    bus = NULL;
    bus_watch_id = 0;
    is_mondello = false;
    localSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (localSocket < 0)
    {
        g_printerr ("Failed to open socket.\n");
    }
}

CCamera::~CCamera()
{
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);
}

int CCamera::pipeline_make()
{
    /* Create gstreamer elements */
    pipeline = gst_pipeline_new ("pipeline");
    if (NULL == pipeline)
    {
        g_printerr ("Pipeline could not be created. Exiting.\n");
        return -1;
    }
    
    source = gst_element_factory_make ("icamerasrc", "icamerasrc");
    if (NULL == source)
    {
        g_printerr ("icamerasrc could not be created. Exiting.\n");
        return -1;
    }

    filter = gst_element_factory_make ("capsfilter", "filter");
    if (NULL == filter)
    {
        g_printerr ("capsfilter could not be created. Exiting.\n");
        return -1;
    }
    
    sink = gst_element_factory_make ("fakesink", "fakesink");
    if (NULL == sink)
    {
        g_printerr ("xvimagesink could not be created. Exiting.\n");
        return -1;
    }
    
    /* we add all elements into the pipeline */
    gst_bin_add_many (GST_BIN (pipeline), source, filter, sink, NULL);
    gst_element_link_many(source, filter, sink, NULL);

    return 0;
}

void CCamera::set_elements_properties(GstElement *element, const char *properties)
{
    char *token;
    char pro_str[1024];
    GObjectClass *oclass;
    GParamSpec *prop;

    //default values
    is_mondello = false;
    is_interlaced = false;
    
    strncpy(pro_str, properties, 1024);
    token = strtok( (char*)pro_str, ", ");
    while (token != NULL) {
        char *ptr = strstr(token, "=");
        if (ptr == NULL)
        {
            token = strtok(NULL, ", ");
            continue;
        }
        
        char *str_value = ptr + 1;
        char *property = token;
        *ptr = '\0';

        g_print("%s: %s\n", property, str_value);

        if (strncmp(property, "device-name", strlen("device-name")) == 0 &&
            strncmp(str_value, "mondello", strlen("mondello")) == 0)
        {
            is_mondello = true;
            str_value[strlen("mondello")] = 0;
            g_print("Mondello is true\n"); 
        }

        if (strncmp(property, "interlace-mode", strlen("interlace-mode")) == 0 && 
            strncmp(str_value, "true", strlen("true")) == 0)
        {
            is_interlaced = true;
        } 

        oclass = G_OBJECT_GET_CLASS (element);
        prop = g_object_class_find_property (oclass, property);
        switch (prop->value_type) {
            gint value_int, value_enum;
            gboolean value_bool;
            case G_TYPE_INT:
                value_int = atoi(str_value);
                g_object_set(G_OBJECT(element), property, value_int, NULL);
                break;
            case G_TYPE_BOOLEAN:
                value_bool = ((strncmp(str_value, "true", strlen("true")) == 0) ? true : false);
                g_object_set(G_OBJECT(element), property, value_bool, NULL);
                break;
             case G_TYPE_STRING:
                g_object_set(G_OBJECT(element), property, str_value, NULL);
                break;
             default:
                if (G_TYPE_IS_ENUM(prop->value_type)) {
                    value_enum = get_enum_value_from_field(prop, str_value);
                    g_object_set(G_OBJECT(element), property, value_enum, NULL);
                } else
                    g_printerr("ERR, this type isn't supported\n");
                break;
        }

        token = strtok(NULL, ", ");
    }
}

void CCamera::get_resolution_info_from_caps(const char *src_caps_filter)
{
    char *token;
    char caps_str[1024];

    // set default value:
    width = 1920;
    height = 1080;
    format = E_PIXEL_FORMAT_YUY2;
    strncpy(caps_str, src_caps_filter, 1024);
    token = strtok( (char*)caps_str, ", ");
    while (token != NULL) {
        char *ptr = strstr(token, "=");
        if (ptr == NULL)
        {
            token = strtok(NULL, ", ");
            continue;
        }
        char *str_value = ptr + 1;
        char *property = token;
        *ptr = '\0';
        g_print("%s: %s\n", property, str_value);
        if (strcmp(property, "width") == 0)
        {
            width = atoi(str_value);
        }
        else if (strcmp(property, "height") == 0)
        {
            height = atoi(str_value);
        }
        else if (strcmp(property, "format") == 0)
        {
            if (strcmp(str_value, "YUY2") == 0)
            {
                format = E_PIXEL_FORMAT_YUY2;
            }
            else if (strcmp(str_value, "NV12") == 0)
            {
                format = E_PIXEL_FORMAT_NV12;
            }
            else if (strcmp(str_value, "RGB888") == 0)
            {
                format = E_PIXEL_FORMAT_RGB888;
            }
            else if (strcmp(str_value, "RGB565") == 0)
            {
                format = E_PIXEL_FORMAT_RGB565;
            }
        }
 
        token = strtok(NULL, ", ");
    }

}

int CCamera::set_attrs_caps(const char *src_pro_attrs, const char *src_caps_filter)
{
    GstCaps *video_caps = NULL;
    set_elements_properties(source, src_pro_attrs);
    video_caps = gst_caps_from_string(src_caps_filter);

    if (video_caps == NULL)
    {
        g_printerr ("video_caps could not be created. Exiting.\n");
        return -1;
    }
    g_object_set(G_OBJECT(filter), "caps", video_caps, NULL);
    gst_caps_unref(video_caps);
    
    get_resolution_info_from_caps(src_caps_filter);
    
    return 0;
}


int CCamera::state_handler(GstState state)
{
    gst_element_set_state (pipeline, state);
    return 0;
}

void CCamera::main_loop()
{
    /* Iterate */
    g_print ("Running...\n");
    g_main_loop_run (loop);
}

int CCamera::watcher_make()
{
    /* we add a message handler */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);    
    return 0;
}

const char* get_script_according_to_resolution(int width, int height, E_PIXEL_FORMAT format, bool is_interlaced)
{
    static S_MONDELLO_SCRIPT mondello_script_table[] = {
        {.width=1920, .height=1080, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=true, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-1080i-free-run.py"},
            
        {.width=1920, .height=1080, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=false, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-1080p-free-run.py"},
            
        {.width=1920, .height=1080, .format=E_PIXEL_FORMAT_RGB888, .is_interlaced=false, 
            .script_name="08_20_Free_run_MIPI_TxA_CSI_4_Lane_RGB888_1920x1080p_60Hz - Mondello.py"},
            
        {.width=720, .height=576, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=true, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-576i-free-run.py"},
            
        {.width=720, .height=576, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=false, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-576p-free-run.py"},
            
        {.width=640, .height=480, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=false, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-VGA-free-run.py"},

        {.width=1280, .height=720, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=false, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-720p-free-run.py"},

        {.width=1280, .height=720, .format=E_PIXEL_FORMAT_RGB888, .is_interlaced=false, 
            .script_name="08_10_Free_run_MIPI_TxA_CSI_4_Lane_RGB888_1280x720p_60Hz-Mondello.py"},

        {.width=720, .height=480, .format=E_PIXEL_FORMAT_YUY2, .is_interlaced=true, 
            .script_name="01_22_HDMI_to_MIPI_TxA_CSI_4_Lane_YUV422_8_Bit_Over_600Mbps -mondello-480i-free-run.py"}
    };

    S_MONDELLO_SCRIPT *ptr = &mondello_script_table[0];
    for (int i = 0; i < sizeof(mondello_script_table)/sizeof(mondello_script_table[0]); i++, ptr++)
    {
        if (width == ptr->width && height == ptr->height && 
            format == ptr->format && is_interlaced == ptr->is_interlaced)
        {
            return ptr->script_name;
        }
    }
    return NULL;
}

int CCamera::send_command_to_mondello_server()
{
    if (!is_mondello)
    {
        return 0;
    }

    if (localSocket < 0)
    {
        g_printerr ("Failed to open socket.\n");
        return -1;
    }

    const char *server_ip = getenv("MONDELLO_SERVER_IP");

    if (NULL == server_ip)
    {
        g_printerr ("Failed to get MONDELLO_SERVER_IP.\n");
        return -1;
    }
    else
    {
        g_printerr ("MONDELLO_SERVER_IP: %s\n", server_ip);
    }

    if (localSocket < 0)
    {
        g_printerr ("Failed to call socket.\n");
        return -1;
    }

    struct sockaddr_in remoteAddr = {};

    remoteAddr.sin_family = AF_INET;

    remoteAddr.sin_port = htons(13579);

    remoteAddr.sin_addr.s_addr = inet_addr(server_ip);

    socklen_t addr_len =sizeof(remoteAddr);

    const char *scrip_name = get_script_according_to_resolution(width, height, format, is_interlaced);

    if (scrip_name == NULL)
    {
        g_printerr ("Failed to get mondello script name.\n");
        return -1;
    }
    
    size_t buf_len = strlen(scrip_name);

    g_print("Mondello script: %s\n", scrip_name);

    if (sendto(localSocket, (void*)scrip_name, buf_len, 0, (const struct sockaddr*)&remoteAddr, addr_len) < 0)
    {
        g_printerr ("UDP send data failed.\n");
        return -1;
    }

    return 0;
}

int CCamera::run_camera(const char *src_pro_attrs, const char *src_caps_filter)
{
    if (set_attrs_caps(src_pro_attrs, src_caps_filter) != 0)
    {
        g_printerr ("Failed to set attrs and caps.\n");
        return -1;
    }

    /* Handle pipeline state */
    /* Set the pipeline to "playing" state*/
    if(state_handler(GST_STATE_PLAYING) != 0)
    {
        g_printerr ("Failed to set pipeline state: playing.\n");
        return -1;
    }

    send_command_to_mondello_server();

    main_loop();

    if(state_handler(GST_STATE_NULL) != 0)
    {
        g_printerr ("Failed to set pipeline state: NULL.\n");
        return -1;
    }
    
    return 0;
}

int main (int argc, char *argv[])
{
    /* Initialization */
    gst_init (&argc, &argv);

    int retcode = 0;

    char attributes_buffer[1024] = {};
    char capsfilter_buffer[1024] = {};

    CCamera *pcam = new CCamera();
    if (pcam == NULL)
    {
        g_printerr ("Failed to new CCamera.\n");
        return -1;
    }

    /* Initialize elements */
    if(pcam->pipeline_make() != 0)
    {
        g_printerr ("Failed to make pipeline.\n");
        retcode = -1;
        goto bail;
    }

    /* Add function to watch bus */
    if(pcam->watcher_make() != 0)
    {
        g_printerr ("Failed to add watcher.\n");
        retcode = -1;
        goto bail;
    }
    
    /* Check input arguments */
    if (argc < 3) {
        while (true)
        {
            g_print("------------------------------------------------------\n");
            g_print("Please input source attributes, e.g.\n"
                "\tprintfps=true num-buffers=100 device-name=mondello\n");
            if (NULL == fgets(attributes_buffer, 1024, stdin))
            {
                g_print("EOF\n");
                break;
            }

            g_print("Please input capsfilter, e.g. \n"
                "\tvideo/x-raw,format=YUY2,width=1280, height=720\n");
            if (NULL == fgets(capsfilter_buffer, 1024, stdin))
            {
                g_print("EOF\n");
                break;
            }

            if (0 != pcam->run_camera(attributes_buffer, capsfilter_buffer))
            {
                g_printerr("Failed to run camera pipeline.\n");
                retcode = -1;
                goto bail;
            }
        }
    }
    else
    {
        for (int i = 1; i < argc; i += 2)
        {
            g_print("------------------------------------------------------\n");
            if (0 != pcam->run_camera(argv[i], argv[i+1]))
            {
                g_printerr("Failed to run camera pipeline.\n");
                retcode = -1;
                goto bail;

            }
        }
    }
    
 bail:
    if (pcam != NULL)
    {
        delete pcam;
    }
    return retcode;
}
