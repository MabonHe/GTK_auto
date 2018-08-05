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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <string.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gst/video/videooverlay.h>
#include <gst/video/colorbalance.h>
#include <gst/video/navigation.h>
#include <gst/video/gstvideosink.h>
#include "gstCaffUI.h"

namespace gstCaff {

GstCaffUI * GstCaffUI::sInstance;
GstCaffUI::GstCaffUI(EventControl * eventControl, RemoteClient* remoteClient ):
    mWindow(NULL)
    ,mMainFixed(NULL)
    ,mEnableClient(NULL)
    ,mRemoteIp(NULL)
    ,mCameNum(NULL)
    ,mCamIndex(NULL)
    ,mStartAll(NULL)
    ,mStopAll(NULL)
    ,mSource(NULL)
    ,mConvert(NULL)
    ,mSink(NULL)
    ,mStart(NULL)
    ,mPause(NULL)
    ,mStop(NULL)
    ,mRecord(NULL)
    ,mSnapshot(NULL)
    ,mCam0Status(NULL)
    ,mCam1Status(NULL)
    ,mCam2Status(NULL)
    ,mCam3Status(NULL)
    ,mCam0Area(NULL)
    ,mCam1Area(NULL)
    ,mCam2Area(NULL)
    ,mCam3Area(NULL)
    ,mIOMode(NULL)
    ,mMipiCapture(NULL)
    ,mFormat(NULL)
    ,mInterlaceMode(NULL)
    ,mResolution(NULL)
    ,mDeInterlace(NULL)
    ,mDeviceName(NULL)
    ,mVcEnable(NULL)
    ,mIrisMode(NULL)
    ,mIrisLevel(NULL)
    ,mAeMode(NULL)
    ,mExposureTimeEg(NULL)
    ,mExposureGainEg(NULL)
    ,mExposureTimeLevel(NULL)
    ,mExposureGainLevel(NULL)
    ,mConvergeSpeedMode(NULL)
    ,mConvergeSpeedLevel(NULL)
    ,mExposureEvLevel(NULL)
    ,mAwbMode(NULL)
    ,mAwbRangeLabel(NULL)
    ,mAwbRangeEg(NULL)
    ,mAwbRangeEntry(NULL)
    ,mAwbWpLabel(NULL)
    ,mAwbWpEntry(NULL)
    ,mAwbRGainLable(NULL)
    ,mAwbGGainLable(NULL)
    ,mAwbBGainLable(NULL)
    ,mAwbRGainScale(NULL)
    ,mAwbGGainScale(NULL)
    ,mAwbBGainScale(NULL)
    ,mColorLabel(NULL)
    ,mColorRangeLabel(NULL)
    ,mColorTansform(NULL)
    ,mAwbRShift(NULL)
    ,mAwbGShift(NULL)
    ,mAwbBShift(NULL)
    ,mRegionMode(NULL)
    ,mWindowSizeScale(NULL)
    ,mDayNightMode(NULL)
    ,mSharpnessLevel(NULL)
    ,mBrightnessLevel(NULL)
    ,mContrastLevel(NULL)
    ,mHueLevel(NULL)
    ,mSaturationLevel(NULL)
    ,mWdrMode(NULL)
    ,mBlcArea(NULL)
    ,mWdrLevel(NULL)
    ,mWdrLabel(NULL)
    ,mNoiseReductionMode(NULL)
    ,mNoiseFilterLevel(NULL)
    ,mSpatialLevel(NULL)
    ,mTemporalLevel(NULL)
    ,mNoiseFilter(NULL)
    ,mSpatial(NULL)
    ,mTemporal(NULL)
    ,mMirrorEffect(NULL)
    ,mSceneMode(NULL)
    ,mSensorReso(NULL)
    ,mCaptureFps(NULL)
    ,mVideoStandard(NULL)
    ,mAdvanced(NULL)
    ,mSetCustomAic(NULL)
    ,mCustomAicParam(NULL)
    ,mReset(NULL)
    ,mRestart(NULL)
    ,mLogLevel(NULL)
    ,mDumpLevel(NULL)
    ,mSaveLog(NULL)
    ,mLogText(NULL)
    ,mTestDual(NULL)
    ,mCam0FrameNum(NULL)
    ,mCam0LoopTime(NULL)
    ,mCam0Start(NULL)
    ,mCam1FrameNum(NULL)
    ,mCam1LoopTime(NULL)
    ,mCam1Start(NULL)
    ,mCamNumber(0)
    ,mCurCameraId(-1)
    ,mEventControl(eventControl)
    ,mRemoteClient(remoteClient)
    ,clientMode(0)
    ,mTestRunning(false)
    ,mTestVC(NULL)
    ,mFisheyeMode(NULL)
{
    printf("@%s\n", __func__);

    int i = 0;
    for (i = 0; i < MAX_REGION_NUM; i++) {
        mRegionEntry[i] = NULL;
        mWeightSpinbutton[i] = NULL;
    }

    for (i = 0; i < MAX_CAMERA_NUM; i++) {
        memset(&mCamSetting, 0, sizeof(CamSetting));
        mCamMode[i] = CAMERA_MODE_STOP;
    }
    mRegion.clear();
    mRegion.reserve(MAX_REGION_NUM);
    mRegionState = REGION_STATE_NONE;
    sInstance = this;
    eventControl->setGstCaffUIHandle(this);
}

int GstCaffUI::init(GtkBuilder *builder)
{
    printf("@%s\n", __func__);
    getWidgetFromBuilder(builder);
    registerSignalForWidget();

    mEventControl->updateUiInterface = GstCaffUI::updateUISetting;
    mEventControl->notifyEOF = GstCaffUI::handleFrameEnd;
    mEnvSetting.clientEnable = false;
    mEnvSetting.camNumber = 4;
    mEnvSetting.startAll = false;
    mEnvSetting.stopAll = false;
    memset(mEnvSetting.remoteIP, 0, sizeof(mEnvSetting.remoteIP));
    for (int i = 0; i < MAX_CAMERA_NUM; i++) {
        applyDefaultSetting(i);
        mEventControl->setSrcElementType(i, mCamSetting[i].source);
        mEventControl->setSinkElementType(i, mCamSetting[i].sink);
        mEventControl->setConvertElementType(i, mCamSetting[i].convert);
        mEventControl->setDeviceName(i, mCamSetting[i].deviceName);
        mEventControl->setResolution(i, 640, 480);
        mEventControl->setFormat(i, mCamSetting[i].format);
        mEventControl->setInterlaceMode(i, mCamSetting[i].interlaceMode);
        mEventControl->setDeInterlace(i, mCamSetting[i].deInterlace);
        mEventControl->setIoMode(i, mCamSetting[i].IOMode);
    }
    unsetenv("cameraDump");
    unsetenv("cameraDumpPath");

    return 0;
}

void GstCaffUI::updateUISetting(int camId, UpdateUi update)
{
    //TODO, do this in the future.
}

void GstCaffUI::handleFrameEnd(int camId)
{
    char itemId[20] = {0};
    GstCaffUI *mUi = sInstance;
    //ignore this if doesn't in test dual mode.
    if (mUi->mTestRunning == false)
        return;

    mUi->mEofLock.lock();
    snprintf(itemId, 20, "CameraId%d", camId);
    gtk_combo_box_set_active_id((GtkComboBox *)mUi->mCamIndex, itemId);
    gtk_button_clicked (mUi->mStop);
    printf("cameraId: %d run the loopTime: %d succeed\n", camId, (mUi->mCamSetting[camId].loopTime)--);

    if (mUi->mCamSetting[camId].loopTime <= 0) {
        printf("============ CAMERA: %d TEST SUCCEED ===========\n", camId);
        mUi->mEofLock.unlock();
        return;
    }
    sleep(1);
    gtk_button_clicked (mUi->mStart);
    mUi->mEofLock.unlock();
}

void GstCaffUI::showUI(void)
{
    gtk_widget_show((GtkWidget *)(mWindow));
    gtk_widget_show((GtkWidget *)(mMainFixed));
    gtk_widget_hide((GtkWidget*)(mAwbRGainLable));
    gtk_widget_hide((GtkWidget*)(mAwbGGainLable));
    gtk_widget_hide((GtkWidget*)(mAwbBGainLable));
    gtk_widget_hide((GtkWidget*)(mAwbRGainScale));
    gtk_widget_hide((GtkWidget*)(mAwbGGainScale));
    gtk_widget_hide((GtkWidget*)(mAwbBGainScale));
    gtk_widget_hide((GtkWidget*)(mAwbRangeLabel));
    gtk_widget_hide((GtkWidget*)(mAwbRangeEg));
    gtk_widget_hide((GtkWidget*)(mAwbRangeEntry));
    gtk_widget_hide((GtkWidget*)(mAwbWpLabel));
    gtk_widget_hide((GtkWidget*)(mAwbWpEntry));
    gtk_widget_hide((GtkWidget*)(mWdrLevel));
    gtk_widget_hide((GtkWidget*)(mNoiseFilterLevel));
    gtk_widget_hide((GtkWidget*)(mSpatialLevel));
    gtk_widget_hide((GtkWidget*)(mTemporalLevel));
    gtk_widget_hide((GtkWidget*)(mNoiseFilter));
    gtk_widget_hide((GtkWidget*)(mSpatial));
    gtk_widget_hide((GtkWidget*)(mTemporal));
    gtk_widget_hide((GtkWidget*)(mColorLabel));
    gtk_widget_hide((GtkWidget*)(mColorRangeLabel));
    gtk_widget_hide((GtkWidget*)(mColorTansform));
    gtk_widget_hide((GtkWidget*)(mExposureTimeLabel));
    gtk_widget_hide((GtkWidget*)(mExposureGainLabel));
    gtk_widget_hide((GtkWidget*)(mExposureTimeLevel));
    gtk_widget_hide((GtkWidget*)(mExposureGainLevel));
    gtk_widget_hide((GtkWidget*)(mExposureTimeEg));
    gtk_widget_hide((GtkWidget*)(mExposureGainEg));
    gtk_combo_box_set_active_id((GtkComboBox *)mCameNum, "CameraNum1");

}

void GstCaffUI::applyDefaultSetting(int camId)
{
    mCamSetting[camId].updateUI = false;
    strcpy(mCamSetting[camId].source, "icamerasrc");
    strcpy(mCamSetting[camId].sink, "xvimagesink");
    strcpy(mCamSetting[camId].convert, "vaapipostproc");
    memset(mCamSetting[camId].statusInfo, 0, sizeof(mCamSetting[camId].statusInfo));
    strcpy(mCamSetting[camId].IOMode, "userptr");
    strcpy(mCamSetting[camId].format, "YUY2");
    mCamSetting[camId].interlaceMode = false;
    mCamSetting[camId].mipiCaptureMode = false;
    strcpy(mCamSetting[camId].reso, "640x480");
    strcpy(mCamSetting[camId].deInterlace, "none");
    strcpy(mCamSetting[camId].deviceName, "tpg");
    mCamSetting[camId].vcEnable = false;
    strcpy(mCamSetting[camId].IrisMode, "auto");
    mCamSetting[camId].irisLevel = 0;
    strcpy(mCamSetting[camId].aeMode, "auto");
    strcpy(mCamSetting[camId].fisheyemode,"off");
    strcpy(mCamSetting[camId].convergeSpeedMode, "aiq");
    strcpy(mCamSetting[camId].convergeSpeedLevel, "normal");
    mCamSetting[camId].exposureTime = 0;
    mCamSetting[camId].exposureGain = 0;
    mCamSetting[camId].exposureEv = 0;
    strcpy(mCamSetting[camId].awbMode, "auto");
    memset(mCamSetting[camId].awbRange, 0, 20);
    memset(mCamSetting[camId].awbWp, 0, 30);
    mCamSetting[camId].awbRGain = 0;
    mCamSetting[camId].awbGGain = 0;
    mCamSetting[camId].awbBGain = 0;
    mCamSetting[camId].awbRshift = 0;
    mCamSetting[camId].awbGshift = 0;
    mCamSetting[camId].awbBshift = 0;
    strcpy(mCamSetting[camId].regionMode, "none");

    mCamSetting[camId].windowSize = 0;
    strcpy(mCamSetting[camId].daynightMode, "auto");
    mCamSetting[camId].sharpness = 0;
    mCamSetting[camId].brightness = 0;
    mCamSetting[camId].contrast = 0;
    mCamSetting[camId].hue = 0;
    mCamSetting[camId].saturation = 0;
    strcpy(mCamSetting[camId].WdrMode, "auto");
    strcpy(mCamSetting[camId].BlcArea, "off");
    mCamSetting[camId].WdrLevel = 0;
    strcpy(mCamSetting[camId].noiseReductionMode, "off");
    mCamSetting[camId].noiseFilterLevel = 0;
    mCamSetting[camId].spatialLevel = 0;
    mCamSetting[camId].temporialLevel = 0;
    strcpy(mCamSetting[camId].mirrorEffectMode, "OFF");
    strcpy(mCamSetting[camId].sceneMode, "auto");
    strcpy(mCamSetting[camId].sensorResolution, "1080p");
    strcpy(mCamSetting[camId].captureFps, "25");
    strcpy(mCamSetting[camId].videoStandard, "PAL");
    strcpy(mCamSetting[camId].advanced, "None");
    mCamSetting[camId].saveLog = false;
}
void GstCaffUI::applySettingWithEnv(void)
{
    //TODO, do this in the future.
}

void GstCaffUI::getWidgetFromBuilder(GtkBuilder *builder)
{
    mWindow = GTK_WINDOW(gtk_builder_get_object (builder, "Main"));
    mMainFixed = GTK_FIXED(gtk_builder_get_object (builder, "MainPage"));
    mEnableClient = GTK_BUTTON(gtk_builder_get_object (builder, "ClientEnable"));
    mRemoteIp = GTK_ENTRY(gtk_builder_get_object (builder, "RemoteIPEntry"));
    mCameNum = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "ComboCameraNumber"));
    mCamIndex = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "ComboCameraIndex"));
    mStartAll = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "StartAll"));
    mStopAll = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "StopAll"));
    mSource = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "SourceCombox"));
    mSink = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "SinkCombox"));
    mConvert = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "ConvertCombox"));

    mStart = GTK_BUTTON(gtk_builder_get_object (builder, "StartButton"));
    mPause = GTK_BUTTON(gtk_builder_get_object (builder, "PauseButton"));
    mStop = GTK_BUTTON(gtk_builder_get_object (builder, "StopButton"));
    mRecord = GTK_BUTTON(gtk_builder_get_object (builder, "RecordButton"));
    mSnapshot = GTK_BUTTON(gtk_builder_get_object (builder, "SnapshotButton"));

    mCam0Status = GTK_STATUSBAR(gtk_builder_get_object (builder, "Cam0Status"));
    mCam1Status = GTK_STATUSBAR(gtk_builder_get_object (builder, "Cam1Status"));
    mCam2Status = GTK_STATUSBAR(gtk_builder_get_object (builder, "Cam2Status"));
    mCam3Status = GTK_STATUSBAR(gtk_builder_get_object (builder, "Cam3Status"));

    mCam0Area = GTK_DRAWING_AREA(gtk_builder_get_object (builder, "Camera0"));
    mCam1Area = GTK_DRAWING_AREA(gtk_builder_get_object (builder, "Camera1"));
    mCam2Area = GTK_DRAWING_AREA(gtk_builder_get_object (builder, "Camera2"));
    mCam3Area = GTK_DRAWING_AREA(gtk_builder_get_object (builder, "Camera3"));

    mIOMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "IOModeCombox"));
    mMipiCapture = GTK_CHECK_BUTTON(gtk_builder_get_object (builder, "MipiCapture"));
    mFormat = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "FmtCombox"));
    mInterlaceMode = GTK_CHECK_BUTTON(gtk_builder_get_object (builder, "InterlaceMode"));
    mResolution = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "ResolutionCombox"));
    mDeInterlace = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "DeinterlaceCombox"));
    mDeviceName = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "DeviceNameCombox"));
    mVcEnable = GTK_CHECK_BUTTON(gtk_builder_get_object (builder, "VirtualChannelCheck"));

    mIrisMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "IRISModeCombox"));
    mIrisLevel = GTK_SCALE(gtk_builder_get_object (builder, "IRISLevelScale"));
    mAeMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "AeModeCombox"));
    mFisheyeMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "FisheyeModecomboboxtext"));
    mExposureTimeEg = GTK_LABEL(gtk_builder_get_object (builder, "ExposureTimeEg"));
    mExposureGainEg = GTK_LABEL(gtk_builder_get_object (builder, "ExposureGainEg"));
    mExposureTimeLabel = GTK_LABEL(gtk_builder_get_object (builder, "ExposureTime"));
    mExposureGainLabel = GTK_LABEL(gtk_builder_get_object (builder, "ExposureGain"));
    mExposureTimeLevel = GTK_SPIN_BUTTON(gtk_builder_get_object (builder, "ExposureTimeSpinButton"));
    mExposureGainLevel = GTK_SPIN_BUTTON(gtk_builder_get_object (builder, "ExposureGainSpinButton"));
    mExposureEvLevel = GTK_SCALE(gtk_builder_get_object (builder, "ExposureEvScale"));
    mConvergeSpeedMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "ConvergeSpeedMode"));
    mConvergeSpeedLevel = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "ConvergeSpeedLevel"));

    mAwbMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "AwbModeCombox"));
    mAwbRangeLabel = GTK_LABEL(gtk_builder_get_object (builder, "AwbRangeLabel"));
    mAwbRangeEg = GTK_LABEL(gtk_builder_get_object (builder, "AwbRangeEg"));
    mAwbRangeEntry = GTK_ENTRY(gtk_builder_get_object (builder, "AwbRangeEntry"));
    mAwbWpLabel = GTK_LABEL(gtk_builder_get_object (builder, "CoordinateLabel"));
    mAwbWpEntry = GTK_ENTRY(gtk_builder_get_object (builder, "WPCoordinateEntry"));
    mAwbRGainLable = GTK_LABEL(gtk_builder_get_object (builder, "AwbRGainLabel"));
    mAwbGGainLable = GTK_LABEL(gtk_builder_get_object (builder, "AwbGGainLabel"));
    mAwbBGainLable = GTK_LABEL(gtk_builder_get_object (builder, "AwbBGainLabel"));
    mAwbRGainScale = GTK_SCALE(gtk_builder_get_object (builder, "AwbRGainScale"));
    mAwbGGainScale = GTK_SCALE(gtk_builder_get_object (builder, "AwbGGainScale"));
    mAwbBGainScale = GTK_SCALE(gtk_builder_get_object (builder, "AwbBGainScale"));
    mColorLabel = GTK_LABEL(gtk_builder_get_object (builder, "ColorTransformLabel"));
    mColorRangeLabel = GTK_LABEL(gtk_builder_get_object (builder, "ColorTransformRange"));
    mColorTansform = GTK_ENTRY(gtk_builder_get_object (builder, "ColorTransformEntry"));
    mAwbRShift = GTK_SCALE(gtk_builder_get_object (builder, "AwbShiftRScale"));
    mAwbGShift = GTK_SCALE(gtk_builder_get_object (builder, "AwbShiftGScale"));
    mAwbBShift = GTK_SCALE(gtk_builder_get_object (builder, "AwbShiftBScale"));

    mRegionMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "RegionCombox"));
    char regStr[20] = {0}, spinStr[20] = {0};
    for (int i; i < MAX_REGION_NUM; i++) {
        memset(regStr, 0, sizeof(regStr));
        memset(spinStr, 0, sizeof(spinStr));
        snprintf(regStr, 20, "Region%dEntry", i);
        snprintf(spinStr, 20, "Weight%dSpinbutton", i);
        mRegionEntry[i] = GTK_ENTRY(gtk_builder_get_object (builder, regStr));
        mWeightSpinbutton[i] = GTK_SPIN_BUTTON(gtk_builder_get_object (builder, spinStr));
    }
    mWindowSizeScale = GTK_SCALE(gtk_builder_get_object (builder, "WindowSizeScale"));

    mDayNightMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "DayNightCombox"));
    mSharpnessLevel = GTK_SCALE(gtk_builder_get_object (builder, "SharpnessScale"));
    mBrightnessLevel = GTK_SCALE(gtk_builder_get_object (builder, "BrightnessScale"));
    mContrastLevel = GTK_SCALE(gtk_builder_get_object (builder, "ContrastScale"));
    mHueLevel = GTK_SCALE(gtk_builder_get_object (builder, "HueScale"));
    mSaturationLevel = GTK_SCALE(gtk_builder_get_object (builder, "SaturationScale"));

    mWdrMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "WdrCombox"));
    mBlcArea = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "BlcAreaCombox"));
    mWdrLevel = GTK_SCALE(gtk_builder_get_object (builder, "WdrLevelScale"));
    mWdrLabel = GTK_LABEL(gtk_builder_get_object (builder, "WdrLevel"));

    mNoiseReductionMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "NoiseReductionCombox"));
    mNoiseFilterLevel = GTK_SCALE(gtk_builder_get_object (builder, "NoiseFilterScale"));
    mSpatialLevel = GTK_SCALE(gtk_builder_get_object (builder, "SpatialLevelScale"));
    mTemporalLevel = GTK_SCALE(gtk_builder_get_object (builder, "TemporalLevelScale"));
    mNoiseFilter = GTK_LABEL(gtk_builder_get_object (builder, "NoiseFilterLevel"));
    mSpatial = GTK_LABEL(gtk_builder_get_object (builder, "SpatialLevel"));
    mTemporal = GTK_LABEL(gtk_builder_get_object (builder, "TemporalLevel"));

    mMirrorEffect = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "MirrorCombox"));
    mSceneMode = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "SceneModeCombox"));
    mSensorReso = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "SensorResCombox"));
    mCaptureFps = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "CaptureFpsCombox"));
    mVideoStandard = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "VideoStandardCombox"));
    mAdvanced = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder, "mAdvancedCombox"));
    mSetCustomAic = GTK_BUTTON(gtk_builder_get_object (builder, "CustomAicParam"));
    mCustomAicParam = GTK_TEXT_VIEW(gtk_builder_get_object (builder, "CustomAICText"));

    mReset = GTK_BUTTON(gtk_builder_get_object (builder, "ResetSetting"));
    mRestart = GTK_BUTTON(gtk_builder_get_object (builder, "CameraReboot"));
    mLogLevel = GTK_ENTRY(gtk_builder_get_object (builder, "DebugLevel"));
    mDumpLevel = GTK_ENTRY(gtk_builder_get_object (builder, "DumpLevel"));
    mSaveLog = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "ButtonLogSave"));
    mLogText = GTK_TEXT_VIEW(gtk_builder_get_object (builder, "TextLogInfo"));
    mTestDual = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "TestForDual"));
    mTestVC = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "TestVcButton"));
    mCam0FrameNum = GTK_ENTRY(gtk_builder_get_object (builder, "FrameNumForCam0"));
    mCam0LoopTime = GTK_ENTRY(gtk_builder_get_object (builder, "LoopTimeForCam0"));
    mCam0Start = GTK_BUTTON(gtk_builder_get_object (builder, "StartForCam0"));
    mCam1FrameNum = GTK_ENTRY(gtk_builder_get_object (builder, "FrameNumForCam1"));
    mCam1LoopTime = GTK_ENTRY(gtk_builder_get_object (builder, "LoopTimeForCam1"));
    mCam1Start = GTK_BUTTON(gtk_builder_get_object (builder, "StartForCam1"));

    gtk_widget_set_can_focus ((GtkWidget*)mCam0Area, TRUE);
    gtk_widget_set_double_buffered ((GtkWidget*)mCam0Area, FALSE);
    gtk_widget_set_can_focus ((GtkWidget*)mCam1Area, TRUE);
    gtk_widget_set_double_buffered ((GtkWidget*)mCam1Area, FALSE);
    gtk_widget_set_can_focus ((GtkWidget*)mCam2Area, TRUE);
    gtk_widget_set_double_buffered ((GtkWidget*)mCam2Area, FALSE);
    gtk_widget_set_can_focus ((GtkWidget*)mCam3Area, TRUE);
    gtk_widget_set_double_buffered ((GtkWidget*)mCam3Area, FALSE);

}

