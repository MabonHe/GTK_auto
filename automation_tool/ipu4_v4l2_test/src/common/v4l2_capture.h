#ifndef V4L2_CAPTURE_H
#define V4L2_CAPTURE_H

#include "v4l2_define.h"
#include "v4l2_settings.h"

#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <linux/videodev2.h>
#include <linux/v4l2-subdev.h>
#include <poll.h>

enum E_DMABUF_MODE
{
    E_DMABUF_NULL = 0,
    E_DMABUF_EXPORT,
    E_DMABUF_IMPORT
};

#define CALL_API_FUNC_ARGS_0(api) \
    if (api##_func) { \
        if (api##_func(*this) < 0) { \
            return -1; \
        } \
    } else { \
        if (api() < 0) { \
            return -1; \
        } \
    }

#define CALL_API_FUNC_ARGS_1(api, arg) \
    if (api##_func) { \
        if (api##_func(*this, arg) < 0) { \
            return -1; \
        } \
    } else { \
        if (api(arg) < 0) { \
            return -1; \
        } \
    }

#define CALL_API_FUNC_ARGS_2(api, arg1, arg2) \
    if (api##_func) { \
        if (api##_func(*this, arg1, arg2) < 0) { \
            return -1; \
        } \
    } else { \
        if (api(arg1, arg2) < 0) { \
            return -1; \
        } \
    }

typedef struct Buffer {
    unsigned int idx;
    void *start[VIDEO_MAX_PLANES];
    unsigned int  length[VIDEO_MAX_PLANES];
    int dma_buf_fd[VIDEO_MAX_PLANES];
    unsigned int padding[VIDEO_MAX_PLANES];
} Buffer;

typedef void (*FrameDoneCallback) (void *data, uint size, int index);

#define VIDEO_MAX_PLANES 8

// V4L2_Capture
class V4l2_Capture
{
public:
    V4l2_Capture();
    V4l2_Capture(const V4l2_Capture& capture);
    ~V4l2_Capture();

    virtual int open_video(const char *devName);
    virtual int close_video();

    virtual int video_querycap(struct v4l2_capability &cap);

    int video_config(enum v4l2_buf_type type, enum v4l2_memory memory);
    int video_set_callback(FrameDoneCallback callback);

    virtual int video_enum_format(struct v4l2_fmtdesc &fmtdesc, enum v4l2_buf_type type);
    virtual int video_get_format(struct v4l2_format &format);
    virtual int video_try_format(struct v4l2_format &format);
    virtual int video_set_format(struct v4l2_format &format);
    virtual int prepare();
    virtual int set_streamon_info();
    virtual int release();
    virtual int do_capture(int nframes = 1, int skip_frames = 0);
    virtual int set_import_dmabuf_fds(Buffer *dmaImportBuffers);

    // Setting function handler for testing
    int (*video_querycap_func)(const V4l2_Capture &capture, struct v4l2_capability &cap);
    int (*video_get_format_func)(const V4l2_Capture &capture, struct v4l2_format &format);
    int (*video_try_format_func)(const V4l2_Capture &capture, struct v4l2_format &format);
    int (*video_set_format_func)(const V4l2_Capture &capture, struct v4l2_format &format);
    int (*video_stream_on_func)(const V4l2_Capture &capture, int type);
    int (*video_stream_off_func)(const V4l2_Capture &capture, int type);
    int (*video_qbuf_func)(const V4l2_Capture &capture, struct v4l2_buffer &buf);
    int (*video_reqbufs_func)(const V4l2_Capture &capture, struct v4l2_requestbuffers &reqbufs);
    int (*video_querybuf_func)(const V4l2_Capture &capture, struct v4l2_buffer &buffer);
    int (*map_buffer_func)(const V4l2_Capture &capture, int index, struct v4l2_buffer &buffer);
    int (*malloc_and_map_buffers_func)(const V4l2_Capture &capture);
    int (*unmap_and_free_buffers_func)(const V4l2_Capture &capture);
    int (*video_dqbuf_func)(const V4l2_Capture &capture, struct v4l2_buffer &buffer);
    int (*poll_func)(const V4l2_Capture &capture);
    int (*set_streamon_info_func)(const V4l2_Capture &capture);

protected:
    virtual int video_stream_on(int type);
    virtual int video_stream_off(int type);
    virtual int video_qbuf(struct v4l2_buffer &buf);
    virtual int video_reqbufs(struct v4l2_requestbuffers &reqbufs);
    virtual int video_querybuf(struct v4l2_buffer &buffer);
    virtual int map_buffer(int index, struct v4l2_buffer &buffer);
    virtual int malloc_and_map_buffers();
    virtual int unmap_and_free_buffers();
    virtual int video_dqbuf(struct v4l2_buffer &buffer);
    virtual int poll();
public:
    int mFd;

    bool mIsBlockMode;

    enum v4l2_buf_type mBufType;
    enum v4l2_memory mMemory;
    enum E_DMABUF_MODE mDmabufMode;

    struct v4l2_capability mCap;
    struct v4l2_fmtdesc mFmtdesc;
    struct v4l2_format mFormat;
    struct v4l2_requestbuffers mReqBufs;

    Buffer *mBuffers;
    Buffer *mDmaImportBuffers;
    FrameDoneCallback mCallback;
};

#endif // V4L2_CAPTURE_H
