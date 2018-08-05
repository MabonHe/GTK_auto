#include "v4l2_capture.h"
#include "log.h"

const int BUFFER_SIZE  = 4;
const int POLL_TIMEOUT = 15000;
const char* V4L2_CAPTURE_TAG = "V4l2_Capture";
static V4l2_Settings* settings = V4l2_Settings::instance();

V4l2_Capture::V4l2_Capture(): mFd(-1), mIsBlockMode(false),
    mBufType(V4L2_BUF_TYPE_VIDEO_CAPTURE),
    mMemory(V4L2_MEMORY_MMAP) {

    memset(&mReqBufs, 0, sizeof(struct v4l2_requestbuffers));
    memset(&mFormat, 0, sizeof(struct v4l2_format));
    memset(&mFmtdesc, 0, sizeof(struct v4l2_fmtdesc));
    mBuffers = NULL;
    mCallback = NULL;

    video_querycap_func = NULL;
    video_get_format_func = NULL;
    video_try_format_func = NULL;
    video_set_format_func = NULL;
    video_stream_on_func = NULL;
    video_stream_off_func = NULL;
    video_qbuf_func = NULL;
    video_reqbufs_func = NULL;
    video_querybuf_func = NULL;
    map_buffer_func = NULL;
    malloc_and_map_buffers_func = NULL;
    unmap_and_free_buffers_func = NULL;
    video_dqbuf_func = NULL;
    poll_func = NULL;
    set_streamon_info_func = NULL;

    mDmabufMode = E_DMABUF_NULL;
    mDmaImportBuffers = NULL;
}

V4l2_Capture::V4l2_Capture(const V4l2_Capture &capture) {
    // self copy check
    if (this == &capture)
        return;

    mFd = capture.mFd;
    mBufType = capture.mBufType;
    mMemory = capture.mMemory;
    mReqBufs = capture.mReqBufs;

    memcpy(&mBuffers, capture.mBuffers, sizeof(capture.mBuffers));
}

V4l2_Capture::~V4l2_Capture() {
    unmap_and_free_buffers();
}

int V4l2_Capture::open_video(const char *devName) {
    if (mFd != -1) {
        return 0;
    }

    int mode = mIsBlockMode ? O_RDWR : O_RDWR | O_NONBLOCK;
    mFd = open(devName, mode);
    if (mFd == -1) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to open subdev device node %s %s", __func__, devName, strerror(errno));
        return -1;
    }
    LogI(V4L2_CAPTURE_TAG, "%s: Open subdev %s successfully.", __func__, devName);
    return 0;
}

int V4l2_Capture::close_video() {
    if (mFd) {
        int ret = close(mFd);
        mFd = 0;
        return ret;
    }
    return -1;
}

int V4l2_Capture::video_stream_on(int type) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to stream on mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "Unable to %s streaming: %s (%d).\n", "start", strerror(errno), errno);
        return ret;
    }

    return ret;
}


int V4l2_Capture::video_stream_off(int type) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to stream off mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_STREAMOFF, &type);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "Unable to %s streaming: %s (%d).\n", "stop", strerror(errno), errno);
        return ret;
    }

    return ret;
}

int V4l2_Capture::video_querycap(struct v4l2_capability &cap) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to querycap, mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_QUERYCAP, &cap);
    if(ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_QUERYCAP failed: %s", strerror(errno));
        return -1;
    }

    return ret;
}

int V4l2_Capture::video_config(enum v4l2_buf_type type, enum v4l2_memory memory) {
    mBufType = type;
    mMemory = memory;
    return 0;
}