void GstCaffUI::registerSignalForWidget(void)
{
    g_signal_connect(G_OBJECT(mWindow), "destroy",G_CALLBACK(GstCaffUI::mainWinowQuitCallback), this);
    g_signal_connect(G_OBJECT(mEnableClient), "clicked", G_CALLBACK(GstCaffUI::enableClientCallback), this);
    g_signal_connect(G_OBJECT(mCameNum), "changed", G_CALLBACK(GstCaffUI::cameraNumChangedCallback), this);
    g_signal_connect(G_OBJECT(mCamIndex), "changed", G_CALLBACK(GstCaffUI::cameraIndexChangedCallback), this);
    g_signal_connect(G_OBJECT(mStartAll), "toggled", G_CALLBACK(GstCaffUI::startAllCameraCallback), this);
    g_signal_connect(G_OBJECT(mStopAll), "toggled", G_CALLBACK(GstCaffUI::stopAllCameraCallback), this);
    g_signal_connect(G_OBJECT(mSource), "changed", G_CALLBACK(GstCaffUI::sourceElementChangedCallback), this);
    g_signal_connect(G_OBJECT(mSink), "changed", G_CALLBACK(GstCaffUI::sinkElementChangedCallback), this);
    g_signal_connect(G_OBJECT(mConvert), "changed", G_CALLBACK(GstCaffUI::convertElementChangedCallback), this);


    g_signal_connect(G_OBJECT(mStart), "clicked", G_CALLBACK(GstCaffUI::cameraModeChangeCallback), this);
    g_signal_connect(G_OBJECT(mPause), "clicked", G_CALLBACK(GstCaffUI::cameraModeChangeCallback), this);
    g_signal_connect(G_OBJECT(mStop), "clicked", G_CALLBACK(GstCaffUI::cameraModeChangeCallback), this);
    g_signal_connect(G_OBJECT(mRecord), "clicked", G_CALLBACK(GstCaffUI::cameraModeChangeCallback), this);
    g_signal_connect(G_OBJECT(mSnapshot), "clicked", G_CALLBACK(GstCaffUI::cameraModeChangeCallback), this);

    g_signal_connect(G_OBJECT(mCam0Area), "realize", G_CALLBACK(GstCaffUI::drawAreaRealizeCallback), this);
    g_signal_connect (G_OBJECT(mCam0Area), "draw", G_CALLBACK (GstCaffUI::drawAreaDrawCallback), this);
    g_signal_connect (G_OBJECT(mCam0Area), "button_press_event", G_CALLBACK (GstCaffUI::drawAreaPressCallback), this);
    g_signal_connect(G_OBJECT(mCam1Area), "realize", G_CALLBACK(GstCaffUI::drawAreaRealizeCallback), this);
    g_signal_connect (G_OBJECT(mCam1Area), "draw", G_CALLBACK (GstCaffUI::drawAreaDrawCallback), this);
    g_signal_connect (G_OBJECT(mCam1Area), "button_press_event", G_CALLBACK (GstCaffUI::drawAreaPressCallback), this);
    g_signal_connect(G_OBJECT(mCam2Area), "realize", G_CALLBACK(GstCaffUI::drawAreaRealizeCallback), this);
    g_signal_connect (G_OBJECT(mCam2Area), "draw", G_CALLBACK (GstCaffUI::drawAreaDrawCallback), this);
    g_signal_connect (G_OBJECT(mCam2Area), "button_press_event", G_CALLBACK (GstCaffUI::drawAreaPressCallback), this);
    g_signal_connect(G_OBJECT(mCam3Area), "realize", G_CALLBACK(GstCaffUI::drawAreaRealizeCallback), this);
    g_signal_connect (G_OBJECT(mCam3Area), "draw", G_CALLBACK (GstCaffUI::drawAreaDrawCallback), this);
    g_signal_connect (G_OBJECT(mCam3Area), "button_press_event", G_CALLBACK (GstCaffUI::drawAreaPressCallback), this);
    gtk_widget_set_events((GtkWidget *)mCam0Area , GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_events((GtkWidget *)mCam1Area , GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_events((GtkWidget *)mCam2Area , GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_events((GtkWidget *)mCam3Area , GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(mIOMode), "changed", G_CALLBACK(GstCaffUI::IOModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mMipiCapture), "toggled", G_CALLBACK(GstCaffUI::enableMipiCaptureCallback), this);
    g_signal_connect(G_OBJECT(mFormat), "changed", G_CALLBACK(GstCaffUI::formatChangedCallback), this);
    g_signal_connect(G_OBJECT(mInterlaceMode), "toggled", G_CALLBACK(GstCaffUI::enableInterlaceCallback), this);
    g_signal_connect(G_OBJECT(mResolution), "changed", G_CALLBACK(GstCaffUI::resolutionChangedCallback), this);
    g_signal_connect(G_OBJECT(mDeInterlace), "changed", G_CALLBACK(GstCaffUI::deInterlaceChangedCallback), this);
    g_signal_connect(G_OBJECT(mDeviceName), "changed", G_CALLBACK(GstCaffUI::deviceChangedCallback), this);
    g_signal_connect(G_OBJECT(mVcEnable), "toggled", G_CALLBACK(GstCaffUI::enableVirtualChannelCallback), this);

    //register callback function for AE.
    g_signal_connect(G_OBJECT(mIrisMode), "changed", G_CALLBACK(GstCaffUI::IRISModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mIrisLevel), "value-changed", G_CALLBACK(GstCaffUI::setIrisLevelCallback), this);
    g_signal_connect(G_OBJECT(mAeMode), "changed", G_CALLBACK(GstCaffUI::aeModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mFisheyeMode), "changed", G_CALLBACK(GstCaffUI::fisheyemodechangecallback), this);
    g_signal_connect(G_OBJECT(mExposureTimeLevel), "change-value", G_CALLBACK(GstCaffUI::exposureTimeChangeCallback), this);
    g_signal_connect(G_OBJECT(mExposureTimeLevel), "activate", G_CALLBACK(GstCaffUI::setExposureTimeCallback), this);
    g_signal_connect(G_OBJECT(mExposureGainLevel), "activate", G_CALLBACK(GstCaffUI::setExposureGainCallback), this);
    g_signal_connect(G_OBJECT(mExposureEvLevel), "value-changed", G_CALLBACK(GstCaffUI::setExposureEvCallback), this);
    g_signal_connect(G_OBJECT(mConvergeSpeedLevel), "changed", G_CALLBACK(GstCaffUI::convergeSpeedLevelChangedCallback), this);
    g_signal_connect(G_OBJECT(mConvergeSpeedMode), "changed", G_CALLBACK(GstCaffUI::convergeSpeedModeChangedCallback), this);

    //register callback function for AWB
    g_signal_connect(G_OBJECT(mAwbMode), "changed", G_CALLBACK(GstCaffUI::awbModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mAwbRangeEntry), "activate", G_CALLBACK(GstCaffUI::setAwbRangeCallback), this);
    g_signal_connect(G_OBJECT(mAwbRGainScale), "value-changed", G_CALLBACK(GstCaffUI::awbRGainChangedCallback), this);
    g_signal_connect(G_OBJECT(mAwbGGainScale), "value-changed", G_CALLBACK(GstCaffUI::awbGGainChangedCallback), this);
    g_signal_connect(G_OBJECT(mAwbBGainScale), "value-changed", G_CALLBACK(GstCaffUI::awbBGainChangedCallback), this);
    g_signal_connect(G_OBJECT(mColorTansform), "activate", G_CALLBACK(GstCaffUI::awbTransformCallback), this);
    g_signal_connect(G_OBJECT(mAwbRShift), "value-changed", G_CALLBACK(GstCaffUI::setAwbRshiftCallback), this);
    g_signal_connect(G_OBJECT(mAwbGShift), "value-changed", G_CALLBACK(GstCaffUI::setAwbGshiftCallback), this);
    g_signal_connect(G_OBJECT(mAwbBShift), "value-changed", G_CALLBACK(GstCaffUI::setAwbBshiftCallback), this);

    //register callback function for region
    g_signal_connect(G_OBJECT(mRegionMode), "changed", G_CALLBACK(GstCaffUI::regionModeChangedCallback), this);
    for (int i = 0; i < MAX_REGION_NUM; i++) {
        g_signal_connect(G_OBJECT(mWeightSpinbutton[i]), "change-value", G_CALLBACK(GstCaffUI::weightChangedCallback), this);
        g_signal_connect(G_OBJECT(mWeightSpinbutton[i]), "activate", G_CALLBACK(GstCaffUI::setRegionWeightCallback), this);
    }

    g_signal_connect(G_OBJECT(mWindowSizeScale), "value-changed", G_CALLBACK(GstCaffUI::setWindowSizeCallback), this);
    g_signal_connect(G_OBJECT(mDayNightMode), "changed", G_CALLBACK(GstCaffUI::dayNightModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mSharpnessLevel), "value-changed", G_CALLBACK(GstCaffUI::setSharpnessCallback), this);
    g_signal_connect(G_OBJECT(mBrightnessLevel), "value-changed", G_CALLBACK(GstCaffUI::setBrightnessCallback), this);
    g_signal_connect(G_OBJECT(mContrastLevel), "value-changed", G_CALLBACK(GstCaffUI::setContrastCallback), this);
    g_signal_connect(G_OBJECT(mHueLevel), "value-changed", G_CALLBACK(GstCaffUI::setHueCallback), this);
    g_signal_connect(G_OBJECT(mSaturationLevel), "value-changed", G_CALLBACK(GstCaffUI::setSaturationCallback), this);

    g_signal_connect(G_OBJECT(mWdrMode), "changed", G_CALLBACK(GstCaffUI::WDRModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mBlcArea), "changed", G_CALLBACK(GstCaffUI::BlcAreaChangedCallback), this);
    g_signal_connect(G_OBJECT(mWdrLevel), "value-changed", G_CALLBACK(GstCaffUI::setWdrLevelCallback), this);

    g_signal_connect(G_OBJECT(mNoiseReductionMode), "changed", G_CALLBACK(GstCaffUI::NoiseModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mNoiseFilterLevel), "value-changed", G_CALLBACK(GstCaffUI::setNoiseFilterCallback), this);
    g_signal_connect(G_OBJECT(mSpatialLevel), "value-changed", G_CALLBACK(GstCaffUI::setSpatialCallback), this);
    g_signal_connect(G_OBJECT(mTemporalLevel), "value-changed", G_CALLBACK(GstCaffUI::setTemporalCallback), this);

    g_signal_connect(G_OBJECT(mMirrorEffect), "changed", G_CALLBACK(GstCaffUI::mirrorEffectChangedCallback), this);
    g_signal_connect(G_OBJECT(mSceneMode), "changed", G_CALLBACK(GstCaffUI::sceneModeChangedCallback), this);
    g_signal_connect(G_OBJECT(mSensorReso), "changed", G_CALLBACK(GstCaffUI::sensorResolutionChangedCallback), this);
    g_signal_connect(G_OBJECT(mCaptureFps), "changed", G_CALLBACK(GstCaffUI::captureFpsChangedCallback), this);
    g_signal_connect(G_OBJECT(mVideoStandard), "changed", G_CALLBACK(GstCaffUI::videoStandardChangedCallback), this);

    g_signal_connect(G_OBJECT(mAdvanced), "changed", G_CALLBACK(GstCaffUI::advanceFeatureChangedCallback), this);
    g_signal_connect(G_OBJECT(mSetCustomAic), "clicked", G_CALLBACK(GstCaffUI::setCustomAICCallback), this);
    g_signal_connect(G_OBJECT(mReset), "clicked", G_CALLBACK(GstCaffUI::resetSettingCallback), this);
    g_signal_connect(G_OBJECT(mRestart), "clicked", G_CALLBACK(GstCaffUI::restartCameraCallback), this);

    g_signal_connect(G_OBJECT(mLogLevel), "activate", G_CALLBACK(GstCaffUI::setDebugLevelCallback), this);
    g_signal_connect(G_OBJECT(mDumpLevel), "activate", G_CALLBACK(GstCaffUI::setDumpLevelCallback), this);
    g_signal_connect(G_OBJECT(mSaveLog), "toggled", G_CALLBACK(GstCaffUI::saveLogButtonCallback), this);

    g_signal_connect(G_OBJECT(mTestDual), "clicked", G_CALLBACK(GstCaffUI::testDualButtonCallback), this);
    g_signal_connect(G_OBJECT(mTestVC), "clicked", G_CALLBACK(GstCaffUI::testVcButtonCallback), this);
    g_signal_connect(G_OBJECT(mCam0FrameNum), "activate", G_CALLBACK(GstCaffUI::setCamFrameNumCallback), this);
    g_signal_connect(G_OBJECT(mCam0LoopTime), "activate", G_CALLBACK(GstCaffUI::setCamLoopTimeCallback), this);
    g_signal_connect(G_OBJECT(mCam0Start), "clicked", G_CALLBACK(GstCaffUI::startCamButtonCallback), this);
    g_signal_connect(G_OBJECT(mCam1FrameNum), "activate", G_CALLBACK(GstCaffUI::setCamFrameNumCallback), this);
    g_signal_connect(G_OBJECT(mCam1LoopTime), "activate", G_CALLBACK(GstCaffUI::setCamLoopTimeCallback), this);
    g_signal_connect(G_OBJECT(mCam1Start), "clicked", G_CALLBACK(GstCaffUI::startCamButtonCallback), this);
}

void GstCaffUI::testDualButtonCallback(GtkWidget *widget, gpointer data)
{
    char itemId[20] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;

    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        printf("exit test dual mode, reset all settings\n");
        cbData->mTestRunning = false;
        //reset all settings
        for (int i = 0; i < MAX_CAMERA_NUM; i++) {
            snprintf(itemId, 20, "CameraId%d", i);
            gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCamIndex, itemId);
            gtk_button_clicked (cbData->mStop);
            cbData->applyDefaultSetting(i);
            cbData->updateUIforCamera(i, false);
        }
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCameNum, "CameraNum1");
        cbData->mEventControl->setTestDualMode(false);

        return;
    }

    cbData->mTestRunning = true;
    cbData->mEventControl->setTestDualMode(true);
    printf("enter test dual mode, set default setting\n");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCameNum, "CameraNum2");

    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCamIndex, "CameraId0");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mSource, "icamerasrc");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mConvert, "videoconvert");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mSink, "ximagesink");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mResolution, "Reso1920x1080");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mFormat, "FmtUYVY");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mDeviceName, "Device-mondello");

    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCamIndex, "CameraId1");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mSource, "icamerasrc");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mConvert, "videoconvert");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mSink, "ximagesink");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mResolution, "Reso1920x1080");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mFormat, "FmtUYVY");
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mDeviceName, "Device-mondello-2");

    for(int i = 0; i < MAX_CAMERA_NUM; i++) {
        cbData->mCamSetting[i].frameNum = 0;
        cbData->mCamSetting[i].loopTime = 0;
    }
}

