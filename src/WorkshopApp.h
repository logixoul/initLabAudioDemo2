#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include <cmath>
#include <atomic>

namespace workshop {
	std::atomic<float> frequency = 440.0f; // Default frequency
	static const int SAMPLE_RATE = 48000;

	class WorkshopApp : public IAudioStreamProvider {
	public:
		workshop::AudioSystem audioSystem;
		workshop::GuiSystem guiSystem;

		WorkshopApp() {
			audioSystem.init(SAMPLE_RATE, this);
			guiSystem.init();
		}

		void readControls() {
			float f = frequency;
			if (ImGui::SliderFloat("Frequency", &f, 50.0f, 1000.0f, "%.0f Hz")) {
				frequency = f;
			}
		}

		float phase = 0.0f;

		void getAudioData(float* pOutput, ma_uint32 frameCount) override {
			const float phaseStep = frequency / SAMPLE_RATE;
			static const float oneTurn = 2.0f * 3.14159265f;
			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = 0.2f * std::sin(phase * oneTurn);
				phase = std::fmod(phase + phaseStep, 1.0f);
			}
		}
	};
}

#endif // WORKSHOPAPP_H