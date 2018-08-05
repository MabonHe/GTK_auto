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

#include "LibToolFunction.h"
#define PATHMAX 512
#define MAX_TIME_BUFF 24

#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
#include "LibToolFunction_ETW.h"
#pragma comment(lib, "tinyxml-lib.lib")
#elif defined(_WINODWS_MOBILE)
#pragma comment(lib, "tinyxml-lib.mobile.lib")
#endif

CLogger* CLogger::m_instance = NULL;

CStringFormat::CStringFormat()
{
    m_str[0] = 0;
}

CStringFormat::CStringFormat(const char *format, ...)
{
    va_list args;

    m_str[0] = 0;

    va_start(args, format);

#ifdef _WINDOWS
    vsprintf_s(m_str, LOG_BUFFER_SIZE, format, args);
#else
    vsnprintf(m_str, LOG_BUFFER_SIZE, format, args);
#endif

    va_end(args);
}

#if defined(_WINDOWS)
CStringFormat::CStringFormat(const WCHAR *format, ...)
{
    va_list args;

    m_str[0] = 0;

    WCHAR temp_str[LOG_BUFFER_SIZE];

    va_start(args, format);
    wvsprintf(temp_str, format, args);
    va_end(args);

    size_t cnt;
    wcstombs_s(&cnt, m_str, temp_str, LOG_BUFFER_SIZE);
}
#endif

const char* CStringFormat::Format(const char *format, ...)
{
    va_list args;

    m_str[0] = 0;

    va_start(args, format);

#ifdef _WINDOWS
    vsprintf_s(m_str, LOG_BUFFER_SIZE, format, args);
#else
    vsnprintf(m_str, LOG_BUFFER_SIZE, format, args);
#endif

    va_end(args);

    return m_str;
}

const char* CStringFormat::Get()
{
    return m_str;
}

CException::CException(const char *format, ...)
{
    va_list args;
#ifdef _WINDOWS
    int n = sprintf_s(m_str, LOG_BUFFER_SIZE, "<Exception>");
#else
    int n =  snprintf(m_str, LOG_BUFFER_SIZE, "<Exception>");
#endif
    va_start(args, format);

#ifdef _WINDOWS
    vsprintf_s(m_str + n, LOG_BUFFER_SIZE - n, format, args);
#else
    vsnprintf(m_str + n, LOG_BUFFER_SIZE - n, format, args);
#endif

    va_end(args);
}

const char* CException::Format(const char *format, ...)
{
    va_list args;

#ifdef _WINDOWS
    int n = sprintf_s(m_str, LOG_BUFFER_SIZE, "<Exception>");
#else
    int n =  snprintf(m_str, LOG_BUFFER_SIZE, "<Exception>");
#endif

    va_start(args, format);

#ifdef _WINDOWS
    vsprintf_s(m_str + n, LOG_BUFFER_SIZE - n, format, args);
#else
    vsnprintf(m_str + n, LOG_BUFFER_SIZE - n, format, args);
#endif

    va_end(args);

    return m_str;
}

void CException::Print()
{
    puts(Get());
}

const char* CException::GetException()
{
    return Get();
}


CLogger::CLogger()
{
    m_fp = NULL;
    m_bOpen = false;
    m_LogLevel = LOG_LEVEL_VERBOSE;

#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
    m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    InitializeCriticalSection(&m_cs);
    GetConsoleScreenBufferInfo(m_hStdOut, &m_sbi);
#else
    pthread_mutex_init(&m_mutex_lock, NULL);
#endif

}

void CLogger::SetFile(FILE *fp)
{
    m_fp = fp;
}

CLogger* CLogger::GetInstance()
{
    if (CLogger::m_instance == NULL)
    {
        CLogger::m_instance = new CLogger();
    }

    return m_instance;
}

CLogger::~CLogger()
{
    if (m_fp != NULL)
    {
        fclose(m_fp);
    }
#ifdef _WINDOWS
    DeleteCriticalSection(&m_cs);
#else
    pthread_mutex_destroy(&m_mutex_lock);
#endif
}