void GstCaffUI::testVcButtonCallback(GtkWidget *widget, gpointer data)
{
    char indexItemId[20] = {0};
    char nameItemId[60] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;

    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        printf("exit test VC function, reset all settings\n");
        //reset all settings
        for (int i = 0; i < MAX_CAMERA_NUM; i++) {
            snprintf(indexItemId, 20, "CameraId%d", i);
            gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCamIndex, indexItemId);
            gtk_button_clicked (cbData->mStop);
            cbData->applyDefaultSetting(i);
            cbData->updateUIforCamera(i, false);
        }
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCameNum, "CameraNum1");

        return;
    }

    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCameNum, "CameraNum4");
    for(int i = 0; i < MAX_CAMERA_NUM; i++) {
        snprintf(indexItemId, 20, "CameraId%d", i);
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCamIndex, indexItemId);
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mSource, "icamerasrc");
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mConvert, "videoconvert");
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mSink, "ximagesink");
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mFormat, "FmtUYVY");
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mResolution, "Reso640x480");
        snprintf(nameItemId, 60, "Device-ov10635-vc");
        if (i > 0)
            snprintf(nameItemId, 60, "Device-ov10635-vc-%d", i+1);
        gtk_combo_box_set_active_id((GtkComboBox *)cbData->mDeviceName, nameItemId);
        gtk_toggle_button_set_active((GtkToggleButton*)cbData->mVcEnable, true);

    }
}

void GstCaffUI::setCamFrameNumCallback(GtkWidget *widget, gpointer data)
{
    char *str = NULL;
    int camId = -1;
    int value = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;

    if (widget == (GtkWidget*)cbData->mCam0FrameNum)
        camId = 0;
    else if (widget == (GtkWidget*)cbData->mCam1FrameNum)
        camId = 1;
    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        value = atoi(str);
        if(cbData->clientMode) {
            printf("this feature isn't supported in client mode \n");
        } else {
            printf("change num-buffers to %d, camId: %d\n", value, camId);
            cbData->mCamSetting[camId].frameNum = value;
            cbData->mEventControl->setNumBuffers(camId, value);
        }
    }
    return;
}

void GstCaffUI::setCamLoopTimeCallback(GtkWidget *widget, gpointer data)
{
    int camId = -1;
    char *str = NULL;
    int loopTime;
    GstCaffUI *cbData = (GstCaffUI *)data;

    if (widget == (GtkWidget*)cbData->mCam0LoopTime)
        camId = 0;
    else if (widget == (GtkWidget*)cbData->mCam1LoopTime)
        camId = 1;
    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        loopTime = atoi(str);
        cbData->mCamSetting[camId].loopTime = loopTime;
        printf("set the cameraId: %d, loopTime: %d\n", camId, loopTime);
    }

    return;
}

void GstCaffUI::startCamButtonCallback(GtkWidget *widget, gpointer data)
{
    int camId = -1;
    char itemId[20] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;

    if (widget == (GtkWidget*)cbData->mCam0Start)
        camId = 0;
    else if (widget == (GtkWidget*)cbData->mCam1Start)
        camId = 1;

    snprintf(itemId, 20, "CameraId%d", camId);
    gtk_combo_box_set_active_id((GtkComboBox *)cbData->mCamIndex, itemId);
    gtk_button_clicked (cbData->mStart);

    return;
}

void GstCaffUI::enableClientCallback(GtkWidget *widget, gpointer data)
{
    bool validIP = true;
    int section = 0, dot = 0;
    char *ipAddrPtr = NULL, *tmpPtr = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    g_print("enter %s\n", __func__);

    tmpPtr = ipAddrPtr = (char *)gtk_entry_get_text((GtkEntry *)(cbData->mRemoteIp));
    while (*tmpPtr) {
        if (*tmpPtr == '.') {
            dot++;
            if (section > 255) {
                validIP = false;
                break;
            } else if (section > 0)
                section = 0;
        } else if (*tmpPtr >= '0' && *tmpPtr <= '9') {
            section = section * 10 + (*tmpPtr - '0');
        } else {
            validIP = false;
            break;
        }
        tmpPtr++;
    }
    if (dot != 3)
        validIP = false;

    if (!validIP)
        printf("the IP addres: %s is INVALID\n", ipAddrPtr);
    else {
        cbData->mRemoteClient->connectToServer(ipAddrPtr);
        cbData->clientMode = 1;
        //reset the camera setting when connect to server.
        for (int i = 0; i < MAX_CAMERA_NUM; i++) {
            //cbData->mRemoteClient->setCmd2RemoteClient(i, "source=");
            //cbData->mRemoteClient->setCmd2RemoteClient(i, "sink=");
            //cbData->mRemoteClient->setCmd2RemoteClient(i, "convert=");
            cbData->mRemoteClient->setCmd2RemoteClient(i, (char*)"type=remote");
            cbData->mRemoteClient->setCmd2RemoteClient(i, (char*)"resolution=640x480");
            cbData->mRemoteClient->setCmd2RemoteClient(i, (char*)"format=YUY2");
            cbData->mRemoteClient->setCmd2RemoteClient(i, (char*)"interlace=false");
            cbData->mRemoteClient->setCmd2RemoteClient(i, (char*)"deinterlace=none");
        }
    }
}

void GstCaffUI::cameraNumChangedCallback(GtkWidget *widget, gpointer data)
{
    int camNum = 0;
    char *camNumPtr = NULL;
    char camIndexItem[30] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;

    camNumPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if (!camNumPtr)
        return;

    camNum = atoi(camNumPtr);
    switch (camNum) {
        case 1:
            cbData->mCamNumber = 1;
            gtk_widget_show((GtkWidget*)(cbData->mCam0Area));
            gtk_widget_hide((GtkWidget*)(cbData->mCam1Area));
            gtk_widget_hide((GtkWidget*)(cbData->mCam2Area));
            gtk_widget_hide((GtkWidget*)(cbData->mCam3Area));
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 0, 0);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 0, 0);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 0, 0);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 1440, 960);
            gtk_combo_box_text_remove_all((GtkComboBoxText*)cbData->mCamIndex);
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId0", "0");
            break;
        case 2:
            cbData->mCamNumber = 2;
            gtk_widget_show((GtkWidget*)(cbData->mCam0Area));
            gtk_widget_show((GtkWidget*)(cbData->mCam1Area));
            gtk_widget_hide((GtkWidget*)(cbData->mCam2Area));
            gtk_widget_hide((GtkWidget*)(cbData->mCam3Area));
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 720, 960);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam1Area, 720, 960);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam2Area, 0, 0);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam3Area, 0, 0);
            gtk_combo_box_text_remove_all((GtkComboBoxText*)cbData->mCamIndex);
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId0", "0");
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId1", "1");
            break;
        case 3:
            cbData->mCamNumber = 3;
            gtk_widget_show((GtkWidget*)(cbData->mCam0Area));
            gtk_widget_show((GtkWidget*)(cbData->mCam1Area));
            gtk_widget_show((GtkWidget*)(cbData->mCam2Area));
            gtk_widget_hide((GtkWidget *)(cbData->mCam3Area));
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 720, 480);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam1Area, 720, 960);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam2Area, 720, 480);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam3Area, 0, 0);
            gtk_combo_box_text_remove_all((GtkComboBoxText*)cbData->mCamIndex);
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId0", "0");
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId1", "1");
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId2", "2");
            break;
        case 4:
            cbData->mCamNumber = 4;
            gtk_widget_show((GtkWidget*)(cbData->mCam0Area));
            gtk_widget_show((GtkWidget*)(cbData->mCam1Area));
            gtk_widget_show((GtkWidget*)(cbData->mCam2Area));
            gtk_widget_show((GtkWidget*)(cbData->mCam3Area));
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam0Area, 720, 480);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam1Area, 720, 480);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam2Area, 720, 480);
            gtk_widget_set_size_request ((GtkWidget*)cbData->mCam3Area, 720, 480);
            gtk_combo_box_text_remove_all((GtkComboBoxText*)cbData->mCamIndex);
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId0", "0");
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId1", "1");
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId2", "2");
            gtk_combo_box_text_append((GtkComboBoxText*)cbData->mCamIndex, "CameraId3", "3");
        default:
            break;
    }

    //set the default cameraId to 0
    gtk_combo_box_set_active_id((GtkComboBox*)cbData->mCamIndex, "CameraId0");

}

