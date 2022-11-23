### How to install Gstreamer :
# Ubuntu : 

Run the following command:

apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 libjson-glib-dev gstreamer1.0-nice gstreamer1.0-pulseaudio 

# Windows :

There are two installer binaries gstreamer-1.0 and gstreamer-devel make sure you install both of them

download the windows  installer from https://gstreamer.freedesktop.org/download/#windows

# Mac :

There are two installer binaries gstreamer-1.0 and gstreamer-devel make sure you install both of them

download the Mac installer from https://gstreamer.freedesktop.org/download/#macos

export  PKG_CONFIG_PATH="/Library/Frameworks/GStreamer.framework/Versions/Current/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"


### Build and Run p2p Example 

## Compiling src code Linux
gcc sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/Linux_librws_static.a
