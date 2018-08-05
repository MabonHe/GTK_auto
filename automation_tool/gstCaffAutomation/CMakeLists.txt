include(FindPkgConfig)
PKG_CHECK_MODULES(BUILD gstreamer-1.0 gstreamer-base-1.0 glib-2.0 gstreamer-video-1.0 gtk+-3.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
                    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/OpenCV/Include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/../LibContentAnalyzer 
                    ${CMAKE_CURRENT_SOURCE_DIR}/../LibSimilarityCheck 
                    ${CMAKE_CURRENT_SOURCE_DIR}/../LibToolFunction
                    ${CMAKE_CURRENT_SOURCE_DIR}/../camera2hal-caff
                    /usr/include
                    ) 
                    
foreach(P ${BUILD_INCLUDE_DIRS})
    include_directories(${P})
endforeach()

foreach(P ${BUILD_LIBRARY_DIRS})
    link_directories(${P})
endforeach()

SET(GST_CAFF_SOURCE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/../camera2hal-caff)
                
SET(SRC_LIST    ${GST_CAFF_SOURCE_FOLDER}/EventControl.cpp
                ${GST_CAFF_SOURCE_FOLDER}/FileWindow.cpp
                ${GST_CAFF_SOURCE_FOLDER}/gstCaffUI.cpp
                #${GST_CAFF_SOURCE_FOLDER}/main.cpp
                ${GST_CAFF_SOURCE_FOLDER}/PipelineManage.cpp
                ${GST_CAFF_SOURCE_FOLDER}/RemoteClient.cpp
                ${GST_CAFF_SOURCE_FOLDER}/RemoteProxy.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestConfig.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestPerformance.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestQueue.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestSampleCallback.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestScheduler.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestTool.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/gstCaffAutoTestMain.cpp
    )

add_executable(gstCaffAutoTest ${SRC_LIST})

SET(OPENCV_LIB_PATH ${CameraValidation_SOURCE_DIR}/3rdParty/OpenCV/Lib)
SET(TINYXML_LIB_PATH ${CameraValidation_SOURCE_DIR}/3rdParty/TinyXML/Lib)

target_link_libraries(gstCaffAutoTest   ContentAnalyzer 
                                        SimilarityCheck 
                                        ToolFunction 
                                        ${OPENCV_LIB_PATH}/libopencv_highgui.so
                                        ${OPENCV_LIB_PATH}/libopencv_imgproc.so
                                        ${OPENCV_LIB_PATH}/libopencv_core.so
                                        ${OPENCV_LIB_PATH}/libopencv_legacy.so
                                        ${OPENCV_LIB_PATH}/libopencv_video.so
                                        ${OPENCV_LIB_PATH}/libopencv_flann.so
                                        ${OPENCV_LIB_PATH}/libopencv_features2d.so
                                        ${OPENCV_LIB_PATH}/libopencv_calib3d.so
                                        ${OPENCV_LIB_PATH}/libopencv_ml.so
                                        ${OPENCV_LIB_PATH}/libopencv_objdetect.so
                                        ${OPENCV_LIB_PATH}/libopencv_contrib.so
                                        ${TINYXML_LIB_PATH}/tinystr.o
                                        ${TINYXML_LIB_PATH}/tinyxml.o
                                        ${TINYXML_LIB_PATH}/tinyxmlerror.o
                                        ${TINYXML_LIB_PATH}/tinyxmlparser.o
                                        ${BUILD_LIBRARIES}
                                        pthread
                                        gstallocators-1.0
                      )