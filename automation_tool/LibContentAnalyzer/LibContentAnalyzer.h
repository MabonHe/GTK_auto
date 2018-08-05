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

#ifndef __LIB_CONTENT_ANALYZER_H__
#define __LIB_CONTENT_ANALYZER_H__

#include <vector>
#include "opencv2/opencv.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include <mfapi.h>
#endif
using namespace std;
using namespace cv;

#ifdef _WINDOWS
    #ifdef LIBCONTENTANALYZER_EXPORTS
        #define LIBCONTENTANALYZER_API __declspec(dllexport)
    #else
        #define LIBCONTENTANALYZER_API __declspec(dllimport)
    #endif

    #ifndef _WINDOWS_MOBILE
        #ifdef _DEBUG
            #pragma comment(lib, "opencv_highgui231d.lib")
            #pragma comment(lib, "opencv_imgproc231d.lib")
            #pragma comment(lib, "opencv_core231d.lib")
            #pragma comment(lib, "opencv_video231d.lib")
            #pragma comment(lib, "opencv_legacy231d.lib")
            #pragma comment(lib, "opencv_features2d231d.lib")
            #pragma comment(lib, "opencv_calib3d231d.lib")
            #pragma comment(lib, "opencv_flann231d.lib")
        #else
            #pragma comment(lib, "opencv_highgui231.lib")
            #pragma comment(lib, "opencv_imgproc231.lib")
            #pragma comment(lib, "opencv_core231.lib")
            #pragma comment(lib, "opencv_video231.lib")
            #pragma comment(lib, "opencv_legacy231.lib")
            #pragma comment(lib, "opencv_features2d231.lib")
            #pragma comment(lib, "opencv_calib3d231.lib")
            #pragma comment(lib, "opencv_flann231.lib")
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
    #define LIBCONTENTANALYZER_API
#endif

#ifndef LONG
#define LONG long
#endif

#ifndef UINT64
#define UINT64 unsigned long long
#endif

#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef INT32
#define INT32 int
#endif

#ifndef FLOAT
#define FLOAT float
#endif

enum AnalyzerType
{
    ANALYZER_BRIGHTNESS_COMPARE = 0,
    ANALYZER_CHECKMUTEMODE,
    ANALYZER_CONTRAST_COMPARE,
    ANALYZER_EXPOSURE_COMPARE,
    ANALYZER_HUE_COMPARE,
    ANALYZER_TORCH_COMPARE,
    ANALYZER_WHITEBALANCE_COMPARE,
    ANALYZER_ZOOM_COMPARE,
    ANALYZER_THUMBNAIL_COMPARE,
    ANALYZER_JPEGQUALITY_COMPARE,
    ANALYZER_FOCUS_COMPARE,
    ANALYZER_COLOREFFECT_NEGATIVE,
    ANALYZER_COLOREFFECT_GRASSGREEN,
    ANALYZER_COLOREFFECT_MONO,
    ANALYZER_COLOREFFECT_SEPIA,
    ANALYZER_COLOREFFECT_SKYBLUE,
    ANALYZER_COLOREFFECT_SKINWHITEN,
    ANALYZER_HDR_COMPARE,
    ANALYZER_FOV_COMPARE,
    ANALYZER_SIMILARITY_COMPARE,
    ANALYZER_GDC_COMPARE,
    ANALYZER_3A_AUTO_CONSISTENT,
    ANALYZER_VIDEO_BASIC,
    ANALYZER_ULL_COMPARE,
    ANALYZER_AUTOAWB_COMPARE,
    ANALYZER_AUTOAE_COMPARE,
    ANALYZER_AUTOAF_COMPARE,
    ANALYZER_FACE_DETECTION,
    ANALYZER_METADATA,
    ANALYZER_GREEN_CORRUPTION,
	ANALYZER_ATE_COMPARE,
    ANALYZER_CHESSBOARD_COMPARE,
    ANALYZER_CAMERA_POSITION_COMPARE,
    ANALYZER_COLORCAST_COMPARE
};

enum AnalyzerParamType
{
    PARAM_INT,
    PARAM_FLOAT,
    PARAM_DOUBLE,
    PARAM_LONG,
    PARAM_ALL
};

typedef struct _AnalyzerParamVal
{
    int i;
    float f;
    double d;
    long long l;
}AnalyzerParamVal;

typedef struct _PARAMETER
{
    AnalyzerParamType type;
    AnalyzerParamVal  val;
    int cCount;
}PARAMETER;

#ifndef FaceRectInfoBlobHeader
typedef struct tagFaceRectInfoBlobHeader
{
    unsigned long Size;     // Size of this header + all FaceRectInfo following
    unsigned long Count;    // Number of FaceRectInfo's in the blob
} FaceRectInfoBlobHeader;
#endif

#ifndef RECT
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT;
#endif