void GstCaffUI::updateSettingSource(char *source)
{
    printf("enter %s, source element: %s\n", __func__, source);
    gtk_combo_box_set_active_id((GtkComboBox *)mSource, source);
}

void GstCaffUI::updateSettingSink(char *sink)
{
    printf("enter %s, sink element: %s\n", __func__, sink);
    gtk_combo_box_set_active_id((GtkComboBox *)mSink, sink);
}

void GstCaffUI::updateSettingConvert(char *convert)
{
    printf("enter %s, convert element: %s\n", __func__, convert);
    gtk_combo_box_set_active_id((GtkComboBox *)mConvert, convert);
}

void GstCaffUI::updateSettingIOMode(char *ioMode)
{
    char itemId[30] = {0};
    printf("enter %s, io-mode: %s\n", __func__, ioMode);
    snprintf(itemId, 30, "IO%s", ioMode);
    gtk_combo_box_set_active_id((GtkComboBox *)mIOMode, itemId);
}

void GstCaffUI::updateSettingFmt(char *fmt)
{
    char itemId[30] = {0};
    printf("enter %s, format: %s\n", __func__, fmt);
    snprintf(itemId, 30, "Fmt%s", fmt);
    gtk_combo_box_set_active_id((GtkComboBox *)mFormat, itemId);
}

void GstCaffUI::updateSettingInterlaceMode(bool interlaceMode)
{
    printf("enter %s, interlaceMode: %s\n", __func__, interlaceMode ? "true" : "false");
    gtk_toggle_button_set_active((GtkToggleButton*)mInterlaceMode, interlaceMode);
}

void GstCaffUI::updateSettingVcEnable(bool enable)
{
    printf("enter %s, enable VC: %s\n", __func__, enable ? "true" : "false");
    gtk_toggle_button_set_active((GtkToggleButton*)mVcEnable, enable);
}

void GstCaffUI::updateSettingMipiCaptureMode(bool mode)
{
    printf("enter %s, mipiCaptureMode: %s\n", __func__, mode ? "true" : "false");
    gtk_toggle_button_set_active((GtkToggleButton*)mMipiCapture, mode);
}

void GstCaffUI::updateSettingResolution(char *reso)
{
    char itemId[30] = {0};
    printf("enter %s, Resolution: %s\n", __func__, reso);
    snprintf(itemId, 30, "Reso%s", reso);
    gtk_combo_box_set_active_id((GtkComboBox*)mResolution, itemId);
}

void GstCaffUI::updateSettingDeinterlace(char *deinterlace)
{
    const char *itemId = NULL;
    printf("enter %s, deinterlace: %s\n", __func__, deinterlace);
    if (strcmp(deinterlace, "sw_bob") == 0)
        itemId = "DeinterlaceSw";
    else if (strcmp(deinterlace, "hw_bob") == 0)
        itemId = "DeinterlaceHw";
    else
        itemId = "DeinterlaceNone";
    gtk_combo_box_set_active_id((GtkComboBox *)mDeInterlace, itemId);
}

void GstCaffUI::updateSettingDeviceName(char *name)
{
    char itemId[100] = {0};
    printf("enter %s, device name: %s\n", __func__, name);
    snprintf(itemId, 100, "Device-%s", name);
    gtk_combo_box_set_active_id((GtkComboBox *)mDeviceName, itemId);
}

void GstCaffUI::updateSettingIRISMode(char *iris)
{
    char itemId[30] = {0};
    printf("enter %s, IRIS-mode: %s\n", __func__, iris);
    snprintf(itemId, 30, "IRIS%s", iris);
    gtk_combo_box_set_active_id((GtkComboBox *)mIrisMode, itemId);
}

void GstCaffUI::updateSettingIrisLevel(int level)
{
    printf("enter %s, Iris-level: %d\n", __func__, level);
    gtk_range_set_value((GtkRange*)mIrisLevel, (double)level);
}

void GstCaffUI::updateSettingAeMode(char *mode)
{
    char itemId[30] = {0};
    printf("enter %s, ae-mode: %s\n", __func__, mode);
    snprintf(itemId, 30, "AeMode-%s", mode);
    gtk_combo_box_set_active_id((GtkComboBox *)mAeMode, itemId);
}
void GstCaffUI::updateSettingFisheyeMode(char *mode)
{
    char itemId[30] = {0};
    printf("enter %s, fisheye-mode: %s\n", __func__, mode);
    snprintf(itemId, 30, "fisheyemode-%s", mode);
    gtk_combo_box_set_active_id((GtkComboBox *)mFisheyeMode, itemId);
}
void GstCaffUI::updateSettingConvergeSpeedMode(char *value)
{
    char itemId[30] = {0};
    snprintf(itemId, 30, "From%s", value);
    printf("enter %s, converge-speed-mode: %s\n", __func__, itemId);
    gtk_combo_box_set_active_id((GtkComboBox *)mConvergeSpeedMode, itemId);
}

void GstCaffUI::updateSettingConvergeSpeedLevel(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, converge-speed-level: %s\n", __func__, value);
    gtk_combo_box_set_active_id((GtkComboBox *)mConvergeSpeedLevel, value);
}

void GstCaffUI::updateSettingExposureTime(int exposureTime)
{
    char exposureTimeStr[10];
    snprintf(exposureTimeStr, 10, "%d", exposureTime);
    printf("enter %s, ExposureTime: %s\n", __func__, exposureTimeStr);
    gtk_entry_set_text((GtkEntry *)(mExposureTimeLevel), exposureTimeStr);
}

void GstCaffUI::updateSettingExposureGain(int gain)
{
    char gainStr[10];
    snprintf(gainStr, 10, "%d", gain);
    printf("enter %s, ExposureGain: %s\n", __func__, gainStr);
    gtk_entry_set_text((GtkEntry *)(mExposureGainLevel), gainStr);
}

void GstCaffUI::updateSettingExposureEv(int ev)
{
    printf("enter %s, ExposureEv: %d\n", __func__, ev);
    gtk_range_set_value((GtkRange*)mExposureEvLevel, (double)ev);
}

void GstCaffUI::updateSettingAwbMode(char *mode)
{
    char itemId[30] = {0};
    char *awbModePtr = NULL;
    printf("enter %s, AWB-mode: %s\n", __func__, mode);
    snprintf(itemId, 30, "Awb%s", mode);
    gtk_combo_box_set_active_id((GtkComboBox *)mAwbMode, itemId);
}

void GstCaffUI::updateSettingAwbRange(char *value)
{
    printf("enter %s, Awb-Range: %s\n", __func__, value);
    gtk_entry_set_text((GtkEntry *)mAwbRangeEntry, value);
}

void GstCaffUI::updateSettingAwbWp(char *position)
{
    printf("enter %s, Awb-whitePoint: %s\n", __func__, position);
    gtk_entry_set_text((GtkEntry *)mAwbWpEntry, position);
}

