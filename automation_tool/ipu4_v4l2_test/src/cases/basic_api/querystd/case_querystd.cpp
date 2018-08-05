#include "case_querystd.h"

extern const char* QUERYSTD_TEST = "QUERYSTD_Test";

Querystd_Test::Querystd_Test() {
}

Querystd_Test::~Querystd_Test() {
}


int Querystd_Test::vidioc_get_std(const char* dev)
{
    V4l2_Test::setup_dev(dev);
    int ret = ioctl(fd, VIDIOC_G_STD, &std_id);
    if (ret == 0){
        LogI(QUERYSTD_TEST, "The STD flags of VIDIOC are: %d", std_id);
        return 0;
    }
    else {
        LogE(QUERYSTD_TEST, "Get STD flags fails");
        return -1;
    }
}

int Querystd_Test::vidioc_set_std(const char* dev)
{
    V4l2_Test::setup_dev(dev);
    int ret=ioctl(fd, VIDIOC_S_STD, &std_id);
    if(ret == 0){
        LogI(QUERYSTD_TEST, "Set STD flags %d of VIDIOC successfully", std_id);
        return 0;
    }
    else{
        LogE(QUERYSTD_TEST, "The STD flags being set is ambiguous, please reset after checking");
        return -1;
    }
}
