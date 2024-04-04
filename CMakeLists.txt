# CMake for AntMediaGstreamerWebRTC

cmake_minimum_required(VERSION 3.29)

cmake_policy(SET CMP0048 NEW)

project(
  AntMediaGstreamerWebRTC
  VERSION 1.0
  LANGUAGES C)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

add_subdirectory(librws)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTLIBS REQUIRED
        gobject-2.0
        glib-2.0
        gstreamer-webrtc-1.0
        gstreamer-sdp-1.0
        gstreamer-pbutils-1.0
        json-glib-1.0)

include_directories(
        ${GSTLIBS_INCLUDE_DIRS}
        librws)

add_executable(sendRecvAnt sendRecvAnt.c)
add_dependencies(sendRecvAnt rws_static)
target_link_libraries(sendRecvAnt PRIVATE ${GSTLIBS_LIBRARIES} rws_static)

install(TARGETS sendRecvAnt DESTINATION bin)
