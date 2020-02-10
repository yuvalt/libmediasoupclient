
#ifndef MSC_ADM_HPP
#define MSC_ADM_HPP

#include <api/media_stream_interface.h>
#include <modules/audio_device/include/audio_device_defines.h>
#include <modules/audio_device/include/audio_device.h>
#include <string>
#include <vector>

namespace mediasoupclient
{
    class AudioDeviceManager {
    private: 
        static AudioDeviceManager* instance;
        AudioDeviceManager();

    public:
        static AudioDeviceManager* getInstance() {
            if (!instance) {
                instance = new AudioDeviceManager;
            }

            return instance;
        }

	    rtc::scoped_refptr<webrtc::AudioDeviceModule> audioDeviceModule;

        std::vector<std::string> recordingDevices();

        std::vector<std::string> playoutDevices();

        void setPlayoutDevice(uint16_t index);

        void setRecordingDevice(uint16_t index);
    };
}

#endif