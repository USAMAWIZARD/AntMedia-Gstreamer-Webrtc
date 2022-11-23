windows 
gcc -I ./ sendRecvAnt.c -o D:\\gstreamer\\1.0\\mingw_x86_64\\bin\\sendRecvAnt.exe ./libs/win64_x86_librws.lib $(pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 )
D:\\gstreamer\\1.0\\mingw_x86_64\\bin\\sendRecvAnt.exe  --ip AMS_IP

Mac
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./mac_m1_librws_static.a
./sendRecvAnt  --ip AMS_IP


Linux
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/Linux_librws_static.a
./sendRecvAnt  --ip AMS_IP