void GstCaffUI::updateSettingAwbGainR(int value)
{
    printf("enter %s, Awb-Gain-R: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mAwbRGainScale, (double)value);
}

void GstCaffUI::updateSettingAwbGainG(int value)
{
    printf("enter %s, Awb-Gain-G: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mAwbGGainScale, (double)value);
}

void GstCaffUI::updateSettingAwbGainB(int value)
{
    printf("enter %s, Awb-Gain-B: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mAwbBGainScale, (double)value);
}

void GstCaffUI::updateSettingAwbShiftR(int value)
{
    printf("enter %s, Awb-ShiftR: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mAwbRShift, (double)value);
}

void GstCaffUI::updateSettingAwbShiftG(int value)
{
    printf("enter %s, Awb-ShiftG: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mAwbGShift, (double)value);
}

void GstCaffUI::updateSettingAwbShiftB(int value)
{
    printf("enter %s, Awb-ShiftB: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mAwbBShift, (double)value);
}

void GstCaffUI::updateSettingWindowSize(int size)
{
    printf("enter %s, WindowSize: %d\n", __func__, size);
    gtk_range_set_value((GtkRange*)mWindowSizeScale, (double)size);
}

void GstCaffUI::updateSettingRegionMode(char *mode)
{
    char itemId[30] = {0};
    snprintf(itemId, 30, "%s", mode);
    printf("enter %s, region-Mode: %s\n", __func__, itemId);
    gtk_combo_box_set_active_id((GtkComboBox *)mRegionMode, itemId);
}

void GstCaffUI::updateSettingDaynightMode(char *value)
{
    const char *itemId = NULL;
    printf("enter %s, Daynight-mode: %s\n", __func__, value);
    if (strcmp(value, "day-mode") == 0)
        itemId = "DayNight-Day";
    else if (strcmp(value, "night-mode") == 0)
        itemId = "DayNight-Night";
    else
        itemId = "DayNight-Auto";
    gtk_combo_box_set_active_id((GtkComboBox *)mDayNightMode, itemId);
}

void GstCaffUI::updateSettingSharpness(int value)
{
    printf("enter %s, Sharpness: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mSharpnessLevel, (double)value);
}

void GstCaffUI::updateSettingBrightness(int value)
{
    printf("enter %s, Brightness: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mBrightnessLevel, (double)value);
}

void GstCaffUI::updateSettingContrast(int value)
{
    printf("enter %s, Contrast: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mContrastLevel, (double)value);
}

void GstCaffUI::updateSettingHue(int value)
{
    printf("enter %s, Hue: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mHueLevel, (double)value);
}

void GstCaffUI::updateSettingSaturation(int value)
{
    printf("enter %s, Saturation: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mSaturationLevel, (double)value);
}

void GstCaffUI::updateSettingWDRMode(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, WDR-mode: %s\n", __func__, value);
    snprintf(itemId, 30, "Wdr%s", value);
    gtk_combo_box_set_active_id((GtkComboBox *)mWdrMode, itemId);
}

void GstCaffUI::updateSettingBlcArea(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, BLC-Area: %s\n", __func__, value);
    snprintf(itemId, 30, "Blc%s", value);
    gtk_combo_box_set_active_id((GtkComboBox *)mBlcArea, itemId);
}

void GstCaffUI::updateSettingWdrValue(int value)
{
    printf("enter %s, WDR-level: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mWdrLevel, (double)value);
}

void GstCaffUI::updateSettingNoiseReductionMode(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, Noise-Reduction: %s\n", __func__, value);
    snprintf(itemId, 30, "NoiseReduction%s", value);
    gtk_combo_box_set_active_id((GtkComboBox *)mNoiseReductionMode, itemId);
}

void GstCaffUI::updateSettingNoiseFilter(int value)
{
    printf("enter %s, Noise-Filter: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mNoiseFilterLevel, (double)value);
}

void GstCaffUI::updateSettingSpatial(int value)
{
    printf("enter %s, Spatial-level: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mSpatialLevel, (double)value);
}

void GstCaffUI::updateSettingTemporal(int value)
{
    printf("enter %s, Temporal-level: %d\n", __func__, value);
    gtk_range_set_value((GtkRange*)mTemporalLevel, (double)value);
}

void GstCaffUI::updateSettingMirrorEffect(char *value)
{
    const char *itemId = NULL;
    printf("enter %s, MirrorEffect: %s\n", __func__, value);
    if (strcmp(value, "OFF") == 0)
        itemId = "MirrorOFF";
    else if (strcmp(value, "Up/Down") == 0)
        itemId = "MirrorUp-Down";
    else if (strcmp(value, "Left/Right"))
        itemId = "MirrorLeft-Right";
    else
        itemId = "MirrorCenter";
    gtk_combo_box_set_active_id((GtkComboBox *)mMirrorEffect, itemId);
}

void GstCaffUI::updateSettingSceneMode(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, SceneMode: %s\n", __func__, value);
    snprintf(itemId, 30, "SceneMode%s", value);
    gtk_combo_box_set_active_id((GtkComboBox *)mSceneMode, itemId);
}

void GstCaffUI::updateSettingSensorReso(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, SensorReso: %s\n", __func__, value);
    snprintf(itemId, 30, "Reso%s", value);
    gtk_combo_box_set_active_id((GtkComboBox *)mSensorReso, itemId);
}

void GstCaffUI::updateSettingCaptureFps(char *value)
{
    const char *itemId = NULL;
    printf("enter %s, CaptureFps: %s\n", __func__, value);
    if (strcmp(value, "25") == 0)
        itemId = "CaptureFps25";
    else if (strcmp(value, "30") == 0)
        itemId = "CaptureFps30";
    else
        itemId = "CaptureFps50";
    gtk_combo_box_set_active_id((GtkComboBox *)mCaptureFps, itemId);
}

void GstCaffUI::updateSettingVideoStandard(char *value)
{
    const char *itemId = NULL;
    printf("enter %s, VideoStandard: %s\n", __func__, value);
    if (strcmp(value, "PAL") == 0)
        itemId = "VS-50Hz";
    else
        itemId = "VS-60Hz";
    gtk_combo_box_set_active_id((GtkComboBox *)mVideoStandard, itemId);
}

void GstCaffUI::updateSettingAdvancedFeature(char *value)
{
    char itemId[30] = {0};
    printf("enter %s, Advanced-feature: %s\n", __func__, value);
    snprintf(itemId, 30, "Advance%s", value);
    gtk_combo_box_set_active_id((GtkComboBox *)mAdvanced, itemId);
}

void GstCaffUI::updateUIforCamera(int cameraId, bool updateUI)
{
    CamSetting *Setting = NULL;

    mCurCameraId = cameraId;
    printf("the current cameraId: %d\n", mCurCameraId);
    Setting = &mCamSetting[cameraId];
    Setting->updateUI = updateUI;
    updateSettingSource(Setting->source);
    updateSettingSink(Setting->sink);
    updateSettingConvert(Setting->convert);
    updateSettingIOMode(Setting->IOMode);
    updateSettingFmt(Setting->format);
    updateSettingInterlaceMode(Setting->interlaceMode);
    updateSettingVcEnable(Setting->vcEnable);
    updateSettingMipiCaptureMode(Setting->mipiCaptureMode);
    updateSettingResolution(Setting->reso);
    updateSettingDeinterlace(Setting->deInterlace);
    updateSettingDeviceName(Setting->deviceName);
    updateSettingIRISMode(Setting->IrisMode);
    updateSettingAeMode(Setting->aeMode);
    updateSettingFisheyeMode(Setting->fisheyemode);
    updateSettingConvergeSpeedMode(Setting->convergeSpeedMode);
    updateSettingConvergeSpeedLevel(Setting->convergeSpeedLevel);
    updateSettingExposureTime(Setting->exposureTime);
    updateSettingExposureGain(Setting->exposureGain);
    updateSettingExposureEv(Setting->exposureEv);
    updateSettingAwbMode(Setting->awbMode);
    updateSettingAwbRange(Setting->awbRange);
    updateSettingAwbWp(Setting->awbWp);
    updateSettingAwbGainR(Setting->awbRGain);
    updateSettingAwbGainG(Setting->awbGGain);
    updateSettingAwbGainB(Setting->awbBGain);
    updateSettingAwbShiftR(Setting->awbRshift);
    updateSettingAwbShiftG(Setting->awbGshift);
    updateSettingAwbShiftB(Setting->awbBshift);
    updateSettingWindowSize(Setting->windowSize);
    updateSettingRegionMode(Setting->regionMode);
    updateSettingDaynightMode(Setting->daynightMode);
    updateSettingSharpness(Setting->sharpness);
    updateSettingBrightness(Setting->brightness);
    updateSettingContrast(Setting->contrast);
    updateSettingHue(Setting->hue);
    updateSettingSaturation(Setting->saturation);
    updateSettingWDRMode(Setting->WdrMode);
    updateSettingBlcArea(Setting->BlcArea);
    updateSettingWdrValue(Setting->WdrLevel);
    updateSettingNoiseReductionMode(Setting->noiseReductionMode);
    updateSettingNoiseFilter(Setting->noiseFilterLevel);
    updateSettingSpatial(Setting->spatialLevel);
    updateSettingTemporal(Setting->temporialLevel);
    updateSettingMirrorEffect(Setting->mirrorEffectMode);
    updateSettingSceneMode(Setting->sceneMode);
    updateSettingSensorReso(Setting->sensorResolution);
    updateSettingCaptureFps(Setting->captureFps);
    updateSettingVideoStandard(Setting->videoStandard);
    updateSettingAdvancedFeature(Setting->advanced);
    Setting->updateUI = false;
}

void GstCaffUI::UpdateCameraNum(int ncount)
{
    switch (ncount)
    {
    case 1:
        gtk_combo_box_set_active_id((GtkComboBox *)this->mCameNum, "CameraNum1");
        break;
    case 2:
        gtk_combo_box_set_active_id((GtkComboBox *)this->mCameNum, "CameraNum2");
        break;
    case 3:
        gtk_combo_box_set_active_id((GtkComboBox *)this->mCameNum, "CameraNum3");
        break;
    case 4:
        gtk_combo_box_set_active_id((GtkComboBox *)this->mCameNum, "CameraNum4");
        break;
    default:
        printf("Camera num is out of range");
        break;
    }
}

void GstCaffUI::cameraIndexChangedCallback(GtkWidget *widget, gpointer data)
{
    char *camIndex = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    camIndex = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if (camIndex) {
        int camId = atoi(camIndex);
        cbData->updateUIforCamera(camId, true);
    }
}

void GstCaffUI::startAllCameraCallback(GtkWidget *widget, gpointer data)
{
    GstCaffUI *cbData = (GstCaffUI *)data;
    for (int i = 0; i < cbData->mCamNumber; i++) {
       if (cbData->mCamMode[i] == CAMERA_MODE_STOP) {
           printf("start preview: CameraId 0 : %d\n", i);
           cbData->mEventControl->waitSem();
           cbData->mEventControl->setStreamType(i, STREAM_TYPE_PREVIEW);
           cbData->mEventControl->setCameraState(i, CAMERA_STATE_START);
           cbData->mEventControl->postSem();
       }
    }
}

void GstCaffUI::stopAllCameraCallback(GtkWidget *widget, gpointer data)
{
    GstCaffUI *cbData = (GstCaffUI *)data;
    for (int i = 0; i < cbData->mCamNumber; i++) {
        printf("stop CameraId: %d\n", i);
        cbData->mEventControl->setCameraState(i,CAMERA_STATE_STOP);
    }
}

void GstCaffUI::sourceElementChangedCallback(GtkWidget *widget, gpointer data)
{
    char *sourceElement = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    sourceElement = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    strcpy(cbData->mCamSetting[camId].source, sourceElement);
    if (cbData->clientMode) {
        //set the sourceElement in remote proxy
        char temp[128] = "sourceElement=" ;
        cbData->mRemoteClient->setCmd2RemoteClient(camId, strcat(temp,sourceElement));
        printf("set the sourceElement\n");
    } else {
        cbData->mEventControl->waitSem();
        cbData->mEventControl->setSrcElementType(camId, sourceElement);
        cbData->mEventControl->getCameraState(camId, stateOld);
        if(stateOld != CAMERA_STATE_STOP) {
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
        }
        cbData->mEventControl->postSem();
    }
    g_free(sourceElement);
}

void GstCaffUI::sinkElementChangedCallback(GtkWidget *widget, gpointer data)
{
    char *sinkElement = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    sinkElement = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    strcpy(cbData->mCamSetting[camId].sink, sinkElement);
    if (cbData->clientMode) {
        //set the sinkElement in remote proxy
        char temp[128] = "sinkElement=" ;
        cbData->mRemoteClient->setCmd2RemoteClient(camId, strcat(temp,sinkElement));
        printf("set the sinkElement\n");
    } else {
        cbData->mEventControl->waitSem();
        cbData->mEventControl->setSinkElementType(camId, sinkElement);
        cbData->mEventControl->getCameraState(camId, stateOld);
        if(stateOld != CAMERA_STATE_STOP) {
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
        }
        cbData->mEventControl->postSem();
    }
    g_free(sinkElement);
}

void GstCaffUI::convertElementChangedCallback(GtkWidget *widget, gpointer data)
{
    char *convertElement = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    convertElement = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    strcpy(cbData->mCamSetting[camId].convert, convertElement);
    if (cbData->clientMode) {
        //set the sinkElement in remote proxy
        char temp[128] = "convertElement=" ;
        cbData->mRemoteClient->setCmd2RemoteClient(camId, strcat(temp,convertElement));
        printf("set the convertElement\n");
    } else {
        cbData->mEventControl->waitSem();
        cbData->mEventControl->setConvertElementType(camId, convertElement);
        cbData->mEventControl->getCameraState(camId, stateOld);
        if(stateOld != CAMERA_STATE_STOP) {
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
        }
        cbData->mEventControl->postSem();
    }
    g_free(convertElement);
}

bool GstCaffUI::processCurrentModeMachine(CamMode *newMode, CamMode oldMode)
{
    bool needToStop = false;
    switch (oldMode) {
        case CAMERA_MODE_STOP:
            if (*newMode == CAMERA_MODE_PAUSED || *newMode == CAMERA_MODE_SNAPSHOT) {
                needToStop = false;
                *newMode = CAMERA_MODE_STOP;
            }
            break;
        case CAMERA_MODE_PREVIEW:
            if (*newMode == CAMERA_MODE_STOP) {
                *newMode = CAMERA_MODE_STOP;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_PREVIEW) {
                *newMode = CAMERA_MODE_PREVIEW;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_SNAPSHOT) {
                *newMode = CAMERA_MODE_SNAPSHOT;
                needToStop = true;
            } else if (*newMode == CAMERA_MODE_RECORDING) {
                *newMode = CAMERA_MODE_RECORDING;
                needToStop = true;
            } else if (*newMode == CAMERA_MODE_PAUSED) {
                *newMode = CAMERA_MODE_PREVIEW_PAUSED;
                needToStop = false;
            }
            printf("the camera switch from PREVIEW to %d, needToStop= %d\n", *newMode, needToStop);
            break;
        case CAMERA_MODE_RECORDING:
            if (*newMode == CAMERA_MODE_STOP) {
                *newMode = CAMERA_MODE_STOP;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_PREVIEW) {
                *newMode = CAMERA_MODE_PREVIEW;
                needToStop = true;
            } else if (*newMode == CAMERA_MODE_SNAPSHOT) {
                *newMode = CAMERA_MODE_SNAPSHOT;
                needToStop = true;
            } else if (*newMode == CAMERA_MODE_RECORDING) {
                *newMode = CAMERA_MODE_RECORDING;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_PAUSED) {
                *newMode = CAMERA_MODE_RECORDING_PAUSED;
                needToStop = false;
            }
            printf("the camera switch from RECORDING to %d, needToStop = %d\n", *newMode, needToStop);
            break;
        case CAMERA_MODE_PREVIEW_PAUSED:
            if (*newMode == CAMERA_MODE_STOP) {
                *newMode = CAMERA_MODE_STOP;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_PREVIEW) {
                *newMode = CAMERA_MODE_PREVIEW;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_SNAPSHOT) {
                *newMode = CAMERA_MODE_PREVIEW_PAUSED;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_RECORDING) {
                *newMode = CAMERA_MODE_RECORDING;
                needToStop = true;
            } else if (*newMode == CAMERA_MODE_PAUSED){
                *newMode = CAMERA_MODE_PREVIEW_PAUSED;
                needToStop = false;
            }
            printf("the camera switch from PREVIEW_PAUSED to %d, needToStop = %d\n", *newMode, needToStop);
            break;
        case CAMERA_MODE_RECORDING_PAUSED:
            if (*newMode == CAMERA_MODE_STOP) {
                *newMode = CAMERA_MODE_STOP;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_PREVIEW) {
                *newMode = CAMERA_MODE_PREVIEW;
                needToStop = true;
            } else if (*newMode == CAMERA_MODE_SNAPSHOT) {
                *newMode = CAMERA_MODE_RECORDING_PAUSED;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_RECORDING) {
                *newMode = CAMERA_MODE_RECORDING;
                needToStop = false;
            } else if (*newMode == CAMERA_MODE_PAUSED){
                *newMode = CAMERA_MODE_RECORDING_PAUSED;
                needToStop = false;
            }
            printf("the camera switch from PREVIEW_PAUSED to %d, needToStop = %d\n", *newMode, needToStop);
            break;

        default:
            printf("the Old Camera Mode is error\n");
            break;
    }
    return needToStop;
}

void GstCaffUI::cameraModeChangeCallback(GtkWidget *widget, gpointer data)
{
    int camId = -1;
    CamMode newMode;
    STREAM_TYPE type, typeOld;
    type = typeOld = STREAM_TYPE_NONE;
    bool needToStop = false;
    GstCaffUI *cbData = (GstCaffUI *)data;
    camId = cbData->mCurCameraId;
    if (camId < 0)
        return;

    if (widget == (GtkWidget*)(cbData->mStart))
        newMode = CAMERA_MODE_PREVIEW;
    else if (widget == (GtkWidget*)(cbData->mPause))
        newMode = CAMERA_MODE_PAUSED;
    else if (widget == (GtkWidget*)(cbData->mRecord))
        newMode = CAMERA_MODE_RECORDING;
    else if (widget == (GtkWidget*)(cbData->mSnapshot))
        newMode = CAMERA_MODE_SNAPSHOT;
    else if (widget == (GtkWidget*)(cbData->mStop))
        newMode = CAMERA_MODE_STOP;
    else
        printf("the button callback is error\n");

    needToStop = cbData->processCurrentModeMachine(&newMode, cbData->mCamMode[camId]);
    printf("the oldMode: %d, newMode = %d, needToStop = %d\n", cbData->mCamMode[camId], newMode, needToStop);

    if (newMode == CAMERA_MODE_STOP) {
        printf("stop the camera: CameraId: %d\n", camId);
        cbData->mEventControl->setCameraState(camId,CAMERA_STATE_STOP);
    } else if (newMode == CAMERA_MODE_PREVIEW_PAUSED
            || newMode == CAMERA_MODE_RECORDING_PAUSED) {
        printf("paused the camera, CameraId: %d\n", camId);
        cbData->mEventControl->setCameraState(camId,CAMERA_STATE_PAUSE);
    } else {
        if (newMode == CAMERA_MODE_SNAPSHOT) {
            printf("take a picture and recovery the old mode\n");
            cbData->mEventControl->setCameraState(camId,CAMERA_STATE_STOP);
            cbData->mEventControl->waitSem();
            //cbData->mEventControl->setStreamType(camId, STREAM_TYPE_SNAPSHOT);
            cbData->mEventControl->setCaptureNum(camId, 1);
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
            cbData->mEventControl->postSem();
            needToStop = true;
            newMode = cbData->mCamMode[camId];
        }
        if (needToStop) {
            printf("stop the camera before start a new pipeline, CameraId: %d\n", camId);
            cbData->mEventControl->setCameraState(camId,CAMERA_STATE_STOP);
        }
        if (newMode == CAMERA_MODE_PREVIEW) {
            type = STREAM_TYPE_PREVIEW;
            printf("start preview: CameraId: %d\n", camId);
        }
        if (newMode == CAMERA_MODE_RECORDING) {
            type = STREAM_TYPE_RECORDING;
            printf("start recording: CameraId: %d\n", camId);
        }

        printf("start preview: CameraId 0 : %d\n", camId);
        if(cbData->clientMode)
        {
            cbData->mRemoteClient->setCmd2RemoteClient(camId,(char *)"state=stop");
            cbData->mRemoteClient->setCmd2RemoteClient(camId,(char *)"type=remote");
            cbData->mRemoteClient->setCmd2RemoteClient(camId,(char *)"state=play");
        }
        else
        {
            cbData->mEventControl->waitSem();
            cbData->mEventControl->getStreamType(camId, typeOld);
            if (typeOld != type) {
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
            }
            cbData->mEventControl->setStreamType(camId, type);
            cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
            cbData->mEventControl->postSem();
        }
    }
    if (newMode != CAMERA_MODE_SNAPSHOT)
        cbData->mCamMode[camId] = newMode;

}

void GstCaffUI::resolutionChangedCallback(GtkWidget *widget, gpointer data)
{
    char * token;
    char *resPtr = NULL;
    int width = 0, height = 0;
    int widthOld = 0, heightOld = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    resPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    strcpy(cbData->mCamSetting[camId].reso, resPtr);
    if(cbData->clientMode)
    {
        char cmd[64] = "resolution=";
        cbData->mRemoteClient->setCmd2RemoteClient(0,strcat(cmd,cbData->mCamSetting[camId].reso));
    }
    else
    {
        token = strtok((char*)resPtr, "x");
        if (token != NULL)
            width = atoi(token);
        token = strtok(NULL, "x");
        if (token != NULL)
            height = atoi(token);
        g_free(resPtr);

        cbData->mEventControl->getResolution(camId, widthOld, heightOld);
        printf("change resolution from %dx%d to %dx%d, cameraId: %d\n",
                widthOld, heightOld, width, height, camId);
        if(widthOld != width || heightOld != height) {
            if (cbData->mRegionState != REGION_STATE_NONE) {
                cbData->mRegion.clear();
                for (int i = 0; i < MAX_REGION_NUM; i++) {
                    gtk_entry_set_text((GtkEntry *)(cbData->mRegionEntry[i]), "");
                    gtk_entry_set_text((GtkEntry *)(cbData->mWeightSpinbutton[i]), "");
                }
            }
            cbData->mEventControl->waitSem();
            cbData->mEventControl->setResolution(camId, width, height);
            cbData->mEventControl->getCameraState(camId, stateOld);
            if(stateOld != CAMERA_STATE_STOP) {
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
            }
            cbData->mEventControl->postSem();
        }
    }
}

void GstCaffUI::formatChangedCallback(GtkWidget *widget, gpointer data)
{
    char * formatPtr = NULL;
    char formatOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    formatPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode)
    {
        char cmd[64] = "format=";
        //cbData->mRemoteClient->setCmd2RemoteClient(0,"state=stop");
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,formatPtr));
        //cbData->mRemoteClient->setCmd2RemoteClient(0,"state=start");
    }
    else
    {
        cbData->mEventControl->getFormat(camId, formatOld);
        printf("change the format from %s to %s, camId: %d\n", formatOld, formatPtr, camId);
        strcpy(cbData->mCamSetting[camId].format, formatPtr);

        if(strcmp(formatOld, formatPtr) != 0)
        {
            cbData->mEventControl->waitSem();
            cbData->mEventControl->setFormat(camId, formatPtr);
            cbData->mEventControl->getCameraState(camId, stateOld);
            if(stateOld != CAMERA_STATE_STOP) {
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
            }
            cbData->mEventControl->postSem();
        }
    }

    g_free(formatPtr);
}

void GstCaffUI::enableMipiCaptureCallback(GtkWidget *widget, gpointer data)
{
    bool mode = false;
    bool modeOld = false;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int cameraId = cbData->mCurCameraId;
    if (cameraId < 0 || cbData->mCamSetting[cameraId].updateUI == true)
        return;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        printf("Enabel Mipi capture mode: %d\n", cameraId);
        cbData->mCamSetting[cameraId].mipiCaptureMode = true;
        setenv("cameraMipiCapture", "true", 1);
    } else {
        printf("Disable Mipi capture mode: %d\n", cameraId);
        cbData->mCamSetting[cameraId].mipiCaptureMode = false;
        setenv("cameraMipiCapture", "false", 1);
    }
}

void GstCaffUI::enableVirtualChannelCallback(GtkWidget *widget, gpointer data)
{
    bool mode = false;
    bool modeOld = false;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int cameraId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (cameraId < 0 || cbData->mCamSetting[cameraId].updateUI == true)
        return;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        printf("the virtual channel is ON, cameraId: %d\n", cameraId);
        mode = true;
    } else {
        printf("the virtual channel is OFF, cameraId: %d\n", cameraId);
        mode = false;
    }

    if(cbData->clientMode)
    {
        //implement this in the future.
        printf("this feature isn't supported in client mode \n");
    } else {
        if(modeOld != mode) {
            printf("change virtual channel to %s, cameraId: %d\n", mode ? "enable" : "disable", cameraId);
            cbData->mCamSetting[cameraId].vcEnable = mode;
            cbData->mEventControl->setVcEnable(cameraId, mode);
        }
    }
}

void GstCaffUI::enableInterlaceCallback(GtkWidget *widget, gpointer data)
{
    bool mode = false;
    bool modeOld = false;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int cameraId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (cameraId < 0 || cbData->mCamSetting[cameraId].updateUI == true)
        return;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        printf("the interlaceMode is ON, cameraId: %d\n", cameraId);
        mode = true;
    } else {
        printf("the interlaceMode is OFF, set deInterlace to NULL, cameraId: %d\n", cameraId);
        mode = false;
    }

    if(cbData->clientMode)
    {
        if(mode == true) {
            cbData->mRemoteClient->setCmd2RemoteClient(cameraId,(char *)"interlace=true");
        } else {
            cbData->mRemoteClient->setCmd2RemoteClient(cameraId,(char *)"interlace=false");
        }
    } else {
        cbData->mEventControl->getInterlaceMode(cameraId, modeOld);
        if(modeOld != mode) {
            printf("change InterlaceMode to %s, cameraId: %d\n", mode ? "enable" : "disable", cameraId);
            cbData->mCamSetting[cameraId].interlaceMode = mode;
            cbData->mEventControl->waitSem();
            cbData->mEventControl->setInterlaceMode(cameraId, mode);
            cbData->mEventControl->getCameraState(cameraId, stateOld);
            if(stateOld != CAMERA_STATE_STOP) {
                cbData->mEventControl->setCameraState(cameraId, CAMERA_STATE_STOP);
                cbData->mEventControl->setCameraState(cameraId, CAMERA_STATE_START);
            }
            cbData->mEventControl->postSem();
        }
    }
}

void GstCaffUI::IOModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *IoModePtr = NULL;
    char IoModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int cameraId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (cameraId < 0 || cbData->mCamSetting[cameraId].updateUI == true)
        return;

    IoModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    cbData->mEventControl->getIOMode(cameraId, IoModeOld);
    printf("change IO-mode from %s to %s, cameraId: %d\n", IoModeOld, IoModePtr, cameraId);

    if(cbData->clientMode)
    {
        char cmd[64] = "IO-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(cameraId,strcat(cmd,IoModePtr));
    } else {
        if(strcmp(IoModeOld,IoModePtr) != 0) {
            strcpy(cbData->mCamSetting[cameraId].IOMode, IoModePtr);
            cbData->mEventControl->waitSem();
            cbData->mEventControl->setIoMode(cameraId, IoModePtr);
            cbData->mEventControl->getCameraState(cameraId, stateOld);
            if(stateOld != CAMERA_STATE_STOP) {
                cbData->mEventControl->setCameraState(cameraId, CAMERA_STATE_STOP);
                cbData->mEventControl->setCameraState(cameraId, CAMERA_STATE_START);
            }
            cbData->mEventControl->postSem();
        }
    }
}

void GstCaffUI::deInterlaceChangedCallback(GtkWidget *widget, gpointer data)
{
    bool isInterlaceEnable = false;
    char *deInterlacePtr = NULL;
    char deInterlaceOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int cameraId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (cameraId < 0 || cbData->mCamSetting[cameraId].updateUI == true)
        return;

    isInterlaceEnable = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cbData->mInterlaceMode));
    if (isInterlaceEnable) {
        deInterlacePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
        cbData->mEventControl->getDeInterlace(cameraId, deInterlaceOld);
        printf("change deinterlace-method from %s to %s, cameraId: %d\n",
                                        deInterlaceOld, deInterlacePtr, cameraId);
        if(strcmp(deInterlaceOld, deInterlacePtr) != 0) {
            if(cbData->clientMode)
            {
                char temp[128] = "deinterlace=" ;
                cbData->mRemoteClient->setCmd2RemoteClient(cameraId,strcat(temp,deInterlacePtr));
            } else {
                strcpy(cbData->mCamSetting[cameraId].deInterlace, deInterlacePtr);
                cbData->mEventControl->waitSem();
                cbData->mEventControl->setDeInterlace(cameraId, deInterlacePtr);
                cbData->mEventControl->getCameraState(cameraId, stateOld);
                if(stateOld != CAMERA_STATE_STOP)
                {
                    cbData->mEventControl->setCameraState(cameraId, CAMERA_STATE_STOP);
                    cbData->mEventControl->setCameraState(cameraId, CAMERA_STATE_START);
                }
                cbData->mEventControl->postSem();
            }
        }
    }
    g_free(deInterlacePtr);
}

void GstCaffUI::deviceChangedCallback(GtkWidget *widget, gpointer data)
{
    char *deviceNamePtr = NULL;
    char deviceNameOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    deviceNamePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    cbData->mEventControl->getDeviceName(camId, deviceNameOld);
    printf("change deviceName from %s to %s, cameraId: %d\n",
                                        deviceNameOld, deviceNamePtr, camId);
    if(strcmp(deviceNameOld, deviceNamePtr) != 0) {
        if(cbData->clientMode) {
            //implement this in the future.
            printf("this feature isn't supported in client mode \n");
        } else {
            strcpy(cbData->mCamSetting[camId].deviceName, deviceNamePtr);
            cbData->mEventControl->waitSem();
            cbData->mEventControl->setDeviceName(camId, deviceNamePtr);
            cbData->mEventControl->getCameraState(camId, stateOld);
            if(stateOld != CAMERA_STATE_STOP)
            {
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_STOP);
                cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
            }
            cbData->mEventControl->postSem();
        }
    }
    g_free(deviceNamePtr);
}

void GstCaffUI::IRISModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *irisModePtr = NULL;
    char irisModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    irisModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "IRIS-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,irisModePtr));
    } else {
        cbData->mEventControl->getIrisMode(camId, irisModeOld);
        if(strcmp(irisModeOld, irisModePtr) != 0) {
            printf("change the IRIS-mode from %s to %s, camId: %d\n", irisModeOld, irisModePtr, camId);
            strcpy(cbData->mCamSetting[camId].IrisMode, irisModePtr);
            cbData->mEventControl->setIrisMode(camId, irisModePtr);
        }
    }
    g_free(irisModePtr);

}

void GstCaffUI::aeModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *aeModePtr = NULL;
    char aeModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    aeModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if (strcmp(aeModePtr, "manual") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mExposureTimeLabel));
        gtk_widget_show((GtkWidget*)(cbData->mExposureTimeEg));
        gtk_widget_show((GtkWidget*)(cbData->mExposureGainLabel));
        gtk_widget_show((GtkWidget*)(cbData->mExposureGainEg));
        gtk_widget_show((GtkWidget*)(cbData->mExposureTimeLevel));
        gtk_widget_show((GtkWidget*)(cbData->mExposureGainLevel));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mExposureTimeLabel));
        gtk_widget_hide((GtkWidget*)(cbData->mExposureGainLabel));
        gtk_widget_hide((GtkWidget*)(cbData->mExposureTimeLevel));
        gtk_widget_hide((GtkWidget*)(cbData->mExposureGainLevel));
        gtk_widget_hide((GtkWidget*)(cbData->mExposureTimeEg));
        gtk_widget_hide((GtkWidget*)(cbData->mExposureGainEg));
    }

    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;
    if(cbData->clientMode) {
        //implement this in the future.
        printf("this feature isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAeMode(camId, aeModeOld);
        if(strcmp(aeModeOld, aeModePtr) != 0) {
            printf("change the ae-mode from %s to %s, camId: %d\n", aeModeOld, aeModePtr, camId);
            strcpy(cbData->mCamSetting[camId].aeMode, aeModePtr);
            cbData->mEventControl->setAeMode(camId, aeModePtr);
        }
    }
    g_free(aeModePtr);

}
void GstCaffUI::fisheyemodechangecallback(GtkWidget *widget, gpointer data)
{
    char FisheyeModeOld[20] = {0};
    char *FisheyeModePtr = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    FisheyeModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);

    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;
    if(cbData->clientMode) {
        //implement this in the future.
        printf("this feature isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getFisheyeMode(camId, FisheyeModeOld);
        if(strcmp(FisheyeModeOld, FisheyeModePtr) != 0) {
            printf("chnage the fisheyemode from %s to %s, camid:%d\n",FisheyeModeOld,FisheyeModePtr,camId);
            strcpy(cbData->mCamSetting[camId].fisheyemode, FisheyeModePtr);
            cbData->mEventControl->setFisheyeMode(camId, FisheyeModePtr);
        }
    }
    g_free(FisheyeModePtr);

}
void GstCaffUI::convergeSpeedModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *cvgSpeedMode = NULL;
    char cvgSpeedModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    cvgSpeedMode = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        //implement this in the future.
        printf("this feature isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getConvergeSpeedMode(camId, cvgSpeedModeOld);
        if (strcmp(cvgSpeedModeOld, cvgSpeedMode) != 0) {
            printf("change the converge-speed-mode from %s to %s, camId: %d\n",
                    cvgSpeedModeOld, cvgSpeedMode, camId);
            strcpy(cbData->mCamSetting[camId].convergeSpeedMode, cvgSpeedMode);
            cbData->mEventControl->setConvergeSpeedMode(camId, cvgSpeedMode);
        }
    }
    g_free(cvgSpeedMode);
}