int V4l2_Capture::video_enum_format(struct v4l2_fmtdesc &fmtdesc, enum v4l2_buf_type type) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to enum format, mFd = %d", __func__, mFd);
        return -1;
    }

    // get buffer type first
    CALL_API_FUNC_ARGS_1(video_querycap, mCap);
    type = mBufType;

    unsigned int i;
    int ret;
    for (i = 0; ; ++i) {
        fmtdesc.index = i;
        fmtdesc.type = type;
        ret = ioctl(mFd, VIDIOC_ENUM_FMT, &fmtdesc);

        if (errno == EINVAL) {
            LogI(V4L2_CAPTURE_TAG, "Enum format done.");
            ret = 0;
            break;
        }

        if (ret < 0) {
            LogE(V4L2_CAPTURE_TAG, "VIDIOC_ENUM_FMT failed: %s", strerror(errno));
            return -1;
        }
        if (i != fmtdesc.index) {
            LogE(V4L2_CAPTURE_TAG, "Warning: drive returned wrong format index %u.\n", fmtdesc.index);
        }
        if (type != fmtdesc.type) {
            LogE(V4L2_CAPTURE_TAG, "Warning: drive returned wrong format type %u.\n", fmtdesc.type);
        }
        LogI(V4L2_CAPTURE_TAG, "{ pixelformat = ''%c%c%c%c'', description = ''%s'' }\n",
                fmtdesc.pixelformat & 0xFF,
                (fmtdesc.pixelformat >> 8) & 0xFF,
                (fmtdesc.pixelformat >> 16) & 0xFF,
                (fmtdesc.pixelformat >> 24) & 0xFF,
                fmtdesc.description);
    }
    return ret;
}

int V4l2_Capture::video_get_format(struct v4l2_format &format) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to get format, mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_G_FMT, &format);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_G_FMT failed: %s", strerror(errno));
        return -1;
    }
    return ret;
}

int V4l2_Capture::video_try_format(struct v4l2_format &format) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to try format, mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_TRY_FMT, &format);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_TRY_FMT failed: %s", strerror(errno));
        return -1;
    }
    return ret;
}

int V4l2_Capture::video_set_format(struct v4l2_format &format) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to set format, mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_S_FMT failed: %s", strerror(errno));
        return -1;
    }
    return ret;
}

int V4l2_Capture::video_set_callback(FrameDoneCallback callback) {
    if (callback == NULL)
        return -1;
    mCallback = callback;
    return 0;
}

int V4l2_Capture::video_qbuf(struct v4l2_buffer &buf) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to qbuf, mFd = %d", __func__, mFd);
        return -1;
    }
    int ret = ioctl(mFd, VIDIOC_QBUF, &buf);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_QBUF failed: %s", strerror(errno));
        return -1;
    }
    return ret;
}

int V4l2_Capture::video_reqbufs(struct v4l2_requestbuffers &reqbufs) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to reqbufs, mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_REQBUFS, &reqbufs);
    if (ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_REQBUFS failed: %s", strerror(errno));
        return -1;
    }
    if ((int)reqbufs.count < BUFFER_SIZE) {
        LogE(V4L2_CAPTURE_TAG, "%s: Got less buffers than requested! %d < %d",__func__, (int)reqbufs.count, BUFFER_SIZE);
        return -1;
    }
    return ret;
}

int V4l2_Capture::video_querybuf(struct v4l2_buffer &buffer) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to querybuf, mFd = %d", __func__, mFd);
        return -1;
    }

    int ret = ioctl(mFd, VIDIOC_QUERYBUF, &buffer);
    if(ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_QUERYBUF failed: %s", strerror(errno));
        return -1;
    }
    return ret;
}

