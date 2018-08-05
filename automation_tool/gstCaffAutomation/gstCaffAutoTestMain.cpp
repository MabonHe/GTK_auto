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

#include "gstCaffAutoTestConfig.h"
#include "gstCaffAutoTestScheduler.h"
#include "gstCaffUI.h"

using namespace std;

#define CASE_NAME_MAX_LEN 512
#define CAMERA_NAME_MAX_LEN 64
#define TRACE_CMD_MAX_LEN 512
#define LOG_FILE_NAME "gst-caff-auto-test.log"
#define DEFAULT_CONFIG_FILE_NAME "gst-caff-auto-test-default-config.xml"

// Global Var
int gWaitTimeMax = 5000;
int gCheckPoint = 3;

CLogger *error_logger = CLogger::GetInstance();

void help()
{
    printf("\nThis is ICG SW Validaiton Linux Camera Test Application.\n"
        "Usage:\n"
        "CameraFeatureTestApp.exe\n"
            "   [--config=xxx.xml  Run test cases as config file (xxx.xml) settings]\n"
            "   [--case_id=<case_id>  Run specified test case defined as <case_id>]\n"
            "   [--cam0=<camera name>]\n"
            "   [--cam1=<camera name>]\n"
            "   [--cam2=<camera name>]\n"
            "   [--cam3=<camera name>]\n"
            "   [--preview_warm=<preview warm time (ms) before recording or taking photo>]\n"
            "   [--wait_timeout=<timeout time in ms to wait Capture Engine event>]\n"
            "   [--check_point=<0: basic; 1: basic + performance; 2: basic + feature; 3: all>]\n"
            "   [--load_result=<0: no; !=0: yes>]\n"
            "   [--log_path=<Directory to put the log file and images>]\n"
            "   [--log_level=<0(Result)/1(Error)/2(Warning)/3(Info)/4(Verbose)>]\n"
            "   [--help  Output help information]\n"
            "Example:\n"
            "   1. CameraFeatureTestApp --config=xxx.xml --log_path=\"/home/root/log/\" --cam0=\"IMX175\" --cam1=\"OV2722\"\n"
            "   2. CameraFeatureTestApp --config=xxx.xml --log_path=\"/home/root/log/\" --case_id=xxx --cam0=\"IMX175\" --cam1=\"OV2722\"\n" );
}

bool GetCommandArguments(int argc, char *argv[], const char *inbuff, char *outbuff, int outmaxlen)
{
    assert (inbuff != NULL && outbuff != NULL && outmaxlen > 0);

    size_t len = strlen(inbuff);

    for (int i=1; i<argc; i++)
    {
        if ( strncmp(inbuff, argv[i], len) == 0 )
        {
            if (strlen(argv[i]) >  len)
            {
                strncpy(outbuff, argv[i]+len, outmaxlen);
            }
            return true;
        }
    }

    return false;
}

int CheckBasicResult(CCaseInfo *pCaseInfo)
{
    int retcode = 0;

    int basic_res = pCaseInfo->GetExecutionBasicResult();

    if (basic_res > 0)
    {
        LOG_INFO("Basic check result: pass", NULL);
        retcode = 0;
    }
    else if (0 == basic_res)
    {
        LOG_ERROR("Basic check result: unknown", NULL);
        if (NULL != pCaseInfo->GetBasicErrorMessage())
        {
            LOG_ERROR(pCaseInfo->GetBasicErrorMessage(), NULL);
        }
        retcode = -1;
    }
    else if (basic_res < 0)
    {
        LOG_ERROR("Basic check result: fail", NULL);
        LOG_ERROR(pCaseInfo->GetBasicErrorMessage(), NULL);
        retcode = EXCUTE_RESULT_FAIL_BASIC;
    }

    return retcode;
}

