////////////////////////////////////////////////////////////////////////////////////
// Copyright?(2014)?Intel Corporation All Rights Reserved.
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
// notice or any other notice embedded in Materials by Intel or Intel?s suppliers
// or licensors in any way.
//
////////////////////////////////////////////////////////////////////////////////////

#include "LibContentAnalyzerColorCast.h"
#include <fstream>

double CheckColorCount(Mat &im, int istart, int iend, int jstart, int jend);

CContenAnalyzerColorcast::CContenAnalyzerColorcast() :
	CContenAnalyzerBase(ANALYZER_COLORCAST_COMPARE)
{
	// Step 1: Set Conten Analyzer Name
	SetContenAnalyzerName("AnalyzerColorcastCompare");

	// Step 2: Initialize algorithm parameters
	m_color_count_upper_bound = 5;

	// Step 3: Algorithm table
	DATA_PARSER params[] = {
		{ offsetof(CContenAnalyzerColorcast, m_ratio_th), "%lf", "ratio_th", 0, 50, 2.5 },
		{ offsetof(CContenAnalyzerColorcast, m_color_count_upper_bound), "%d", "m_color_count_upper_bound", 0, 50, 5},
		{ 0, NULL, NULL, 0, 0, 0 }
	};

	// Step 4: Set Algorithm table, so that all parameters can be loaded in Analyze function
	SetAlgorithmParameterParser(params);
}

CContenAnalyzerColorcast::~CContenAnalyzerColorcast()
{
}

ANALYZER_RETRUN CContenAnalyzerColorcast::
Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference)
{
	// Step 1: Check Input Image count
	if (img_array.size() != 1)
	{
		LOG_ERROR("Input image count is expected as 1", NULL);
		return ANALYZER_IMAGE_EXCEPTION;
	}

	// Step 2: Check Input Image
	if (img_array[0].img.empty())
	{
		LOG_ERROR("Input image is empty", NULL);
		return ANALYZER_IMAGE_EXCEPTION;
	}

	// Step 3: Analyze
	/*
	int step_size = 5;
	int row_step = img_array[0].img.rows / step_size, col_step = img_array[0].img.cols / step_size;
	for (int i = 0; i < step_size; i++)
	{
		for (int j = 0; j < step_size; j++)
		{
			double ratio = CheckColorCount(img_array[0].img, row_step * i, row_step * (i + 1),
				col_step * j, col_step * (j + 1));
			if (ratio > m_ratio_th)
			{
				LOG_ERROR(CStringFormat("index(%d, %d):Ratio value is larger than Ratio threshold(%lf)", i, j, m_ratio_th).Get(),
					CStringFormat("Its value is %lf", ratio).Get());
				//return ANALYZER_RETRUN_FAIL;
			}
		}
	}
	*/

	double ratio = CheckColorCount(img_array[0].img, 0, img_array[0].img.rows - 1, 0, img_array[0].img.cols - 1);
	if(ratio > m_ratio_th)
	{
		LOG_ERROR(CStringFormat("index(%d, %d):Ratio value is larger than Ratio threshold(%lf)", 0, 0, m_ratio_th).Get(),
			CStringFormat("Its value is %lf", ratio).Get());
		return ANALYZER_RETRUN_FAIL;
	}
	return ANALYZER_RETRUN_PASS;
}

double CheckColorCount(Mat &im ,int istart, int iend, int jstart, int jend)
{
	int rows = iend - istart + 1, cols = jend - jstart +1;
	int MN = rows * cols;
	Mat lab;
	vector<Mat> vHSV;
	vector<Mat> vLab;
	vector<int>HistA(256, 0), HistB(256, 0);

	uchar *pa = NULL, *pb = NULL;
	cvtColor(im, lab, CV_BGR2Lab);
	split(lab, vLab);
	int sumA = 0, sumB = 0;
	double Ma = 0, Mb = 0;

	for (int i = istart; i <= iend; i++)
	{
		pa = vLab[1].data + i * vLab[1].step;
		pb = vLab[2].data + i * vLab[2].step;

		for (int j = jstart; j <= jend; j++)
		{
			sumA += pa[j];
			sumB += pb[j];
			HistA[pa[j]]++;
			HistB[pb[j]]++;
		}
	}

	double Da = sumA / MN - 128;
	double Db = sumB / MN - 128;

	double D = sqrt(Da*Da + Db*Db);
	LOG_INFO(CStringFormat("D is %lf", D).Get(), NULL);

	for (int i = 0; i < rows; i++)
	{
		pa = vLab[1].data + i * vLab[1].step;
		pb = vLab[2].data + i * vLab[2].step;

		for (int j = 0; j < cols; j++)
		{
			Ma += (pa[j] - 128 - Da);
			Mb += (pb[j] - 128 - Db);
		}
	}

	Ma = Ma / MN;
	Mb = Mb / MN;

	double M = sqrt(Ma*Ma + Mb*Mb);
        if ( M == 0 )
        {
            LOG_ERROR(CStringFormat("M is %lf is illegal", M).Get(), NULL);
            return 100.0;
        }
	double K = (double)(D / M);
	LOG_INFO(CStringFormat("K is %lf", K).Get(), NULL);
	ofstream myfile1("histA.txt", ios::ate);
	ofstream myfile2("histB.txt", ios::ate);
	for (int i = 0; i < 256; i++)
	{
		myfile1 << HistA[i] << endl;
		myfile2 << HistB[i] << endl;
	}

	return K;
}
