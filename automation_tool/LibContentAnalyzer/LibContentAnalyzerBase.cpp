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

#include "LibContentAnalyzerBase.h"

CContenAnalyzerBase::CContenAnalyzerBase(AnalyzerType type) : m_AnalyzerType(type)
{
    memset(m_pAnalyzerName, 0, MAX_ANALYZER_NAME_LEN);

    m_pDataParser = NULL;
}

CContenAnalyzerBase::~CContenAnalyzerBase()
{
    if (NULL != m_pDataParser)
    {
        delete [] m_pDataParser;
        m_pDataParser = NULL;
    }
}

bool CContenAnalyzerBase::SetContenAnalyzerName(const char *name)
{
#ifdef _WINDOWS
    if (NULL == strncpy_s(m_pAnalyzerName, name, MAX_ANALYZER_NAME_LEN))
#else
    if (NULL == strncpy(m_pAnalyzerName, name, MAX_ANALYZER_NAME_LEN))
#endif
    {
        return false;
    }

    m_pAnalyzerName[MAX_ANALYZER_NAME_LEN - 1] = 0;

    return true;
}

void CContenAnalyzerBase::SetAlgorithmParameterParser(const DATA_PARSER *pDataParser)
{
    if (pDataParser == NULL)
        return;

    if (m_pDataParser != NULL)
    {
        delete m_pDataParser;
        m_pDataParser = NULL;
    }

    int nParameters = 0;

    while (pDataParser[nParameters++].description != NULL){}

    m_pDataParser = new DATA_PARSER[nParameters];

    memcpy(m_pDataParser, pDataParser, sizeof(DATA_PARSER) *nParameters);
}

bool CContenAnalyzerBase::LoadAlgorithmParameters(const char *path)
{
    if (m_pDataParser == NULL || path == NULL)
    {
        LOG_INFO("No parameters to be loaded", NULL);
        return true;
    }

    TiXmlDocument doc;

    if (!doc.LoadFile(path))
    {
        LOG_ERROR(CStringFormat("Failed to load file %s", path).Get(), NULL);
        return false;
    }

    if (ReadChildElement(&doc, m_pDataParser, (void*)this, m_pAnalyzerName) == NULL)
    {
        LOG_ERROR(CStringFormat("Failed to read parameters of analyzer %s", m_pAnalyzerName).Get(), NULL);
        return false;
    }

    return true;
}

bool CContenAnalyzerBase::SaveAlgorithmParameters(const char *path)
{
    if (m_pDataParser == NULL)
    {
        return true;
    }

    TiXmlDocument doc;
    doc.LoadFile(path);

    if (WriteChildElement(&doc, m_pDataParser, (void*)this, m_pAnalyzerName) == NULL)
    {
        LOG_ERROR(CStringFormat("Failed to write parameters of analyzer %s",
            m_pAnalyzerName).Get(), NULL);

        return false;
    }

    if (!doc.SaveFile(path))
    {
        LOG_ERROR(CStringFormat("Failed to save file to %s", path).Get(), NULL);
        return false;
    }

    return true;
}
