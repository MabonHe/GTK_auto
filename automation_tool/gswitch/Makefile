CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar

CXXFLAGS= -g -I/usr/include `pkg-config --cflags gstreamer-1.0 glib-2.0 gstreamer-video-1.0`
LDFLAGS= -L/usr/lib64/ `pkg-config --libs  gstreamer-1.0 gobject-2.0  glib-2.0 gstreamer-video-1.0` -Wl,-Bstatic -L/usr/lib64/ -Wl,-Bdynamic -lgcc_s -lpthread

srcdir=src
srcobjs=$(patsubst %.cpp, %.o, $(shell ls $(srcdir)/*.cpp))

appbin=gswitch

all: $(appbin)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(appbin): $(srcobjs)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(srcobjs) $(appbin)
