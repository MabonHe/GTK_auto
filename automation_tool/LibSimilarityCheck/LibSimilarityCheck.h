////////////////////////////////////////////////////////////////////////////////////
// Copyright (2014) Intel Corporation All Rights Reserved.
//
// The source code, information and material ("Material") contained herein is owned
// by Intel Corporation or its suppliers or licensors, and title to such Material
// remains with Intel Corporation or its suppliers or licensors. The Material
// contains proprietary information of Intel or its suppliers and licensors. The
// Material is protected by worldwide copyright laws and treaty provisions.
// No part of the Material may be used, copied, reproduced, modified, published,
// uploaded, posted, transmitted, distributed or disclosed in any way without Intel's
// prior express written permission. No license under any patent, copyright or other
// intellectual property rights in the Material is granted to or conferred upon you,
// either expressly, by implication, inducement, estoppel or otherwise. Any license
// under such intellectual property rights must be express and approved by Intel in
// writing.
// Unless otherwise agreed by Intel in writing, you may not remove or alter this
// notice or any other notice embedded in Materials by Intel or Intel’s suppliers
// or licensors in any way.
//
////////////////////////////////////////////////////////////////////////////////////

#ifndef _LIB_SIMILARITY_CHECK_H_
#define _LIB_SIMILARITY_CHECK_H_

#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;

#ifdef _WINDOWS
#ifdef SIMILARITYCHECK_EXPORTS
#define SIMILARITYCHECK_API __declspec(dllexport)
#else
#define SIMILARITYCHECK_API __declspec(dllimport)
#endif
#ifndef _WINODWS_MOBILE
#ifdef _DEBUG
#pragma comment(lib, "opencv_highgui231d.lib")
#pragma comment(lib, "opencv_imgproc231d.lib")
#pragma comment(lib, "opencv_core231d.lib")
#pragma comment(lib, "opencv_video231d.lib")
#pragma comment(lib, "opencv_legacy231d.lib")
#pragma comment(lib, "opencv_features2d231d.lib")
#pragma comment(lib, "opencv_calib3d231d.lib")
#else
#pragma comment(lib, "opencv_highgui231.lib")
#pragma comment(lib, "opencv_imgproc231.lib")
#pragma comment(lib, "opencv_core231.lib")
#pragma comment(lib, "opencv_video231.lib")
#pragma comment(lib, "opencv_legacy231.lib")
#pragma comment(lib, "opencv_features2d231.lib")
#pragma comment(lib, "opencv_calib3d231.lib")
#endif
#else
#pragma comment(lib, "opencv_core.mobile.lib")
#pragma comment(lib, "opencv_imgproc.mobile.lib")
#pragma comment(lib, "opencv_imgcodecs.mobile.lib")
#pragma comment(lib, "opencv_flann.mobile.lib")
#pragma comment(lib, "opencv_features2d.mobile.lib")
#pragma comment(lib, "opencv_calib3d.mobile.lib")
#pragma comment(lib, "opencv_ml.mobile.lib")
#endif
#else
#define SIMILARITYCHECK_API
#endif

/******************************************************************************
// @ Reference code @

// Load full FOV image and test image
Mat img_full_fov, img_to_test;
img_full_fov = imread("Path\\img_full_fov.jpg");
img_to_test  = imread("Path\\img_to_test.jpg");

// Step 1: Calculate Homography Matrix, Max FOV Image -> Test Image
Mat H;
bool ret = CalculateHomographyMatrix(img_full_fov, img_to_test, H);
if (!ret)
return;

// Step 2: Generate Reference Image From Full Fov image, the reference image would have the same resolution and FOV as the image to test.
Mat img_reference;
ret = GenerateReferenceFromFullFov(img_full_fov, H, img_reference, img_to_test.cols, img_to_test.rows);
if (!ret)
return;

// Step 3: Calculate SSIM of Reference image and test image.
double ssim_avg = 0, ssim_min = 0;
V_SSIM_BLOCK artifact_blocks;
CalculateSsimCn3(img_reference, img_to_test, 16, 0.4, ssim_avg, ssim_min, artifact_blocks);
// You can use the average ssim, min ssim and artifact blocks etc. information to make a judagement.

// Step 4: Draw artifact blocks on the image for debug usage.
DrawArtifactBlocks(img_to_test, artifact_blocks);

// Note: if you have reference image, you can skip Step 1 and 2.
******************************************************************************/


typedef struct _SSIM_BLOCK
{
    int ichannel;       ///< index of image channel, 0, 1, 2
    int x;              ///< Position X of the block
    int y;              ///< Position Y of the block
    int kernel_size;    ///< SSIM kernel size
    double ssim_val;    ///< SSIM value of the block
    int neighbor_count;   ///< If the block is isolated in the block array
}SSIM_BLOCK;

typedef vector<SSIM_BLOCK> V_SSIM_BLOCK;

