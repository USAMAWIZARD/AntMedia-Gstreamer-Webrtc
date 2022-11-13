#windows
all:
	gcc  sendRecvAnt.c   -o D:\\gstreamer\\1.0\\mingw_x86_64\\bin\sendRecvAnt.exe  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0`  ./libs/win64_librws.lib
run:
	D:\\gstreamer\\1.0\\mingw_x86_64\\bin\sendRecvAnt.exe
clean:
	D:\\gstreamer\\1.0\\mingw_x86_64\\bin\sendRecvAnt.exe