#ifndef FaceRectInfo
typedef struct tagFaceRectInfo
{
    RECT Region;            // Relative coordinates on the frame (Q31 format)
    long confidenceLevel;   // Confidence Level of the region being a face
} FaceRectInfo;
#endif

typedef struct _FACE_BLOB_BUF
{
    FaceRectInfoBlobHeader Header;
    FaceRectInfo           FaceInfo[16];
} FACE_BLOB_BUF;

#ifndef CapturedMetadataExposureCompensation
typedef struct tagCapturedMetadataExposureCompensation
{
    UINT64  Flags;  // KSCAMERA_EXTENDEDPROP_EVCOMP_XXX step flag
    INT32   Value;  // EV Compensation value in units of the step
} CapturedMetadataExposureCompensation;
#endif

#ifndef CapturedMetadataWhiteBalanceGains
typedef struct tagCapturedMetadataWhiteBalanceGains
{
    FLOAT    R;
    FLOAT    G;
    FLOAT    B;
} CapturedMetadataWhiteBalanceGains;
#endif

// Metadata info
typedef struct _METADATA_INFO
{
    bool            bvalid;
    CapturedMetadataExposureCompensation ev_comp;
    UINT64          exposure_time_us;
    UINT32          iso_speed;

    UINT32          lens_pos;
    UINT32          focus_state;

    UINT32          awb_temperture;
    CapturedMetadataWhiteBalanceGains awb_gains;

    UINT32          flash;

    UINT64          sensor_fps;

    FACE_BLOB_BUF   face_roi_buffer;

} METADATA_INFO, *PMETADATA_INFO;

typedef struct _INPUT_IMAGE
{
    PARAMETER param;
    Mat img;
    METADATA_INFO metadata;
} INPUT_IMAGE;

typedef vector<INPUT_IMAGE> INPUT_IMAGE_ARRAY;

enum ANALYZER_RETRUN
{
    ANALYZER_RETRUN_PASS = 0,
    ANALYZER_RETRUN_FAIL,
    ANALYZER_PARAMETER_EXCEPTION,
    ANALYZER_IMAGE_EXCEPTION,
    ANALYZER_OTHER_EXCEPTION,
    ANALYZER_RETURN_NONE
};

LIBCONTENTANALYZER_API extern vector<double> ratio;

class LIBCONTENTANALYZER_API CContenAnalyzerInterface
{
public:
    /******************************************************************************
    * Function:         GetContenAnalyzerType
    * Description:      Output ContenAnalyzer Type
    *
    * Return:           Return AnalyzerType
    ******************************************************************************/
    virtual AnalyzerType GetContenAnalyzerType() = 0;

    /******************************************************************************
    * Function:         GetContenAnalyzerName
    * Description:      Get Conten Analyzer name
    *
    * Return:           Return Analyzer Name
    ******************************************************************************/
    virtual const char* GetContenAnalyzerName() = 0;

    /******************************************************************************
    * Function:         LoadAlgorithmParameters
    * Description:      Load algorithm internal parameters (including the judement threshold) from XML configuration file
    * Parameters:       [in]path     -  configuration file path name
    *
    * Return:           Return boolean val for sucess and failure
    ******************************************************************************/
    virtual bool LoadAlgorithmParameters(const char *path) = 0;

    /******************************************************************************
    * Function:         Analyze
    * Description:      Analyze function abstract interface.
    * Parameters:       [in]parameter_file_path     -  parameter configuration file path name
    * Parameters:       [in]img_array               -  input images
    * Parameters:       [out]img_debug              -  output debug image
    * Parameters:       [out]img_reference          -  output reference image
    *
    * Return:           Return ANALYZER_RETRUN value
    ******************************************************************************/
    virtual ANALYZER_RETRUN Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference) = 0;

};

/******************************************************************************
* Function:         FactoryCreateAnalyzer
* Description:      Create Analyzer Instance and return the pointer.
* Parameters:       [in]type     -  Analyzer Type
*
* Return:           Return pointer of Analyzer Instance
******************************************************************************/
LIBCONTENTANALYZER_API CContenAnalyzerInterface* FactoryCreateAnalyzer(AnalyzerType type);

/******************************************************************************
* Function:         FactoryDestroyAnalyzer
* Description:      Destroy Analyzer Instance and clear the pointer.
* Parameters:       [in]ppAnalyzer     -  Pointer of Analyzer Instance pointer
*
* Return:           Return void
******************************************************************************/
LIBCONTENTANALYZER_API void FactoryDestroyAnalyzer(CContenAnalyzerInterface **ppanalyzer);


