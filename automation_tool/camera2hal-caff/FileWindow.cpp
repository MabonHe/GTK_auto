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
#include "FileWindow.h"
#include "EventControl.h"

namespace gstCaff {

FileWindow::FileWindow(bool *curCtrl):
    mWindow(NULL)
    ,mOpen(NULL)
    ,mSave(NULL)
    ,mQuit(NULL)
    ,mOpenDialog(NULL)
    ,mSaveDialog(NULL)
    ,mCurCtrl(curCtrl)
{
    printf("@%s\n", __func__);
}

int FileWindow::init(GtkBuilder *builder)
{
    printf("@%s\n", __func__);
    getWidgetFromBuilder(builder);
    registerSignalForWidget();

    return 0;
}

void FileWindow::reSetWindowWithEnv(void)
{
    //TODO, do this in the future.
    printf("@%s function isn't inplemented\n", __func__);
}

void FileWindow::getWidgetFromBuilder(GtkBuilder *builder)
{
    mWindow = GTK_WINDOW(gtk_builder_get_object (builder, "FileWindow"));
    mOpen = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "Open"));
    mSave = GTK_TOGGLE_BUTTON(gtk_builder_get_object (builder, "Save"));
    mQuit = GTK_BUTTON(gtk_builder_get_object (builder, "Quit"));
    mOpenDialog = gtk_file_chooser_dialog_new("Open file...", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
                            "gtk-cancel", GTK_RESPONSE_CANCEL, "gtk-open", GTK_RESPONSE_ACCEPT, NULL);
    mSaveDialog = gtk_file_chooser_dialog_new("Save file...", NULL, GTK_FILE_CHOOSER_ACTION_SAVE,
                            "gtk-cancel", GTK_RESPONSE_CANCEL, "gtk-save", GTK_RESPONSE_ACCEPT, NULL);
}

void FileWindow::registerSignalForWidget(void)
{
    g_signal_connect(G_OBJECT(mOpen), "toggled", G_CALLBACK(FileWindow::openButtonCallback), this);
    g_signal_connect(G_OBJECT(mSave), "toggled", G_CALLBACK(FileWindow::saveButtonCallback), this);
    g_signal_connect(G_OBJECT(mQuit), "clicked", G_CALLBACK(FileWindow::quitButtonCallback), this);
}

void FileWindow::openButtonCallback(GtkWidget *toggle, gpointer data)
{
    int i;
    FileWindow *cbData = (FileWindow *)data;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle))) {
        char * openFile = NULL;
        //gtk_widget_set_opacity(GTK_WIDGET(cbData->mOpenDialog), 0.7);
        gint result = gtk_dialog_run (GTK_DIALOG (cbData->mOpenDialog));
        if (result == GTK_RESPONSE_ACCEPT) {
            openFile = (char *)gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (cbData->mOpenDialog));
            for (i = 0; i < MAX_CAMERA_NUM; i++) {
                if (cbData->mCurCtrl[i])
                    break;
            }
            if (i == MAX_CAMERA_NUM)
                printf("ERR: the open file must to be put one camera.\n");
            else {
                //we put the open file to the first camera if the current control > 1
                printf("the open file name is %s\n", openFile);
            }
        }
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), 0);
        gtk_widget_hide ((GtkWidget *)(cbData->mOpenDialog));
    } else {
        gtk_widget_hide ((GtkWidget *)(cbData->mOpenDialog));
    }
}

void FileWindow::saveButtonCallback(GtkWidget *toggle, gpointer data)
{
    int i;
    FileWindow *cbData = (FileWindow *)data;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle))) {
        char *saveFile = NULL;
        //gtk_widget_set_opacity(GTK_WIDGET(cbData->mSaveDialog), 0.7);
        gint result = gtk_dialog_run (GTK_DIALOG (cbData->mSaveDialog));
        if (result == GTK_RESPONSE_ACCEPT) {
            saveFile = (char *)gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(cbData->mSaveDialog));
            for (i = 0; i < MAX_CAMERA_NUM; i++) {
                if (cbData->mCurCtrl[i])
                    break;
            }
            if (i == MAX_CAMERA_NUM)
                printf("ERR: the save file must to be put one camera.\n");
            else {
                //we put the save file to the first camera if the current control > 1
                printf("the save file name is %s\n", saveFile);
            }
        }
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), 0);
        gtk_widget_hide ((GtkWidget *)cbData->mSaveDialog);
    } else {
        gtk_widget_hide ((GtkWidget *)cbData->mSaveDialog);
    }
}

void FileWindow::quitButtonCallback(GtkWidget *button, gpointer data)
{
    //quit play app.
    gtk_main_quit();
}

}
