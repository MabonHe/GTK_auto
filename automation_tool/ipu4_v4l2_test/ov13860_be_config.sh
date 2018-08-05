
# #!/bin/sh
# res=1920x1080
# format=SGRBG10

src_subdev=$(media-ctl -e "ov13860 binner 2-0010")
yavta -w "0x009f0901 1" $src_subdev

media-ctl -r

# pixel array format setup
media-ctl -V '"ov13860 pixel array 2-0010":0 [fmt:'$format'/4224x3120]' -v

# binner format setup
media-ctl -V '"ov13860 binner 2-0010":0 [fmt:'$format'/4224x3120]' -v
media-ctl -V '"ov13860 binner 2-0010":0 [compose:(0,0)/'$res']' -v
media-ctl -V '"ov13860 binner 2-0010":1 [fmt:'$format'/'$res']' -v

# scaler format setup
# media-ctl -V '"ov13860 scaler 2-0010":0 [fmt:'$format'/2104x1560]' -v
# media-ctl -V '"ov13860 scaler 2-0010":1 [fmt:'$format'/'$res']' -v
# media-ctl -V '"ov13860 scaler 2-0010":1 [crop:(0,0)/'$res']' -v

# CSI2 port 0 format setup
media-ctl -V '"Intel IPU4 CSI-2 0":0 [fmt:'$format'/'$res']' -v
media-ctl -V '"Intel IPU4 CSI-2 0":1 [fmt:'$format'/'$res']' -v

# CSI2 BE format setup
media-ctl -V '"Intel IPU4 CSI2 BE":0 [fmt:'$format'/'$res']' -v
media-ctl -V '"Intel IPU4 CSI2 BE":1 [fmt:'$format'/'$res']' -v

# create link for entities
media-ctl -l '"ov13860 binner 2-0010":1 -> "Intel IPU4 CSI-2 0":0[1]' -v
media-ctl -l '"Intel IPU4 CSI-2 0":1 -> "Intel IPU4 CSI2 BE":0[1]' -v
media-ctl -l '"Intel IPU4 CSI2 BE":1 -> "Intel IPU4 CSI2 BE capture":0[1]' -v

# capture RAW8 1080p image from BE capture
# yavta -u -n1 --capture=5 -s '$format' -F -f '$format' /dev/video5


