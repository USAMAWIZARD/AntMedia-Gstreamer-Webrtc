#if __APPLE__
#define VIDEO_SINK "osxaudiosink"
#else
#define VIDEO_SINK "autovideosink"
#endif
#define GST_USE_UNSTABLE_API
#define STUN_SERVER "stun://stun.l.google.com:19302"
#define TURN_SERVER ""
#define AUDIO_ENCODE "  ! audioconvert ! audioresample   ! opusenc bitrate=192000  ! rtpopuspay "
#define VIDEO_ENCODE " ! timeoverlay time-mode=2 halignment=right valignment=bottom   ! videoconvert ! video/x-raw,format=I420 ! x264enc  speed-preset=3 tune=zerolatency ! rtph264pay "
#define RTP_CAPS_H264 " application/x-rtp,media=video,encoding-name=H264,payload=96,clock-rate=90000 "
#define RTP_CAPS_OPUS " application/x-rtp,media=audio,encoding-name=OPUS,payload=97 "

#include <stdlib.h>
#include <glib.h>
#include <gst/gst.h>
#include <gst/webrtc/webrtc.h>
#include <json-glib/json-glib.h>
#include "librws.h"

gboolean is_joined = FALSE;
GstElement *gst_pipe;
static gchar *ws_server_addr = "";
static gint ws_server_port = 5080;
static rws_socket _socket = NULL;
gchar *mode = "publish";
gchar **play_streamids = NULL;
gchar *filename = "";

static GOptionEntry entries[] =
    {
        {"ip", 's', 0, G_OPTION_ARG_STRING, &ws_server_addr, "ip address of antmedia server", NULL},
        {"port", 'p', 0, G_OPTION_ARG_INT, &ws_server_port, "Antmedia server Port default : 5080", NULL},
        {"filename",'f', 0, G_OPTION_ARG_STRING, &filename, "specify file path which you want to stream", NULL},
        {"mode", 'm', 0, G_OPTION_ARG_STRING, &mode, "publish or  play or p2p default :publish", NULL},
        //{"video codec", 'c', 0, G_OPTION_ARG_STRING, &vencoding, "video codecs h264 or vp8", NULL},
        {"streamids", 'i', 0, G_OPTION_ARG_STRING_ARRAY, &play_streamids, "you can pass n number of streamid to play like this -i streamid -i streamid ....", NULL},
        {NULL}};
