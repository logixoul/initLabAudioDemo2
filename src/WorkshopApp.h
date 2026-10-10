#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include <cmath>
#include <atomic>

namespace workshop {
	std::atomic<float> frequency = 440.0f; // Default frequency

	class WorkshopApp : public IAudioStreamProvider {
	public:
		workshop::AudioSystem audioSystem;
		workshop::GuiSystem guiSystem;

		WorkshopApp() {
			audioSystem.init(48000, this);
			guiSystem.init();
		}

		void readControls() {
			float f = frequency;
			if (ImGui::SliderFloat("Frequency", &f, 50.0f, 1000.0f, "%.0f Hz")) {
				frequency = f;
			}
		}

		float phase = 0.0f;

		void getAudioData(float* pOutput, ma_uint32 frameCount, ma_uint32 sampleRate) override {
			float phaseStep = 2 * 3.14159265f * frequency / sampleRate;
			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = 0.2f * std::sin(phase);
				phase = std::fmod(phase + phaseStep, 2 * 3.14159265f);
			}
		}
	};
}

#endif // WORKSHOPAPP_H