int V4l2_Capture::malloc_and_map_buffers() {
    memset(&mReqBufs, 0, sizeof(struct v4l2_requestbuffers));
    mReqBufs.memory = mMemory;
    mReqBufs.count = BUFFER_SIZE;
    mReqBufs.type = mBufType;
    CALL_API_FUNC_ARGS_1(video_reqbufs, mReqBufs);

    if (mBuffers != NULL) {
        LogE(V4L2_CAPTURE_TAG, "%s: Buffers have been malloced", __func__);
        return -1;
    }

    mBuffers = (Buffer *)malloc(BUFFER_SIZE * sizeof (Buffer));
    if (!mBuffers) {
        LogE(V4L2_CAPTURE_TAG, "Malloc buffers error: %s", __func__);
        return -1;
    }

    memset(mBuffers, 0, BUFFER_SIZE * sizeof (Buffer));

    for (uint i = 0; i < mReqBufs.count; i++) {
        struct v4l2_plane planes[VIDEO_MAX_PLANES];
        struct v4l2_buffer buffer;
        memset(&buffer, 0, sizeof buffer);
        memset(&planes, 0, sizeof planes);
        buffer.type = mBufType;
        buffer.memory = mMemory;
        buffer.index = i;
        if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
            buffer.length = VIDEO_MAX_PLANES;
            buffer.m.planes = planes;
        }

        CALL_API_FUNC_ARGS_1(video_querybuf, buffer);


        CALL_API_FUNC_ARGS_2(map_buffer, i, buffer);

        CALL_API_FUNC_ARGS_1(video_qbuf, buffer);
    }
    return 0;
}

static int dmabuf_export(int v4lfd, enum v4l2_buf_type bt, int index, int *dmafd)
{
    struct v4l2_exportbuffer expbuf;

    memset(&expbuf, 0, sizeof(expbuf));
    expbuf.type = bt;
    expbuf.index = index;
    if (ioctl(v4lfd, VIDIOC_EXPBUF, &expbuf) == -1) {
        LogE(V4L2_CAPTURE_TAG, "%s: failed to call ioctl VIDIOC_EXPBUF", __func__);
        return -1;
    }

    *dmafd = expbuf.fd;

    return 0;
}

static int dmabuf_export_mp(int v4lfd, enum v4l2_buf_type bt, int index,
	int dmafd[], int n_planes)
{
    int i;

    for (i = 0; i < n_planes; ++i) {
        struct v4l2_exportbuffer expbuf;

        memset(&expbuf, 0, sizeof(expbuf));
        expbuf.type = bt;
        expbuf.index = index;
        expbuf.plane = i;
        if (ioctl(v4lfd, VIDIOC_EXPBUF, &expbuf) == -1) {
            LogE(V4L2_CAPTURE_TAG, "%s: failed to call ioctl VIDIOC_EXPBUF", __func__);
            while (i)
                close(dmafd[--i]);
            return -1;
        }
        dmafd[i] = expbuf.fd;
    }

    return 0;
}