static gchar *get_string_from_json_object(JsonObject *object)
{
    JsonNode *root;
    JsonGenerator *generator;
    gchar *text;
    /* Make it the root node */
    root = json_node_init_object(json_node_alloc(), object);
    generator = json_generator_new();
    json_generator_set_root(generator, root);
    text = json_generator_to_data(generator, NULL);
    /* Release everything */
    g_object_unref(generator);
    json_node_free(root);
    return text;
}
static void on_answer_created(GstPromise *promise, gpointer webrtcbin_id)
{
    gchar *sdp_text;
    JsonObject *sdp_answer_json;
    GstWebRTCSessionDescription *answer = NULL;
    const GstStructure *reply;
    GstElement *webrtc;
    g_assert_cmphex(gst_promise_wait(promise), ==, GST_PROMISE_RESULT_REPLIED);
    reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, NULL);
    gst_promise_unref(promise);
    promise = gst_promise_new();
    webrtc = gst_bin_get_by_name(GST_BIN(gst_pipe), (gchar *)webrtcbin_id);
    g_signal_emit_by_name(webrtc, "set-local-description", answer, promise);
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);
    //{"command":"takeConfiguration","streamId":"stream1","type":"answer","sdp":"v=0\r\no=- 4971157306988353964 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE 0 1 2\r\na=msid-semantic: WMS\r\nm=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:5VeC\r\na=ice-pwd:jG+CvMaIrfHRJZEugwfkNs7O\r\na=ice-options:trickle\r\na=fingerprint:sha-256 8F:94:8C:92:C4:0A:E4:D7:ED:FB:24:cd:9B:75:AD:A3:BA:CE:A7:67:AA:34:EB:12:06:A5:BF:5D:B2:73:C5:86\r\na=setup:active\r\na=mid:0\r\na=extmap:14 urn:ietf:params:rtp-hdrext:toffset\r\na=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:13 urn:3gpp:video-orientation\r\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:12 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\r\na=extmap:11 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\r\na=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\r\na=extmap:9 http://www.webrtc.org/experiments/rtp-hdrext/color-space\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:5 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\na=extmap:6 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\na=recvonly\r\na=rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:96 H264/90000\r\na=rtcp-fb:96 goog-remb\r\na=rtcp-fb:96 transport-cc\r\na=rtcp-fb:96 ccm fir\r\na=rtcp-fb:96 nack\r\na=rtcp-fb:96 nack pli\r\na=fmtp:96 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\na=rtpmap:97 rtx/90000\r\na=fmtp:97 apt=96\r\na=rtpmap:98 red/90000\r\na=rtpmap:99 rtx/90000\r\na=fmtp:99 apt=98\r\na=rtpmap:100 ulpfec/90000\r\nm=audio 9 UDP/TLS/RTP/SAVPF 111 110\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:5VeC\r\na=ice-pwd:jG+CvMaIrfHRJZEugwfkNs7O\r\na=ice-options:trickle\r\na=fingerprint:sha-256 8F:94:8C:92:C4:0A:E4:D7:ED:FB:24:cd:9B:75:AD:A3:BA:CE:A7:67:AA:34:EB:12:06:A5:BF:5D:B2:73:C5:86\r\na=setup:active\r\na=mid:1\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=recvonly\r\na=rtcp-mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 transport-cc\r\na=fmtp:111 minptime=10;useinbandfec=1; stereo=1\r\na=rtpmap:110 telephone-event/48000\r\nm=application 9 UDP/DTLS/SCTP webrtc-datachannel\r\nc=IN IP4 0.0.0.0\r\na=ice-ufrag:5VeC\r\na=ice-pwd:jG+CvMaIrfHRJZEugwfkNs7O\r\na=ice-options:trickle\r\na=fingerprint:sha-256 8F:94:8C:92:C4:0A:E4:D7:ED:FB:24:cd:9B:75:AD:A3:BA:CE:A7:67:AA:34:EB:12:06:A5:BF:5D:B2:73:C5:86\r\na=setup:active\r\na=mid:2\r\na=sctp-port:5000\r\na=max-message-size:262144\r\n"}
    /* Send answer to peer */
    sdp_text = gst_sdp_message_as_text(answer->sdp);
    printf("answer : %s", sdp_text);
    sdp_answer_json = json_object_new();
    json_object_set_string_member(sdp_answer_json, "sdp", sdp_text);
    json_object_set_string_member(sdp_answer_json, "type", "answer");
    json_object_set_string_member(sdp_answer_json, "streamId", (gchar *)webrtcbin_id);
    json_object_set_string_member(sdp_answer_json, "command", "takeConfiguration");
    sdp_text = get_string_from_json_object(sdp_answer_json);
    rws_socket_send_text(_socket, sdp_text);
    gst_webrtc_session_description_free(answer);
}

