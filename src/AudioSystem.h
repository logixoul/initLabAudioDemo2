#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H
#include <miniaudio.h>
#include <stdexcept>
#include <iostream>

namespace workshop {

class IAudioStreamProvider {
public:
    virtual void getAudioData(float* pOutput, ma_uint32 frameCount) = 0;
};

class AudioSystem {
private:
    ma_device _device;

public:
    void init(int sampleRate, IAudioStreamProvider* userData = nullptr) {
        ma_device_config deviceConfig;

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format   = ma_format_f32;
        deviceConfig.playback.channels = 1;
        deviceConfig.sampleRate        = sampleRate;
        deviceConfig.dataCallback      = &AudioSystem::audioDataCallback;
        deviceConfig.pUserData         = userData;

        if (ma_device_init(NULL, &deviceConfig, &_device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to open playback device.");
        }

        std::cout << "Device Name: " << _device.playback.name << std::endl;

        if (ma_device_start(&_device) != MA_SUCCESS) {
            ma_device_uninit(&_device);
            throw std::runtime_error("Failed to start playback device.");
        }
    }

    ~AudioSystem() {
        ma_device_uninit(&_device);
    }

	private:
	static void audioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
		IAudioStreamProvider* pStreamProvider = (IAudioStreamProvider*)pDevice->pUserData;
		float* fOutput = (float*)pOutput;
		pStreamProvider->getAudioData(fOutput, frameCount);
		(void)pInput;   // Unused
	}
};

}
#endif // AUDIOSYSTEM_H