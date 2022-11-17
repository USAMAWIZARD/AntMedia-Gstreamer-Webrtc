windows 
gcc -I ./ sendRecvAnt.c -o D:\\gstreamer\\1.0\\mingw_x86_64\\bin\\sendRecvAnt.exe ./libs/win64_librws.lib $(pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 )

Mac
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-rtp-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0  gstreamer-net-1.0  gstreamer-pbutils-1.0` ./mac_librws_static.a


GST_DEBUG=3 D:\\gstreamer\\1.0\\mingw_x86_64\\bin\\sendRecvAnt.exe


Linux
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-rtp-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0  gstreamer-net-1.0  gstreamer-pbutils-1.0` ./libs/Linux_librws_static.a 