#include <miniaudio.h>
#include <stdexcept>
#include <iostream>

namespace audioWorkshop {

class MiniAudioWrapper {
private:
    ma_device device;

public:
    MiniAudioWrapper(int sampleRate, ma_device_data_proc audioDataCallback) {
        ma_device_config deviceConfig;

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format   = ma_format_f32;
        deviceConfig.playback.channels = 1;
        deviceConfig.sampleRate        = sampleRate;
        deviceConfig.dataCallback      = audioDataCallback;
        deviceConfig.pUserData         = NULL;

        if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to open playback device.");
        }

        std::cout << "Device Name: " << device.playback.name << std::endl;

        if (ma_device_start(&device) != MA_SUCCESS) {
            ma_device_uninit(&device);
            throw std::runtime_error("Failed to start playback device.");
        }
    }

    ~MiniAudioWrapper() {
        ma_device_uninit(&device);
    }
};

}