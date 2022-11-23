# Gstreamer Antmedia Webrtc Connection
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
``` gcc File_Name.c  -o  File_Name  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/Linux_librws_static.a ```
## Compiling src code on Mac
``` gcc File_Name.c  -o  File_Name  `pkg-config --cflags --libs gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0  json-glib-1.0 ` ./libs/mac_m1_librws_static.a ```


## Running peer to peer Example 
Two peers will connect in p2p mode with Bi-Directional audio video stream <br>
On first peer ```./sendRecvAnt  --ip AMS_IP --streamid streamid ``` <br>
On second peer ```./sendRecvAnt  --ip AMS_IP --streamid streamid ``` <br>

## Running Send Recv Example
We can either send or recive streams from AMS 

#### sending stream To AMS from gstreamer 
video stream with id stream1 will be send to AMS <br>
``` sendRecvAnt --ip AMS_IP ```<br>
#### receiving stream:
will recive stream with id stream1 in Gstreamer  <br>
``` sendRecvAnt --ip AMS_IP --mode play -i stream1 ``` <br>
you can also specifie N number of stream ids  like this to recive multiple streams <br>
``` sendRecvAnt --ip AMS_IP --mode play -i stream1 -i stream2 -i stream3 ```

