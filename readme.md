# Gstreamer Antmedia Webrtc Connection
This program will help you to connect AntMedia to Gstreamer.
There are three modes available.
1. you can connect two peers with each other in p2p mode using AMS server.
2. you can publish a audio video streams to AMS from gstreamer in publish mode
3. you can play the streams in gstreamer in play mode.

## How to install Gstreamer :
### Ubuntu : 

Run the following command:
``` 
apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 libjson-glib-dev gstreamer1.0-nice gstreamer1.0-pulseaudio 
```

### Windows :

There are two installer binaries gstreamer-1.0 and gstreamer-devel make sure you install both of them <br>

download the windows  installer from https://gstreamer.freedesktop.org/download/#windows

### Mac :

There are two installer binaries gstreamer-1.0 and gstreamer-devel make sure you install both of them <br>

download the Mac installer from https://gstreamer.freedesktop.org/download/#macos <br>
Add to path ``` export  PKG_CONFIG_PATH="/Library/Frameworks/GStreamer.framework/Versions/Current/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}" ```


## Build and Running Examples

I have supplied binaries of compiled websocket libraries for winx86_64 , macos (m1) and linux(x86_64) in libs directory you can compile the websocket library by yourself from [here](https://github.com/OlehKulykov/librws) if you are using some other operating system or CPU architecture.

## Compiling src code on Linux
``` gcc ./sendRecvAnt  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/Linux_librws_static.a ```
## Compiling src code on Mac
``` gcc ./sendRecvAnt.c  -o  sendRecvAnt  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/mac_m1_librws_static.a ```


## peer to peer  Mode
Two peers will connect in p2p mode with Bi-Directional audio video stream <br>
On first peer ```./sendRecvAnt --mode p2p --ip AMS_IP --streamid streamid ``` <br>
On second peer ```./sendRecvAnt --mode p2p --ip AMS_IP --streamid streamid ``` <br>

by default videotest src will be streamed if you want to use a file for streaming please specify file name with as -f test.mp4<br>
only mp4 file will work
## Send or receive Streams from gstreamer to AMS Server 
We can either send or receive streams from gstreamer to  AMS 

#### sending stream To AMS from gstreamer  
video stream with id stream1 will be send to.Publish is the defalut mode<br>
``` sendRecvAnt --ip AMS_IP -i stream1 ```<br>
#### receiving stream In Gstreamer from AMS:
will receive  stream with id stream1 in Gstreamer  <br>
``` sendRecvAnt --ip AMS_IP --mode play -i stream1 ``` <br>
you can also specifie N number of stream ids  like this to receive multiple streams <br>
``` sendRecvAnt --ip AMS_IP --mode play -i stream1 -i stream2 -i stream3 ``` and so on