int CheckPerformanceResult(CCaseInfo *pCaseInfo)
{
    int retcode = 0;

    int perf_res = pCaseInfo->GetExecutionPerformanceResult();

    if (perf_res > 0)
    {
        LOG_INFO("Performance check result: pass", NULL);
        retcode = 0;
    }
    else if (0 == perf_res)
    {
        LOG_ERROR("Performance check result: unknown", NULL);
        if (NULL != pCaseInfo->GetPerformanceErrorMessage())
        {
            LOG_ERROR(pCaseInfo->GetPerformanceErrorMessage(), NULL);
        }
        retcode = -1;
    }
    else if (perf_res < 0)
    {
        LOG_ERROR("Performance check result: fail", NULL);
        LOG_ERROR(pCaseInfo->GetPerformanceErrorMessage(), NULL);
        retcode = EXCUTE_RESULT_FAIL_PERFORMANCE;
    }

    return retcode;
}

int CheckFeatureResult(CCaseInfo *pCaseInfo)
{
    int retcode = 0;

    int feature_res = pCaseInfo->GetExecutionFeatureResult();

    if (feature_res > 0)
    {
        LOG_INFO("Performance check result: pass", NULL);
        retcode = 0;
    }
    else if (0 == feature_res)
    {
        LOG_ERROR("Performance check result: unknown", NULL);
        if (NULL != pCaseInfo->GetFeatureErrorMessage())
        {
            LOG_ERROR(pCaseInfo->GetFeatureErrorMessage(), NULL);
        }
        retcode = -1;
    }
    else if (feature_res < 0)
    {
        LOG_ERROR("Performance check result: fail", NULL);
        LOG_ERROR(pCaseInfo->GetFeatureErrorMessage(), NULL);
        retcode = EXCUTE_RESULT_FAIL_FEATURE;
    }

    return retcode;
}

