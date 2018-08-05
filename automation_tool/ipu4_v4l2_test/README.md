V4L2 API Test Suite
===================

This is V4L2 API Test Suite for IPU4 device.

How to use
----------

Please install gtest-dev library:

    sudo apt-get install libgtest-dev

and then:

    make
    ./ipu4_v4l2_test

use:

    make install

to generate case scripts.

Directory struct
----------------

    ├── main.cpp
    ├── Makefile
    ├── ReadMe.md
    └── src
        ├── cases // cases directory
        │   ├── basic_api
        │   │   ├── close
        │   │   │   ├── case_close.cpp
        │   │   ├── open
        │   │   │   ├── case_open.cpp
        │   │   └── querycap
        │   │       └── case_querycap.cpp
        │   ├── mc_stdioctl
        │   ├── mc_yavta
        │   ├── subdev_stdioctl
        │   └── subdev_yavta
        └── common // common class
            ├── log.cpp
            ├── log.h
            ├── v4l2_define.h
            ├── v4l2_test.cpp
            └── v4l2_test.h