/******************************************************************************
* Function:         CalculateHomographyMatrix
* Description:      Calculate the Homography Matrix for full FOV image and test image
* Parameters:       [in]full  -  Full FOV image (max resolution still image) (3 channels)
* Parameters:       [in]frame -  Frame (preview, video, still) (3 channels)
* Parameters:       [in/out]H -  Homography Matrix (3x3 double)
*
* Return:           true: success, false: failed
******************************************************************************/
SIMILARITYCHECK_API bool CalculateHomographyMatrix(
    Mat &full,
    Mat &frame,
    Mat &H);

/******************************************************************************
* Function:         GenerateReferenceFromFullFov
* Description:      Generate reference  Homography Matrix for full FOV image and test image
* Parameters:       [in]full  -  Full FOV image (max resolution still image) (3 channels)
* Parameters:       [in]H     -  Homography Matrix
* Parameters:       [out]reference -  Reference image
* Parameters:       [in]w -  Reference image width
* Parameters:       [in]h -  Reference image height
*
* Return:           true: success, false: failed
******************************************************************************/
SIMILARITYCHECK_API bool GenerateReferenceFromFullFov(
    Mat &full,
    Mat &H,
    Mat &reference,
    int w,
    int h);

/******************************************************************************
* Function:         CalculateSsimCn3
* Description:      Calculate the SSIM similarity between two images
* Parameters:       [in]src1  -  image 1, the reference image
* Parameters:       [in]src2  -  image 2, the test image
* Parameters:       [in]kernel_size  -  SSIM method kernel size, suggest to use 16~32
* Parameters:       [in]similarity_threshold  -  threshold for local blocks
* Parameters:       [out]ssim_avg  -  average SSIM value
* Parameters:       [out]ssim_min  -  minimal SSIM value of all blocks
* Parameters:       [out]artifact_blocks  -  all blocks of which SSIM value is smaller than the threshold
*
* Return:           void
******************************************************************************/
SIMILARITYCHECK_API void CalculateSsimCn3(
    Mat &src1,
    Mat &src2,
    int kernel_size,
    double similarity_threshold,
    double &ssim_avg,
    double &ssim_min,
    V_SSIM_BLOCK &artifact_blocks,
    int filter_w = 0,
    int filter_h = 0,
    int neighbor_count_th = 0,
    bool ignore_boundary = false);

/******************************************************************************
* Function:         CalculatePsnrCn3
* Description:      Calculate the PSNR between two images
* Parameters:       [in]src1  -  image 1, the reference image
* Parameters:       [in]src2  -  image 2, the test image
* Parameters:       [in]kernel_size  -  PSNR method kernel size, suggest to use 16~32
* Parameters:       [in]PSNR_threshold  -  threshold for local blocks
* Parameters:       [out]psnr_avg  -  average PSNR value
* Parameters:       [out]psnr_min  -  minimal PSNR value of all blocks
* Parameters:       [out]artifact_blocks  -  all blocks of which SSIM value is smaller than the threshold
*
* Return:           void
******************************************************************************/
SIMILARITYCHECK_API void CalculatePsnrCn3(
    Mat &src1,
    Mat &src2,
    int kernel_size,
    double psnr_threshold,
    double &psnr_avg,
    double &psnr_min,
    V_SSIM_BLOCK &artifact_blocks,
    int filter_w = 0,
    int filter_h = 0,
    int neighbor_count_th = 0,
    bool ignore_boundary = false);

/******************************************************************************
* Function:         DrawArtifactBlocks
* Description:      Draw retangles on the image where artifacts are found
* Parameters:       [in]src  -  image to draw
* Parameters:       [in]artifact_blocks  -  all blocks of which SSIM value is smaller than the threshold
*
* Return:           void
******************************************************************************/
SIMILARITYCHECK_API void DrawArtifactBlocks(
    Mat src,
    V_SSIM_BLOCK &artifact_blocks);

/******************************************************************************
* Function:         CalculateHomographyMatrix_cb(cb: chessboard method)
* Description:      Calculate the Homography Matrix for full FOV image and test image
* Parameters:       [in]full  -  Full FOV image (max resolution still image) (3 channels)
* Parameters:       [in]frame -  Frame (preview, video, still) (3 channels)
* Parameters:       [in/out]H -  Homography Matrix (3x3 double)
*
* Return:           true: success, false: failed
******************************************************************************/
SIMILARITYCHECK_API bool CalculateHomographyMatrix_cb(
    Mat &full,
    Mat &frame,
    Mat &H);

/******************************************************************************
* Function:         CalculateHomographyMatrix(sift kernel)
* Description:      Calculate the Homography Matrix for full FOV image and test image
* Parameters:       [in]full  -  Full FOV image (max resolution still image) (3 channels)
* Parameters:       [in]frame -  Frame (preview, video, still) (3 channels)
* Parameters:       [in/out]H -  Homography Matrix (3x3 double)
*
* Return:           true: success, false: failed
******************************************************************************/
SIMILARITYCHECK_API bool CalculateHomographyMatrix_sift(
    Mat &full,
    Mat &frame,
    Mat &H);

#endif