void GstCaffUI::convergeSpeedLevelChangedCallback(GtkWidget *widget, gpointer data)
{
    char *cvgSpeedLevel = NULL;
    char cvgSpeedLevelOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    cvgSpeedLevel = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        //implement this in the future.
        printf("this feature isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getConvergeSpeedLevel(camId, cvgSpeedLevelOld);
        if (strcmp(cvgSpeedLevelOld, cvgSpeedLevel) != 0) {
            printf("change the converge-speed-level from %s to %s, camId: %d\n",
                    cvgSpeedLevelOld, cvgSpeedLevel, camId);
            strcpy(cbData->mCamSetting[camId].convergeSpeedLevel, cvgSpeedLevel);
            cbData->mEventControl->setConvergeSpeedLevel(camId, cvgSpeedLevel);
        }
    }
    g_free(cvgSpeedLevel);
}

void GstCaffUI::setIrisLevelCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "IRIS-level=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getIrisLevel(camId, oldValue);
        if(value != oldValue) {
            printf("change Iris level from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].irisLevel = value;
            cbData->mEventControl->setIrisLevel(camId, value);
        }
    }
}

void GstCaffUI::exposureTimeChangeCallback(GtkWidget *widget,  GtkScrollType  type, gpointer data)
{
    char *str = NULL;
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        value = atoi(str);
        switch (type) {
          case GTK_SCROLL_STEP_UP:
            value += 1;
            break;
          case GTK_SCROLL_STEP_DOWN:
            value -= 1;
            break;
          case GTK_SCROLL_PAGE_UP:
            value += 10;
            break;
          case GTK_SCROLL_PAGE_DOWN:
            value -= 10;
            break;
          case GTK_SCROLL_START:
            value = 0;
            break;
          case GTK_SCROLL_END:
            value = 1000000;
            break;
          default:
            printf("ERR: unknown type for gtk spin button\n");
            break;
        }
        if (value < 0)
            value = 0;
        if (value > 1000000)
            value = 1000000;
        if(cbData->clientMode) {
            char cmd[64] = "exposure-time=";
            sprintf(cmd+strlen(cmd), "%d", value);
            cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
        } else {
            cbData->mEventControl->getExposureTime(camId, oldValue);
            if(value != oldValue) {
                printf("change exposureTime from %d to %d, camId: %d\n", oldValue, value, camId);
                cbData->mCamSetting[camId].exposureTime = value;
                cbData->mEventControl->setExposureTime(camId, value);
            }
        }

    }
}

void GstCaffUI::setExposureTimeCallback(GtkWidget *widget, gpointer data)
{
    char *str = NULL;
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        value = atoi(str);
        if (value < 0)
            value = 0;
        if (value > 1000000)
            value = 1000000;

        if(cbData->clientMode) {
            char cmd[64] = "exposure-time=";
            sprintf(cmd+strlen(cmd), "%d", value);
            cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
        } else {
            cbData->mEventControl->getExposureTime(camId, oldValue);
            if(value != oldValue) {
                printf("change exposureTime from %d to %d, camId: %d\n", oldValue, value, camId);
                cbData->mCamSetting[camId].exposureTime = value;
                cbData->mEventControl->setExposureTime(camId, value);
            }
        }
    }
}

void GstCaffUI::setExposureGainCallback(GtkWidget *widget, gpointer data)
{ char *str = NULL;
    float value = 0.0, oldValue = 0.0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        value = atof(str);
        if (value < 0.0)
            value = 0.0;
        if (value > 60.0)
            value = 60.0;
        if(cbData->clientMode) {
            char cmd[64] = "exposure-gain=";
            sprintf(cmd+strlen(cmd), "%f", value);
            cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
        } else {
            cbData->mEventControl->getExposureGain(camId, oldValue);
            if(value != oldValue) {
                printf("change exposureGain from %f to %f, camId: %d\n", oldValue, value, camId);
                cbData->mCamSetting[camId].exposureGain = value;
                cbData->mEventControl->setExposureGain(camId, value);
            }
        }
    }
}

void GstCaffUI::setExposureEvCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "exposure-ev=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getExposureEv(camId, oldValue);
        if(value != oldValue) {
            printf("change exposureEv from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].exposureEv = value;
            cbData->mEventControl->setExposureEv(camId, value);
        }
    }
}

