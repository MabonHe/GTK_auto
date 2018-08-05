#!/usr/bin/python
#
#  INTEL CONFIDENTIAL
#
#  Copyright (C) 2016 Intel Corporation
#  All Rights Reserved.
#
#  The source code contained or described herein and all documents
#  related to the source code ("Material") are owned by Intel Corporation
#  or licensors. Title to the Material remains with Intel
#  Corporation or its licensors. The Material contains trade
#  secrets and proprietary and confidential information of Intel or its
#  licensors. The Material is protected by worldwide copyright
#  and trade secret laws and treaty provisions. No part of the Material may
#  be used, copied, reproduced, modified, published, uploaded, posted,
#  transmitted, distributed, or disclosed in any way without Intel's prior
#  express written permission.
#
#  No License under any patent, copyright, trade secret or other intellectual
#  property right is granted to or conferred upon you by disclosure or
#  delivery of the Materials, either expressly, by implication, inducement,
#  estoppel or otherwise. Any license under such intellectual property rights
#  must be express and approved by Intel in writing.
#
from socket import *
import sys

PORT = 4321
BUFSIZ = 1024
ADDR = (sys.argv[1], PORT)

tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR)

def printhelp():
    print "properties support lists as below, can input them in one line,use ',' to seperate, no space needed"
    print "for example: cameraId=0;state=play,resolution=320x240\n"
    print "cameraId"
    print "state"
    print "resolution"
    print "format"
    print "interlace"
    print "deinterlace"


if len(sys.argv) > 2:
    tcpCliSock.send(sys.argv[2])
    data1 = tcpCliSock.recv(BUFSIZ)
    print data1
else:
    while True:
        data = raw_input('> ')
        if not data:
            continue
        if data == 'exit':
            tcpCliSock.close()
            exit()
        if data == 'help':
            printhelp()
        else:
            tcpCliSock.send(data)
            data1 = tcpCliSock.recv(BUFSIZ)
            if not data1:
                break
            print data1

tcpCliSock.close()

