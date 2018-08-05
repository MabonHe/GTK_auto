/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2016 Intel Corporation
 * All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */
#ifndef _GSTCAFF_UI_H_
#define _GSTCAFF_UI_H_

#include <gtk/gtk.h>
#include "EventControl.h"
#include "RemoteClient.h"
#include <vector>
#include <mutex>

#define MAX_CAMERA_NUM 4
#define MAX_REGION_NUM 5
#define REGION_WINDOW_SIZE 30
namespace gstCaff {

typedef enum
{
    CAMERA_MODE_STOP = 0,
    CAMERA_MODE_PREVIEW,
    CAMERA_MODE_SNAPSHOT,
    CAMERA_MODE_RECORDING,
    CAMERA_MODE_PAUSED,
    CAMERA_MODE_PREVIEW_PAUSED,
    CAMERA_MODE_RECORDING_PAUSED,
} CamMode;

typedef struct _EnvSetting
{
    bool clientEnable;
    char remoteIP[32];
    int camNumber;
    bool startAll;
    bool stopAll;
} EnvSetting;

typedef enum
{
    REGION_STATE_NONE = 0,
    REGION_STATE_AE,
    REGION_STATE_AWB,
} RegionState;

struct TouchRegion
{
    int left;
    int top;
    int right;
    int bottom;
    int weight;
};

typedef struct _CamSetting
{
    bool updateUI;
    char source[32];
    char sink[32];
    char convert[32];
    char statusInfo[100];
    char IOMode[20];
    char format[10];
    bool interlaceMode;
    bool mipiCaptureMode;
    char reso[20];
    char deInterlace[10];
    char deviceName[100];
    bool vcEnable;
    char IrisMode[20];
    int irisLevel;
    char aeMode[20];
    char fisheyemode[20];
    char convergeSpeedMode[20];
    char convergeSpeedLevel[20];
    int exposureTime;
    float exposureGain;
    int exposureEv;

    char awbMode[20];
    char awbRange[20];
    char awbWp[30];
    int awbRGain;
    int awbBGain;
    int awbGGain;
    int awbRshift;
    int awbGshift;
    int awbBshift;
    int windowSize;
    char regionMode[20];
    char daynightMode[20];
    int sharpness;
    int brightness;
    int contrast;
    int hue;
    int saturation;
    char WdrMode[20];
    char BlcArea[20];
    int WdrLevel;
    char noiseReductionMode[20];
    int noiseFilterLevel;
    int spatialLevel;
    int temporialLevel;
    char mirrorEffectMode[20];
    char sceneMode[20];
    char sensorResolution[20];
    char captureFps[20];
    char videoStandard[20];
    char advanced[20];
    bool saveLog;
    int frameNum;
    int loopTime;
    //guintptr embedXid
} CamSetting;

class GstCaffUI {
public:
    GstCaffUI(EventControl * eventControl, RemoteClient* remoteClient );
    ~GstCaffUI(){}
    int init(GtkBuilder *builder);
    void applySettingWithEnv(void);
    void showUI();
    void UpdateCameraNum(int ncount);

public:
    static void enableClientCallback(GtkWidget *widget, gpointer data);
    static void cameraNumChangedCallback(GtkWidget *widget, gpointer data);
    static void cameraIndexChangedCallback(GtkWidget *widget, gpointer data);
    static void startAllCameraCallback(GtkWidget *widget, gpointer data);
    static void stopAllCameraCallback(GtkWidget *widget, gpointer data);
    static void sourceElementChangedCallback(GtkWidget *widget, gpointer data);
    static void sinkElementChangedCallback(GtkWidget *widget, gpointer data);
    static void convertElementChangedCallback(GtkWidget *widget, gpointer data);
    static void cameraModeChangeCallback(GtkWidget *widget, gpointer data);
    static void drawAreaDrawCallback(GtkWidget * widget, cairo_t * cr, gpointer data);
    static void drawAreaRealizeCallback(GtkWidget *widget, gpointer data);
    static void drawAreaPressCallback(GtkWidget *widget, GdkEventButton *event, gpointer data);
    static void mainWinowQuitCallback(GtkWidget *button, gpointer data);
    static void IOModeChangedCallback(GtkWidget *widget, gpointer data);
    static void formatChangedCallback(GtkWidget *widget, gpointer data);
    static void enableInterlaceCallback(GtkWidget *widget, gpointer data);
    static void enableVirtualChannelCallback(GtkWidget *widget, gpointer data);
    static void enableMipiCaptureCallback(GtkWidget *widget, gpointer data);
    static void resolutionChangedCallback(GtkWidget *widget, gpointer data);
    static void deInterlaceChangedCallback(GtkWidget *widget, gpointer data);
    static void deviceChangedCallback(GtkWidget *widget, gpointer data);
    static void IRISModeChangedCallback(GtkWidget *widget, gpointer data);
    static void setIrisLevelCallback(GtkWidget *widget, gpointer data);
    static void aeModeChangedCallback(GtkWidget *widget, gpointer data);
    static void setExposureTimeCallback(GtkWidget *widget,  gpointer data);
    static void exposureTimeChangeCallback(GtkWidget *widget,  GtkScrollType  type, gpointer data);
    static void setExposureGainCallback(GtkWidget *widget,  gpointer data);
    static void setExposureEvCallback(GtkWidget *widget, gpointer data);
    static void convergeSpeedLevelChangedCallback(GtkWidget *widget, gpointer data);
    static void convergeSpeedModeChangedCallback(GtkWidget *widget, gpointer data);
    static void awbModeChangedCallback(GtkWidget *widget, gpointer data);
    static void setAwbRangeCallback(GtkWidget *widget, gpointer data);
    static void awbRGainChangedCallback(GtkWidget *widget, gpointer data);
    static void awbGGainChangedCallback(GtkWidget *widget, gpointer data);
    static void awbBGainChangedCallback(GtkWidget *widget, gpointer data);
    static void awbTransformCallback(GtkWidget *widget, gpointer data);
    static void setAwbRshiftCallback(GtkWidget *widget, gpointer data);
    static void setAwbGshiftCallback(GtkWidget *widget, gpointer data);
    static void setAwbBshiftCallback(GtkWidget *widget, gpointer data);