void GstCaffUI::awbModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *awbModePtr = NULL;
    char awbModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    awbModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    cbData->mEventControl->getAwbMode(camId, awbModeOld);
    if (strcmp(awbModePtr, "manual_gain") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mAwbRGainLable));
        gtk_widget_show((GtkWidget*)(cbData->mAwbGGainLable));
        gtk_widget_show((GtkWidget*)(cbData->mAwbBGainLable));
        gtk_widget_show((GtkWidget*)(cbData->mAwbRGainScale));
        gtk_widget_show((GtkWidget*)(cbData->mAwbGGainScale));
        gtk_widget_show((GtkWidget*)(cbData->mAwbBGainScale));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mAwbRGainLable));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbGGainLable));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbBGainLable));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbRGainScale));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbGGainScale));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbBGainScale));
    }

    if (strcmp(awbModePtr, "cct_range") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mAwbRangeLabel));
        gtk_widget_show((GtkWidget*)(cbData->mAwbRangeEg));
        gtk_widget_show((GtkWidget*)(cbData->mAwbRangeEntry));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mAwbRangeLabel));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbRangeEg));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbRangeEntry));
    }

    if (strcmp(awbModePtr, "white_point") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mAwbWpLabel));
        gtk_widget_show((GtkWidget*)(cbData->mAwbWpEntry));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mAwbWpLabel));
        gtk_widget_hide((GtkWidget*)(cbData->mAwbWpEntry));
    }

    if (strcmp(awbModePtr, "color_transform") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mColorLabel));
        gtk_widget_show((GtkWidget*)(cbData->mColorRangeLabel));
        gtk_widget_show((GtkWidget*)(cbData->mColorTansform));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mColorLabel));
        gtk_widget_hide((GtkWidget*)(cbData->mColorRangeLabel));
        gtk_widget_hide((GtkWidget*)(cbData->mColorTansform));
    }

    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    if(cbData->clientMode) {
        char cmd[64] = "awb-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,awbModePtr));
    } else {
        if(strcmp(awbModeOld, awbModePtr) != 0) {
            printf("change the AWB-mode from %s to %s, camId: %d\n", awbModeOld, awbModePtr, camId);
            strcpy(cbData->mCamSetting[camId].awbMode, awbModePtr);
            cbData->mEventControl->setAwbMode(camId, awbModePtr);
        }
    }
    g_free(awbModePtr);
}

void GstCaffUI::setAwbRangeCallback(GtkWidget *widget, gpointer data)
{
    char *awbRangePtr = NULL;
    char awbRangeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    awbRangePtr = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    cbData->mEventControl->getAwbRange(camId, awbRangeOld);
    if (!awbRangePtr)
        return;

    if(cbData->clientMode) {
        char cmd[64] = "awb-range=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,awbRangePtr));
    } else {
        if(strcmp(awbRangeOld, awbRangePtr) != 0) {
            printf("change the cct_range to %s, camId: %d\n", awbRangePtr, camId);
            strcpy(cbData->mCamSetting[camId].awbRange, awbRangePtr);
            cbData->mEventControl->setAwbRange(camId, awbRangePtr);
        }
    }
}

void GstCaffUI::awbRGainChangedCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        printf("Awb R-Gain isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAwbRGain(camId, oldValue);
        if(value != oldValue) {
            printf("change awb-gain-r from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].awbRGain = value;
            cbData->mEventControl->setAwbRGain(camId, value);
        }
    }
}

void GstCaffUI::awbGGainChangedCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        printf("Awb G-Gain isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAwbGGain(camId, oldValue);
        if(value != oldValue) {
            printf("change awb-gain-g from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].awbGGain = value;
            cbData->mEventControl->setAwbGGain(camId, value);
        }
    }
}

void GstCaffUI::awbBGainChangedCallback(GtkWidget *widget, gpointer data)
{

    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        printf("Awb B-Gain isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAwbBGain(camId, oldValue);
        if(value != oldValue) {
            printf("change awb-gain-b from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].awbBGain = value;
            cbData->mEventControl->setAwbBGain(camId, value);
        }
    }
}

void GstCaffUI::awbTransformCallback(GtkWidget *widget, gpointer data)
{
    char *colorTransform = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    colorTransform = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (colorTransform) {
        if(cbData->clientMode) {
            //implement this in the future.
            printf("Color-transform isn't supported on client mode\n");
        } else {
            printf("set the Awb colorTransform: %s\n", colorTransform);
            cbData->mEventControl->setColorTransform(camId, colorTransform);
        }
    }
}

void GstCaffUI::setAwbRshiftCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        //implement this in the future.
        printf("Awb Shift R isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAwbShiftR(camId, oldValue);
        if(value != oldValue) {
            printf("change awb-shift R from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].awbRshift = value;
            cbData->mEventControl->setAwbShiftR(camId, value);
        }
    }
}

void GstCaffUI::setAwbGshiftCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        //implement this in the future.
        printf("Awb Shift G isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAwbShiftG(camId, oldValue);
        if(value != oldValue) {
            printf("change awb-shift G from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].awbGshift = value;
            cbData->mEventControl->setAwbShiftG(camId, value);
        }
    }
}

void GstCaffUI::setAwbBshiftCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        //implement this in the future.
        printf("Awb Shift B isn't supported on client mode\n");
    } else {
        cbData->mEventControl->getAwbShiftB(camId, oldValue);
        if(value != oldValue) {
            printf("change awb-shift B from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].awbBshift = value;
            cbData->mEventControl->setAwbShiftB(camId, value);
        }
    }
}

void GstCaffUI::regionModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *regionModePtr = NULL;
    char regionModeOld[64] = {0};
    bool stateChanged = false;
    RegionState state = REGION_STATE_NONE;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    regionModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if (strcmp(regionModePtr, "none") == 0) {
        for (int i = 0; i < MAX_REGION_NUM; i++){
            gtk_widget_hide((GtkWidget*)(cbData->mRegionEntry[i]));
            gtk_widget_hide((GtkWidget*)(cbData->mWeightSpinbutton[i]));

        }
        state = REGION_STATE_NONE;
    } else {
        for (int i = 0; i < MAX_REGION_NUM; i++) {
            gtk_widget_show((GtkWidget*)(cbData->mRegionEntry[i]));
            gtk_widget_show((GtkWidget*)(cbData->mWeightSpinbutton[i]));

        }
        if (strcmp(regionModePtr, "ae_region") == 0)
            state = REGION_STATE_AE;
        else
            state = REGION_STATE_AWB;
    }

    //the region state changed, need to clear the queue and entry.
    if (state != cbData->mRegionState) {
        cbData->mRegion.clear();
        for (int i = 0; i < MAX_REGION_NUM; i++) {
            gtk_entry_set_text((GtkEntry *)(cbData->mRegionEntry[i]), "");
            gtk_entry_set_text((GtkEntry *)(cbData->mWeightSpinbutton[i]), "");
        }
    }

    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    printf("change the region mode to %s, camId: %d\n", regionModePtr, camId);
    strcpy(cbData->mCamSetting[camId].regionMode, regionModePtr);
    cbData->mRegionState = state;
    g_free(regionModePtr);
}

void GstCaffUI::setWindowSizeCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        //implement this in the future.
        printf("set Window size isn't supported on client Mode\n");
    } else {
        cbData->mEventControl->getWindowSize(camId, oldValue);
        if(value != oldValue) {
            printf("change Window Size from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].windowSize = value;
            cbData->mEventControl->setWindowSize(camId, value);
        }
    }
}

void GstCaffUI::dayNightModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *dayNightModePtr = NULL;
    char dayNightModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    dayNightModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "daynight-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,dayNightModePtr));
    } else {
        cbData->mEventControl->getDayNightMode(camId, dayNightModeOld);
        if(strcmp(dayNightModeOld, dayNightModePtr) != 0) {
            printf("change the Daynight-mode from %s to %s, camId: %d\n", dayNightModeOld, dayNightModePtr, camId);
            strcpy(cbData->mCamSetting[camId].daynightMode, dayNightModePtr);
            cbData->mEventControl->setDayNightMode(camId, dayNightModePtr);
        }
    }
    g_free(dayNightModePtr);
}

void GstCaffUI::setSharpnessCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "sharpness=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getSharpness(camId, oldValue);
        if(value != oldValue) {
            printf("change Sharpness from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].sharpness = value;
            cbData->mEventControl->setSharpness(camId, value);
        }
    }
}

void GstCaffUI::setBrightnessCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "brigheness=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getBrightness(camId, oldValue);
        if(value != oldValue) {
            printf("change Brightness from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].brightness = value;
            cbData->mEventControl->setBrightness(camId, value);
        }
    }
}

void GstCaffUI::setContrastCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);

    if(cbData->clientMode) {
        char cmd[64] = "constrast=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getContrast(camId, oldValue);
        if(value != oldValue) {
            printf("change Contrast from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].contrast = value;
            cbData->mEventControl->setContrast(camId, value);
        }
    }
}

void GstCaffUI::setHueCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "hue=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getHue(camId, oldValue);
        if(value != oldValue) {
            printf("change Hue from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].hue = value;
            cbData->mEventControl->setHue(camId, value);
        }
    }
}

void GstCaffUI::setSaturationCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "saturation=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getSaturation(camId, oldValue);
        if(value != oldValue) {
            printf("change Saturation from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].saturation = value;
            cbData->mEventControl->setSaturation(camId, value);
        }
    }
}

void GstCaffUI::WDRModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *wdrModePtr = NULL;
    char wdrModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    wdrModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "WDR-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,wdrModePtr));
    } else {
        cbData->mEventControl->getWdrMode(camId, wdrModeOld);
        if(strcmp(wdrModeOld, wdrModePtr) != 0) {
            printf("change the WDR-mode from %s to %s, camId: %d\n", wdrModeOld, wdrModePtr, camId);
            strcpy(cbData->mCamSetting[camId].WdrMode, wdrModePtr);
            cbData->mEventControl->setWdrMode(camId, wdrModePtr);
        }
    }
    g_free(wdrModePtr);
}

void GstCaffUI::BlcAreaChangedCallback(GtkWidget *widget, gpointer data)
{
    char *enableBlcPtr = NULL;
    char enableBlcOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    enableBlcPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    cbData->mEventControl->getBlcMode(camId, enableBlcOld);

    if (strcmp(enableBlcPtr, "ON") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mWdrLevel));
        gtk_widget_show((GtkWidget*)(cbData->mWdrLabel));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mWdrLevel));
        gtk_widget_hide((GtkWidget*)(cbData->mWdrLabel));
    }
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    if(cbData->clientMode) {
        char cmd[64] = "BLC-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,enableBlcPtr));
    } else {
        if(strcmp(enableBlcOld, enableBlcPtr) != 0) {
            printf("change the BLC-mode from %s to %s, camId: %d\n", enableBlcOld, enableBlcPtr, camId);
            strcpy(cbData->mCamSetting[camId].BlcArea, enableBlcPtr);
            cbData->mEventControl->setBlcMode(camId, enableBlcPtr);
        }
    }
    g_free(enableBlcPtr);
}

void GstCaffUI::setWdrLevelCallback(GtkWidget *widget, gpointer data)
{
    char *enableBlcPtr = NULL;
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    enableBlcPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)cbData->mWdrMode);
    if (enableBlcPtr && (strcmp(enableBlcPtr, "ON") == 0)) {
        value = (int)gtk_range_get_value ((GtkRange*)widget);
        if(cbData->clientMode) {
            char cmd[64] = "blc-level=";
            sprintf(cmd+strlen(cmd), "%d", value);
            cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
        } else {
            cbData->mEventControl->getBlcAreaLevel(camId, oldValue);
            if(value != oldValue) {
                printf("change Blc-Area level from %d to %d, camId: %d\n", oldValue, value, camId);
                cbData->mCamSetting[camId].WdrLevel = value;
                cbData->mEventControl->setBlcAreaLevel(camId, value);
            }
        }
    }
}

void GstCaffUI::NoiseModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *noiseReductionModePtr = NULL;
    char noiseReductionModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;

    noiseReductionModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if (strcmp(noiseReductionModePtr, "normal") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mNoiseFilterLevel));
        gtk_widget_show((GtkWidget*)(cbData->mNoiseFilter));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mNoiseFilterLevel));
        gtk_widget_hide((GtkWidget*)(cbData->mNoiseFilter));
    }
    if (strcmp(noiseReductionModePtr, "manual") == 0) {
        gtk_widget_show((GtkWidget*)(cbData->mSpatialLevel));
        gtk_widget_show((GtkWidget*)(cbData->mTemporalLevel));
        gtk_widget_show((GtkWidget*)(cbData->mSpatial));
        gtk_widget_show((GtkWidget*)(cbData->mTemporal));
    } else {
        gtk_widget_hide((GtkWidget*)(cbData->mSpatialLevel));
        gtk_widget_hide((GtkWidget*)(cbData->mTemporalLevel));
        gtk_widget_hide((GtkWidget*)(cbData->mSpatial));
        gtk_widget_hide((GtkWidget*)(cbData->mTemporal));
    }
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    if(cbData->clientMode) {
        char cmd[64] = "noiseReduction-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,noiseReductionModePtr));
    } else {
        cbData->mEventControl->getNoiseReductionMode(camId, noiseReductionModeOld);
        if(strcmp(noiseReductionModePtr, noiseReductionModeOld) != 0) {
            printf("change the noiseReduction-mode from %s to %s, camId: %d\n", noiseReductionModeOld, noiseReductionModePtr, camId);
            strcpy(cbData->mCamSetting[camId].noiseReductionMode, noiseReductionModePtr);
            cbData->mEventControl->setNoiseReductionMode(camId, noiseReductionModePtr);
        }
    }
    g_free(noiseReductionModePtr);

}

void GstCaffUI::setNoiseFilterCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "noiseFilterLevel=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getNoiseFilter(camId, oldValue);
        if(value != oldValue) {
            printf("change noiseFilterLevel from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].noiseFilterLevel = value;
            cbData->mEventControl->setNoiseFilter(camId, value);
        }
    }
}

void GstCaffUI::setSpatialCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "spatialLevel=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getSpatialLevel(camId, oldValue);
        if(value != oldValue) {
            printf("change spatialLevel from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].spatialLevel = value;
            cbData->mEventControl->setSpatialLevel(camId, value);
        }
    }
}

void GstCaffUI::setTemporalCallback(GtkWidget *widget, gpointer data)
{
    int value = 0, oldValue = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    value = (int)gtk_range_get_value ((GtkRange*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "temporialLevel=";
        sprintf(cmd+strlen(cmd), "%d", value);
        cbData->mRemoteClient->setCmd2RemoteClient(camId,cmd);
    } else {
        cbData->mEventControl->getTemporalLevel(camId, oldValue);
        if(value != oldValue) {
            printf("change temporialLevel from %d to %d, camId: %d\n", oldValue, value, camId);
            cbData->mCamSetting[camId].temporialLevel = value;
            cbData->mEventControl->setTemporalLevel(camId, value);
        }
    }
}

void GstCaffUI::mirrorEffectChangedCallback(GtkWidget *widget, gpointer data)
{
    char *mirrorEffectModePtr = NULL;
    char mirrorEffectModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    mirrorEffectModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "mirrorEffect-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,mirrorEffectModePtr));
    } else {
        cbData->mEventControl->getMirrorEffectMode(camId, mirrorEffectModeOld);
        if(strcmp(mirrorEffectModeOld, mirrorEffectModePtr) != 0) {
            printf("change the mirrorEffect-mode from %s to %s, camId: %d\n", mirrorEffectModeOld, mirrorEffectModePtr, camId);
            strcpy(cbData->mCamSetting[camId].mirrorEffectMode, mirrorEffectModePtr);
            cbData->mEventControl->setMirrorEffectMode(camId, mirrorEffectModePtr);
        }
    }
    g_free(mirrorEffectModePtr);
}

void GstCaffUI::sceneModeChangedCallback(GtkWidget *widget, gpointer data)
{
    char *sceneModePtr = NULL;
    char sceneModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    sceneModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "scene-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,sceneModePtr));
    } else {
        cbData->mEventControl->getSceneMode(camId, sceneModeOld);
        if(strcmp(sceneModeOld, sceneModePtr) != 0) {
            printf("change the scene-mode from %s to %s, camId: %d\n", sceneModeOld, sceneModePtr, camId);
            strcpy(cbData->mCamSetting[camId].sceneMode, sceneModePtr);
            cbData->mEventControl->setSceneMode(camId, sceneModePtr);
        }
    }
    g_free(sceneModePtr);
}

