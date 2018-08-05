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
#include <fstream>
#include <set>
#include <list>
#include <numeric>
#include "LibContentAnalyzerGDC.h"
//basic parameters defined here
typedef enum FindMethod
{
    BlobDetector = 0,// This would be faster than Sym and Asym
    ChessBorad,// Pattern use chessboard
    SymCircles,// Pattern use Symmetric dots
    AsymCircles,// Pattern use Asymmetric dots
}FindMethod;

Size patternSize = Size(40, 40);

float squareSize = 1.0f;

float maxRectifiedDistance = (float)(squareSize / 2.0);

// Basic methods declared here
bool checkGDC(vector<Point2f> points, Mat &img_frame, double varTH);
void hierarchicalClustering(const vector<Point2f> points, const Size &patternSz, vector<Point2f> &patternPoints);
void findCorners(const std::vector<cv::Point2f> &hull2f, std::vector<cv::Point2f> &corners);
void getSortedCorners(const std::vector<cv::Point2f> &hull2f, const std::vector<cv::Point2f> &corners, const std::vector<cv::Point2f> &outsideCorners, std::vector<cv::Point2f> &sortedCorners);
void rectifyPatternPoints(const std::vector<cv::Point2f> &patternPoints, const std::vector<cv::Point2f> &sortedCorners, std::vector<cv::Point2f> &rectifiedPatternPoints);
void parsePatternPoints(const std::vector<cv::Point2f> &patternPoints, const std::vector<cv::Point2f> &rectifiedPatternPoints, std::vector<cv::Point2f> &centers);
void findGrid(const std::vector<cv::Point2f> points, cv::Size _patternSize, vector<Point2f>& centers);
bool CalculateGDC_BLOB(Mat &img_frame, bool bflag, Mat &intrinsic, Mat &distortion, double varTH, FindMethod Mehod = BlobDetector);
bool CalculateGDC_CB(Mat &img_frame, double& var, Mat &img, FindMethod Method = ChessBorad);
double findGDCVal(vector<Point2f> points, Mat &img_frame, Mat &img_dot);
double findPillowGDCVal(vector<Point2f> points, Mat &img_frame, Mat &img_dot);

CContentAnalyzerGDC::CContentAnalyzerGDC() :
CContenAnalyzerBase(ANALYZER_GDC_COMPARE)
{
    // Step 1: Set Conten Analyzer Name
    SetContenAnalyzerName("AnalyzerGDCCompare");

    // Step 2: Initialize algorithm parameters
    m_varTH = 0.1;

    DATA_PARSER params[] = {
        { offsetof(CContentAnalyzerGDC, m_varTH), "%lf", "varTH", 0, 120, 100 },
        { 0, NULL, NULL, 0, 0, 0 }
    };

    // Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
    SetAlgorithmParameterParser(params);
}


CContentAnalyzerGDC::~CContentAnalyzerGDC()
{}

