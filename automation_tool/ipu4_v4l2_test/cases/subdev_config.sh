#!/bin/sh

# subdev configuration
yavta -w "0x009f0901 0" /dev/v4l-subdev9
yavta -w "0x009e0903 100" /dev/v4l-subdev8
yavta -w "0x009e0902 3000" /dev/v4l-subdev8
yavta -w "0x009e0901 3000" /dev/v4l-subdev8
yavta -w "0x00980911 1000" /dev/v4l-subdev8
yavta -w "0x00980915 0" /dev/v4l-subdev8
yavta -w "0x00980914 1" /dev/v4l-subdev8
yavta -w "0x00982901 8" /dev/v4l-subdev9
yavta -w "0x00982902 104" /dev/v4l-subdev9
