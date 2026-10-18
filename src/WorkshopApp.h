#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include <cmath>
#include <mutex>

namespace workshop {
	float frequency = 440.0f; // Default frequency
	static const int SAMPLE_RATE = 48000;
	std::mutex mut;

	class WorkshopApp : public IAudioStreamProvider, public IGuiListener {
	public:
		workshop::AudioSystem _audioSystem;
		workshop::GuiSystem _guiSystem;

		WorkshopApp() {
			_audioSystem.init(SAMPLE_RATE, this);
			_guiSystem.init(this);
		}

		void update() {
			if (ImGui::SliderFloat("Frequency", &frequency, 50.0f, 1000.0f, "%.0f Hz")) {
			}
		}

		float _phase = 0.0f;

		void getAudioData(float* pOutput, ma_uint32 frameCount) override {
			std::unique_lock<std::mutex> lock(mut);
			const float phaseStep = frequency / SAMPLE_RATE;
			static const float oneTurn = 2.0f * 3.14159265f;
			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = 0.2f * std::sin(_phase * oneTurn);
				_phase = std::fmod(_phase + phaseStep, 1.0f);
			}
		}

		virtual void onEvent(const sapp_event* ev) override {

		}
	};
}

#endif // WORKSHOPAPP_H