ANALYZER_RETRUN CContentAnalyzerGDC::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{

    // Step 1: Check Input Image count
    if (img_array.size() < 2)
    {
        LOG_ERROR("Input image count is expected as 1", NULL);
        return ANALYZER_IMAGE_EXCEPTION;
    }

    if (img_array.size() == 2)
    {
        goto image;
    }
    else
    {
        goto video;
    }
image:
    {
        // Step 2: Check Input Image
        Mat img_bypass, img_frame;
        img_bypass = img_array[0].img;
        img_frame = img_array[1].img;

        if (img_bypass.empty() || img_frame.empty())
        {
            LOG_ERROR("Load image error", NULL);
            return ANALYZER_IMAGE_EXCEPTION;
        }

        // Step 3: Analyzer
        //bool bflag = static_cast<bool>(img_array[1].param.val.i);
        Mat distortion, intrinsic;
        double var0, var1;
        if (!CalculateGDC_CB(img_bypass, var0, img_reference))
        {
            LOG_ERROR("Load pattern for non-gdc error", NULL);
        }
        if (!CalculateGDC_CB(img_frame, var1, img_debug))
        {
            LOG_ERROR("Load pattern for gdc error", NULL);
        }

        if (var0 < var1)
        {
            LOG_ERROR(CStringFormat("GDC effect check failed: effect: %lf > none effect: %lf", var1, var0).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO(CStringFormat("GDC effect check passed: effect: %lf < none effect: %lf", var1, var0).Get(), NULL);
            return ANALYZER_RETRUN_PASS;
        }
    }

video:
    {
        Mat img_bypass, img_frame;
        img_bypass = img_array[0].img;
        int index = img_array.size() / 2;
        // choose mid frame as reference img for the video frame buffer
        // to ensure the 3A is converged
        img_frame = img_array[index].img;

        if (img_bypass.empty())
        {
            LOG_ERROR("Load preview image error", NULL);
            return ANALYZER_IMAGE_EXCEPTION;
        }
        else if (img_frame.empty())
        {
            LOG_ERROR("Load mid video frame buffer error", NULL);
            return ANALYZER_IMAGE_EXCEPTION;
        }

        // Step 3: Analyzer
        //bool bflag = static_cast<bool>(img_array[1].param.val.i);
        Mat distortion, intrinsic;
        double var0, var1;

        if (!CalculateGDC_CB(img_bypass, var0, img_reference))
        {
            LOG_ERROR("Load pattern for non-gdc preview frame error", NULL);
        }
        if (!CalculateGDC_CB(img_frame, var1, img_debug))
        {
            LOG_ERROR("Load pattern for gdc video frame error", NULL);
        }

        if (var0 < var1)
        {
            LOG_ERROR(CStringFormat("GDC effect check failed: video frame: %lf > preview frame: %lf", var1, var0).Get(), NULL);
            return ANALYZER_RETRUN_FAIL;
        }
        else
        {
            LOG_INFO(CStringFormat("GDC effect check passed: video frame:: %lf < preview frame: %lf", var1, var0).Get(), NULL);
            return ANALYZER_RETRUN_PASS;
        }
    }
}


bool CalculateGDC_BLOB(Mat &img_frame, bool bflag, Mat &intrinsic, Mat &distortion, double varTH, FindMethod Method)
{
    const int PATTERN_ROW = 40;
    const int PATTERN_COL = 40;
    const Size PATTERN_SIZE = Size(PATTERN_COL, PATTERN_ROW);

    // use blobs pattern
    if (Method == BlobDetector)
    {
        // creation
        /*cv::SimpleBlobDetector::Params params;
        params.minDistBetweenBlobs = 50.0f;
        params.filterByInertia = false;
        params.filterByConvexity = false;
        params.filterByColor = false;
        params.filterByCircularity = false;
        params.filterByArea = false;*/
        // set up and create the detector using the parameters
        cv::Ptr<cv::FeatureDetector> blob_detector = new cv::SimpleBlobDetector();
        blob_detector->create("SimpleBlob");
        // detect
        vector<cv::KeyPoint> keypoints;
        blob_detector->detect(img_frame, keypoints);

        vector<Point2f> points;
        for (size_t i = 0; i < keypoints.size(); i++)
        {
            float X = keypoints[i].pt.x;
            float Y = keypoints[i].pt.y;
            Point2f cur(X, Y);
            points.push_back(cur);
        }

        vector<Point2f> centers;
        findGrid(points, patternSize, centers);
        if (centers.size() != PATTERN_ROW*PATTERN_COL)
        {
            LOG_ERROR("Corner points do not equal to pattern size\n", NULL);
            return false;
        }
        // Enable this part for test purpose
        /*Point2f prev(0, 0);
        for (int i = 0; i < centers.size(); i++)
        {
            Point2f cur = centers[i];
            circle(img_frame, centers[i], 6, Scalar(255, 0, 0), 1, 8, 0);
            if (i >= 1) line(img_frame, prev, cur, Scalar(0, 255, 0), 2, 8, 0);
            prev = cur;
        }*/

        return checkGDC(centers, img_frame, varTH);
    }
    else
    {
        Mat gray;
        cvtColor(img_frame, gray, CV_BGR2GRAY);
        int square_size = 5;
        int ncorner_count_full = 0;
        vector<vector<Point2f> > image_corners(1);
        image_corners.clear();
        vector<vector<Point3f> > object_corners(1);
        object_corners.clear();

        vector<Point2f>corners_full;
        corners_full.clear();
        vector<Point3f>obj_corners;
        obj_corners.clear();
        for (int i = 0; i < PATTERN_ROW; i++)
            for (int j = 0; j < PATTERN_COL; j++)
            {
                obj_corners.push_back(Point3f(float(j*square_size), float(i*square_size), 0));
            }

        if (Method == SymCircles)
        {
            if (!findCirclesGrid(gray, PATTERN_SIZE, corners_full, CALIB_CB_SYMMETRIC_GRID))
            {
                LOG_ERROR("Corner points do not equal to pattern size\n", NULL);
                return false;
            }
        }
        else
            {
                if (!findCirclesGrid(gray, PATTERN_SIZE, corners_full, CALIB_CB_ASYMMETRIC_GRID))
                {
                    LOG_ERROR("Corner points do not equal to pattern size\n", NULL);
                    return false;
                }
            }
        vector<Mat> rvecs, tvecs;
        image_corners.push_back(corners_full);
        object_corners.push_back(obj_corners);
        calibrateCamera(object_corners, image_corners, PATTERN_SIZE, intrinsic, distortion, rvecs, tvecs);
    }
    return false;
}
bool CalculateGDC_CB(Mat &img_frame, double& var, Mat &img, FindMethod Method)
{
    const int PATTERN_ROW = 7;
    const int PATTERN_COL = 6;
    const Size PATTERN_SIZE = Size(PATTERN_COL, PATTERN_ROW);
    if (Method == ChessBorad)
    {
        LOG_INFO("using chessborad method", NULL);
        Mat gray;
        cvtColor(img_frame, gray, CV_BGR2GRAY);
        int square_size = 5;// This should be equal to the real size
        int ncorner_count_full = 0;
        vector<vector<Point2f> > image_corners(1);
        image_corners.clear();
        vector<vector<Point3f> > object_corners(1);
        object_corners.clear();

        vector<Point2f>corners_full;
        corners_full.clear();
        vector<Point3f>obj_corners;
        obj_corners.clear();
        for (int i = 0; i < PATTERN_ROW; i++)
            for (int j = 0; j < PATTERN_COL; j++)
            {
                obj_corners.push_back(Point3f(float(j*square_size), float(i*square_size), 0));
            }

        if (!findChessboardCorners(gray, PATTERN_SIZE, corners_full))
        {
            LOG_ERROR("Corner points do not equal to pattern size\n", NULL);
            return false;
        }
        else
        {
            LOG_INFO(CStringFormat("Find pattern %d x %d", PATTERN_ROW, PATTERN_COL).Get(), NULL);
        }

        cornerSubPix(gray, corners_full, PATTERN_SIZE, Size(-1, -1),
            TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.01));

        vector<Mat> rvecs, tvecs;
        image_corners.push_back(corners_full);
        object_corners.push_back(obj_corners);
        //calibrateCamera(object_corners, image_corners, PATTERN_SIZE, intrinsic, distortion, rvecs, tvecs);
        var = findGDCVal(corners_full, img_frame, img);
        return true;
    }
}

