#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include <cmath>
#include <atomic>+

namespace workshop {
	std::atomic<float> frequency = 440.0f; // Default frequency
	static const float oneTurn = 2 * 3.14159265f;
	static const int SAMPLE_RATE = 48000;

	class OscBase {
	public:
		float phase = 0.0f;

		virtual void getNextValue() {
			phase++;
		}
	};

	class Sine : public OscBase {
	public:
		virtual void getNextValue() override {
			OscBase::getNextValue();
			const float phaseStep = frequency / sampleRate;
			phase = std::fmod(phase + phaseStep, 1.0f);
			return 0.2f * std::sin(phase * oneTurn);
		}
	};

	class Env {
	public:
		float phase = 0.0f;

		float getNextValue(int sampleRate, bool* finished) {
			phase++;
			const float tailLength = 0.15f * sampleRate;
			if(phase > tailLength) {
				finished = true;
				return 0.0f;
			}
			finished = false;
			return pow((tailLength - phase) / tailLength, 2.0f);
		}
	};

	class NoteInstance {
	public:
		int pitchIndex;
		OscBase* osc = new Sine();
		Env env;

		float getNextValue() {
			return env.getNextValue() * osc->getNextValue();
		}
	};
	
	class WorkshopApp : public IAudioStreamProvider {
	public:
		workshop::AudioSystem audioSystem;
		workshop::GuiSystem guiSystem;
		OscBase* osc = new Sine(440);

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

		void getAudioData(float* pOutput, ma_uint32 frameCount, ma_uint32 sampleRate) override {
			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = 0.2f * std::sin(phase * oneTurn);
				phase = std::fmod(phase + phaseStep, 1.0f);
			}
		}
	};
}

#endif // WORKSHOPAPP_H