    static void regionModeChangedCallback(GtkWidget *widget, gpointer data);
    static void weightChangedCallback(GtkWidget *widget,  GtkScrollType  type, gpointer data);
    static void setRegionWeightCallback(GtkWidget *widget, gpointer data);

    static void setWindowSizeCallback(GtkWidget *widget, gpointer data);
    static void dayNightModeChangedCallback(GtkWidget *widget, gpointer data);
    static void setSharpnessCallback(GtkWidget *widget, gpointer data);
    static void setBrightnessCallback(GtkWidget *widget, gpointer data);
    static void setContrastCallback(GtkWidget *widget, gpointer data);
    static void setHueCallback(GtkWidget *widget, gpointer data);
    static void setSaturationCallback(GtkWidget *widget, gpointer data);
    static void WDRModeChangedCallback(GtkWidget *widget, gpointer data);
    static void BlcAreaChangedCallback(GtkWidget *widget, gpointer data);
    static void setWdrLevelCallback(GtkWidget *widget, gpointer data);
    static void NoiseModeChangedCallback(GtkWidget *widget, gpointer data);
    static void setNoiseFilterCallback(GtkWidget *widget, gpointer data);
    static void setSpatialCallback(GtkWidget *widget, gpointer data);
    static void setTemporalCallback(GtkWidget *widget, gpointer data);
    static void mirrorEffectChangedCallback(GtkWidget *widget, gpointer data);
    static void sceneModeChangedCallback(GtkWidget *widget, gpointer data);
    static void sensorResolutionChangedCallback(GtkWidget *widget, gpointer data);
    static void captureFpsChangedCallback(GtkWidget *widget, gpointer data);
    static void videoStandardChangedCallback(GtkWidget *widget, gpointer data);
    static void advanceFeatureChangedCallback(GtkWidget *widget, gpointer data);
    static void setCustomAICCallback(GtkWidget *widget, gpointer data);
    static void resetSettingCallback(GtkWidget *widget, gpointer data);
    static void restartCameraCallback(GtkWidget *widget, gpointer data);
    static void setDebugLevelCallback(GtkWidget *widget, gpointer data);
    static void saveLogButtonCallback(GtkWidget *widget, gpointer data);
    static void testDualButtonCallback(GtkWidget *widget, gpointer data);
    static void testVcButtonCallback(GtkWidget *widget, gpointer data);
    static void setDumpLevelCallback(GtkWidget *widget, gpointer data);
    static void setCamFrameNumCallback(GtkWidget *widget, gpointer data);
    static void setCamLoopTimeCallback(GtkWidget *widget, gpointer data);
    static void startCamButtonCallback(GtkWidget *widget, gpointer data);
    static void updateUISetting(int camId, UpdateUi update);
    static void handleFrameEnd(int camId);
    static void fisheyemodechangecallback(GtkWidget *widget, gpointer data);
private:
    void getWidgetFromBuilder(GtkBuilder *builder);
    void registerSignalForWidget(void);
    void applyDefaultSetting(int camId);
    void setRegionPosition(int camId, bool updateSpin);
    int getRealMousePostion(int camId, int event_x, int event_y, struct TouchRegion &region);
    void updateUIforCamera(int cameraId, bool updateUI);
    void updateSettingSource(char *source);
    void updateSettingSink(char *sink);
    void updateSettingConvert(char *convert);
    void updateSettingIOMode(char *ioMode);
    void updateSettingFmt(char *fmt);
    void updateSettingInterlaceMode(bool interlaceMode);
    void updateSettingVcEnable(bool enable);
    void updateSettingMipiCaptureMode(bool mode);
    void updateSettingResolution(char *reso);
    void updateSettingDeinterlace(char *deinterlace);
    void updateSettingDeviceName(char *name);
    void updateSettingIRISMode(char *iris);
    void updateSettingIrisLevel(int level);
    void updateSettingAeMode(char *mode);
    void updateSettingConvergeSpeedMode(char *value);
    void updateSettingConvergeSpeedLevel(char *value);
    void updateSettingExposureTime(int exposureTime);
    void updateSettingExposureGain(int gain);
    void updateSettingExposureEv(int ev);
    void updateSettingAwbMode(char *mode);
    void updateSettingAwbRange(char *value);
    void updateSettingAwbWp(char *position);
    void updateSettingAwbGainR(int value);
    void updateSettingAwbGainG(int value);
    void updateSettingAwbGainB(int value);
    void updateSettingAwbShiftR(int value);
    void updateSettingAwbShiftG(int value);
    void updateSettingAwbShiftB(int value);
    void updateSettingWindowSize(int size);
    void updateSettingRegionMode(char *mode);
    void updateSettingDaynightMode(char *value);
    void updateSettingSharpness(int value);
    void updateSettingBrightness(int value);
    void updateSettingContrast(int value);
    void updateSettingHue(int value);
    void updateSettingSaturation(int value);
    void updateSettingWDRMode(char *value);
    void updateSettingBlcArea(char *value);
    void updateSettingWdrValue(int value);
    void updateSettingNoiseReductionMode(char *value);
    void updateSettingNoiseFilter(int value);
    void updateSettingSpatial(int value);
    void updateSettingTemporal(int value);
    void updateSettingMirrorEffect(char *value);
    void updateSettingSceneMode(char *value);
    void updateSettingSensorReso(char *value);
    void updateSettingCaptureFps(char *value);
    void updateSettingVideoStandard(char *value);
    void updateSettingAdvancedFeature(char *value);
    void updateSettingFisheyeMode(char *mode);

