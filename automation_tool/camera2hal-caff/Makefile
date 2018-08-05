#
#  INTEL CONFIDENTIAL
#
#  Copyright (C) 2016 Intel Corporation
#  All Rights Reserved.
#
#  The source code contained or described herein and all documents
#  related to the source code ("Material") are owned by Intel Corporation
#  or licensors. Title to the Material remains with Intel
#  Corporation or its licensors. The Material contains trade
#  secrets and proprietary and confidential information of Intel or its
#  licensors. The Material is protected by worldwide copyright
#  and trade secret laws and treaty provisions. No part of the Material may
#  be used, copied, reproduced, modified, published, uploaded, posted,
#  transmitted, distributed, or disclosed in any way without Intel's prior
#  express written permission.
#
#  No License under any patent, copyright, trade secret or other intellectual
#  property right is granted to or conferred upon you by disclosure or
#  delivery of the Materials, either expressly, by implication, inducement,
#  estoppel or otherwise. Any license under such intellectual property rights
#  must be express and approved by Intel in writing.
#
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar

#CXXFLAGS= -g -I/usr/include -I./utils/ -I../src -I/usr/include/libcamhal/ `pkg-config --cflags gstreamer-1.0 glib-2.0 gstreamer-video-1.0 gtk+-3.0`
#LDFLAGS= -L/usr/lib64/ -lcamhal -lpthread `pkg-config --libs  gstreamer-1.0 gobject-2.0  glib-2.0 gstreamer-video-1.0 gtk+-3.0`

CXXFLAGS= -g -I/usr/include `pkg-config --cflags gstreamer-1.0 glib-2.0 gstreamer-video-1.0 gtk+-3.0` -std=c++11
LDFLAGS= -L/usr/lib64/ -lpthread `pkg-config --libs  gstreamer-1.0 gobject-2.0  glib-2.0 gstreamer-video-1.0 gtk+-3.0` -std=c++11

casedir=.

srcobjs=$(patsubst %.cpp, %.o, $(shell ls $(casedir)/*.cpp))

mainobj=main.o

testbin=gstCaff

all: $(testbin)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(testbin): $(mainobj) $(srcobjs)
	$(CXX) -o $@ $^ $(LDFLAGS)
clean:
	rm -f $(mainobj) $(srcobjs) $(testbin)
	rm *.avi
