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

#ifndef _LIB_TOOL_FUNCTION_H_
#define _LIB_TOOL_FUNCTION_H_

#include <assert.h>
#include <time.h>
#include <stdio.h>
#ifdef _WINDOWS
#include <windows.h>
#else
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#endif
#include "../3rdParty/TinyXML/Include/tinyxml.h"

#if defined(_WINDOWS)
    #ifdef LIBTOOLFUNCTION_EXPORTS
        #define TOOL_FUNCTION_EXPORTS __declspec(dllexport)
    #else
        #define TOOL_FUNCTION_EXPORTS __declspec(dllimport)
    #endif
#else
    #define TOOL_FUNCTION_EXPORTS
#endif

#define LOG_BUFFER_SIZE 1024

#define EXCEPTION(str) {\
    throw CException("%s %d: %s\n", __FILE__, __LINE__, str); \
}

#define LOG_OUTPUT(level, primary_msg, secondary_msg) {\
    CLogger::GetInstance()->Log(level, __FILE__, __FUNCTION__, __LINE__, primary_msg, secondary_msg);\
}

#define LOG_VERBOSE(primary_msg, secondary_msg) LOG_OUTPUT(LOG_LEVEL_VERBOSE, primary_msg, secondary_msg)
#define LOG_INFO(primary_msg, secondary_msg) LOG_OUTPUT(LOG_LEVEL_INFO, primary_msg, secondary_msg)
#define LOG_WARNING(primary_msg, secondary_msg) LOG_OUTPUT(LOG_LEVEL_WARNING, primary_msg, secondary_msg)
#define LOG_ERROR(primary_msg, secondary_msg) LOG_OUTPUT(LOG_LEVEL_ERROR, primary_msg, secondary_msg)
#define LOG_RESULT(result) {\
    CLogger::GetInstance()->Result(__FILE__, __FUNCTION__, __LINE__, result);\
}

typedef struct _DATA_PARSER
{
    size_t offset;
    const char *format;
    const char *description;
    double min;
    double max;
    double default_val;
}DATA_PARSER;

class TOOL_FUNCTION_EXPORTS CStringFormat
{
public:
    CStringFormat();
    CStringFormat(const char *format, ...);
#ifdef _WINDOWS    
    CStringFormat(const WCHAR *format, ...);
#endif
    const char* Format(const char *format, ...);
    const char* Get();

protected:
    char m_str[LOG_BUFFER_SIZE];
};


class TOOL_FUNCTION_EXPORTS CException : public CStringFormat
{
public:
    CException(const char *format, ...);
    const char* Format(const char *format, ...);
    void Print();
    const char* GetException();
private:
    //char m_buff[LOG_BUFFER_SIZE];
};

enum LOG_LEVEL
{
    LOG_LEVEL_RESULT,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_VERBOSE
};


class TOOL_FUNCTION_EXPORTS CLogger
{
public:
    void SetFile(FILE *fp);
    ~CLogger();
    void SetLogLevel(LOG_LEVEL level);

    void Log(LOG_LEVEL level, const char *file_name, const char *func_name,
        int line_num, const char *primary_msg, const char *optional_msg = NULL);

    void Result(const char *file_name, const char *func_name,
        int line_num, bool result);

    static CLogger* GetInstance();
    void Write(LOG_LEVEL level, const char *format, ...);

protected:
    CLogger();


private:
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (CLogger::m_instance)
            {
                delete CLogger::m_instance;
            }
        }
    };

    FILE *m_fp;
    bool  m_bOpen;
    char m_buff[LOG_BUFFER_SIZE];
    LOG_LEVEL m_LogLevel;
    static CLogger* m_instance;
    static CGarbo m_garbo;

#ifdef _WINDOWS
    CRITICAL_SECTION m_cs;
    HANDLE m_hStdOut;
#ifndef _WINODWS_MOBILE
    CONSOLE_SCREEN_BUFFER_INFO m_sbi;
#endif
#else
    pthread_mutex_t m_mutex_lock;
#endif

};

// Global variable, error logger instant
//extern TOOL_FUNCTION_EXPORTS CLogger error_logger;

/******************************************************************************
* Function:         GetCurrentTimeString
* Description:      Get current time like "08/27/2012 13:29:29"
*
* Return:           Return time string, (no need to free the time buff)
******************************************************************************/
TOOL_FUNCTION_EXPORTS const char* GetCurrentTimeString();


/******************************************************************************
* Function:         GetFileExtentionName
* Description:      Get the extention name from the file name
* Parameters:       [in]fullname     -  full path name
* Parameters:       [out]ext         -  extention name
* Parameters:       [in]ext_max_size -  extention name buffer max size
*
* Return:           Return boolean val for success and failure
******************************************************************************/
TOOL_FUNCTION_EXPORTS bool GetFileExtentionName(const char *fullname, char *ext, int ext_max_size);

/******************************************************************************
* Function:         GetFileName
* Description:      Get the extention name from the file name
* Parameters:       [in]fullname     -  full path name
* Parameters:       [out]ext         -  file name
* Parameters:       [in]max_size     -  file name buffer max size
*
* Return:           Return boolean val for success and failure
******************************************************************************/
TOOL_FUNCTION_EXPORTS bool GetFileName(const char *fullname, char *filename, int max_size);

/******************************************************************************
* Function:         ReadChildElement
* Description:      Read the data descripted in the  DATA_PARSER table
* Parameters:       [in]node     -  parent node to read
* Parameters:       [in]dp_table -  parser table
* Parameters:       [in]pdata    -  Base address for the offset in the parser table
* Parameters:       [in]name     -  tag name
*
* Return:           Return the child element pointer for success, and NULL for failure
******************************************************************************/
TOOL_FUNCTION_EXPORTS TiXmlElement* ReadChildElement(
    TiXmlNode *node,
    DATA_PARSER *dp_table,
    void *pdata,
    const char *name);

/******************************************************************************
* Function:         WriteChildElement
* Description:      Write the data descripted in the  DATA_PARSER table to xml
* Parameters:       [in]node     -  parent node to write
* Parameters:       [in]dp_table -  parser table
* Parameters:       [in]pdata    -  Base address for the offset in the parser table
* Parameters:       [in]name     -  tag name
*
* Return:           Return the child element pointer for success, and NULL for failure
******************************************************************************/
TOOL_FUNCTION_EXPORTS TiXmlElement* WriteChildElement(
    TiXmlNode *node,
    DATA_PARSER *dp_table,
    void *pdata,
    const char *name);

#ifdef _WIDNOWS
/******************************************************************************
* Class:            Socket communication with script
* Struct:           Cmd message
******************************************************************************/

class TOOL_FUNCTION_EXPORTS CClientNet
{
public:
    int Connect(int port, char* address);
    int SendMsg(char* msg, int len);
    int ReceiveMsg();
    void Close();
    CClientNet();
    ~CClientNet();
private:
    SOCKET m_sock;
};

struct TOOL_FUNCTION_EXPORTS socketCommand
{
    char szDev[512];
    char szAct[512];
    char szCmd[512];
    socketCommand(char* device, char* action)
    {
        strcpy_s(szDev, 512, "Device:");
        strcpy_s(szAct, 512, "Action:");
        strcpy_s(szCmd, 512, "");
        strcat_s(szDev, 512, device);
        strcat_s(szAct, 512, action);
    }
    char* get()
    {
        strcat_s(szCmd, 512, szDev);
        strcat_s(szCmd, 512, ";");
        strcat_s(szCmd, 512, szAct);
        return szCmd;
    }

};
#endif
#endif