static void send_ice_candidate_message(GstElement *webrtcbin, guint mline_index,
                                       gchar *candidate, gpointer streamid)
{
    JsonObject *ice_json;
    gchar *json_string;
    ice_json = json_object_new();
    json_object_set_string_member(ice_json, "candidate", candidate);
    json_object_set_string_member(ice_json, "streamId", (gchar *)streamid);
    json_object_set_string_member(ice_json, "command", "takeCandidate");
    json_object_set_string_member(ice_json, "id", "0");
    json_object_set_int_member(ice_json, "label", 0);
    json_string = get_string_from_json_object(ice_json);
    json_object_unref(ice_json);
    rws_socket_send_text(_socket, json_string);
    g_free(json_string);
}
static void
handle_media_stream(GstPad *pad, GstElement *gst_pipe, gchar *convert_name,
                    char *sink_name)
{
    GstPad *qpad;
    GstElement *q, *conv, *resample, *sink, *toverlay;
    GstPadLinkReturn ret;

    g_print("Trying to handle stream with %s ! %s %d", convert_name, sink_name);
    q = gst_element_factory_make("queue", NULL);
    sink = gst_element_factory_make(sink_name, NULL);
    g_object_set(G_OBJECT(sink), "sync", FALSE);
    conv = gst_element_factory_make(convert_name, NULL);

    if (g_strcmp0(convert_name, "audioconvert") == 0)
    {
        g_print("audio stream");
        resample = gst_element_factory_make("audioresample", NULL);
        gst_bin_add_many(GST_BIN(gst_pipe), q, conv, resample, sink, NULL);
        gst_element_sync_state_with_parent(q);
        gst_element_sync_state_with_parent(sink);
        gst_element_sync_state_with_parent(resample);
        gst_element_sync_state_with_parent(conv);
        gst_element_link_many(q, conv, resample, sink, NULL);
    }
    else
    {
        g_print("video stream");
        toverlay = gst_element_factory_make("timeoverlay", NULL);
        gst_bin_add_many(GST_BIN(gst_pipe), q, conv, toverlay, sink, NULL);
        gst_element_sync_state_with_parent(q);
        gst_element_sync_state_with_parent(conv);
        gst_element_sync_state_with_parent(sink);
        gst_element_sync_state_with_parent(toverlay);
        gst_element_link_many(q, conv, toverlay, sink, NULL);
    }
    qpad = gst_element_get_static_pad(q, "sink");
    ret = gst_pad_link(pad, qpad);
    g_assert_cmphex(ret, ==, GST_PAD_LINK_OK);
}
void on_incoming_stream(GstElement *webrtc, GstPad *pad)
{
    GstElement *decode, *depay, *parse, *rtpjitterbuffer;
    GstPad *sinkpad, *srcpad, *decoded_pad;
    GstCaps *caps;
    const gchar *mediatype;
    gchar *convert_name, *sink_name;

    caps = gst_pad_get_current_caps(pad);
    mediatype = gst_structure_get_string(gst_caps_get_structure(caps, 0), "media");
    printf("--------------------------%s stream recived ----------------------------------", mediatype);

    if (g_str_has_prefix(mediatype, "video"))
    {
        decode = gst_element_factory_make("avdec_h264", NULL);
        depay = gst_element_factory_make("rtph264depay", NULL);
        parse = gst_element_factory_make("h264parse", NULL);
        convert_name = "videoconvert";
        sink_name = VIDEO_SINK;
    }
    else if (g_str_has_prefix(mediatype, "audio"))
    {
        decode = gst_element_factory_make("opusdec", NULL);
        depay = gst_element_factory_make("rtpopusdepay", NULL);
        parse = gst_element_factory_make("opusparse", NULL);
        convert_name = "audioconvert";
        sink_name = "autoaudiosink";
    }
    else
    {
        g_printerr("Unknown pad %s, ignoring", GST_PAD_NAME(pad));
    }

    rtpjitterbuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
    gst_bin_add_many(GST_BIN(gst_pipe), rtpjitterbuffer, depay, parse, decode, NULL);
    sinkpad = gst_element_get_static_pad(rtpjitterbuffer, "sink");
    g_assert(gst_pad_link(pad, sinkpad) == GST_PAD_LINK_OK);
    gst_element_link_many(rtpjitterbuffer, depay, parse, decode, NULL);
    decoded_pad = gst_element_get_static_pad(decode, "src");
    gst_element_sync_state_with_parent(depay);
    gst_element_sync_state_with_parent(parse);
    gst_element_sync_state_with_parent(decode);
    gst_element_sync_state_with_parent(rtpjitterbuffer);

    handle_media_stream(decoded_pad, gst_pipe, convert_name, sink_name);
}
static void on_offer_created(GstPromise *promise, const gchar *stream_id)
{
    GstWebRTCSessionDescription *offer = NULL;
    const GstStructure *reply;
    gchar *sdp_string;
    GstPad *sinkpad, *srcpad;

    reply = gst_promise_get_reply(promise);
    GstElement *webrtc;
    webrtc = gst_bin_get_by_name(GST_BIN(gst_pipe), stream_id);
    g_assert_nonnull(webrtc);
    gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer, NULL);
    gst_promise_unref(promise);
    g_signal_emit_by_name(webrtc, "set-local-description", offer, NULL);
    sdp_string = gst_sdp_message_as_text(offer->sdp);

    g_print(" offer created:\n%s\n", sdp_string);
    //{"command":"takeConfiguration","streamId":"stream1","type":"offer","sdp":"v=0\r\no=- 5144169820850006428 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE 0 1 2\r\na=extmap-allow-mixed\r\na=msid-semantic: WMS FEqivmkaUt4W7y8D5ttytZPEoTj7t6Gc9FI5\r\nm=audio 9 UDP/TLS/RTP/SAVPF 111 63 103 104 9 0 8 106 105 13 110 112 113 126\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:4iBT\r\na=ice-pwd:hTkhmsWTnru/AGLBHTvU6uIE\r\na=ice-options:trickle\r\na=fingerprint:sha-256 E9:59:25:C8:F6:5E:F6:72:60:D6:DD:66:57:32:71:9F:2B:37:CB:EA:FC:5D:E3:8D:65:AC:32:03:F1:60:02:A1\r\na=setup:actpass\r\na=mid:0\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=sendrecv\r\na=msid:FEqivmkaUt4W7y8D5ttytZPEoTj7t6Gc9FI5 39ba0a4f-ebb5-4082-b6c2-2cce1ea98ebb\r\na=rtcp-mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 transport-cc\r\na=fmtp:111 minptime=10;useinbandfec=1\r\na=rtpmap:63 red/48000/2\r\na=fmtp:63 111/111\r\na=rtpmap:103 ISAC/16000\r\na=rtpmap:104 ISAC/32000\r\na=rtpmap:9 G722/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:106 CN/32000\r\na=rtpmap:105 CN/16000\r\na=rtpmap:13 CN/8000\r\na=rtpmap:110 telephone-event/48000\r\na=rtpmap:112 telephone-event/32000\r\na=rtpmap:113 telephone-event/16000\r\na=rtpmap:126 telephone-event/8000\r\na=ssrc:2066310935 cname:XVtqz2qdUaQl3WTa\r\na=ssrc:2066310935 msid:FEqivmkaUt4W7y8D5ttytZPEoTj7t6Gc9FI5 39ba0a4f-ebb5-4082-b6c2-2cce1ea98ebb\r\nm=video 9 UDP/TLS/RTP/SAVPF 96 97 102 122 127 121 125 107 108 109 124 120 39 40 45 46 98 99 100 101 123 119 114 115 116\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:4iBT\r\na=ice-pwd:hTkhmsWTnru/AGLBHTvU6uIE\r\na=ice-options:trickle\r\na=fingerprint:sha-256 E9:59:25:C8:F6:5E:F6:72:60:D6:DD:66:57:32:71:9F:2B:37:CB:EA:FC:5D:E3:8D:65:AC:32:03:F1:60:02:A1\r\na=setup:actpass\r\na=mid:1\r\na=extmap:14 urn:ietf:params:rtp-hdrext:toffset\r\na=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:13 urn:3gpp:video-orientation\r\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:5 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\r\na=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\r\na=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\r\na=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/color-space\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:10 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\na=extmap:11 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\na=sendrecv\r\na=msid:FEqivmkaUt4W7y8D5ttytZPEoTj7t6Gc9FI5 e36904c9-d4a4-4e47-932c-4a04716102e4\r\na=rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:96 VP8/90000\r\na=rtcp-fb:96 goog-remb\r\na=rtcp-fb:96 transport-cc\r\na=rtcp-fb:96 ccm fir\r\na=rtcp-fb:96 nack\r\na=rtcp-fb:96 nack pli\r\na=rtpmap:97 rtx/90000\r\na=fmtp:97 apt=96\r\na=rtpmap:102 H264/90000\r\na=rtcp-fb:102 goog-remb\r\na=rtcp-fb:102 transport-cc\r\na=rtcp-fb:102 ccm fir\r\na=rtcp-fb:102 nack\r\na=rtcp-fb:102 nack pli\r\na=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42001f\r\na=rtpmap:122 rtx/90000\r\na=fmtp:122 apt=102\r\na=rtpmap:127 H264/90000\r\na=rtcp-fb:127 goog-remb\r\na=rtcp-fb:127 transport-cc\r\na=rtcp-fb:127 ccm fir\r\na=rtcp-fb:127 nack\r\na=rtcp-fb:127 nack pli\r\na=fmtp:127 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42001f\r\na=rtpmap:121 rtx/90000\r\na=fmtp:121 apt=127\r\na=rtpmap:125 H264/90000\r\na=rtcp-fb:125 goog-remb\r\na=rtcp-fb:125 transport-cc\r\na=rtcp-fb:125 ccm fir\r\na=rtcp-fb:125 nack\r\na=rtcp-fb:125 nack pli\r\na=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\na=rtpmap:107 rtx/90000\r\na=fmtp:107 apt=125\r\na=rtpmap:108 H264/90000\r\na=rtcp-fb:108 goog-remb\r\na=rtcp-fb:108 transport-cc\r\na=rtcp-fb:108 ccm fir\r\na=rtcp-fb:108 nack\r\na=rtcp-fb:108 nack pli\r\na=fmtp:108 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42e01f\r\na=rtpmap:109 rtx/90000\r\na=fmtp:109 apt=108\r\na=rtpmap:124 H264/90000\r\na=rtcp-fb:124 goog-remb\r\na=rtcp-fb:124 transport-cc\r\na=rtcp-fb:124 ccm fir\r\na=rtcp-fb:124 nack\r\na=rtcp-fb:124 nack pli\r\na=fmtp:124 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=4d001f\r\na=rtpmap:120 rtx/90000\r\na=fmtp:120 apt=124\r\na=rtpmap:39 H264/90000\r\na=rtcp-fb:39 goog-remb\r\na=rtcp-fb:39 transport-cc\r\na=rtcp-fb:39 ccm fir\r\na=rtcp-fb:39 nack\r\na=rtcp-fb:39 nack pli\r\na=fmtp:39 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=4d001f\r\na=rtpmap:40 rtx/90000\r\na=fmtp:40 apt=39\r\na=rtpmap:45 AV1/90000\r\na=rtcp-fb:45 goog-remb\r\na=rtcp-fb:45 transport-cc\r\na=rtcp-fb:45 ccm fir\r\na=rtcp-fb:45 nack\r\na=rtcp-fb:45 nack pli\r\na=rtpmap:46 rtx/90000\r\na=fmtp:46 apt=45\r\na=rtpmap:98 VP9/90000\r\na=rtcp-fb:98 goog-remb\r\na=rtcp-fb:98 transport-cc\r\na=rtcp-fb:98 ccm fir\r\na=rtcp-fb:98 nack\r\na=rtcp-fb:98 nack pli\r\na=fmtp:98 profile-id=0\r\na=rtpmap:99 rtx/90000\r\na=fmtp:99 apt=98\r\na=rtpmap:100 VP9/90000\r\na=rtcp-fb:100 goog-remb\r\na=rtcp-fb:100 transport-cc\r\na=rtcp-fb:100 ccm fir\r\na=rtcp-fb:100 nack\r\na=rtcp-fb:100 nack pli\r\na=fmtp:100 profile-id=2\r\na=rtpmap:101 rtx/90000\r\na=fmtp:101 apt=100\r\na=rtpmap:123 H264/90000\r\na=rtcp-fb:123 goog-remb\r\na=rtcp-fb:123 transport-cc\r\na=rtcp-fb:123 ccm fir\r\na=rtcp-fb:123 nack\r\na=rtcp-fb:123 nack pli\r\na=fmtp:123 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=64001f\r\na=rtpmap:119 rtx/90000\r\na=fmtp:119 apt=123\r\na=rtpmap:114 red/90000\r\na=rtpmap:115 rtx/90000\r\na=fmtp:115 apt=114\r\na=rtpmap:116 ulpfec/90000\r\na=ssrc-group:FID 110044450 155465437\r\na=ssrc:110044450 cname:XVtqz2qdUaQl3WTa\r\na=ssrc:110044450 msid:FEqivmkaUt4W7y8D5ttytZPEoTj7t6Gc9FI5 e36904c9-d4a4-4e47-932c-4a04716102e4\r\na=ssrc:155465437 cname:XVtqz2qdUaQl3WTa\r\na=ssrc:155465437 msid:FEqivmkaUt4W7y8D5ttytZPEoTj7t6Gc9FI5 e36904c9-d4a4-4e47-932c-4a04716102e4\r\nm=application 9 UDP/DTLS/SCTP webrtc-datachannel\r\nc=IN IP4 0.0.0.0\r\na=ice-ufrag:4iBT\r\na=ice-pwd:hTkhmsWTnru/AGLBHTvU6uIE\r\na=ice-options:trickle\r\na=fingerprint:sha-256 E9:59:25:C8:F6:5E:F6:72:60:D6:DD:66:57:32:71:9F:2B:37:CB:EA:FC:5D:E3:8D:65:AC:32:03:F1:60:02:A1\r\na=setup:actpass\r\na=mid:2\r\na=sctp-port:5000\r\na=max-message-size:262144\r\n"}

    gchar *json_string;
    JsonObject *play_stream;
    play_stream = json_object_new();
    json_object_set_string_member(play_stream, "command", "takeConfiguration");
    json_object_set_string_member(play_stream, "streamId", stream_id);
    json_object_set_string_member(play_stream, "type", "offer");
    json_object_set_string_member(play_stream, "sdp", sdp_string);
    json_string = get_string_from_json_object(play_stream);
    g_print("sending offer to %s", stream_id);
    printf("\n%s\n", json_string);
    rws_socket_send_text(_socket, json_string);
    gst_webrtc_session_description_free(offer);
}
static void
on_negotiation_needed(GstElement *webrtc, gpointer user_data)
{
    GstPromise *promise;
    g_print("negotiation  needed");
    gchar *to = gst_element_get_name(webrtc);
    promise = gst_promise_new_with_change_func((GstPromiseChangeFunc)on_offer_created, (gpointer)to, NULL);
    g_signal_emit_by_name(webrtc, "create-offer", NULL, promise);
}
static void create_webrtc(gchar *webrtcbin_id, GstWebRTCSessionDescription *offersdp, gboolean send_offer)
{

    GstElement *tee, *audio_q, *video_q, *webrtc;
    GstPad *sinkpad, *srcpad;
    GstPadLinkReturn ret;
    printf("created webrtc bin with id %s", webrtcbin_id);
    webrtc = gst_element_factory_make("webrtcbin", webrtcbin_id);
    GST_IS_ELEMENT(webrtc);

    g_object_set(G_OBJECT(webrtc), "bundle-policy", GST_WEBRTC_BUNDLE_POLICY_MAX_BUNDLE, NULL);
    // g_object_set(G_OBJECT(webrtc), "turn-server", TURN_SERVER, NULL);
    g_object_set(G_OBJECT(webrtc), "stun-server", STUN_SERVER, NULL);
    gst_bin_add_many(GST_BIN(gst_pipe), webrtc, NULL);

    video_q = gst_element_factory_make("queue", NULL);
    gst_bin_add_many(GST_BIN(gst_pipe), video_q, NULL);
    srcpad = gst_element_get_static_pad(video_q, "src");
    g_assert_nonnull(srcpad);
    sinkpad = gst_element_request_pad_simple(webrtc, "sink_%u"); // linking video to webrtc element
    g_assert_nonnull(sinkpad);
    ret = gst_pad_link(srcpad, sinkpad);
    g_assert_cmpint(ret, ==, GST_PAD_LINK_OK);
    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);

    tee = gst_bin_get_by_name(GST_BIN(gst_pipe), "video_tee");
    g_assert_nonnull(tee);
    srcpad = gst_element_request_pad_simple(tee, "src_%u"); // linking video to webrtc element
    g_assert_nonnull(srcpad);
    sinkpad = gst_element_get_static_pad(video_q, "sink");
    g_assert_nonnull(sinkpad);
    ret = gst_pad_link(srcpad, sinkpad);
    g_assert_cmpint(ret, ==, GST_PAD_LINK_OK);
    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);

    audio_q = gst_element_factory_make("queue", NULL);
    gst_bin_add_many(GST_BIN(gst_pipe), audio_q, NULL);
    srcpad = gst_element_get_static_pad(audio_q, "src");
    g_assert_nonnull(srcpad);
    sinkpad = gst_element_request_pad_simple(webrtc, "sink_%u"); // linking audio to webrtc element
    g_assert_nonnull(sinkpad);
    ret = gst_pad_link(srcpad, sinkpad);
    g_assert_cmpint(ret, ==, GST_PAD_LINK_OK);
    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);
    
    tee = gst_bin_get_by_name(GST_BIN(gst_pipe), "audio_tee");
    g_assert_nonnull(tee);
    srcpad = gst_element_request_pad_simple(tee, "src_%u"); // linking audio to webrtc element
    g_assert_nonnull(srcpad);
    sinkpad = gst_element_get_static_pad(audio_q, "sink");
    g_assert_nonnull(sinkpad);
    ret = gst_pad_link(srcpad, sinkpad);
    g_assert_cmpint(ret, ==, GST_PAD_LINK_OK);
    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);

    g_signal_connect(webrtc, "on-ice-candidate", G_CALLBACK(send_ice_candidate_message), (gpointer)webrtcbin_id);
    if (send_offer)
        g_signal_connect(webrtc, "on-negotiation-needed", G_CALLBACK(on_negotiation_needed), (gpointer)NULL);
    g_signal_connect(webrtc, "pad-added", G_CALLBACK(on_incoming_stream), NULL);

    ret = gst_element_sync_state_with_parent(audio_q);
    g_assert_true(ret);
    ret = gst_element_sync_state_with_parent(video_q);
    g_assert_true(ret);
    ret = gst_element_sync_state_with_parent(webrtc);
    g_assert_true(ret);
}
void on_socket_received_text(rws_socket socket, const char *text, const unsigned int length)
{
    JsonParser *json_parser;
    JsonObject *object;
    JsonNode *root;
    const gchar *msg_type, *offer, *type, *candidate, *recived_sdp, *answersdp;
    GstWebRTCSessionDescription *offersdp;
    GstSDPMessage *sdp;
    int ret;
    gchar *webrtcbin_id;
    GstElement *webrtc;
    char Text_Nterminated[8 * 1024];
    memcpy(Text_Nterminated, text, length);
    Text_Nterminated[length] = 0;
    GstPromise *promise;
    g_print("received text message: %s\n", Text_Nterminated);
    json_parser = json_parser_new();

    if (json_parser_load_from_data(json_parser, Text_Nterminated, -1, NULL))
    {
        root = json_parser_get_root(json_parser);
        object = json_node_get_object(root);
        msg_type = json_object_get_string_member(object, "command");
        if (g_strcmp0(msg_type, "takeConfiguration") == 0)
        {
            type = json_object_get_string_member(object, "type");
            recived_sdp = json_object_get_string_member(object, "sdp");
            webrtcbin_id = json_object_get_string_member(object, "streamId");
            g_print("%s\n", recived_sdp);
            ret = gst_sdp_message_new(&sdp);
            g_assert_cmphex(ret, ==, GST_SDP_OK);
            ret = gst_sdp_message_parse_buffer((guint8 *)recived_sdp, strlen(recived_sdp), sdp);
            if (ret != GST_SDP_OK)
            {
                g_error("Could not parse SDP string\n");
                exit(1);
            }

            if (g_strcmp0(type, "offer") == 0)
            {

                create_webrtc(webrtcbin_id, offersdp, FALSE);
                webrtc = gst_bin_get_by_name(GST_BIN(gst_pipe), webrtcbin_id);
                g_assert_nonnull(webrtc);

                offersdp = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_OFFER, sdp);
                promise = gst_promise_new();
                g_signal_emit_by_name(webrtc, "set-remote-description", offersdp, promise);
                gst_promise_interrupt(promise);
                gst_promise_unref(promise);
                promise = gst_promise_new_with_change_func(on_answer_created, webrtcbin_id, NULL);
                g_signal_emit_by_name(webrtc, "create-answer", NULL, promise);
            }
            else if (g_strcmp0(type, "answer") == 0)
            {
                webrtc = gst_bin_get_by_name(GST_BIN(gst_pipe), webrtcbin_id);
                g_assert_nonnull(webrtc);

                answersdp = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_ANSWER, sdp);
                promise = gst_promise_new();
                g_signal_emit_by_name(webrtc, "set-remote-description", answersdp, promise);
                gst_promise_interrupt(promise);
            }
        }
        else if (g_strcmp0(msg_type, "takeCandidate") == 0)
        {
            candidate = json_object_get_string_member(object, "candidate");
            webrtcbin_id = json_object_get_string_member(object, "streamId");
            g_print("ice %s %s\n", candidate, webrtcbin_id);
            webrtc = gst_bin_get_by_name(GST_BIN(gst_pipe), webrtcbin_id);
            g_assert_nonnull(webrtc);
            g_signal_emit_by_name(webrtc, "add-ice-candidate", 0, candidate);
        }
        else if (g_strcmp0(msg_type, "notification") == 0)
        {
            type = json_object_get_string_member(object, "definition");
            if (g_strcmp0(type, "joined") == 0)
                is_joined = TRUE;
        }
        else if (g_strcmp0(msg_type, "start") == 0 || g_strcmp0(msg_type,"startNewP2PConnection")==0  )
        {
            webrtcbin_id = json_object_get_string_member(object, "streamId");
            create_webrtc(webrtcbin_id, offersdp, TRUE);
        }
        else if ( g_strcmp0(msg_type,"connectWithNewId")==0){
        
        webrtcbin_id = json_object_get_string_member(object, "streamId");

        JsonObject *publish_stream = json_object_new();
        json_object_set_string_member(publish_stream, "command", "join");
        json_object_set_string_member(publish_stream, "streamId", webrtcbin_id);
        json_object_set_boolean_member(publish_stream, "multiPeer", FALSE);
        json_object_set_string_member(publish_stream, "mode", "both");
         gchar *json_string = get_string_from_json_object(publish_stream);
        rws_socket_send_text(socket, json_string);

        }

    }
}

