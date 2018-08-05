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

#ifndef __LIB_CONTENT_ANALYZER_BASE_H__
#define __LIB_CONTENT_ANALYZER_BASE_H__
#include "LibContentAnalyzer.h"

#include "../3rdParty/TinyXML/Include/tinyxml.h"
#include "../LibToolFunction/LibToolFunction.h"

#ifndef _WINDOWS_MOBILE
#pragma comment(lib, "LibToolFunction.lib")
#pragma comment(lib, "tinyxml-lib.lib")
#else
#pragma comment(lib, "LibToolFunction.mobile.lib")
#pragma comment(lib, "tinyxml-lib.mobile.lib")
#endif


#define MAX_ERROR_MESSAGE_BUFFER_SIZE 1024
#define MAX_ANALYZER_NAME_LEN 64

class CContenAnalyzerBase : public CContenAnalyzerInterface
{
public:
    CContenAnalyzerBase(AnalyzerType type);



    virtual ~CContenAnalyzerBase();

    /******************************************************************************
    * Function:         LoadAlgorithmParameters
    * Description:      Load algorithm internal parameters (including the judement threshold) from XML configuration file
    * Parameters:       [in]path     -  configuration file path name
    *
    * Return:           Return boolean val for sucess and failure
    ******************************************************************************/
    bool LoadAlgorithmParameters(const char *path);

    /******************************************************************************
    * Function:         Analyze
    * Description:      Analyze function abstract interface.
    * Parameters:       [in]parameter_file_path     -  parameter configuration file path name
    * Parameters:       [in]img_array               -  input images
    * Parameters:       [in]img_debug               -  output image
    *
    * Return:           Return ANALYZER_RETRUN value
    ******************************************************************************/
    virtual ANALYZER_RETRUN Analyze(INPUT_IMAGE_ARRAY img_array, Mat &img_debug, Mat &img_reference) = 0;


    /******************************************************************************
    * Function:         GetContenAnalyzerType
    * Description:      Output ContenAnalyzer Type
    *
    * Return:           Return AnalyzerType
    ******************************************************************************/
    AnalyzerType GetContenAnalyzerType() {
        return m_AnalyzerType;
    }

    /******************************************************************************
    * Function:         GetContenAnalyzerName
    * Description:      Get Conten Analyzer name
    *
    * Return:           Return Analyzer Name
    ******************************************************************************/
    const char* GetContenAnalyzerName() {
        return m_pAnalyzerName;
    }

protected:
    /******************************************************************************
    * Function:         LoadAlgorithmParameters
    * Description:      Load algorithm internal parameters (including the judement threshold) from XML configuration file
    * Parameters:       [in]path     -  configuration file path name
    *
    * Return:           Return boolean val for sucess and failure
    ******************************************************************************/
    bool SaveAlgorithmParameters(const char *path);


    /******************************************************************************
    * Function:         SetContenAnalyzerName
    * Description:      Set Conten Analyzer name
    *
    * Return:           Return Analyzer Name
    ******************************************************************************/
    bool SetContenAnalyzerName(const char *name);

    void SetAlgorithmParameterParser(const DATA_PARSER *pDataParser);

private:
    CContenAnalyzerBase(const CContenAnalyzerBase &src) {};
    CContenAnalyzerBase& operator=(const CContenAnalyzerBase &src) { return *this; };

private:
    AnalyzerType m_AnalyzerType;
    char m_pAnalyzerName[MAX_ANALYZER_NAME_LEN];
    DATA_PARSER *m_pDataParser;
};


#endif //__LIB_CONTENT_ANALYZER_BASE_H__