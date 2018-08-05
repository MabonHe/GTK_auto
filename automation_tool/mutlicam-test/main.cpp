/*
 * Copyright (C) 2018-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "linux/videodev2.h"
#include "api/ICamera.h"
#include "api/Parameters.h"


using namespace std;
using namespace icamera;


int main(int argc, char** argv) {

	int mainCamId = 0;
	int secondCamId = 0;
	int caseNo = 0;
	int ret = 0;
	int i,j;
	
	/*handle testcase parameters*/
	if(argc != 2) {
		printf("USE COMMAND [mutlicam-test caseA] for case [Config A Start A Config B Start B] \n");
		printf("USE COMMAND [mutlicam-test caseB] for case [Config A Config B Start A Start B] \n");
		return 0;
	}

	char* cmd = argv[1];
	
	if(!strcmp(cmd,"caseA")){
		caseNo = 1;
	} else if(!strcmp(cmd,"caseB")){
		caseNo = 2;
	} else {
		printf("USE COMMAND [mutlicam-test caseA] for case [Config A Start A Config B Start B] \n");
		printf("USE COMMAND [mutlicam-test caseB] for case [Config A Config B Start A Start B] \n");
		return 0;
	}


	/* define cameraName here */
	char* cameraName = "mondello";
	char* cameraName2 = "mondello-2";
	printf("cameras are %s and %s\n",cameraName, cameraName2);
	
	/* init camera device */
	ret = camera_hal_init();
	if(ret){
		printf("camera_hal_init TEST FAIL!\n");
		return ret;
	}

	/* get camera id*/
	int camNum= get_number_of_cameras();

	for (int i = 0; i < camNum; i++) {
		camera_info_t info;
		get_camera_info(i, info);
		if (!strcmp(info.name,cameraName)) {
			mainCamId = i;
 			continue;
		}
		if (!strcmp(info.name,cameraName2)) {
			secondCamId = i;
			continue;
		}
	} 
					
	if ((mainCamId == 0)||(secondCamId == 0)) {
		printf("NOT two Cams!! return\n");
		return 0;
	}

	
	/* open cameras*/
	printf("mainCameraId:%d camera_device_open\n",mainCamId);
	ret = camera_device_open(mainCamId);
	if(ret) {
		printf("camera_device_open CAM1 TEST FAIL!\n");
		return ret;
	}

	printf("secondCamId:%d camera_device_open\n",secondCamId);
	ret = camera_device_open(secondCamId);
	if(ret) {
		printf("camera_device_open CAM2 TEST FAIL!\n");
		return ret;
	}

	/* hardcode the stream info*/ 
	stream_config_t stream_list;
    stream_t streams[1];
	streams[0].id=0;
    streams[0].width = 640;
    streams[0].height = 480;
    streams[0].format = V4L2_PIX_FMT_UYVY;
    streams[0].field = 0;
	streams[0].stride = 1280;
	streams[0].size = 615680;
 	streams[0].memType = V4L2_MEMORY_USERPTR;
 	stream_list.num_streams = 1;
	stream_list.streams = streams;
    stream_list.operation_mode = 2;

	const int page_size = getpagesize();
	const int bufferCount = 8;
	
	/* allocate buffer */
    camera_buffer_t *buffer;
    camera_buffer_t buffers[bufferCount];
	camera_buffer_t *buffer2;
    camera_buffer_t buffers2[bufferCount];
	camera_buffer_t *buf1;
	camera_buffer_t *buf2;

	if(caseNo == 1){
		printf("RuncaseA [Config A Start A Config B Start B] \n");
		
		/* camera 1 config*/
	    ret = camera_device_config_streams(mainCamId,  &stream_list);
		if(ret) {
			printf("camera_device_config_streams CAM1 TEST FAIL!\n");
			return ret;
		}

		for (j = 0, buffer = buffers; j < bufferCount; j++, buffer++) {
			memset(buffer, 0, sizeof(camera_buffer_t));
			buffer->s = streams[0];
		
			ret = posix_memalign(&buffer->addr, page_size, buffer->s.size);
			if(ret) {
				printf("posix_memalign CAM1 TEST FAIL!\n");
				return ret;
			}
			ret = camera_stream_qbuf(mainCamId, &buffer);
			if(ret) {
				printf("camera_stream_qbuf CAM1 TEST FAIL!\n");
				return ret;
			}
		}

		/* camera 1 start*/
		ret = camera_device_start(mainCamId);
		if(ret) {
			printf("camera_device_start CAM1 TEST FAIL!\n");
			return ret;
		}

		/* camera 2 config*/
		ret = camera_device_config_streams(secondCamId,  &stream_list);
		if(ret) {
			printf("camera_device_config_streams CAM2 TEST FAIL!\n");
			return ret;
		}

		for (j = 0, buffer2 = buffers2; j < bufferCount; j++, buffer2++) {
			memset(buffer2, 0, sizeof(camera_buffer_t));
			buffer2->s = streams[0];
	
			ret = posix_memalign(&buffer2->addr, page_size, buffer2->s.size);
			if(ret) {
				printf("posix_memalign CAM2 TEST FAIL!\n");
				return ret;
			}
					
			ret = camera_stream_qbuf(secondCamId, &buffer2);
			if(ret) {
				printf("camera_stream_qbuf CAM2 TEST FAIL!\n");
				return ret;
			}	
		}
		
		ret = camera_device_start(secondCamId);
		if(ret) {
			printf("camera_device_start CAM2 TEST FAIL!\n");
			return ret;
		}
	}

	if(caseNo == 2) {
		printf("Run caseB [Config A Config B Start A Start B] \n");
		
		ret = camera_device_config_streams(mainCamId,  &stream_list);
		if(ret) {
			printf("camera_device_config_streams CAM1 TEST FAIL!\n");
			return ret;
		}
		ret = camera_device_config_streams(secondCamId,  &stream_list);
		if(ret) {
			printf("camera_device_config_streams CAM2 TEST FAIL!\n");
			return ret;
		}

		for (j = 0, buffer = buffers; j < bufferCount; j++, buffer++) {
			memset(buffer, 0, sizeof(camera_buffer_t));
			buffer->s = streams[0];
	
			ret = posix_memalign(&buffer->addr, page_size, buffer->s.size);	
			ret = camera_stream_qbuf(mainCamId, &buffer);
		}
				
		ret = camera_device_start(mainCamId);
		if(ret) {
			printf("camera_device_start CAM1 TEST FAIL!\n");
			return ret;
		}

		for (j = 0, buffer2 = buffers2; j < bufferCount; j++, buffer2++) {
			memset(buffer2, 0, sizeof(camera_buffer_t));
			buffer2->s = streams[0];
	
			ret = posix_memalign(&buffer2->addr, page_size, buffer2->s.size);	
			ret = camera_stream_qbuf(secondCamId, &buffer2);
		}
		
		ret = camera_device_start(secondCamId);
		if(ret) {
			printf("camera_device_start CAM1 TEST FAIL!\n");
			return ret;
		}
				
	}
	
	
	printf("camera_device_close\n");
	camera_device_close(secondCamId);
	camera_device_close(mainCamId);
	
	printf("TEST PASS!!!\n");
	
	
	/* deinit camera device */
   	camera_hal_deinit();

	return 0;
}