static void on_socket_disconnected(rws_socket socket)
{
    rws_error error = rws_socket_get_error(socket);
    if (error)
    {
        printf("\nSocket disconnect with code, error: %i, %s",
               rws_error_get_code(error),
               rws_error_get_description(error));
    }
    g_print("WebSocket connection closed\n");
}

static void on_socket_connected(rws_socket socket)
{
    printf("websocket connected");
    gchar *json_string;
    JsonArray *array = json_array_new();
    gchar pipeline_str[1000];
    if(g_strcmp0(filename,"")==0){
    printf("test video sharing");
    gst_pipe = gst_parse_launch(" tee name=video_tee ! queue ! fakesink  sync=true  tee name=audio_tee ! queue ! fakesink sync=true videotestsrc is-live=true " VIDEO_ENCODE " ! " RTP_CAPS_H264 " !  queue ! video_tee. audiotestsrc  is-live=true wave=red-noise " AUDIO_ENCODE " ! " RTP_CAPS_OPUS " !  queue ! audio_tee. ", NULL);
    }
    else{
    printf("file  sharing");

    sprintf(pipeline_str," tee name=video_tee ! queue ! fakesink  sync=true  tee name=audio_tee ! queue ! fakesink sync=true filesrc location=%s  ! qtdemux name=demuxtee  demuxtee. ! decodebin " VIDEO_ENCODE " ! " RTP_CAPS_H264 " !  queue ! video_tee. demuxtee. ! decodebin " AUDIO_ENCODE " ! " RTP_CAPS_OPUS " !  queue ! audio_tee. ",filename);
    gst_pipe = gst_parse_launch(pipeline_str, NULL);
    }
    gst_element_set_state(gst_pipe, GST_STATE_READY);
    gst_element_set_state(gst_pipe, GST_STATE_PLAYING);
    if (g_strcmp0(mode, "p2p") == 0)
    {
        if (play_streamids == NULL){
            printf(" \n please pass streamid as argument which you want to play example : -i streamid -i streamid  ....\n");
            exit(0);
        }
        JsonObject *publish_stream = json_object_new();
        json_object_set_string_member(publish_stream, "command", "join");
        json_object_set_string_member(publish_stream, "streamId", play_streamids[0]);
        json_object_set_boolean_member(publish_stream, "multiPeer", TRUE);
        json_object_set_string_member(publish_stream, "mode", "both");
        json_string = get_string_from_json_object(publish_stream);
        rws_socket_send_text(socket, json_string);
    }
    else if (g_strcmp0(mode, "play") == 0)
    {
        if (play_streamids == NULL){
            printf(" \n please pass streamid as argument which you want to play example : -i streamid -i streamid  ....\n");
            exit(0);
        }
        for (int i = 0; play_streamids[i]; i++)
        {
            JsonObject *play_stream = json_object_new();
            json_object_set_string_member(play_stream, "command", "play");
            json_object_set_string_member(play_stream, "streamId", play_streamids[i]);
            json_object_set_string_member(play_stream, "room", "");
            json_object_set_array_member(play_stream, "trackList", array);
            json_string = get_string_from_json_object(play_stream);
            rws_socket_send_text(socket, json_string);
        }
    }
    else
    {
        if (play_streamids == NULL){
            printf(" \n please pass streamid as argument which you want to publish example : -i streamid \n");
            exit(0);
        }
        JsonObject *publish_stream = json_object_new();
        json_object_set_string_member(publish_stream, "command", "publish");
        json_object_set_string_member(publish_stream, "streamId", play_streamids[0]);
        json_object_set_boolean_member(publish_stream, "video", TRUE);
        json_object_set_boolean_member(publish_stream, "audio", TRUE);
        json_string = get_string_from_json_object(publish_stream);
        rws_socket_send_text(socket, json_string);
    }

}