void GstCaffUI::sensorResolutionChangedCallback(GtkWidget *widget, gpointer data)
{
    char *sensorModePtr = NULL;
    char sensorModeOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    sensorModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "sensor-mode=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,sensorModePtr));
    } else {
        cbData->mEventControl->getSensorMode(camId, sensorModeOld);
        if(strcmp(sensorModeOld, sensorModePtr) != 0) {
            printf("change the sensor-mode from %s to %s, camId: %d\n", sensorModeOld, sensorModePtr, camId);
            strcpy(cbData->mCamSetting[camId].sensorResolution, sensorModePtr);
            cbData->mEventControl->setSensorMode(camId, sensorModePtr);
        }
    }
    g_free(sensorModePtr);
}

void GstCaffUI::captureFpsChangedCallback(GtkWidget *widget, gpointer data)
{
    char *captureFpsPtr = NULL;
    char captureFpsOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    captureFpsPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);

    if(cbData->clientMode) {
        char cmd[64] = "capture-fps=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,captureFpsPtr));
    } else {
        cbData->mEventControl->getCaptureFps(camId, captureFpsOld);
        if(strcmp(captureFpsOld, captureFpsPtr) != 0) {
            printf("change the capture-fps from %s to %s, camId: %d\n", captureFpsOld, captureFpsPtr, camId);
            strcpy(cbData->mCamSetting[camId].captureFps, captureFpsPtr);
            cbData->mEventControl->setCaptureFps(camId, captureFpsPtr);
        }
    }
    g_free(captureFpsPtr);
}

void GstCaffUI::videoStandardChangedCallback(GtkWidget *widget, gpointer data)
{
    char *videoStandardPtr = NULL;
    char videoStandardOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    videoStandardPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "video-standard=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,videoStandardPtr));
    } else {
        cbData->mEventControl->getVideoStandard(camId, videoStandardOld);
        if(videoStandardPtr && strcmp(videoStandardOld, videoStandardPtr) != 0) {
            printf("change the video-standard from %s to %s, camId: %d\n", videoStandardOld, videoStandardPtr, camId);
            strcpy(cbData->mCamSetting[camId].videoStandard, videoStandardPtr);
            cbData->mEventControl->setVideoStandard(camId, videoStandardPtr);
        }
    }
    g_free(videoStandardPtr);
}

void GstCaffUI::setCustomAICCallback(GtkWidget *widget, gpointer data)
{
    gchar *AicParam;
    GtkTextIter start,end;
    GtkTextBuffer *buffer;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(cbData->mCustomAicParam));
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffer),&start,&end);
    AicParam = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer),&start,&end,FALSE);
    printf("The Custom aic param is %s, len = %zu\n", AicParam, strlen(AicParam));
    cbData->mEventControl->setCustomAicParam(camId, AicParam);
}

void GstCaffUI::advanceFeatureChangedCallback(GtkWidget *widget, gpointer data)
{
    char *advancedPtr = NULL;
    char advancedOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    advancedPtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
    if(cbData->clientMode) {
        char cmd[64] = "advance-feature=";
        cbData->mRemoteClient->setCmd2RemoteClient(camId,strcat(cmd,advancedPtr));
    } else {
        cbData->mEventControl->getAdvanceFeature(camId, advancedOld);
        if(strcmp(advancedOld, advancedPtr) != 0) {
            printf("change the advance-feature from %s to %s, camId: %d\n", advancedOld, advancedPtr, camId);
            strcpy(cbData->mCamSetting[camId].advanced, advancedPtr);
            cbData->mEventControl->setAdvanceFeature(camId, advancedPtr);
        }
    }
    g_free(advancedPtr);
}

void GstCaffUI::resetSettingCallback(GtkWidget *widget, gpointer data)
{
    char *advancedPtr = NULL;
    char advancedOld[64] = {0};
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    cbData->applyDefaultSetting(camId);
    cbData->updateUIforCamera(camId, false);
}

void GstCaffUI::restartCameraCallback(GtkWidget *widget, gpointer data)
{
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    CAMERA_STATE stateOld = CAMERA_STATE_STOP;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    if(cbData->clientMode) {
        cbData->mRemoteClient->setCmd2RemoteClient(camId, (char*)"state=stop");
        cbData->mRemoteClient->setCmd2RemoteClient(camId, (char*)"state=play");
    } else {
        cbData->mEventControl->getCameraState(camId, stateOld);
        if (stateOld != CAMERA_STATE_STOP)
            cbData->mEventControl->setCameraState(camId,CAMERA_STATE_STOP);
        cbData->mEventControl->waitSem();
        cbData->mEventControl->setCameraState(camId, CAMERA_STATE_START);
        cbData->mEventControl->postSem();
    }
}

void GstCaffUI::setDebugLevelCallback(GtkWidget *widget, gpointer data)
{
    char *debugPtr = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    debugPtr = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (debugPtr != NULL) {
        setenv("cameraDebug", debugPtr, 1);
        //now we use the environment to set debug level.
        cbData->mEventControl->setDebugLevel(camId, -1);
        printf("set cameraDebug Level: %s\n", debugPtr);
    }
}

void GstCaffUI::setDumpLevelCallback(GtkWidget *widget, gpointer data)
{
    char *dumpStr = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    dumpStr = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (dumpStr != NULL) {
        setenv("cameraDump", dumpStr, 1);
        setenv("cameraDumpPath", "./dump", 1);

        //now we use the environment to set debug level.
        cbData->mEventControl->setDebugLevel(camId, -1);
        printf("set cameraDump Level: %s\n", dumpStr);
    }

    return;
}


void GstCaffUI::saveLogButtonCallback(GtkWidget *widget, gpointer data)
{
    //TODO, do this in the future.
}

void GstCaffUI::drawAreaDrawCallback(GtkWidget * widget, cairo_t * cr, gpointer data)
{
    printf("enter %s\n", __func__);
    int width, height;
    GstCaffUI *cbData = (GstCaffUI *)data;
    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_rectangle (cr, 0, 0, width, height);
    cairo_fill (cr);
}

void GstCaffUI::drawAreaRealizeCallback(GtkWidget *widget, gpointer data)
{
    printf("enter %s\n", __func__);
    guintptr mEmbedXid;
    GstCaffUI *cbData = (GstCaffUI *)data;

    GtkWindow *window = (GtkWindow *)gtk_widget_get_window (widget);
    mEmbedXid = GDK_WINDOW_XID ((GdkWindow*)window);
    g_print ("widget: %p, Window realize: video window XID = %" G_GUINTPTR_FORMAT "\n", widget, mEmbedXid);
    if (widget == (GtkWidget *)cbData->mCam0Area) {
        cbData->mEventControl->setVideoWindow(0, mEmbedXid);
        cbData->mEventControl->setDrawingAreaPosition(0, (GtkWidget*)cbData->mCam0Area);
        //only one camera is supported in remote client display mode in current status.
        cbData->mRemoteClient->setVideoWindow(mEmbedXid);
        cbData->mRemoteClient->setDrawingAreaPosition((GtkWidget*)cbData->mCam0Area);
    } else if (widget == (GtkWidget *)cbData->mCam1Area) {
        cbData->mEventControl->setVideoWindow(1, mEmbedXid);
        cbData->mEventControl->setDrawingAreaPosition(1, (GtkWidget*)cbData->mCam1Area);
    } else if (widget == (GtkWidget *)cbData->mCam2Area) {
        cbData->mEventControl->setVideoWindow(2, mEmbedXid);
        cbData->mEventControl->setDrawingAreaPosition(2, (GtkWidget*)cbData->mCam2Area);
    } else if (widget == (GtkWidget *)cbData->mCam3Area) {
        cbData->mEventControl->setVideoWindow(3, mEmbedXid);
        cbData->mEventControl->setDrawingAreaPosition(3, (GtkWidget*)cbData->mCam3Area);
    }
}

//get real region window according to the position of mouse clicked
//the display area is 1440x960, the window size is REGION_WINDOW_SIZExREGION_WINDOW_SIZE
//1. calculate the crop width or crop height according to the display ratio and resolution ratio.
//   the operation is ignored if the coordinate in crop area.
//2. convert the coordinate value to a window.
//   select the coordinate as center point
//     left = x - REGION_WINDOW_SIZE;
//     top = y - REGION_WINDOW_SIZE;
//     right = x + REGION_WINDOW_SIZE;
//     bottom = y + REGION_WINDOW_SIZE;
int GstCaffUI::getRealMousePostion(int camId, int event_x, int event_y, struct TouchRegion &region)
{
    float disRatio = 0.0f, frameRatio = 0.0f;
    int cropWidth = 0, cropHeight = 0;
    int width = 0, height = 0;
    int disWidth = 1440, disHeight = 960;

    mEventControl->getResolution(camId, width, height);
    disRatio = 1.0 * disWidth / disHeight;
    frameRatio = 1.0 * width / height;
    if (disRatio > frameRatio) {
        //crop width;
        cropWidth = (disWidth - disHeight * frameRatio) / 2;
        if (event_x - cropWidth < 0 || event_x + cropWidth > disWidth)
            return -1;
        region.left = (event_x - REGION_WINDOW_SIZE - cropWidth) * height / disHeight;
        region.top = (event_y - REGION_WINDOW_SIZE) * height / disHeight;
        region.right = (event_x + REGION_WINDOW_SIZE - cropWidth) * height / disHeight;
        region.bottom = (event_y + REGION_WINDOW_SIZE) * height / disHeight;
    } else {
        //crop height;
        cropHeight = (disHeight - disWidth / frameRatio) / 2;
        if (event_y - cropHeight < 0 || event_y + cropHeight > disHeight)
            return -1;
        region.top = (event_y - REGION_WINDOW_SIZE - cropHeight) * width / disWidth;
        region.left = (event_x - REGION_WINDOW_SIZE) * width / disWidth;
        region.bottom = (event_y + REGION_WINDOW_SIZE - cropHeight) * width / disWidth;
        region.right = (event_x + REGION_WINDOW_SIZE) * width / disWidth;
    }

    if (region.right >= width)
        region.right = width;
    if (region.bottom >= height)
        region.bottom = height;
    if (region.left < 0)
        region.left = 0;
    if (region.top < 0)
        region.top = 0;

    //set the default weight value to 1, user can change
    //on another spinbutton.
    region.weight = 1;

    return 0;
}

//comprise the region vector to a string with a specific rule, and pass item
//to eventControl.
void GstCaffUI::setRegionPosition(int camId, bool updateSpin)
{
    char regionStr[128] = {0};
    char tmp1[30] = {0}, tmp2[10] = {0}, tmp3[40] = {0};


    for(int i = 0; i < mRegion.size(); i++) {
        snprintf(tmp1, 30, "%d,%d,%d,%d",
                mRegion[i].left, mRegion[i].top, mRegion[i].right, mRegion[i].bottom);
        snprintf(tmp2, 10, "%d", mRegion[i].weight);
        gtk_entry_set_text((GtkEntry *)(mRegionEntry[i]), tmp1);
        if (updateSpin)
            gtk_entry_set_text((GtkEntry *)(mWeightSpinbutton[i]), tmp2);
        snprintf(tmp3, 40, "%s,%s;", tmp1, tmp2);
        strcat(regionStr, tmp3);
    }
    printf("the regionStr is %s\n", regionStr);

    switch (mRegionState) {
        case REGION_STATE_AE:
            mEventControl->setAeRegion(camId, regionStr);
            break;
        case REGION_STATE_AWB:
            mEventControl->setAwbRegion(camId, regionStr);
            break;
        default:
            printf("ERROR: the region Mode is error\n");
    }
}

//the call back function when user click on the draw area.
void GstCaffUI::drawAreaPressCallback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    printf("enter %s\n", __func__);
    int ret = 0;
    int camId = 0;
    struct TouchRegion region;
    char *awbModePtr = NULL;
    char *regionPtr = NULL;
    GstCaffUI *cbData = (GstCaffUI *)data;
    //the region mode is only supported on one camera now.
    //this feature isn't supported on client Mode
    if (cbData->mCamNumber > 1 || cbData->clientMode)
        return;

    if (widget == (GtkWidget *)cbData->mCam0Area) {
        camId = 0;
    } else if (widget == (GtkWidget *)cbData->mCam1Area) {
        camId = 1;
    } else if (widget == (GtkWidget *)cbData->mCam2Area) {
        camId = 2;
    } else if (widget == (GtkWidget *)cbData->mCam3Area) {
        camId = 3;
    }
    ret = cbData->getRealMousePostion(camId, (int)event->x, (int)event->y, region);
    //the position is invalid
    if (ret < 0)
        return;
    awbModePtr = (char *)gtk_combo_box_text_get_active_text((GtkComboBoxText*)cbData->mAwbMode);
    if (strcmp(awbModePtr, "white_point") == 0) {
        char item[30] = {0};
        snprintf(item, 30, "%d,%d", region.left, region.top);
        strcpy(cbData->mCamSetting[camId].awbWp, item);
        gtk_entry_set_text((GtkEntry *)cbData->mAwbWpEntry, item);
        cbData->mEventControl->setAwbWp(camId, item);
        return;
    }

    if (cbData->mRegionState != REGION_STATE_NONE) {
        if (cbData->mRegion.size() >= MAX_REGION_NUM)
            cbData->mRegion.erase(cbData->mRegion.begin());
        cbData->mRegion.push_back(region);
        cbData->setRegionPosition(camId, true);
    }
    return;
}

//the call back function when user change the weight spinbutton by keyboard(+-1, +-10)
void GstCaffUI::weightChangedCallback(GtkWidget *widget,  GtkScrollType  type, gpointer data)
{
    char *str = NULL;
    int index = 0, value = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;
    //this feature isn't supported on client Mode
    if (cbData->clientMode)
        return;

    for (int i = 0; i < MAX_REGION_NUM; i++) {
        if (widget == (GtkWidget*)cbData->mWeightSpinbutton[i]) {
            index = i;
            break;
        }
    }
    if ((index+1) > cbData->mRegion.size())
        return;
    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        value = atoi(str);
        switch (type) {
          case GTK_SCROLL_STEP_UP:
            value += 1;
            break;
          case GTK_SCROLL_STEP_DOWN:
            value -= 1;
            break;
          case GTK_SCROLL_PAGE_UP:
            value += 10;
            break;
          case GTK_SCROLL_PAGE_DOWN:
            value -= 10;
            break;
          case GTK_SCROLL_START:
            value = 0;
            break;
          case GTK_SCROLL_END:
            value = 1000;
            break;
          default:
            printf("ERR: unknown type for gtk spin button\n");
            break;
        }
        if (value < 0)
            value = 0;
        if (value > 1000)
            value = 1000;
        cbData->mRegion[index].weight = value;
        cbData->setRegionPosition(camId, false);
    }
}

//the call back function when user input another value in weight spin button.
void GstCaffUI::setRegionWeightCallback(GtkWidget *widget, gpointer data)
{
    char *str = NULL;
    int value = 0, index = 0;
    GstCaffUI *cbData = (GstCaffUI *)data;
    int camId = cbData->mCurCameraId;
    if (camId < 0 || cbData->mCamSetting[camId].updateUI == true)
        return;

    for (int i = 0; i < MAX_REGION_NUM; i++) {
        if (widget == (GtkWidget*)cbData->mWeightSpinbutton[i]) {
            index = i;
            break;
        }
    }
    if ((index+1) > cbData->mRegion.size())
        return;

    str = (char *)gtk_entry_get_text((GtkEntry *)(widget));
    if (str != NULL) {
        value = atoi(str);
        if (value < 0)
            value = 0;
        if (value > 1000)
            value = 1000;
        cbData->mRegion[index].weight = value;
        cbData->setRegionPosition(camId, false);
    }
}

void GstCaffUI::mainWinowQuitCallback(GtkWidget *button, gpointer data)
{
    GstCaffUI *cbData = (GstCaffUI *)data;
    gtk_main_quit();
}

}