void CLogger::SetLogLevel(LOG_LEVEL level)
{
    if (level > LOG_LEVEL_VERBOSE)
    {
        m_LogLevel = LOG_LEVEL_VERBOSE;
    }
    else if (level < LOG_LEVEL_RESULT)
    {
        m_LogLevel = LOG_LEVEL_RESULT;
    }
    else
    {
        m_LogLevel = level;
    }
}

void CLogger::Log(LOG_LEVEL level, const char *file_name, const char *func_name,
    int line_num, const char *primary_msg, const char *optional_msg)
{
    assert(file_name != NULL &&
        func_name != NULL &&
        primary_msg != NULL &&
        primary_msg[0] != NULL);

    if (NULL == optional_msg || optional_msg[0] == 0)
    {
        Write(level, "[File:%s][Method:%s][Line:%d][Primary:%s]",
            file_name, func_name, line_num, primary_msg);
    }
    else
    {
        Write(level, "[File:%s][Method:%s][Line:%d][Primary:%s][Optional:%s]",
            file_name, func_name, line_num, primary_msg, optional_msg);
    }
}

void CLogger::Result(const char *file_name, const char *func_name,
    int line_num, bool result)
{
    assert(file_name != NULL && func_name != NULL);

#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
    if (result)
    {
        SetConsoleTextAttribute(m_hStdOut, BACKGROUND_INTENSITY | BACKGROUND_GREEN);
    }
    else
    {
        SetConsoleTextAttribute(m_hStdOut, BACKGROUND_INTENSITY | BACKGROUND_RED);
    }
#endif

    Write(LOG_LEVEL_RESULT, "[File:%s][Method:%s][Line:%d][Result:%s]",
        file_name, func_name, line_num, result?"pass":"fail");
}