bool checkGDC(vector<Point2f> points, Mat &img_frame, double varTH)
{
    vector<Point2f>lpoints00, lpoints01, lpoints10, lpoints11;
    for (int j = 0; j < patternSize.width; j++)
    {
        lpoints00.push_back(points[j]);
        lpoints11.push_back(points[j + patternSize.width*(patternSize.height - 1)]);

    }

    for (int i = 0; i < patternSize.height; i++)
    {
        lpoints01.push_back(points[i*(patternSize.width) + patternSize.width - 1]);
        lpoints10.push_back(points[i*(patternSize.width)]);
    }

    Vec4f param00, param01, param10, param11;

    fitLine(Mat(lpoints00), param00, CV_DIST_L2, 0, 0.01, 0.01);
    fitLine(Mat(lpoints01), param01, CV_DIST_L2, 0, 0.01, 0.01);
    fitLine(Mat(lpoints10), param10, CV_DIST_L2, 0, 0.01, 0.01);
    fitLine(Mat(lpoints11), param11, CV_DIST_L2, 0, 0.01, 0.01);

    double err00, err01, err10, err11;
    err00 = .0f;
    err01 = .0f;
    err10 = .0f;
    err11 = .0f;
    for (size_t i = 0; i < lpoints00.size(); i++)
    {
        err00 += pow(abs(param00[0] * (lpoints00[i].y - param00[3]) - param00[1] * (lpoints00[i].x - param00[2])), 2);
        err11 += pow(abs(param11[0] * (lpoints11[i].y - param11[3]) - param11[1] * (lpoints11[i].x - param11[2])), 2);
    }

    for (size_t i = 0; i < lpoints01.size(); i++)
    {
        err01 += pow(abs(param01[0] * (lpoints01[i].y - param01[3]) - param01[1] * (lpoints01[i].x - param01[2])), 2);
        err10 += pow(abs(param10[0] * (lpoints10[i].y - param10[3]) - param10[1] * (lpoints10[i].x - param10[2])), 2);
    }

    float x0 = param00[2];
    float y0 = param00[3];
    float x1 = x0 - 200 * param00[0];
    float y1 = y0 - 200 * param00[1];
    line(img_frame, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(0, 0, 255), 3);

    x0 = param01[2];
    y0 = param01[3];
    x1 = x0 - 200 * param01[0];
    y1 = y0 - 200 * param01[1];
    line(img_frame, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(0, 0, 255), 3);

    x0 = param10[2];
    y0 = param10[3];
    x1 = x0 - 200 * param10[0];
    y1 = y0 - 200 * param10[1];
    line(img_frame, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(0, 0, 255), 3);

    x0 = param11[2];
    y0 = param11[3];
    x1 = x0 - 200 * param11[0];
    y1 = y0 - 200 * param11[1];
    line(img_frame, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(0, 0, 255), 3);
    // Enbale for test purpose
    /*std::cout << err00 << std::endl;
    std::cout << err01 << std::endl;
    std::cout << err10 << std::endl;
    std::cout << err11 << std::endl;*/
    if (err00 > varTH || err01 > varTH || err10 > varTH || err11 > varTH)
    {
        LOG_ERROR(CStringFormat("The err00 var is %lf", err00).Get(), NULL);
        LOG_ERROR(CStringFormat("The err01 var is %lf", err01).Get(), NULL);
        LOG_ERROR(CStringFormat("The err10 var is %lf", err10).Get(), NULL);
        LOG_ERROR(CStringFormat("The err11 var is %lf", err11).Get(), NULL);
        return false;
    }

    return true;
}
void hierarchicalClustering(const vector<Point2f> points, const Size &patternSz, vector<Point2f> &patternPoints)
{
#ifdef HAVE_TEGRA_OPTIMIZATION
    if (tegra::hierarchicalClustering(points, patternSz, patternPoints))
        return;
#endif
    int j, n = (int)points.size();
    size_t pn = static_cast<size_t>(patternSz.area());

    patternPoints.clear();
    if (pn >= points.size())
    {
        if (pn == points.size())
            patternPoints = points;
        return;
    }

    Mat dists(n, n, CV_32FC1, Scalar(0));
    Mat distsMask(dists.size(), CV_8UC1, Scalar(0));
    for (int i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            dists.at<float>(i, j) = (float)norm(points[i] - points[j]);
            distsMask.at<uchar>(i, j) = 255;
            //TODO: use symmetry
            distsMask.at<uchar>(j, i) = 255;//distsMask.at<uchar>(i, j);
            dists.at<float>(j, i) = dists.at<float>(i, j);
        }
    }

    vector<std::list<size_t> > clusters(points.size());
    for (size_t i = 0; i<points.size(); i++)
    {
        clusters[i].push_back(i);
    }

    int patternClusterIdx = 0;
    while (clusters[patternClusterIdx].size() < pn)
    {
        Point minLoc;
        minMaxLoc(dists, 0, 0, &minLoc, 0, distsMask);
        int minIdx = min(minLoc.x, minLoc.y);
        int maxIdx = max(minLoc.x, minLoc.y);

        distsMask.row(maxIdx).setTo(0);
        distsMask.col(maxIdx).setTo(0);
        Mat tmpRow = dists.row(minIdx);
        Mat tmpCol = dists.col(minIdx);
        (cv::min)(dists.row(minLoc.x), dists.row(minLoc.y), tmpRow);
        tmpRow.copyTo(tmpCol);
        clusters[minIdx].splice(clusters[minIdx].end(), clusters[maxIdx]);
        patternClusterIdx = minIdx;
    }

    //the largest cluster can have more than pn points -- we need to filter out such situations
    if (clusters[patternClusterIdx].size() != static_cast<size_t>(patternSz.area()))
    {
        return;
    }

    patternPoints.reserve(clusters[patternClusterIdx].size());
    for (std::list<size_t>::iterator it = clusters[patternClusterIdx].begin(); it != clusters[patternClusterIdx].end(); it++)
    {
        patternPoints.push_back(points[*it]);
    }
}
void findCorners(const std::vector<cv::Point2f> &hull2f, std::vector<cv::Point2f> &corners)
{
    //find angles (cosines) of vertices in convex hull
    vector<float> angles;
    for (size_t i = 0; i<hull2f.size(); i++)
    {
        Point2f vec1 = hull2f[(i + 1) % hull2f.size()] - hull2f[i % hull2f.size()];
        Point2f vec2 = hull2f[(i - 1 + static_cast<int>(hull2f.size())) % hull2f.size()] - hull2f[i % hull2f.size()];
        float angle = (float)(vec1.ddot(vec2) / (norm(vec1) * norm(vec2)));
        angles.push_back(angle);
    }

    //sort angles by cosine
    //corners are the most sharp angles (6)
    Mat anglesMat = Mat(angles);
    Mat sortedIndices;
    sortIdx(anglesMat, sortedIndices, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    CV_Assert(sortedIndices.type() == CV_32SC1);
    CV_Assert(sortedIndices.cols == 1);
    const int cornersCount = 4;
    Mat cornersIndices;
    cv::sort(sortedIndices.rowRange(0, cornersCount), cornersIndices, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
    corners.clear();
    for (int i = 0; i<cornersCount; i++)
    {
        corners.push_back(hull2f[cornersIndices.at<int>(i, 0)]);
    }
}
void getSortedCorners(const std::vector<cv::Point2f> &hull2f, const std::vector<cv::Point2f> &corners, const std::vector<cv::Point2f> &outsideCorners, std::vector<cv::Point2f> &sortedCorners)
{
    Point2f firstCorner;
    if (false)
    {
        Point2f center = std::accumulate(corners.begin(), corners.end(), Point2f(0.0f, 0.0f));
        center *= 1.0 / corners.size();

        vector<Point2f> centerToCorners;
        for (size_t i = 0; i<outsideCorners.size(); i++)
        {
            centerToCorners.push_back(outsideCorners[i] - center);
        }

        //TODO: use CirclesGridFinder::getDirection
        float crossProduct = centerToCorners[0].x * centerToCorners[1].y - centerToCorners[0].y * centerToCorners[1].x;
        //y axis is inverted in computer vision so we check > 0
        bool isClockwise = crossProduct > 0;
        firstCorner = isClockwise ? outsideCorners[1] : outsideCorners[0];
    }
    else
    {
        firstCorner = corners[0];
    }

    std::vector<Point2f>::const_iterator firstCornerIterator = std::find(hull2f.begin(), hull2f.end(), firstCorner);
    sortedCorners.clear();
    for (vector<Point2f>::const_iterator it = firstCornerIterator; it != hull2f.end(); it++)
    {
        vector<Point2f>::const_iterator itCorners = std::find(corners.begin(), corners.end(), *it);
        if (itCorners != corners.end())
        {
            sortedCorners.push_back(*it);
        }
    }
    for (vector<Point2f>::const_iterator it = hull2f.begin(); it != firstCornerIterator; it++)
    {
        vector<Point2f>::const_iterator itCorners = std::find(corners.begin(), corners.end(), *it);
        if (itCorners != corners.end())
        {
            sortedCorners.push_back(*it);
        }
    }

    if (true)
    {
        double dist1 = norm(sortedCorners[0] - sortedCorners[1]);
        double dist2 = norm(sortedCorners[1] - sortedCorners[2]);

        if ((dist1 > dist2 && patternSize.height > patternSize.width) || (dist1 < dist2 && patternSize.height < patternSize.width))
        {
            for (size_t i = 0; i<sortedCorners.size() - 1; i++)
            {
                sortedCorners[i] = sortedCorners[i + 1];
            }
            sortedCorners[sortedCorners.size() - 1] = firstCorner;
        }
    }
}
void rectifyPatternPoints(const std::vector<cv::Point2f> &patternPoints, const std::vector<cv::Point2f> &sortedCorners, std::vector<cv::Point2f> &rectifiedPatternPoints)
{
    //indices of corner points in pattern
    vector<Point> trueIndices;
    trueIndices.push_back(Point(0, 0));
    trueIndices.push_back(Point(patternSize.width - 1, 0));
    if (false)
    {
        trueIndices.push_back(Point(patternSize.width - 1, 1));
        trueIndices.push_back(Point(patternSize.width - 1, patternSize.height - 2));
    }
    trueIndices.push_back(Point(patternSize.width - 1, patternSize.height - 1));
    trueIndices.push_back(Point(0, patternSize.height - 1));

    vector<Point2f> idealPoints;
    for (size_t idx = 0; idx<trueIndices.size(); idx++)
    {
        int i = trueIndices[idx].y;
        int j = trueIndices[idx].x;
        if (false)
        {
            idealPoints.push_back(Point2f((2 * j + i % 2)*squareSize, i*squareSize));
        }
        else
        {
            idealPoints.push_back(Point2f(j*squareSize, i*squareSize));
        }
    }

    Mat homography = findHomography(Mat(sortedCorners), Mat(idealPoints), 0);
    Mat rectifiedPointsMat;
    transform(patternPoints, rectifiedPointsMat, homography);
    rectifiedPatternPoints.clear();
    convertPointsFromHomogeneous(rectifiedPointsMat, rectifiedPatternPoints);
}
void parsePatternPoints(const std::vector<cv::Point2f> &patternPoints, const std::vector<cv::Point2f> &rectifiedPatternPoints, std::vector<cv::Point2f> &centers)
{
    flann::LinearIndexParams flannIndexParams;
    flann::Index flannIndex(Mat(rectifiedPatternPoints).reshape(1), flannIndexParams);

    centers.clear();
    for (int i = 0; i < patternSize.height; i++)
    {
        for (int j = 0; j < patternSize.width; j++)
        {
            Point2f idealPt;
            if (false)
                idealPt = Point2f((2 * j + i % 2)*squareSize, i*squareSize);
            else
                idealPt = Point2f(j*squareSize, i*squareSize);

            vector<float> query = Mat(idealPt);
            int knn = 1;
            vector<int> indices(knn);
            vector<float> dists(knn);
            flannIndex.knnSearch(query, indices, dists, knn, flann::SearchParams());
            centers.push_back(patternPoints.at(indices[0]));

            if (dists[0] > maxRectifiedDistance)
            {
#ifdef DEBUG_CIRCLES
                cout << "Pattern not detected: too large rectified distance" << endl;
#endif
                centers.clear();
                return;
            }
        }
    }
}
void findGrid(const std::vector<cv::Point2f> points, cv::Size _patternSize, vector<Point2f>& centers)
{
    centers.clear();
    if (points.empty())
    {
        return;
    }

    vector<Point2f> patternPoints;
    hierarchicalClustering(points, patternSize, patternPoints);
    if (patternPoints.empty())
    {
        return;
    }

#ifdef DEBUG_CIRCLES
    Mat patternPointsImage(1024, 1248, CV_8UC1, Scalar(0));
    drawPoints(patternPoints, patternPointsImage);
    imshow("pattern points", patternPointsImage);
#endif

    vector<Point2f> hull2f;
    convexHull(Mat(patternPoints), hull2f, false);
    const size_t cornersCount = 4;
    if (hull2f.size() < cornersCount)
        return;

    vector<Point2f> corners;
    findCorners(hull2f, corners);
    if (corners.size() != cornersCount)
        return;

    vector<Point2f> outsideCorners, sortedCorners;
    if (false)
    {
        /*findOutsideCorners(corners, outsideCorners);
        const size_t outsideCornersCount = 2;
        if (outsideCorners.size() != outsideCornersCount)
        return;*/
    }
    getSortedCorners(hull2f, corners, outsideCorners, sortedCorners);
    if (sortedCorners.size() != cornersCount)
        return;

    vector<Point2f> rectifiedPatternPoints;
    rectifyPatternPoints(patternPoints, sortedCorners, rectifiedPatternPoints);
    if (patternPoints.size() != rectifiedPatternPoints.size())
        return;

    parsePatternPoints(patternPoints, rectifiedPatternPoints, centers);
}
double findGDCVal(vector<Point2f> points, Mat &img_frame, Mat &img_dot)
{
    //-------------> Overall case detection
    img_dot = Mat::zeros(img_frame.rows, img_frame.cols, CV_8UC1);
    const int PATTERN_ROW = 7;
    const int PATTERN_COL = 6;
    const Size patternSize = Size(PATTERN_COL, PATTERN_ROW);

    vector<Point2f>lpoints00[PATTERN_ROW], lpoints11[PATTERN_COL];
    Point2f prev, cur;
    for (int i = 0; i < patternSize.height; i++)
    {
        for (int j = 0; j < patternSize.width; j++)
        {
            cur = points[j + patternSize.width*i];
            lpoints00[i].push_back(points[j + patternSize.width*i]);
            circle(img_dot, points[j + patternSize.width*i], 2, Scalar(255, 255, 255), 2);
            if(0 != j)line(img_dot, prev, cur, Scalar(255, 255, 255), 1);
            prev = cur;
        }
    }

    for (int j = 0; j < patternSize.width; j++)
    {
        for (int i = 0; i < patternSize.height; i++)
        {
            cur = points[j + patternSize.width*i];
            lpoints11[j].push_back(points[j + patternSize.width*i]);
            circle(img_dot, points[j + patternSize.width*i], 2, Scalar(255, 255, 255), 2);
            if (0 != i)line(img_dot, prev, cur, Scalar(255, 255, 255), 1);
            prev = cur;
        }
    }

    Vec4f param00[PATTERN_ROW], param11[PATTERN_COL];

    for (int i = 0; i !=PATTERN_ROW; i++)
    {
        fitLine(Mat(lpoints00[i]), param00[i], CV_DIST_L2, 0, 0.01, 0.01);
        float x0 = param00[i][2];
        float y0 = param00[i][3];
        float x1 = x0 - 20 * param00[i][0];
        float y1 = y0 - 20 * param00[i][1];
        //line(img_dot, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(255, 255, 255), 1);
    }

    for (int i = 0; i != PATTERN_COL; i++)
    {
        fitLine(Mat(lpoints11[i]), param11[i], CV_DIST_L2, 0, 0.01, 0.01);
        float x0 = param11[i][2];
        float y0 = param11[i][3];
        float x1 = x0 - 20 * param11[i][0];
        float y1 = y0 - 20 * param11[i][1];
        //line(img_dot, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(255, 255, 255), 1);
    }

    float err00[PATTERN_ROW], err11[PATTERN_COL], errVer = 0.0, errHor = 0.0;

    for (int i = 0; i < PATTERN_ROW; i++)
    {
        err00[i] = 0.0;
    }

    for (int i = 0; i < PATTERN_COL; i++)
    {
        err11[i] = 0.0;
    }

    float avgVer = 0.0, avgHor = 0.0, varVer = 0.0, varHor = 0.0;
    for (int i = 0; i < PATTERN_ROW; i++)
    {
        avgVer += param00[i][1] / param00[i][0];
        //cout << param00[i][1] / param00[i][0] << endl;
    }

    for (int i = 0; i < PATTERN_COL; i++)
    {
        avgHor += param11[i][1] / param11[i][0];
        //cout << param11[i][1] / param11[i][0] << endl;
    }
    avgVer /= PATTERN_ROW;
    avgHor /= PATTERN_COL;

    for (int i = 0; i < PATTERN_ROW; i++)
    {
        varVer += pow(param00[i][1] / param00[i][0] - avgVer, 2);
    }
    for (int i = 0; i < PATTERN_COL; i++)
    {

        varHor += pow(param11[i][1] / param11[i][0] - avgHor, 2);
    }

    LOG_INFO(CStringFormat("varVer: %lf, varHor: %lf", varVer, varHor).Get(), NULL);
    return pow(varVer, 2) + pow(varHor, 2);
}
double findPillowGDCVal(vector<Point2f> points, Mat &img_frame, Mat &img_dot)
{
    #define SAVE_DOT_IMAGE
    //-------------> Overall case detection
    img_dot = Mat::zeros(img_frame.rows, img_frame.cols, CV_8UC1);
    const int PATTERN_ROW = 7;
    const int PATTERN_COL = 6;
    const Size patternSize = Size(PATTERN_COL, PATTERN_ROW);

    vector<Point2f>lpoints00[PATTERN_ROW], lpoints11[PATTERN_COL];
    Point2f prev, cur;
    for (int i = 0; i < patternSize.height; i++)
    {
        for (int j = 0; j < patternSize.width; j++)
        {
            cur = points[j + patternSize.width*i];
            lpoints00[i].push_back(points[j + patternSize.width*i]);
            #ifdef SAVE_DOT_IMAGE
                circle(img_dot, points[j + patternSize.width*i], 2, Scalar(255, 255, 255), 2);
            #endif
            if (0 != j)line(img_dot, prev, cur, Scalar(255, 255, 255), 1);
            prev = cur;
        }
    }

    for (int j = 0; j < patternSize.width; j++)
    {
        for (int i = 0; i < patternSize.height; i++)
        {
            cur = points[j + patternSize.width*i];
            lpoints11[j].push_back(points[j + patternSize.width*i]);
            #ifdef SAVE_DOT_IMAGE
                circle(img_dot, points[j + patternSize.width*i], 2, Scalar(255, 255, 255), 2);
            #endif
            if (0 != i)line(img_dot, prev, cur, Scalar(255, 255, 255), 1);
            prev = cur;
        }
    }

    Vec4f param00[PATTERN_ROW], param11[PATTERN_COL];

    for (int i = 0; i != PATTERN_ROW; i++)
    {
        fitLine(Mat(lpoints00[i]), param00[i], CV_DIST_L2, 0, 0.01, 0.01);
        float x0 = param00[i][2];
        float y0 = param00[i][3];
        float x1 = x0 - 20 * param00[i][0];
        float y1 = y0 - 20 * param00[i][1];

        #ifdef SHOW_LINE
            line(img_dot, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(255, 255, 255), 1);
        #endif
    }

    for (int i = 0; i != PATTERN_COL; i++)
    {
        fitLine(Mat(lpoints11[i]), param11[i], CV_DIST_L2, 0, 0.01, 0.01);
        float x0 = param11[i][2];
        float y0 = param11[i][3];
        float x1 = x0 - 20 * param11[i][0];
        float y1 = y0 - 20 * param11[i][1];
        #ifdef SHOW_LINE
            line(img_dot, Point2f(x0, y0), cv::Point2f(x1, y1), cv::Scalar(255, 255, 255), 1);
        #endif
    }

    int midRow = PATTERN_ROW / 2;
    int midCol = PATTERN_COL / 2;
    double difRow[3] = {0.0, 0.0, 0.0}, difCol[3] = {0.0, 0.0, 0.0};
    double distX[3] = { 0.0, 0.0, 0.0 }, distY[3] = {0.0, 0.0, 0.0};
    distX[0] = max(abs(lpoints11[0][0].x - lpoints11[0][PATTERN_ROW - 1].x), abs(lpoints11[PATTERN_COL - 1][0].x - lpoints11[PATTERN_COL - 1][PATTERN_ROW - 1].x));
    distX[1] = max(abs(lpoints11[0][0].y - lpoints11[0][PATTERN_ROW - 1].y), abs(lpoints11[PATTERN_COL - 1][0].y - lpoints11[PATTERN_COL - 1][PATTERN_ROW - 1].y));
    distY[0] = max(abs(lpoints00[0][0].x - lpoints00[0][PATTERN_COL - 1].x), abs(lpoints00[PATTERN_ROW - 1][0].x - lpoints00[PATTERN_ROW - 1][PATTERN_COL - 1].x));
    distY[1] = max(abs(lpoints00[0][0].y - lpoints00[0][PATTERN_COL - 1].y), abs(lpoints00[PATTERN_ROW - 1][0].y - lpoints00[PATTERN_ROW - 1][PATTERN_COL - 1].y));

    difCol[0] = abs(lpoints11[0][0].x / 2 + lpoints11[0][PATTERN_ROW - 1].x / 2 - lpoints11[0][midRow].x) + \
        abs(lpoints11[PATTERN_COL - 1][0].x / 2 + lpoints11[PATTERN_COL - 1][PATTERN_ROW - 1].x / 2 - lpoints11[0][midRow].x);

    difRow[0] = abs(lpoints00[0][0].x / 2 + lpoints00[0][PATTERN_COL - 1].x / 2 - lpoints00[0][midCol].x) + \
        abs(lpoints00[PATTERN_ROW - 1][0].x / 2 + lpoints00[PATTERN_ROW - 1][PATTERN_COL - 1].x / 2 - lpoints00[0][midCol].x);

    difCol[1] = abs(lpoints11[0][0].y / 2 + lpoints11[0][PATTERN_ROW - 1].y / 2 - lpoints11[0][midRow].y) + \
        abs(lpoints11[PATTERN_COL - 1][0].y / 2 + lpoints11[PATTERN_COL - 1][PATTERN_ROW - 1].y / 2 - lpoints11[0][midRow].y);

    difRow[1] = abs(lpoints00[0][0].y / 2 + lpoints00[0][PATTERN_COL - 1].y / 2 - lpoints00[0][midCol].y) + \
        abs(lpoints00[PATTERN_ROW - 1][0].y / 2 + lpoints00[PATTERN_ROW - 1][PATTERN_COL - 1].y / 2 - lpoints00[0][midCol].y);

    // std error: error per unit area
    distX[2] = sqrt(pow(distX[0], 2) + pow(distX[1], 2));
    distY[2] = sqrt(pow(distY[0], 2) + pow(distY[1], 2));
    difCol[2] = pow(0.5 * difCol[1] / (distX[2] * distY[2]) + 0.5 * difCol[2] / (distX[2] * distY[2]), 2);
    difRow[2] = pow(0.5 * difRow[1] / (distY[2] * distX[2]) + 0.5 * difRow[2] / (distY[2] * distX[2]), 2);

    return sqrt(difCol[2] + difRow[2]);

}
