# #!/bin/sh
# res=1920x1080
# format=SGRBG10

# subdev configuration
yavta -w "0x009f0901 0" /dev/v4l-subdev9
yavta -w "0x009e0903 100" /dev/v4l-subdev8
yavta -w "0x009e0902 3000" /dev/v4l-subdev8
yavta -w "0x009e0901 3000" /dev/v4l-subdev8
yavta -w "0x00980911 1000" /dev/v4l-subdev8
yavta -w "0x00980915 0" /dev/v4l-subdev8
yavta -w "0x00980914 1" /dev/v4l-subdev8
yavta -w "0x00982901 8" /dev/v4l-subdev9
yavta -w "0x00982902 104" /dev/v4l-subdev9

# pixel array format setup
media-ctl -V '"imx214 pixel array 1-001a":0 [fmt:SGRBG10/4208x3120]' -v

# binner format setup
media-ctl -V '"imx214 binner 1-001a":0 [fmt:SGRBG10/4208x3120]' -v
media-ctl -V '"imx214 binner 1-001a":0 [compose:(0,0)/2104x1560]' -v
media-ctl -V '"imx214 binner 1-001a":1 [fmt:SGRBG10/2104x1560]' -v

# scaler format setup
media-ctl -V '"imx214 scaler 1-001a":0 [fmt:SGRBG10/2104x1560]' -v
media-ctl -V '"imx214 scaler 1-001a":1 [fmt:'$format'/'$res']' -v
media-ctl -V '"imx214 scaler 1-001a":1 [crop:(0,0)/'$res']' -v

# CSI2 port 0 format setup
media-ctl -V '"Intel IPU4 CSI-2 0":0 [fmt:'$format'/'$res']' -v
media-ctl -V '"Intel IPU4 CSI-2 0":1 [fmt:'$format'/'$res']' -v

# CSI2 BE format setup
media-ctl -V '"Intel IPU4 CSI2 BE":0 [fmt:'$format'/'$res']' -v
media-ctl -V '"Intel IPU4 CSI2 BE":1 [fmt:'$format'/'$res']' -v

# create link for entities
media-ctl -l '"imx214 scaler 1-001a":1 -> "Intel IPU4 CSI-2 0":0[1]' -v
media-ctl -l '"Intel IPU4 CSI-2 0":1 -> "Intel IPU4 CSI2 BE":0[1]' -v
media-ctl -l '"Intel IPU4 CSI2 BE":1 -> "Intel IPU4 CSI2 BE capture":0[1]' -v

# capture RAW8 1080p image from BE capture
# yavta -u -n1 --capture=5 -s 1920x1080 -F -f SGRBG8 /dev/video5