    bool processCurrentModeMachine(CamMode *newMode, CamMode oldMode);

private:
    GtkWindow *mWindow;
    GtkFixed *mMainFixed;
    GtkButton *mEnableClient;
    GtkEntry *mRemoteIp;
    GtkComboBoxText *mCameNum;
    GtkComboBoxText *mCamIndex;
    GtkToggleButton *mStartAll;
    GtkToggleButton *mStopAll;
    GtkComboBoxText *mSource;
    GtkComboBoxText *mSink;
    GtkComboBoxText *mConvert;

    GtkButton *mStart;
    GtkButton *mPause;
    GtkButton *mStop;
    GtkButton *mRecord;
    GtkButton *mSnapshot;

    GtkStatusbar *mCam0Status;
    GtkStatusbar *mCam1Status;
    GtkStatusbar *mCam2Status;
    GtkStatusbar *mCam3Status;

    GtkDrawingArea *mCam0Area;
    GtkDrawingArea *mCam1Area;
    GtkDrawingArea *mCam2Area;
    GtkDrawingArea *mCam3Area;

    GtkComboBoxText *mIOMode;
    GtkCheckButton *mMipiCapture;
    GtkComboBoxText *mFormat;
    GtkCheckButton *mInterlaceMode;
    GtkComboBoxText *mResolution;
    GtkComboBoxText *mDeInterlace;
    GtkComboBoxText *mDeviceName;
    GtkCheckButton *mVcEnable;

