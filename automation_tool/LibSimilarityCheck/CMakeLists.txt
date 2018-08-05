INCLUDE_DIRECTORIES(. ../3rdParty/OpenCV/Include)

SET(SRC_LIST Filter.cpp Match.cpp Psnr.cpp Ssim.cpp)

add_library(SimilarityCheck ${SRC_LIST})