// Pattern related definiton start here
// Set grayscale location
const Point3f grayscale[3][7] = { { Point3f(-3, -1, 1), Point3f(-3, 0, 1), Point3f(-3, 1, 1), Point3f(-3, 2, 1), Point3f(-3, 3, 1), Point3f(-3, 4, 1), Point3f(-3, 5, 1) }, \
{ Point3f(-2, 0, 1), Point3f(-2, 1, 1), Point3f(-2, 2, 1), Point3f(-2, 3, 1), Point3f(-2, 4, 1), Point3f(-2, 5, 1), Point3f(-2, 6, 1) },
{ Point3f(0, 0, 0), Point3f(255 / 6 * 1, 255 / 6 * 1, 255 / 6 * 1), Point3f(255 / 6 * 2, 255 / 6 * 2, 255 / 6 * 2), Point3f(255 / 6 * 3, 255 / 6 * 3, 255 / 6 * 3), Point3f(255 / 6 * 4, 255 / 6 * 4, 255 / 6 * 4), Point3f(255 / 6 * 5, 255 / 6 * 5, 255 / 6 * 5), Point3f(255 / 6 * 3, 255 / 6 * 6, 255 / 6 * 6) } };

// Set slanted edge location
const Point3f verticalSlantedEdge[7] = { Point3f(0, 11, 1), Point3f(0, 11.75, 1), Point3f(0, 13, 1), Point3f(6, 13, 1), Point3f(6, 12.25, 1), Point3f(6, 11, 1), Point3f(3, 12, 1) };

// Set 24 color chart location
const Point3f colorChart[2][24] = { { Point3f(-3, -3, 1), Point3f(-2, -3, 1), Point3f(-1, -3, 1), Point3f(0, -3, 1), Point3f(1, -3, 1), Point3f(2, -3, 1), Point3f(3, -3, 1), Point3f(4, -3, 1), Point3f(5, -3, 1), Point3f(6, -3, 1), Point3f(7, -3, 1), Point3f(8, -3, 1), \
Point3f(-3, 7, 1), Point3f(-2, 7, 1), Point3f(-1, 7, 1), Point3f(0, 7, 1), Point3f(1, 7, 1), Point3f(2, 7, 1), Point3f(3, 7, 1), Point3f(4, 7, 1), Point3f(5, 7, 1), Point3f(6, 7, 1), Point3f(7, 7, 1), Point3f(8, 7, 1) }, \
{Point3f(-2, -2, 1), Point3f(-1, -2, 1), Point3f(0, -2, 1), Point3f(1, -2, 1), Point3f(2, -2, 1), Point3f(3, -2, 1), Point3f(4, -2, 1), Point3f(5, -2, 1), Point3f(6, -2, 1), Point3f(7, -2, 1), Point3f(8, -2, 1), Point3f(9, -2, 1), \
Point3f(-2, 8, 1), Point3f(-1, 8, 1), Point3f(0, 8, 1), Point3f(1, 8, 1), Point3f(2, 8, 1), Point3f(3, 8, 1), Point3f(4, 8, 1), Point3f(5, 8, 1), Point3f(6, 8, 1), Point3f(7, 8, 1), Point3f(8, 8, 1), Point3f(9, 8, 1)} \
};

// standard 24 Color
const uchar r[24] = { 94, 241, 97, 90, 164, 140, 255, 7, 222, 69, 187, 255, 0, 64, 203, 255, 207, 0, 255, 249, 180, 117, 53, 0 };
const uchar g[24] = { 28, 149, 119, 103, 131, 253, 116, 47, 29, 0, 255, 142, 0, 173, 0, 217, 3, 148, 255, 249, 180, 117, 53, 0 };
const uchar b[24] = { 13, 108, 171, 39, 196, 153, 21, 122, 42, 68, 19, 0, 142, 38, 0, 0, 124, 189, 255, 249, 180, 117, 53, 0 };

/******************************************************************************
* Function:         generateChessBoardCornerPoints
* Description:      generate virual coordinats for the pattern
* Parameters:       [in] row = 6, col =7
*
* Return:           vector<Point2f> dst, vecotr of virutal points
******************************************************************************/
vector<Point2f> generateChessBoardCornerPoints(int row = 6, int col = 7);

/******************************************************************************
* Function:         getCorrespondingPoint
* Description:      generate corresponding points in image plane
* Parameters:       [in] image, corner points, inpt point, homograph matrix
*
* Return:           Point3f out point in image plane
******************************************************************************/
Point3f getCorrespondingPoint(Mat frame, vector<Point2f> corners, Point3f inP, Mat Hf);

/******************************************************************************
* Function:         findHomoFromGD2IMG
* Description:      calculate homograph matrix
* Parameters:       [in] input image, virtual corner points
*                   [out] Homograph matrix H, real corners in image plane
* Return:           void
******************************************************************************/
bool findHomoFromGD2IMG(Mat &frame_cn3, vector<Point2f> virtualCorners, Mat& H, vector<Point2f>& corners_frame);


/******************************************************************************
* Function:         getCorrespondingMarkedPoints
* Description:      calculate marked points
* Parameters:       [in] input image
*                   [out] marked points
* Return:           Point3f*
******************************************************************************/
Point3f* getCorrespondingMarkedPoints(Mat frame, vector<Point2f> corners, Mat Hf);

#endif