int V4l2_Capture::map_buffer(int index, struct v4l2_buffer &buffer) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to map buffer, mFd = %d", __func__, mFd);
        return -1;
    }

    if (mBuffers == NULL) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to map buffer, mBuffer is NULL", __func__);
        return -1;
    }

    int pageSize = getpagesize();
    int ret = -1;
    unsigned int length;
    unsigned int offset;
    
    mBuffers[index].idx = index;

    switch((uint)mMemory) {
        case V4L2_MEMORY_MMAP:
            if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
                length = buffer.m.planes[0].length;
                offset = buffer.m.planes[0].m.mem_offset;
            } else {
                length = buffer.length;
                offset = buffer.m.offset;
            }

            // DMA buffer export
            if (mDmabufMode == E_DMABUF_EXPORT)
            {
                if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
                {
                    // Hard coded for MP, as currently only support 1
                    dmabuf_export_mp(mFd, mBufType, index, mBuffers[index].dma_buf_fd, 1);

                    if (mBuffers[index].dma_buf_fd[0] == 0)
                    {
                        LogE(V4L2_CAPTURE_TAG, "%s: export dmabuf failed", __func__);
                        return -1;
                    }
                }
                else
                {
                    dmabuf_export(mFd, mBufType, index, &mBuffers[index].dma_buf_fd[0]);
                    if (mBuffers[index].dma_buf_fd[0] == 0)
                    {
                        LogE(V4L2_CAPTURE_TAG, "%s: export dmabuf failed", __func__);
                        return -1;
                    }
                }
            }
            // Mmap
            else
            {
                mBuffers[index].start[0] = mmap(NULL, length,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    mFd, offset);
                
                if (MAP_FAILED == mBuffers[index].start[0]) {
                    LogE(V4L2_CAPTURE_TAG, "%s: MMAP failed: %s", __func__, strerror(errno));
                    return -1;
                }
                mBuffers[index].length[0] = length; // remember for munmap
            }            

            break;
        case V4L2_MEMORY_USERPTR:
            if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
                length = buffer.m.planes[0].length;
            } else {
                length = buffer.length;
            }
            mBuffers[index].length[0] = length;
            ret = posix_memalign(&mBuffers[index].start[0], pageSize, length);

            if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
                buffer.m.planes[0].m.userptr = (unsigned long)mBuffers[index].start[0];
            } else {
                buffer.m.userptr = (unsigned long)mBuffers[index].start[0];
            }
            if (ret < 0) {
                LogE(V4L2_CAPTURE_TAG, "%s: Fail to align memory.", __func__);
                return -1;
            }
            break;
        // DMA buffer importing
        case V4L2_MEMORY_DMABUF:
            if (mDmaImportBuffers == NULL)
            {
                LogE(V4L2_CAPTURE_TAG, "%s: No exported dmabuf from external device.", __func__);
                return -1;
            }
            
            if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
                mBuffers[index].dma_buf_fd[0] = mDmaImportBuffers[index].dma_buf_fd[0];
                buffer.m.planes[0].m.fd = mDmaImportBuffers[index].dma_buf_fd[0];
            } else {
                mBuffers[index].dma_buf_fd[0] = mDmaImportBuffers[index].dma_buf_fd[0];
                buffer.m.fd = mDmaImportBuffers[index].dma_buf_fd[0];
            }

            break;
        case V4L2_MEMORY_OVERLAY:
            LogE(V4L2_CAPTURE_TAG, "%s: Do expilicitly defination in v4l2 document now.", __func__);
            return -1;
            break;
    }
    return 0;
}

int V4l2_Capture::unmap_and_free_buffers() {
    int ret = 0;
    
    if (mBuffers == NULL)
        return 0;

    switch((uint)mMemory) {
        case V4L2_MEMORY_MMAP:
            // DMA buffer export
            if (mDmabufMode == E_DMABUF_EXPORT)
            {
                for (uint i = 0; i < mReqBufs.count;i ++) {
                    close(mBuffers[i].dma_buf_fd[0]);
                    mBuffers[i].dma_buf_fd[0] = -1;
                }
            }
            // Mmap
            else
            {
                for (uint i = 0; i < mReqBufs.count;i ++) {
                    ret = munmap(mBuffers[i].start[0], mBuffers[i].length[0]);
                    if (ret < 0)
                    {
                        LogE(V4L2_CAPTURE_TAG, "%s: Unable to unmap buffer %u: %s (%d).", 
                            __func__, i, strerror(errno), errno);
                    }
                }
            }
            free(mBuffers);
            mBuffers = NULL;
            break;
        case V4L2_MEMORY_USERPTR:
            for (uint i = 0; i < mReqBufs.count; i++) {
                if (mBuffers[i].start[0] != NULL) {
                    free(mBuffers[i].start[0]);
                    mBuffers[i].start[0] = NULL;
                }
            }

            free(mBuffers);
            mBuffers = NULL;
            break;
            
        case V4L2_MEMORY_DMABUF: // DMA buffer importing
            for (uint i = 0; i < mReqBufs.count;i ++) {
                if (mBuffers[i].start[0] != NULL)
                {
                    ret = munmap(mBuffers[i].start[0], mBuffers[i].length[0]);
                    if (ret < 0)
                    {
                        LogE(V4L2_CAPTURE_TAG, "%s: Unable to unmap buffer %u: %s (%d).", 
                            __func__, i, strerror(errno), errno);
                    }
                }
            }
            break;
            
        case V4L2_MEMORY_OVERLAY:
            LogE(V4L2_CAPTURE_TAG, "%s: Do expilicitly defination in v4l2 document now.", __func__);
            return -1;
            break;
    }
    return ret;
}

