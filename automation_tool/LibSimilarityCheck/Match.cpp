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

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "../LibToolFunction/LibToolFunction.h"
#include "LibSimilarityCheck.h"

#ifndef _WINODWS_MOBILE
#pragma comment(lib, "LibToolFunction.lib")
#else
#pragma comment(lib, "LibToolFunction.mobile.lib")
#endif
using namespace cv;

enum { NONE_FILTER = 0, CROSS_CHECK_FILTER = 1 };

//CLogger *error_logger = CLogger::GetInstance();

int getMatcherFilterType(const string& str)
{
    if (str == "NoneFilter")
        return NONE_FILTER;
    if (str == "CrossCheckFilter")
        return CROSS_CHECK_FILTER;
    //CV_Error(CV_StsBadArg, "Invalid filter name");
    return -1;
}

void simpleMatching(Ptr<DescriptorMatcher>& descriptorMatcher,
    const Mat& descriptors1, const Mat& descriptors2,
    vector<DMatch>& matches12)
{
    vector<DMatch> matches;
    descriptorMatcher->match(descriptors1, descriptors2, matches12);
}

void crossCheckMatching(Ptr<DescriptorMatcher>& descriptorMatcher,
    const Mat& descriptors1, const Mat& descriptors2,
    vector<DMatch>& filteredMatches12, int knn = 1)
{
    filteredMatches12.clear();
    vector<vector<DMatch> > matches12, matches21;
    descriptorMatcher->knnMatch(descriptors1, descriptors2, matches12, knn);
    descriptorMatcher->knnMatch(descriptors2, descriptors1, matches21, knn);

    DMatch forward;
    DMatch backward;

    for (size_t m = 0; m < matches12.size(); m++)
    {
        bool findCrossCheck = false;
        for (size_t fk = 0; fk < matches12[m].size(); fk++)
        {
            forward = matches12[m][fk];

            for (size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++)
            {
                backward = matches21[forward.trainIdx][bk];
                if (backward.trainIdx == forward.queryIdx)
                {
                    filteredMatches12.push_back(forward);
                    findCrossCheck = true;
                    break;
                }
            }
            if (findCrossCheck) break;
        }
    }
}

SIMILARITYCHECK_API bool CalculateHomographyMatrix(
    Mat &full,
    Mat &frame,
    Mat &H)
{
    if (frame.empty() || full.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        return false;
    }

    double ransacReprojThreshold = 5;

#ifndef _WINODWS_MOBILE
    Ptr<FeatureDetector> detector = FeatureDetector::create("SURF");
    Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create("SURF");
    Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create("BruteForce");
#else
    Ptr<FeatureDetector> detector = FeatureDetector::create<FeatureDetector>("SURF");
    Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create<DescriptorExtractor>("SURF");
    Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create("BruteForce");
#endif

    int mactherFilterType = getMatcherFilterType("CrossCheckFilter");

    if (detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty())
    {
        LOG_ERROR("Can not create detector or descriptor exstractor or descriptor matcher of given types", NULL);
        return false;
    }

    vector<KeyPoint> keypoints1;
    detector->detect(full, keypoints1);

    Mat descriptors1;
    descriptorExtractor->compute(full, keypoints1, descriptors1);

    vector<KeyPoint> keypoints2;
    detector->detect(frame, keypoints2);

    Mat descriptors2;
    descriptorExtractor->compute(frame, keypoints2, descriptors2);

    if (keypoints1.size() < 20 || keypoints2.size() < 20)
    {
        LOG_ERROR("Cannot find enough key points", NULL);
        return false;
    }

    vector<DMatch> filteredMatches;
    switch (mactherFilterType)
    {
    case CROSS_CHECK_FILTER:
        crossCheckMatching(descriptorMatcher, descriptors1, descriptors2, filteredMatches, 1);
        break;
    default:
        simpleMatching(descriptorMatcher, descriptors1, descriptors2, filteredMatches);
    }

    vector<int> queryIdxs(filteredMatches.size()), trainIdxs(filteredMatches.size());
    for (size_t i = 0; i < filteredMatches.size(); i++)
    {
        queryIdxs[i] = filteredMatches[i].queryIdx;
        trainIdxs[i] = filteredMatches[i].trainIdx;
    }

    vector<Point2f> points1;
    KeyPoint::convert(keypoints1, points1, queryIdxs);

    vector<Point2f> points2;
    KeyPoint::convert(keypoints2, points2, trainIdxs);

#ifndef _WINODWS_MOBILE
    H = findHomography(Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold);
#else
    H = findHomography(Mat(points1), Mat(points2), RANSAC, ransacReprojThreshold);
#endif

    if (H.empty())
    {
        return false;
    }


    if (*((double*)H.ptr(0, 1)) > 0.02 || *((double*)H.ptr(1, 0)) > 0.02)
    {
        return false;
    }

    return true;
}

