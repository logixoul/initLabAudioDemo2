#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include <cmath>
#include <atomic>
#include <memory>

namespace workshop {
	std::atomic<float> frequency = 440.0f; // Default frequency
	static const float oneTurn = 2 * 3.14159265f;
	static const int SAMPLE_RATE = 48000;

	class OscBase {
	public:
		float _phase = 0.0f;

		virtual float getNextValue() = 0;
	};

	class Sine : public OscBase {
	public:
		virtual float getNextValue() override {
			_phase++;
			const float phaseStep = frequency / SAMPLE_RATE;
			_phase = std::fmod(_phase + phaseStep, 1.0f);
			return 0.2f * std::sin(_phase * oneTurn);
		}
	};

	class Env {
	public:
		float _phase = 0.0f;

		float getNextValue(bool* finished) {
			_phase++;
			const float tailLength = 10.15f * SAMPLE_RATE;
			if(_phase > tailLength) {
				*finished = true;
				return 0.0f;
			}
			*finished = false;
			return pow((tailLength - _phase) / tailLength, 2.0f);
		}
	};

	class NoteInstance {
	public:
		int _pitchIndex;
		std::shared_ptr<OscBase> _osc;
		Env _env;
		bool _finished;

		NoteInstance(int pitchIndex) {
			_osc = std::make_shared<Sine>();
			this->_pitchIndex = pitchIndex;
		}

		float getNextValue() {
			bool finished;
			return _env.getNextValue(&finished) * _osc->getNextValue();
			if(finished) {
				this->_finished = true;
			}
		}
	};
	
	class WorkshopApp : public workshop::IAudioStreamProvider, public workshop::IGuiListener {
	public:
		workshop::AudioSystem _audioSystem;
		workshop::GuiSystem _guiSystem;
		NoteInstance* _osc = new NoteInstance(440);

		WorkshopApp() {
			_audioSystem.init(SAMPLE_RATE, this);
			_guiSystem.init(this);
		}

		void readControls() {
			float f = frequency;
			if (ImGui::SliderFloat("Frequency", &f, 50.0f, 1000.0f, "%.0f Hz")) {
				frequency = f;
			}
		}

		void getAudioData(float* pOutput, ma_uint32 frameCount) override {
			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = _osc->getNextValue();
			}
		}

		virtual void onEvent(sapp_event const* ev) override {
			if(ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
				if(ev->key_code == 'A') {
					_osc = new NoteInstance(15);
				}
			}
		}
	};
}

#endif // WORKSHOPAPP_H