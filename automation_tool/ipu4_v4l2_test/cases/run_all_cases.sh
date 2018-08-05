#!/bin/bash
# MC_Stdioctl_Test cases
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=320 -h=240
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=320 -h=240
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=320 -h=240
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=320 -h=240
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=640 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=640 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=640 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=640 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=480
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=720
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=720
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=720
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=720
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=800
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=800
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=800
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1280 -h=800
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1920 -h=1080
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=true -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1920 -h=1080
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_MMAP -format=V4L2_PIX_FMT_SGRBG8 -w=1920 -h=1080
../ipu4_v4l2_test --gtest_filter="MC_Stdioctl_Test.*" -d=/dev/video5 -i=false -m=V4L2_MEMORY_USERPTR -format=V4L2_PIX_FMT_SGRBG8 -w=1920 -h=1080