int V4l2_Capture::video_dqbuf(struct v4l2_buffer &buffer) {
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to dqbuf, mFd = %d", __func__, mFd);
        return -1;
    }
    int ret = ioctl(mFd, VIDIOC_DQBUF, &buffer);
    if(ret < 0) {
        LogE(V4L2_CAPTURE_TAG, "VIDIOC_DQBUF failed: %s, ret: %d", strerror(errno), ret);
        return -1;
    }

    struct Buffer *buf = &mBuffers[buffer.index];

    if (mMemory == V4L2_MEMORY_DMABUF && mDmabufMode == E_DMABUF_IMPORT && 
        buf->start[0] == NULL)
    {
        
        unsigned int len = 0;
        int fd = -1;
        
        if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
        {
            len = buffer.m.planes[0].length;
            fd = buffer.m.planes[0].m.fd;
        }
        else
        {
            len = buffer.length;
            fd = buffer.m.fd;
        }

        buf->start[0] = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
        buf->length[0] = len;
    }
    
    return ret;
}

int V4l2_Capture::poll() {
    struct pollfd pfd = {0,0,0};
    pfd.fd = mFd;
    pfd.events = POLLPRI | POLLIN | POLLERR;

    int ret = 0;
    ret = ::poll(&pfd, 1, POLL_TIMEOUT);

    if (ret == 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Poll time out, mFd = %d\n", __func__, mFd);
        return -1;
    }

    if (ret == -1) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to poll: %s, ret: %d", __func__, strerror(errno), ret);
        return -1;
    }

    if (pfd.revents & POLLERR) {
        LogE(V4L2_CAPTURE_TAG, "%s: Poll received POLLERR, ret: %d", __func__, ret);
        return -1;
    }
    return ret;
}

int V4l2_Capture::set_import_dmabuf_fds(Buffer *dmaImportBuffers)
{
    mDmaImportBuffers = dmaImportBuffers;
    return 0;
}

int V4l2_Capture::prepare()
{
    if (mFd < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Failed to do capture, mFd = %d", __func__, mFd);
        return -1;
    }

    if (mDmabufMode == E_DMABUF_EXPORT && mMemory != V4L2_MEMORY_MMAP)
    {
        LogE(V4L2_CAPTURE_TAG, "%s: DMABUF export should use memory MMAP mode.", __func__);
        return -1;
    }

    if (mDmabufMode == E_DMABUF_IMPORT && mMemory != V4L2_MEMORY_DMABUF)
    {
        LogE(V4L2_CAPTURE_TAG, "%s: DMABUF import should use memory DMABUF mode.", __func__);
        return -1;
    }    

    CALL_API_FUNC_ARGS_1(video_querycap, mCap);

    //Set buffer types
    unsigned int capabilities = mCap.capabilities & V4L2_CAP_DEVICE_CAPS ? mCap.device_caps : mCap.capabilities;

    if (capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        mBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        LogI(V4L2_CAPTURE_TAG, "%s: mBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE", __func__);
    } else if (capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        mBufType =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
        LogI(V4L2_CAPTURE_TAG, "%s: mBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE", __func__);
    }
    settings->bufferType = mBufType;

    mFormat.type = mBufType;

    int width = mFormat.fmt.pix.width;
    int height = mFormat.fmt.pix.height;
    int pixelformat = mFormat.fmt.pix.pixelformat;
    int fieldOrder = mFormat.fmt.pix.field;

    if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		mFormat.fmt.pix_mp.width = width;
        mFormat.fmt.pix_mp.height = height;
        mFormat.fmt.pix_mp.num_planes = 1;
        LogI(V4L2_CAPTURE_TAG, "mFormat.fmt.pix_mp.num_planes=%d", mFormat.fmt.pix_mp.num_planes);
        mFormat.fmt.pix_mp.pixelformat = pixelformat;
        mFormat.fmt.pix_mp.field = fieldOrder;
        for (int i = 0; i < mFormat.fmt.pix_mp.num_planes; i++) {
            mFormat.fmt.pix_mp.plane_fmt[i].bytesperline = 0;
            mFormat.fmt.pix_mp.plane_fmt[i].sizeimage = 0;
        }
    }

    CALL_API_FUNC_ARGS_1(video_try_format, mFormat);
    CALL_API_FUNC_ARGS_1(video_set_format, mFormat);
    CALL_API_FUNC_ARGS_1(video_get_format, mFormat);
	if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		for (int i = 0; i < mFormat.fmt.pix_mp.num_planes; i++) {
		LogI(V4L2_CAPTURE_TAG, " Stride %d, buffer size %d",
			mFormat.fmt.pix_mp.plane_fmt[i].bytesperline,
			mFormat.fmt.pix_mp.plane_fmt[i].sizeimage);
		}
	}
    CALL_API_FUNC_ARGS_0(malloc_and_map_buffers);

    return 0;
}

