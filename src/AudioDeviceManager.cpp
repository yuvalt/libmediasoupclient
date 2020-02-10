#include "AudioDeviceManager.hpp"

namespace mediasoupclient
{

AudioDeviceManager* AudioDeviceManager::instance;

AudioDeviceManager::AudioDeviceManager() {
    audioDeviceModule = rtc::scoped_refptr<webrtc::AudioDeviceModule>(
        webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio)
    );
    audioDeviceModule->Init();
}

std::vector<std::string>  AudioDeviceManager::recordingDevices() {
    auto num = AudioDeviceManager::audioDeviceModule->RecordingDevices();
    auto result = std::vector<std::string>();
    // printf("Input devices: %d\n", num);

    char name[webrtc::kAdmMaxDeviceNameSize];
    char guid[webrtc::kAdmMaxGuidSize];
    for (int i = 0; i < num; i++) {
        audioDeviceModule->RecordingDeviceName(i, name, guid);
        // printf("#%d: %s\n", i, name);
        result.push_back(name);
    }
    return result;
}

std::vector<std::string> AudioDeviceManager::playoutDevices() {
    auto num = AudioDeviceManager::audioDeviceModule->PlayoutDevices();
    auto result = std::vector<std::string>();
    // printf("Input devices: %d\n", num);
    
    char name[webrtc::kAdmMaxDeviceNameSize];
    char guid[webrtc::kAdmMaxGuidSize];
    for (int i = 0; i < num; i++) {
        audioDeviceModule->PlayoutDeviceName(i, name, guid);
        // printf("#%d: %s\n", i, name);
        result.push_back(name);
    }
    return result;
}

void  AudioDeviceManager::setPlayoutDevice(uint16_t index) {
    AudioDeviceManager::audioDeviceModule->SetPlayoutDevice(index);
}

void  AudioDeviceManager::setRecordingDevice(uint16_t index) {
    AudioDeviceManager::audioDeviceModule->SetRecordingDevice(index);
}

}