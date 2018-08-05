INCLUDE_DIRECTORIES(. ../3rdParty/OpenCV/Include)

SET(SRC_LIST    LibConteneAnalyzerAF.cpp                LibContentAnalyzerFocus.cpp
                LibContentAnalyzer3AAutoConsistent.cpp  LibContentAnalyzerFOV.cpp
                LibContentAnalyzerAE.cpp                LibContentAnalyzerGDC.cpp
                LibContentAnalyzerATE.cpp               LibContentAnalyzerGreenCorruption.cpp
                LibContentAnalyzerAutoAWB.cpp           LibContentAnalyzerHDR.cpp
                LibContentAnalyzerBase.cpp              LibContentAnalyzerHue.cpp
                LibContentAnalyzerBrightness.cpp        LibContentAnalyzerJpegQuality.cpp
                LibContentAnalyzerCheckMuteMode.cpp     LibContentAnalyzerSimilarity.cpp
                LibContentAnalyzerColorEffect.cpp       LibContentAnalyzerThumbnail.cpp
                LibContentAnalyzerCommonFunction.cpp    LibContentAnalyzerTorch.cpp
                LibContentAnalyzerContrast.cpp          LibContentAnalyzerULL.cpp
                LibContentAnalyzer.cpp                  LibContentAnalyzerVideoBasic.cpp
                LibContentAnalyzerExposure.cpp          LibContentAnalyzerWhiteBalance.cpp
                LibContentAnalyzerFaceDetection.cpp     LibContentAnalyzerZoom.cpp
                LibContentAnalyzerCheckChessboard.cpp LibContentAnalyzerCameraPosition.cpp
                LibContentAnalyzerColorCast.cpp )

add_library(ContentAnalyzer ${SRC_LIST})