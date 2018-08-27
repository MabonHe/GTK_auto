INCLUDE_DIRECTORIES(. ../3rdParty/OpenCV/Include ../LibContentAnalyzer ../LibSimilarityCheck ../LibToolFunction)

SET(SRC_LIST analyzer.cpp)

add_executable(analyzer_app ${SRC_LIST})

SET(OPENCV_LIB_PATH ${CameraValidation_SOURCE_DIR}/3rdParty/OpenCV/Lib)
SET(TINYXML_LIB_PATH ${CameraValidation_SOURCE_DIR}/3rdParty/TinyXML/Lib)

target_link_libraries(analyzer_app ContentAnalyzer 
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
                      )
