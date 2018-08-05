# NOTES FOR USERS:
#  - host need to have gtest-dev (e.g apt-get install libgtest-dev)
#
# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = /usr/src/gtest

# Where to find user code.
USER_DIR = .

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include -I$(USER_DIR)/include -I$(USER_DIR)/src/common

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -static

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = ipu4_v4l2_test

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

# House-keeping build targets.

all : $(TESTS)

clean :
	rm -rf $(TESTS) gtest.a gtest_main.a *.o $(SRCOBJECT) TestCases

install :
	./generator.sh sh

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
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

SRCOBJECT=$(patsubst %.cpp, %.o,$(shell ls -r $(USER_DIR)/src/common/*.cpp $(USER_DIR)/src/cases/*/*.cpp $(USER_DIR)/src/cases/*/*/*.cpp))

%.o: %.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
ipu4_v4l2_test: main.o gtest_main.a $(SRCOBJECT)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -static -Wl,-s $^ -lpthread -o $@ -lrt -O2
