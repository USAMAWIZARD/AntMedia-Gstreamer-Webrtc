windows 
gcc -I ./ sendRecvAnt.c -o D:\\gstreamer\\1.0\\mingw_x86_64\\bin\\sendRecvAnt.exe ./libs/win64_x86_librws.lib $(pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 )
GST_DEBUG=3 D:\\gstreamer\\1.0\\mingw_x86_64\\bin\\sendRecvAnt.exe  --ip AMS_IP

Mac
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./mac_librws_static.a


Linux
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/Linux_librws_static.a 