int V4l2_Capture::set_streamon_info()
{
    int ret = 0;
    if (settings->nstreams > 0)
    {
        struct v4l2_streamon_info info = {};
        info.total = settings->nstreams;
        info.enable[0] = settings->stream_id;
        
        ret = ioctl(mFd, VIDIOC_S_STREAMON_INFO, &info);
        if (ret < 0)
            printf("Failed to set streamon info: %s (%d)\n",
                strerror(errno), errno);        
    }

    return ret;
}

int V4l2_Capture::release()
{
    CALL_API_FUNC_ARGS_0(unmap_and_free_buffers);
    return 0;
}

int V4l2_Capture::do_capture(int nframes, int skip_frames) 
{

    if (nframes < 0 || skip_frames < 0) {
        LogE(V4L2_CAPTURE_TAG, "%s: Invalid nframes: %d or skip_frames: %d, need >= 0", __func__, nframes, skip_frames);
        return -1;
    }

    if (0 != prepare())
    {
        LogE(V4L2_CAPTURE_TAG, "%s: preparation for capture is failed", __func__);
        return -1;
    }

    CALL_API_FUNC_ARGS_0(set_streamon_info);

    CALL_API_FUNC_ARGS_1(video_stream_on, mBufType);

    LogI(V4L2_CAPTURE_TAG, "%s: Start stream", __func__);

    for (int i = 0; i < skip_frames + nframes; i++) {
        struct v4l2_buffer buffer;
        struct v4l2_plane planes[VIDEO_MAX_PLANES];
        // non-block mode, use poll to select ready buffer
        if (!mIsBlockMode) {
            CALL_API_FUNC_ARGS_0(poll);
        }
        memset(&buffer, 0, sizeof buffer);
        memset(&planes, 0, sizeof planes);
        buffer.type = mBufType;
        buffer.memory = mMemory;
        buffer.length = VIDEO_MAX_PLANES;
        buffer.m.planes = planes;
        CALL_API_FUNC_ARGS_1(video_dqbuf, buffer);

        unsigned int size = 0;
        if (mBufType == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
            size = buffer.m.planes[0].bytesused;;
        } else {
            size = buffer.bytesused;
        }
        if (mCallback && i >= skip_frames) {
            mCallback(mBuffers[buffer.index].start[0], size, i - skip_frames);
        }

        CALL_API_FUNC_ARGS_1(video_qbuf, buffer);
    }

    CALL_API_FUNC_ARGS_1(video_stream_off, mBufType);
    LogI(V4L2_CAPTURE_TAG, "%s: Stop stream", __func__);

    if (0 != release())
    {
        LogE(V4L2_CAPTURE_TAG, "%s: failed to release", __func__);
        return -1;
    }

    return 0;
}