static void websocket_connect()
{
    _socket = rws_socket_create(); // create and store socket handle
    g_assert(_socket);
    rws_socket_set_scheme(_socket, "ws");
    rws_socket_set_host(_socket, ws_server_addr);
    rws_socket_set_path(_socket, "/WebRTCAppEE/websocket");
    rws_socket_set_port(_socket, ws_server_port);
    rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
    rws_socket_set_on_received_text(_socket, &on_socket_received_text);
    rws_socket_set_on_connected(_socket, &on_socket_connected);

#if !defined(RWS_APPVEYOR_CI)
    // connection denied for client applications
    rws_socket_connect(_socket);
#endif
}

gint main(gint argc, gchar **argv)
{
    static GMainLoop *main_loop;
    gst_init(&argc, &argv);

    GError *error = NULL;
    GOptionContext *context;
    context = g_option_context_new("- Gstreamer Antmedia Webrtc Publish and Play");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_print("option parsing failed: %s\n", error->message);
        exit(1);
    }
    if (g_strcmp0(ws_server_addr, "") == 0)
    {
        printf("please enter the ws server  ip address --ip IP_ADDRESS");
        exit(0);
    }

    printf("start %s  %d ", ws_server_addr, ws_server_port);

    main_loop = g_main_loop_new(NULL, FALSE);
    websocket_connect();
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);
    return 0;
}