SIMILARITYCHECK_API bool GenerateReferenceFromFullFov(
    Mat &full,
    Mat &H,
    Mat &reference,
    int w,
    int h)
{
    warpPerspective(full, reference, H, Size(w, h));
    return true;
}

SIMILARITYCHECK_API bool CalculateHomographyMatrix_cb(
    Mat &full_cn3,
    Mat &frame_cn3,
    Mat &H)
{
    Mat full, frame;
    cvtColor(full_cn3, full, CV_BGR2GRAY);
    cvtColor(frame_cn3, frame, CV_BGR2GRAY);

    if (frame.empty() || full.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        return false;
    }

    const int ROW_CORNER = 6;
    const int COL_CORNER = 7;
    const Size PATTERN_SIZE(COL_CORNER, ROW_CORNER);

    int ncorner_count_full = 0;
    int ncorner_count_frame = 0;
    vector<Point2f> corners_full;
    vector<Point2f> corners_frame;

    if (!findChessboardCorners(full, PATTERN_SIZE, corners_full) || !findChessboardCorners(frame, PATTERN_SIZE, corners_frame))
    {
        LOG_ERROR("Corner points do not equal to pattern size", NULL);
        return false;
    }

    cornerSubPix(full, corners_full, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.01));
    cornerSubPix(frame, corners_frame, PATTERN_SIZE, Size(-1, -1),
        TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.01));

    //----------------------reference output, to see chessboard result, enable this part---------------//
    //drawChessboardCorners(frame, PATTERN_SIZE, Mat(corners_frame), true);
    //drawChessboardCorners(full, PATTERN_SIZE, Mat(corners_full), true);
    //imwrite("path\\frame.jpg", frame);
    //imwrite("path\\full.jpg", full);

    double ransacReprojThreshold = 5;

    H = findHomography(Mat(corners_full), Mat(corners_frame), CV_RANSAC, ransacReprojThreshold);

    if (H.empty())
    {
        return false;
    }

    if (*((double*)H.ptr(0, 1)) > 0.02 || *((double*)H.ptr(1, 0)) > 0.02)
    {
        return false;
    }

    return true;
}

SIMILARITYCHECK_API bool CalculateHomographyMatrix_sift(
    Mat &full,
    Mat &frame,
    Mat &H)
{
    if (frame.empty() || full.empty())
    {
        LOG_ERROR("Empty input images", NULL);
        return false;
    }

    double ransacReprojThreshold = 5;

#ifndef _WINODWS_MOBILE
    Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");
    Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create("SIFT");
    Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create("BruteForce");
#else
    Ptr<FeatureDetector> detector = FeatureDetector::create<FeatureDetector>("SIFT");
    Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create<DescriptorExtractor>("SIFT");
    Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create("BruteForce");
#endif

    int mactherFilterType = getMatcherFilterType("CrossCheckFilter");

    if (detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty())
    {
        LOG_ERROR("Can not create detector or descriptor exstractor or descriptor matcher of given types", NULL);
        return false;
    }

    vector<KeyPoint> keypoints1;
    detector->detect(full, keypoints1);

    Mat descriptors1;
    descriptorExtractor->compute(full, keypoints1, descriptors1);

    vector<KeyPoint> keypoints2;
    detector->detect(frame, keypoints2);

    Mat descriptors2;
    descriptorExtractor->compute(frame, keypoints2, descriptors2);

    if (keypoints1.size() < 10 || keypoints2.size() < 10)
    {
        LOG_ERROR("Cannot find enough key points", NULL);
        return false;
    }

    vector<DMatch> filteredMatches;
    switch (mactherFilterType)
    {
    case CROSS_CHECK_FILTER:
        crossCheckMatching(descriptorMatcher, descriptors1, descriptors2, filteredMatches, 1);
        break;
    default:
        simpleMatching(descriptorMatcher, descriptors1, descriptors2, filteredMatches);
    }

    vector<int> queryIdxs(filteredMatches.size()), trainIdxs(filteredMatches.size());
    for (size_t i = 0; i < filteredMatches.size(); i++)
    {
        queryIdxs[i] = filteredMatches[i].queryIdx;
        trainIdxs[i] = filteredMatches[i].trainIdx;
    }

    vector<Point2f> points1;
    KeyPoint::convert(keypoints1, points1, queryIdxs);

    vector<Point2f> points2;
    KeyPoint::convert(keypoints2, points2, trainIdxs);

#ifndef _WINODWS_MOBILE
    H = findHomography(Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold);
#else
    H = findHomography(Mat(points1), Mat(points2), RANSAC, ransacReprojThreshold);
#endif

    if (H.empty())
    {
        return false;
    }


    if (*((double*)H.ptr(0, 1)) > 0.02 || *((double*)H.ptr(1, 0)) > 0.02)
    {
        return false;
    }

    return true;
}