void CLogger::Write(LOG_LEVEL level, const char *format, ...)
{
    if (level > m_LogLevel)
        return;
#ifdef _WINDOWS
    EnterCriticalSection(&m_cs);
#else
    pthread_mutex_lock(&m_mutex_lock);
#endif
    char *LevelFlag = NULL;

    switch(level)
    {
    case LOG_LEVEL_RESULT:
        LevelFlag = (char*)"RESULT";

        break;
    case LOG_LEVEL_ERROR:
        LevelFlag = (char*)"ERROR";
#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
        SetConsoleTextAttribute(m_hStdOut, FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif
        break;
    case LOG_LEVEL_WARNING:
        LevelFlag = (char*)"WARNING";
#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
        SetConsoleTextAttribute(m_hStdOut, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
#endif
        break;
    case LOG_LEVEL_INFO:
        LevelFlag = (char*)"INFO";
#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
        SetConsoleTextAttribute(m_hStdOut, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
#endif
        break;
    case LOG_LEVEL_VERBOSE:
        LevelFlag = (char*)"VERBOSE";
        break;
    }

    va_list args;

    // Time stamp
#ifdef _WINDOWS
    int n = sprintf_s(m_buff, LOG_BUFFER_SIZE, "[%s][%s]", GetCurrentTimeString(), LevelFlag);
#else
    int n = snprintf(m_buff, LOG_BUFFER_SIZE, "[%s][%s]", GetCurrentTimeString(), LevelFlag);
#endif

    va_start(args, format);

#ifdef _WINDOWS
    n += vsprintf_s(m_buff + n, LOG_BUFFER_SIZE - n, format, args);
#else
    // Output Text to buffer
    n += vsnprintf(m_buff + n, LOG_BUFFER_SIZE - n, format, args);
#endif

    va_end(args);

    // Add '\n'
    if (n + 1 < LOG_BUFFER_SIZE)
    {
        m_buff[n] = '\n';
        m_buff[n+1] = 0;
    }

    // Oupt string to STDOUT
    fputs(m_buff, stdout);
#if defined(_WINDOWS) && !defined(_WINODWS_MOBILE)
    SetConsoleTextAttribute(m_hStdOut, m_sbi.wAttributes);
#endif
    // Output to log file
    if (NULL != m_fp)
    {
        fputs(m_buff, m_fp);

        fflush(m_fp);
    }
    
#ifdef _WINDOWS
    switch (level)
    {
    case LOG_LEVEL_RESULT:
    case LOG_LEVEL_INFO:
        EventWriteCameraFeatureTestInfo(m_buff);
        break;
    case LOG_LEVEL_ERROR:
        EventWriteCameraFeatureTestError(m_buff);
        break;
    case LOG_LEVEL_WARNING:
        EventWriteCameraFeatureTestWarn(m_buff);
        break;
    case LOG_LEVEL_VERBOSE:
        EventWriteCameraFeatureTestVerbose(m_buff);
        break;
    }
    LeaveCriticalSection(&m_cs);
#else
    pthread_mutex_unlock(&m_mutex_lock);
#endif
}

TOOL_FUNCTION_EXPORTS const char* GetCurrentTimeString()
{
    static char timebuff[MAX_TIME_BUFF] = {}; // 08/24/2012-17:09:11.123

#ifdef _WINDOWS

    SYSTEMTIME lt = {};
    GetLocalTime(&lt);

    sprintf_s(timebuff, MAX_TIME_BUFF, "%02d/%02d/%04d-%02d:%02d:%02d.%03d", lt.wMonth, lt.wDay, lt.wYear,
        lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
#else
    time_t tm_t = time(NULL);
    tm *ptm = NULL;

    if ((ptm = localtime(&tm_t)) == NULL)
    {
        return NULL;
    }

    snprintf(timebuff, MAX_TIME_BUFF, "%02d/%02d/%04d-%02d:%02d:%02d", ptm->tm_mon+1, ptm->tm_mday, ptm->tm_year+1900,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
#endif
    return timebuff;
}

TOOL_FUNCTION_EXPORTS bool GetFileExtentionName(const char *fullname, char *ext, int ext_max_size)
{
    assert(fullname != NULL && ext != NULL && ext_max_size > 0);

    int len = (int)strlen(fullname);

    int i = 0;

    for (i = len - 1; i >= 0; i--)
    {
        if (fullname[i] == '.')
        {
            break;
        }
    }

    if (i <= 0)
    {
        //EXCEPTION("Filename extention not found");
        return false;
    }

    if (ext_max_size < len - i + 1)
    {
        return false;
    }

#ifdef _WINDOWS
    strcpy_s(ext, ext_max_size, fullname + i);
#else
    strncpy(ext, fullname + i, ext_max_size);
#endif
    return true;
}

TOOL_FUNCTION_EXPORTS bool GetFileName(const char *fullname, char *filename, int max_size)
{
    assert(fullname != NULL && filename != NULL && max_size > 0);

    int len = (int)strlen(fullname);

    int i = 0;

    for (i = len - 1; i >= 0; i--)
    {
        if (fullname[i] == '\\' || fullname[i] == '/')
        {
            break;
        }
    }

    if (max_size < len - i + 1)
    {
        return false;
    }

#ifdef _WINDOWS
    strcpy_s(filename, max_size, fullname + i + 1);
#else
    strncpy(filename, fullname + i + 1, max_size);
#endif

    return true;
}



void GetAttribute(
    TiXmlElement *pelm,
    DATA_PARSER *dp,
    void *pdata)
{
    assert(pelm != NULL && dp != NULL && pdata != NULL);

    void *pd = (void*) ((char*) pdata + dp->offset);

    CLogger *error_logger = CLogger::GetInstance();

    switch (dp->format[1])
    {
    case 's': ///< string
        {
            char **ppstr = (char**) pd;

            const char *val = pelm->Attribute(dp->description);

            if (val == NULL)
            {
                CStringFormat primary_msg("Cannot find parameter %s in %s", dp->description, pelm->Value());
                LOG_WARNING(primary_msg.Get(), NULL);
                *ppstr = NULL;
            }
            else
            {
                if (*ppstr != NULL)
                    free(*ppstr);
#ifdef _WINDOWS
                *ppstr = _strdup(val);
#else
                *ppstr = strdup(val);
#endif
            }
        }
        break;
    case 'd':
        {
            int *pint = (int *) pd;

            int val = (int) dp->default_val;

            if (pelm->Attribute(dp->description, &val) == NULL)
            {
                CStringFormat primary_msg("Cannot find parameter %s in %s", dp->description, pelm->Value());
                LOG_WARNING(primary_msg.Get(), NULL);
            }

            if (val > dp->max || val < dp->min)
            {
                CStringFormat primary_msg("Parameter %s is out of range (%lf, %lf)", dp->description, dp->min, dp->max);
                CStringFormat secondary_msg("its value is %d", val);
                LOG_WARNING(primary_msg.Get(), secondary_msg.Get());
                val = (int)dp->default_val;

            }

            *pint = val;
        }
        break;
    case 'f':
        {
            float *pfval = (float *) pd;
            double val = (double) dp->default_val;
            if (pelm->Attribute(dp->description, &val) == NULL)
            {
                CStringFormat primary_msg("Cannot find parameter %s in %s", dp->description, pelm->Value());
                LOG_WARNING(primary_msg.Get(), NULL);
            }
            if (val > dp->max || val < dp->min)
            {
                CStringFormat primary_msg("Parameter %s is out of range (%lf, %lf)", dp->description, dp->min, dp->max);
                CStringFormat secondary_msg("its value is %lf", val);
                LOG_WARNING(primary_msg.Get(), secondary_msg.Get());

                val = (double)dp->default_val;
            }
            *pfval = (float) val;
        }
        break;
    case 'l':
        {
            if (dp->format[2] == 'f')
            {
                double *pdval = (double *) pd;
                double val = (double) dp->default_val;
                if (pelm->Attribute(dp->description, &val) == NULL)
                {
                    CStringFormat primary_msg("Cannot find parameter %s in %s", dp->description, pelm->Value());
                    LOG_WARNING(primary_msg.Get(), NULL);
                }
                if (val > dp->max || val < dp->min)
                {
                    CStringFormat primary_msg("Parameter %s is out of range (%lf, %lf)", dp->description, dp->min, dp->max);
                    CStringFormat secondary_msg("its value is %lf", val);
                    LOG_WARNING(primary_msg.Get(), secondary_msg.Get());

                    val = (double) dp->default_val;
                }
                *pdval = val;
            }
        }
        break;
    default:
        break;
    }
}

TOOL_FUNCTION_EXPORTS TiXmlElement* ReadChildElement(
    TiXmlNode *node,
    DATA_PARSER *dp_table,
    void *pdata,
    const char *name)
{
    assert(node != NULL && dp_table != NULL && name != NULL && pdata != NULL);
    TiXmlElement *pchild = node->FirstChildElement(name);

    if (pchild == NULL)
    {
        return NULL;
    }

    for (DATA_PARSER *dp = dp_table; dp->description != NULL; dp++)
    {
        GetAttribute(pchild, dp, pdata);
    }

    return pchild;
}

void SetAttribute(
    TiXmlElement *pelm,
    DATA_PARSER *dp,
    void *pdata)
{
    assert(pelm != NULL && dp != NULL);

    void *pd = (void*) ((char*) pdata + dp->offset);

    switch (dp->format[1])
    {
    case 's':
        {
                char **ppstr = (char**) pd;
                if (*ppstr != NULL)
                    pelm->SetAttribute(dp->description, *ppstr);
                else
                    pelm->SetAttribute(dp->description, "null");
        }
        break;
    case 'd':
        {
                int *pint = (int *) pd;
                pelm->SetAttribute(dp->description, *pint);
        }
        break;
    case 'f':
        {
                float *pfval = (float *) pd;
                pelm->SetDoubleAttribute(dp->description, (double) *pfval);
        }
        break;
    case 'l':
        {
                if (dp->format[2] == 'f')
                {
                    double *pdval = (double *) pd;
                    pelm->SetDoubleAttribute(dp->description, *pdval);
                }
        }
        break;
    default:
        break;
    }
}

TOOL_FUNCTION_EXPORTS TiXmlElement* WriteChildElement(
    TiXmlNode *node,
    DATA_PARSER *dp_table,
    void *pdata,
    const char *name)
{
    assert(node != NULL && name != NULL);
    TiXmlElement *pchild = node->FirstChildElement(name);

    if (pchild == NULL)
    {
        pchild = (TiXmlElement *) node->InsertEndChild(TiXmlElement(name));
        if (pchild == NULL)
        {
            LOG_ERROR(CStringFormat("Failed to create node: %s", name).Get(), NULL);
            return NULL;
        }
    }

    for (DATA_PARSER *dp = dp_table; dp != NULL && dp->description != NULL; dp++)
    {
        SetAttribute(pchild, dp, pdata);
    }

    return pchild;
}

// Socket Client realization
// include socket lib
#ifdef _WIDNOWS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"urlmon.lib")

int CClientNet::Connect(int port, char* address)
{
    int rlt = 0;
    int iErrMsg;
    WSADATA wsaData;
    iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (iErrMsg != NO_ERROR)
    {
        LOG_ERROR(CStringFormat("wsastartup failed: %d\n", iErrMsg).Get(), NULL);
        rlt = 1;
        return rlt;
    }

    m_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (m_sock == INVALID_SOCKET)
    {
        LOG_ERROR(CStringFormat("false socket: %d\n", WSAGetLastError()).Get(), NULL);
        rlt = 2;
        return rlt;
    }

    sockaddr_in sockaddrServer;
    sockaddrServer.sin_family = AF_INET;
    sockaddrServer.sin_port = htons(port);
    struct hostent *h;
    h = gethostbyname(address);
    sockaddrServer.sin_addr = *((struct in_addr *)h->h_addr);

    iErrMsg = connect(m_sock, (sockaddr*)&sockaddrServer, sizeof(sockaddr));

    if (iErrMsg < 0)
    {
        LOG_ERROR(CStringFormat("connect fail: %d\n", WSAGetLastError).Get(), NULL);
        rlt = 3;
        return rlt;
    }
    LOG_INFO(CStringFormat("IP: %s PORT: %d", address, port).Get(), NULL);

    return rlt;
}

int CClientNet::SendMsg(char*msg, int len)
{
    int rlt = 0;

    int iErrMsg = 0;

    iErrMsg = send(m_sock, msg, len, 0);
    if (iErrMsg < 0 || iErrMsg != len)
    {
        //LOG_ERROR(CStringFormat("send msg failed with error : %d\n", iErrMsg).Get(), NULL);

        rlt = 1;
        return rlt;
    }
    else
    {
        LOG_INFO(CStringFormat("Successfully sent %d bytes", iErrMsg).Get(), NULL);
    }

    return rlt;
}

void CClientNet::Close()
{
    closesocket(m_sock);
}

int CClientNet::ReceiveMsg()
{
    char buf[1024];
    memset(&buf, 0, 1024);
    int rval = recv(m_sock, buf, sizeof(buf), 0);
    if (rval >0)
    {
        char mm[1024];
        memset(&mm, 0, sizeof(mm));
        memcpy(&mm, &buf, sizeof(buf));
        mm[strlen(buf)] = '\0';
        LOG_INFO(CStringFormat("%s", mm).Get(), NULL);
        if (strcmp(mm, "Fail") == 0)
        {
            return 0;
        }
        else if (strcmp(mm, "Success") == 0)
        {
            return 1;
        }
    }

    return -1;
}

CClientNet::CClientNet()
{}

CClientNet::~CClientNet()
{}
#endif