int main(int argc, char *argv[])
{
    int retcode = 0;
    vector<int> retcode_vec;
    char szCurDir[PATH_MAX_LEN]        = {};
    char szLogFile[PATH_MAX_LEN]       = {};
    char szInConfigFile[PATH_MAX_LEN]  = {};
    char szOutConfigFile[PATH_MAX_LEN] = {};
    char szLogPath[PATH_MAX_LEN]       = {};
    char szCaseName[CASE_NAME_MAX_LEN] = {};
    char szTraceCmd[TRACE_CMD_MAX_LEN] = {};
    char cameras[MAX_CONCURRENT_CAM_COUNT][CAMERA_NAME_MAX_LEN] = { "", "", "", "" };
    LOG_LEVEL logLevel = LOG_LEVEL_VERBOSE;
    char szInConfigFileName[PATH_MAX_LEN] = {};
    int  previewWarmTime = 3000;
    bool bSingleCase = false;

    int  iLoadResult = 0;
    bool bCheckProcessResource = false;

    const char *CONFIG      = "--config=";
    const char *CASE_ID     = "--case_id=";
    const char *LOG_PATH    = "--log_path=";
    const char *CAM0        = "--cam0=";
    const char *CAM1        = "--cam1=";
    const char *CAM2        = "--cam2=";
    const char *CAM3        = "--cam3=";
    const char *LOG_LEVEL_S = "--log_level=";
    const char *PREVIEW_WARM = "--preview_warm=";
    const char *WAIT_TIME_OUT = "--wait_timeout=";
    const char *CHECK_POINT = "--check_point="; // 0: "basic" 1: "basic+performance" 2: "basic+feature" 3: "all"
    const char *LOAD_RESULT = "--load_result=";
    const char *HELP        = "--help";

    FILE *fp = NULL;

    // out buffer for temp usage
    char outbuff[PATH_MAX_LEN] = {};

    // --help
    if (argc < 2 || GetCommandArguments(argc, argv, HELP, outbuff, PATH_MAX_LEN))
    {
        help();
        return -1;
    }

    try
    {
        //
        // Get app directory, and give the log file and config file path name
        //
        if (!GetAppPath(szCurDir, PATH_MAX_LEN))
        {
            EXCEPTION("Failed to get application path!");
        }

        LOG_INFO("Current dir:", szCurDir);

        bool needUpdateUi = true;
        GtkBuilder *builder = NULL;
        GstCaffUI *gstCaffUI = NULL;
        EventControl *eventControl = NULL;
        RemoteClient *remoteClient = NULL;

        // Event control initilization
        eventControl = EventControl::getInstance(needUpdateUi);
        if (NULL == eventControl)
        {
            EXCEPTION("Failed to get event control handle");
        }
        if (0 != eventControl->init())
        {
            EXCEPTION("Failed to init event control");
        }

        remoteClient = new RemoteClient();

        // GTK UI initialization
        gtk_init(&argc, &argv);
        builder = gtk_builder_new();
        if(!gtk_builder_add_from_file(builder,
            CStringFormat("%s/CAFF.glade", szCurDir).Get(), NULL)) {
            EXCEPTION("Failed to load CAFF.glade");
        }

        gstCaffUI = new GstCaffUI(eventControl, remoteClient);
        gstCaffUI->init(builder);
        gstCaffUI->showUI();

        // Give the default config file
        snprintf(szInConfigFile,  PATH_MAX_LEN, "%s/%s", szCurDir, DEFAULT_CONFIG_FILE_NAME);

        // Config file
        GetCommandArguments(argc, argv, CONFIG, szInConfigFile, PATH_MAX_LEN);

        // Default log file
        snprintf(szLogFile, PATH_MAX_LEN, "%s/%s", szCurDir, LOG_FILE_NAME);

        // Log file and Out config file
        if (GetCommandArguments(argc, argv, LOG_PATH, szLogPath, PATH_MAX_LEN))
        {
            snprintf(szLogFile, PATH_MAX_LEN, "%s/%s", szLogPath, LOG_FILE_NAME);

            if (!GetFileName(szInConfigFile, szInConfigFileName, PATH_MAX_LEN))
            {
                EXCEPTION("Failed to GetFileName");
            }

            snprintf(szOutConfigFile, PATH_MAX_LEN, "%s/%s", szLogPath, szInConfigFileName);
        }
        else
        {
            strcpy(szOutConfigFile, szInConfigFile);
        }

        // Camera Name
        GetCommandArguments(argc, argv, CAM0, cameras[0], 64);
        GetCommandArguments(argc, argv, CAM1, cameras[1], 64);
        GetCommandArguments(argc, argv, CAM2, cameras[2], 64);
        GetCommandArguments(argc, argv, CAM3, cameras[3], 64);

        // Case ID
        if (GetCommandArguments(argc, argv, CASE_ID, szCaseName, CASE_NAME_MAX_LEN))
        {
            bSingleCase = true;
        }

        // Log Level
        if (GetCommandArguments(argc, argv, LOG_LEVEL_S, outbuff, PATH_MAX_LEN))
        {
            logLevel = (LOG_LEVEL) atoi(outbuff);
        }
        error_logger->SetLogLevel(logLevel);

        // preview warm time
        if (GetCommandArguments(argc, argv, PREVIEW_WARM, outbuff, PATH_MAX_LEN))
        {
            previewWarmTime = atoi(outbuff);
        }

        // wait timeout time for capture engine events
        if (GetCommandArguments(argc, argv, WAIT_TIME_OUT, outbuff, PATH_MAX_LEN))
        {
            gWaitTimeMax = atoi(outbuff);
        }

        // App Check point
        if (GetCommandArguments(argc, argv, CHECK_POINT, outbuff, PATH_MAX_LEN))
        {
            gCheckPoint = atoi(outbuff);
        }

        // Load result
        if (GetCommandArguments(argc, argv, LOAD_RESULT, outbuff, PATH_MAX_LEN))
        {
            iLoadResult = atoi(outbuff);
        }

        TiXmlDocument doc;
        CScheduler SCheduler(eventControl);

        fp = fopen(szLogFile, "a+");
        if (fp == NULL)
        {
            LOG_WARNING(CStringFormat("Failed to open %s", szLogFile).Get(), NULL);
        }
        else
        {
            error_logger->SetFile(fp);
        }

        if (!doc.LoadFile(szInConfigFile))
        {
            throw CException("%s %d: TiXmlDocument::LoadFile! failed info: %s",
                __FILE__, __LINE__, doc.ErrorDesc());
        }

        TiXmlNode *itp = NULL;
        while (itp = doc.IterateChildren(itp))
        {
            TiXmlElement *pE = (TiXmlElement*)itp;

            if (bSingleCase && 0 != strcmp(szCaseName, pE->Value()))
            {
                continue;
            }

            CCaseInfo *pcaseinfo = new CCaseInfo(pE->Value());

            assert(pcaseinfo != NULL);

            LOG_INFO(CStringFormat("Start Case <%s> from here----------------------", pE->Value()).Get(), NULL);

            try
            {
                pcaseinfo->Read(&doc);
            }
            catch (CException e)
            {
                LOG_ERROR(e.GetException(), NULL);
                retcode = -1;
                break;
            }

            // For executed case just read the sub result from the config
            if (iLoadResult && pcaseinfo->IsResultEnabled())
            {
                int basic_res = pcaseinfo->GetExecutionBasicResult();
                int performance_res = pcaseinfo->GetExecutionPerformanceResult();
                int feature_res = pcaseinfo->GetExecutionFeatureResult();

                switch (gCheckPoint)
                {
                case 0: // Basic
                    LOG_INFO("Check point: Basic", NULL);
                    retcode = CheckBasicResult(pcaseinfo);
                    break;

                case 1: // Basic + Performance
                    LOG_INFO("Check point: Basic + Performance", NULL);
                    retcode = CheckBasicResult(pcaseinfo);
                    if (0 != retcode)
                    {
                        break;
                    }
                    retcode = CheckPerformanceResult(pcaseinfo);
                    break;
                case 2: // Basic + Feature
                    LOG_INFO("Check point: Basic + Feature", NULL);
                    retcode = CheckBasicResult(pcaseinfo);
                    if (0 != retcode)
                    {
                        break;
                    }
                    retcode = CheckFeatureResult(pcaseinfo);
                    break;
                case 3: // Basic + Feature + Performance
                    LOG_INFO("Check point: Basic + Feature + Performance", NULL);
                    retcode = CheckBasicResult(pcaseinfo);
                    if (0 != retcode)
                    {
                        break;
                    }
                    retcode = CheckFeatureResult(pcaseinfo);
                    if (0 != retcode)
                    {
                        break;
                    }
                    retcode = CheckPerformanceResult(pcaseinfo);
                    break;
                }
            }
            else // execute the test
            {
                try
                {
                    char *pcams [MAX_CONCURRENT_CAM_COUNT] = { cameras[0], cameras[1], cameras[2], cameras[3] };
                    char case_log_path[PATH_MAX_LEN] = {};
                    snprintf(case_log_path, PATH_MAX_LEN, "%s/%s", szLogPath, pE->Value());

                    pcaseinfo->SetExeStartTime(GetCurrentTimeString());
                    pcaseinfo->SetStatus(1);
                    pcaseinfo->Write(&doc);
                    doc.SaveFile(szOutConfigFile);
                    doc.SaveFile(szInConfigFile);

                    SCheduler.SetCaseInfo(pcaseinfo);
                    SCheduler.SetExeParams((const char*)case_log_path, (const char**)pcams, previewWarmTime);

                    int exeret = EXCUTE_RESULT_PASS;

                    CScheduler::StartTestExecutionProc((void*)&SCheduler);

                    while (!SCheduler.WaitExecutionCompleted(100))
                    {
                        while (gtk_events_pending ())
                            gtk_main_iteration ();
                    }

                    exeret = SCheduler.GetExeResult();

                    pcaseinfo->SetExecutionBasicResult(!(exeret & EXCUTE_RESULT_FAIL_BASIC));
                    pcaseinfo->SetExecutionPerformanceResult(!(exeret & EXCUTE_RESULT_FAIL_PERFORMANCE));
                    pcaseinfo->SetExecutionFeatureResult(!(exeret & EXCUTE_RESULT_FAIL_FEATURE));

                    switch (gCheckPoint)
                    {
                    case 0: // Basic
                        LOG_INFO("Check point: Basic", NULL);
                        retcode = exeret & EXCUTE_RESULT_FAIL_BASIC;
                        break;

                    case 1: // Basic + Performance
                        LOG_INFO("Check point: Basic + Performance", NULL);
                        retcode = (exeret & EXCUTE_RESULT_FAIL_BASIC) +
                            (exeret & EXCUTE_RESULT_FAIL_PERFORMANCE);
                        break;
                    case 2: // Basic + Feature
                        LOG_INFO("Check point: Basic + Feature", NULL);
                        retcode = (exeret & EXCUTE_RESULT_FAIL_BASIC) +
                            (exeret & EXCUTE_RESULT_FAIL_FEATURE);
                        break;
                    case 3: // Basic + Feature + Performance
                        LOG_INFO("Check point: Basic + Feature + Performance", NULL);
                        retcode = (exeret & EXCUTE_RESULT_FAIL_BASIC) +
                            (exeret & EXCUTE_RESULT_FAIL_FEATURE) +
                            (exeret & EXCUTE_RESULT_FAIL_PERFORMANCE);
                        break;
                    }

                    pcaseinfo->SetExeStopTime(GetCurrentTimeString());
                }
                catch (CException e)
                {
                    LOG_ERROR(e.GetException(), NULL);
                    pcaseinfo->AddBasicErrorMessage(e.GetException());
                    pcaseinfo->SetExecutionBasicResult(false);
                    retcode = -1;
                }

                pcaseinfo->SetStatus(2);
                pcaseinfo->Write(&doc);
                doc.SaveFile(szOutConfigFile);
                doc.SaveFile(szInConfigFile);
            }
            retcode_vec.push_back(retcode);
            LOG_INFO(CStringFormat("End Case <%s> from here----------------------", pE->Value()).Get(), NULL);
            delete pcaseinfo;
        }

        if (0 == retcode_vec.size())
        {
            LOG_ERROR(CStringFormat("Test case %s is not founded in %s", szCaseName, szInConfigFile).Get(), NULL);
            retcode = 2;
            retcode_vec.push_back(retcode);
        }

        delete gstCaffUI;
        delete remoteClient;
        delete eventControl;
    }
    catch (CException e)
    {
        LOG_ERROR(e.GetException(), NULL);
        retcode = -2;
        retcode_vec.push_back(retcode);
    }

    size_t failed_count = 0;
    size_t total_count = retcode_vec.size();
    for (size_t i = 0; i < total_count; i++)
    {
        if (0 != retcode_vec[i])
        {
            failed_count++;
        }
    }

    CStringFormat summary("Summary: total case count: %d; pass case count: %d; fail case count: %d",
        total_count, total_count - failed_count, failed_count);

    if (failed_count > 0)
    {
        LOG_ERROR(summary.Get(), NULL);
    }
    else
    {
        LOG_INFO(summary.Get(), NULL);
    }

    if (0 != failed_count)
    {
        LOG_RESULT(false);
    }
    else
    {
        LOG_RESULT(true);
    }

    if (fp != NULL)
    {
        error_logger->SetFile(NULL);
        fclose(fp);
        fp = NULL;
    }

    return (int)failed_count;
}

