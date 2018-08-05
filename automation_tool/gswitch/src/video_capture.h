#include <gst/gst.h>
#include <glib.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 

enum E_PIXEL_FORMAT
{
    E_PIXEL_FORMAT_YUY2,
    E_PIXEL_FORMAT_NV12,
    E_PIXEL_FORMAT_RGB888,
    E_PIXEL_FORMAT_RGB565
};

typedef struct _S_MONDELLO_SCRIPT
{
    int width;
    int height;
    E_PIXEL_FORMAT format;
    bool is_interlaced;
    const char *script_name;
}S_MONDELLO_SCRIPT;

// gst-launch-1.0 -e icamerasrc printfps=true num-buffers=$NUM_BUFFERS ! "video/x-raw, width=$PREVIEW_WIDTH, height=$PREVIEW_HEIGHT" ! xvimagesink
class CCamera
{
public:
    CCamera();
    ~CCamera();
    int pipeline_make();
    void set_elements_properties(GstElement *element, const char *properties);
    void get_resolution_info_from_caps(const char *src_caps_filter);
    int set_attrs_caps(const char *src_pro_attrs, const char *src_caps_filter);
    int state_handler(GstState state);
    int watcher_make();
    void main_loop();
    int send_command_to_mondello_server();
    int run_camera(const char *src_pro_attrs, const char *src_caps_filter);
private:
    GstElement *pipeline;
    GMainLoop *loop;
    GstElement *source;
    GstElement *sink;
    GstElement *filter;
    GstBus *bus;
    guint bus_watch_id;
    bool is_mondello;
    bool is_interlaced;
    int width;
    int height;
    E_PIXEL_FORMAT format;
    int localSocket;
};


