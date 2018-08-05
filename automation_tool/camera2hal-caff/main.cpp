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
#include "EventControl.h"
#include "RemoteProxy.h"
#include "gstCaffUI.h"


using namespace gstCaff;

int main(int argc, char *argv[])
{
    bool needUpdateUi = true;
    GtkBuilder *builder = NULL;
    GstCaffUI *gstCaffUI = NULL;
    EventControl *eventControl = NULL;
    RemoteProxy *remoteProxy = NULL;
    RemoteClient *remoteClient = NULL;

    if (argc > 1 && (strcmp(argv[1], "-S") == 0))
        needUpdateUi = false;

    eventControl = EventControl::getInstance(needUpdateUi);
    eventControl->init();
    printf("new EventControl() return %p\n",eventControl);

    remoteProxy = new RemoteProxy();
    remoteProxy->tcp_server(needUpdateUi) ;

    remoteClient = new RemoteClient();

    if (argc > 1 && (strcmp(argv[1], "-S") == 0)) {
        //app work on server mode.
        char buf[20];
        while(1) {
            scanf("%19s", (char *)&buf);
            if ((strcmp(buf, "quit") == 0)
               ||(strcmp(buf, "exit") == 0))
                return 0;
            usleep(2000000);
        }
    } else {
        gtk_init(&argc, &argv);
        builder = gtk_builder_new();
        if(!gtk_builder_add_from_file(builder, "./CAFF.glade", NULL)) {
            printf("cannot load file!\n");
            return -1;
        }

        gstCaffUI = new GstCaffUI(eventControl, remoteClient);
        gstCaffUI->init(builder);
        gstCaffUI->showUI();

        gtk_main();
    }

    return 0;
}