    GtkComboBoxText *mIrisMode;
    GtkScale *mIrisLevel;
    GtkComboBoxText *mAeMode;
    GtkComboBoxText *mFisheyeMode;
    GtkLabel *mExposureTimeEg;
    GtkLabel *mExposureGainEg;
    GtkLabel *mExposureTimeLabel;
    GtkLabel *mExposureGainLabel;
    GtkComboBoxText *mConvergeSpeedMode;
    GtkComboBoxText *mConvergeSpeedLevel;
    GtkSpinButton *mExposureTimeLevel;
    GtkSpinButton *mExposureGainLevel;
    GtkScale *mExposureEvLevel;

    GtkComboBoxText *mAwbMode;
    GtkLabel *mAwbRangeEg;
    GtkLabel *mAwbRangeLabel;
    GtkEntry *mAwbRangeEntry;
    GtkLabel *mAwbWpLabel;
    GtkEntry *mAwbWpEntry;
    GtkLabel *mAwbRGainLable;
    GtkLabel *mAwbGGainLable;
    GtkLabel *mAwbBGainLable;
    GtkScale *mAwbRGainScale;
    GtkScale *mAwbGGainScale;
    GtkScale *mAwbBGainScale;
    GtkLabel *mColorLabel;
    GtkLabel *mColorRangeLabel;
    GtkEntry *mColorTansform;
    GtkScale *mAwbRShift;
    GtkScale *mAwbGShift;
    GtkScale *mAwbBShift;

    GtkComboBoxText *mRegionMode;

    GtkEntry *mRegionEntry[MAX_REGION_NUM];
    GtkSpinButton *mWeightSpinbutton[MAX_REGION_NUM];

    GtkScale *mWindowSizeScale;

    GtkComboBoxText *mDayNightMode;
    GtkScale *mSharpnessLevel;
    GtkScale *mBrightnessLevel;
    GtkScale *mContrastLevel;
    GtkScale *mHueLevel;
    GtkScale *mSaturationLevel;

    GtkComboBoxText *mWdrMode;
    GtkComboBoxText *mBlcArea;
    GtkScale *mWdrLevel;
    GtkLabel *mWdrLabel;

    GtkComboBoxText *mNoiseReductionMode;
    GtkScale *mNoiseFilterLevel;
    GtkScale *mSpatialLevel;
    GtkScale *mTemporalLevel;
    GtkLabel *mNoiseFilter;
    GtkLabel *mSpatial;
    GtkLabel *mTemporal;

    GtkComboBoxText *mMirrorEffect;
    GtkComboBoxText *mSceneMode;
    GtkComboBoxText *mSensorReso;
    GtkComboBoxText *mCaptureFps;
    GtkComboBoxText *mVideoStandard;

    GtkComboBoxText *mAdvanced;
    GtkButton *mSetCustomAic;
    GtkTextView *mCustomAicParam;
    GtkButton *mReset;
    GtkButton *mRestart;

    GtkEntry *mLogLevel;
    GtkEntry *mDumpLevel;
    GtkToggleButton *mSaveLog;
    GtkTextView *mLogText;
    GtkToggleButton *mTestDual;
    GtkEntry *mCam0FrameNum;
    GtkEntry *mCam0LoopTime;
    GtkButton *mCam0Start;
    GtkEntry *mCam1FrameNum;
    GtkEntry *mCam1LoopTime;
    GtkButton *mCam1Start;
    GtkToggleButton *mTestVC;

    EnvSetting mEnvSetting;
    CamMode mCamMode[MAX_CAMERA_NUM];
    CamSetting mCamSetting[MAX_CAMERA_NUM];
    int mCamNumber;
    int mCurCameraId;
    EventControl *mEventControl;
    RemoteClient *mRemoteClient;
    int clientMode;
    RegionState mRegionState;
    vector<struct TouchRegion> mRegion;
    bool mTestRunning;
    std::mutex mEofLock;
    static GstCaffUI *sInstance;
};

} //namespace gstCaff

#endif //_GSTCAFF_UI_H_

