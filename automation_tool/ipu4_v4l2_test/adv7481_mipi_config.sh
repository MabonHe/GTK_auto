# #!/bin/sh
# format=UYVY
# res=1920x1080

echo "*** media-ctl configuration begin ***"

ADV7481_PIXEL_ARRAY=`media-ctl -p | grep "entity.*adv7481 pixel" | cut -d ' ' -f 4- | cut -d ' ' -f -4`
ADV7481_BINNER=`media-ctl -p | grep "entity.*adv7481 binner" | cut -d ' ' -f 4- | cut -d ' ' -f -3`

media-ctl -r

media-ctl -V '"Intel IPU4 CSI-2 0":0 [fmt:'$format'/'$res']'
media-ctl -V '"'"$ADV7481_PIXEL_ARRAY"'":0 [fmt:UYVY/1920x1080]'
media-ctl -V '"'"$ADV7481_BINNER"'":0 [fmt:UYVY/1920x1080]'
media-ctl -V '"'"$ADV7481_BINNER"'":0 [compose:(0,0)/'$res']'
media-ctl -V '"'"$ADV7481_BINNER"'":1 [fmt:'$format'/'$res']'

media-ctl -l '"'"$ADV7481_PIXEL_ARRAY"'":0 -> "'"$ADV7481_BINNER"'":0[1]'
media-ctl -l '"'"$ADV7481_BINNER"'":1 -> "Intel IPU4 CSI-2 0":0[1]'
media-ctl -l '"Intel IPU4 CSI-2 0":1 -> "Intel IPU4 CSI-2 0 capture":0[1]'

echo "*** media-ctl configuration end ***"


