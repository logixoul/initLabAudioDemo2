#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include <cmath>
#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <vector>

namespace workshop {
	struct Settings {
		std::atomic<float> echoDelay;
	};
	std::mutex mut;
	static const std::map<char, int> keyMap = { 
		{ 'Q', 0 },
		{ '2', 1 },
		{ 'W', 2 },
		{ '3', 3 },
		{ 'E', 4 },
		{ 'R', 5 },
		{ '5', 6 },
		{ 'T', 7 },
		{ '6', 8 },
		{ 'Y', 9 },
		{ '7', 10 },
		{ 'U', 11 },
		{ 'I', 12 },
	};
	static const float oneTurn = 2 * 3.14159265f;
	static const int SAMPLE_RATE = 48000;

	class OscBase {
	public:
		float _phase = 0.0f;

		virtual float getNextValue() = 0;

		virtual ~OscBase() = default;
	};

	class Sine : public OscBase {
	public:
		float _frequency;
		Sine(float frequency) {
			_frequency = frequency;
		}

		virtual float getNextValue() override {
			_phase++;
			const float phaseStep = _frequency / SAMPLE_RATE;
			_phase = std::fmod(_phase + phaseStep, 1.0f);
			return 0.2f * std::sin(_phase * oneTurn);
		}
	};

	class Env {
	public:
		float _phase = 0.0f;

		float getNextValue(bool* finished) {
			_phase++;
			const float tailLength = 1.00f * SAMPLE_RATE;
			if(_phase > tailLength) {
				*finished = true;
				return 0.0f;
			}
			*finished = false;
			return 1.0;//pow((tailLength - _phase) / tailLength, 2.0f);
		}
	};

	class DelayLine {
		
	};

	class NoteInstance {
	public:
		int _pitchIndex;
		std::shared_ptr<OscBase> _osc;
		Env _env;
		bool _finished;

		NoteInstance(int pitchIndex) {
			static const float twelvethRootOf2 = std::powf(2.0f, 1.0f /12.0f);
			_osc = std::make_shared<Sine>(440.0f * std::powf(twelvethRootOf2, pitchIndex));
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
		std::vector<NoteInstance*> _notes;

		WorkshopApp() {
			_audioSystem.init(SAMPLE_RATE, this);
			_guiSystem.init(this);
		}

		void update() {
			/*float f = frequency;
			if (ImGui::SliderFloat("Frequency", &f, 50.0f, 1000.0f, "%.0f Hz")) {
				frequency = f;
			}*/
			
		}

		virtual void getAudioData(float* pOutput, ma_uint32 frameCount) override {
			std::unique_lock<std::mutex> lock(mut);

			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = 0.0f;
				for(auto note : _notes) {
					pOutput[i] += note->getNextValue();
				}
			}
		}

		virtual void onEvent(sapp_event const* ev) override {
			std::unique_lock<std::mutex> lock(mut);

			if(ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
				if(keyMap.contains(ev->key_code)) {
					_notes.push_back(new NoteInstance(keyMap.at(ev->key_code)));
				}
			}
		}
	};
}

#endif // WORKSHOPAPP_H