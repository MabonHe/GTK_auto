#
#  GStreamer
#  Copyright (C) 2015-2016 Intel Corporation
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#
#  Alternatively, the contents of this file may be used under the
#  GNU Lesser General Public License Version 2.1 (the "LGPL"), in
#  which case the following provisions apply instead of the ones
#  mentioned above:
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Library General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Library General Public License for more details.
#
#  You should have received a copy of the GNU Library General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#  Boston, MA 02111-1307, USA.
#

ifndef PKG_CONFIG
PKG_CONFIG=pkg-config
endif

GTEST_DIR = /usr/src/gtest

testbin=camera_gst_property_test

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -static \
            -I/usr/include/libcamhal/api \
            -I/usr/include/icamerasrc/interfaces

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
all: $(testbin)

clean:
	rm -f $(mainobj) $(srcobjs) $(testbin) gtest.a gtest_main.a gtest-all.o

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

CXXFLAGS+= -I./src `$(PKG_CONFIG) --cflags gstreamer-1.0 glib-2.0 gstreamer-video-1.0` -std=c++11
LDFLAGS=`$(PKG_CONFIG) --libs  gstreamer-1.0 gobject-2.0  glib-2.0 gstreamer-video-1.0 gstreamer-app-1.0 gstreamer-allocators-1.0` -Wl,-Bstatic  -Wl,-Bdynamic  -lcamhal -lgsticamerainterface -lgcc_s -lpthread -std=c++11

srcobjs=$(patsubst %.cpp, %.o, $(shell ls ./src/*.cpp))

mainobj=./src/main.o

%.o: %.cpp $(GTEST_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(testbin): $(mainobj) $(srcobjs) gtest.a
	$(CXX) -o $@ $^ $(LDFLAGS)


