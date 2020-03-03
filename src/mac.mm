#include <sdk/objc/native/api/video_decoder_factory.h>
#include <sdk/objc/native/api/video_encoder_factory.h>
#include <native/src/objc_video_encoder_factory.h>
#include <native/src/objc_video_decoder_factory.h>
#include <sdk/objc/components/video_codec/RTCDefaultVideoEncoderFactory.h>
#include <sdk/objc/components/video_codec/RTCDefaultVideoDecoderFactory.h>
#include <sdk/objc/base/RTCVideoEncoderFactory.h>
#include <sdk/objc/base/RTCVideoDecoderFactory.h>
#include <memory>
#include <absl/memory/memory.h>

std::unique_ptr<webrtc::VideoEncoderFactory> ObjCToNativeVideoEncoderFactory(
    id<RTCVideoEncoderFactory> objc_video_encoder_factory) {
  return absl::make_unique<webrtc::ObjCVideoEncoderFactory>(objc_video_encoder_factory);
}

std::unique_ptr<webrtc::VideoDecoderFactory> ObjCToNativeVideoDecoderFactory(
    id<RTCVideoDecoderFactory> objc_video_decoder_factory) {
  return absl::make_unique<webrtc::ObjCVideoDecoderFactory>(objc_video_decoder_factory);
}

std::unique_ptr<webrtc::VideoEncoderFactory> CreateHardwareEncoderFactory() {
  return ObjCToNativeVideoEncoderFactory([[RTCDefaultVideoEncoderFactory alloc] init]);
}

std::unique_ptr<webrtc::VideoDecoderFactory> CreateHardwareDecoderFactory() {
  return ObjCToNativeVideoDecoderFactory([[RTCDefaultVideoDecoderFactory alloc] init]);
}

