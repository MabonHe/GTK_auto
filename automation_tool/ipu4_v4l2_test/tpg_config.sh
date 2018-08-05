# #!/bin/sh
# format=SGRBG8
# res=1920x1080

media-ctl -r

media-ctl -V '"Intel IPU4 TPG 0":0 [fmt:'$format'/'$res']' -v
media-ctl -l '"Intel IPU4 TPG 0":0 -> "Intel IPU4 TPG 0 capture":0[1]' -v

#DEV_NAME=`media-ctl -e "Intel IPU4 TPG 0 capture"`
# yavta -u -n1 --capture=3 -s $res -F -f $format $DEV_NAME

