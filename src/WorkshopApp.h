#ifndef WORKSHOPAPP_H
#define WORKSHOPAPP_H

#include <miniaudio.h>
#include "./AudioSystem.h"
#include "./GuiSystem.h"
#include "imgui.h"
#include <cmath>
#include <memory>
#include <map>
#include <mutex>
#include <vector>
#include <functional>

namespace workshop {
	struct Settings {
		float echoDelay = 1.0;
		float ssawDetune = 0.01f;
		std::string oscName = "sine";
	} settings;
	static const float TWELVETH_ROOT_OF_2 = std::powf(2.0f, 1.0f /12.0f);

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
		{ '9', 13 },
		{ 'O', 14 },
		{ '0', 15 },
		{ 'P', 16 },
		{ '[', 17 },
		{ '=', 18 },
		{ ']', 19 },
		{ 'Z', -12 },
		{ 'S', -11 },
		{ 'X', -10 },
		{ 'D', -9 },
		{ 'C', -8 },
		{ 'V', -7 },
		{ 'G', -6 },
		{ 'B', -5 },
		{ 'H', -4 },
		{ 'N', -3 },
		{ 'J', -2 },
		{ 'M', -1 },
		{ ',', -0 },
		{ 'L', 1 },
		{ '.', 2 },
		{ ';', 3 },
		{ '/', 4 }
	};
	static const float oneTurn = 2 * 3.14159265f;
	static const int SAMPLE_RATE = 48000;

	class OscBase {
	public:
		float _phase = 0.0;

		virtual float getNextValue() = 0;

		virtual ~OscBase() = default;
	};

	typedef std::shared_ptr<OscBase> OscBasePtr;

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

	class Saw : public OscBase {
	public:
		float _frequency;
		Saw(float frequency) {
			_frequency = frequency;
		}

		virtual float getNextValue() override {
			_phase++;
			const float phaseStep = _frequency / SAMPLE_RATE;
			_phase = std::fmod(_phase + phaseStep, 1.0f);
			return _phase * 2.0 - 1.0;
		}
	};

	class Square : public OscBase {
	public:
		float _frequency;
		Square(float frequency) {
			_frequency = frequency;
		}

		virtual float getNextValue() override {
			_phase++;
			const float phaseStep = _frequency / SAMPLE_RATE;
			_phase = std::fmod(_phase + phaseStep, 1.0f);
			return _phase < .5 ? -1.0 : 1.0;
		}
	};

	class Supersaw : public OscBase {
	public:
		float _frequency;
		std::vector<Saw> _saws;
		Supersaw(float frequency) {
			_frequency = frequency;
			for(int i = 0; i < 7; i++) {
				float freqMul = 1.0 + float(i-3) * settings.ssawDetune;
				_saws.push_back(Saw(frequency * freqMul));
				_saws.back()._phase = rand() / float(RAND_MAX);
			}
		}

		virtual float getNextValue() override {
			float sum = 0.0f;
			for(auto& saw : _saws) {
				sum += saw.getNextValue();
			}
			return sum / _saws.size();
		}
	};

	static const float SHIFT = std::pow(TWELVETH_ROOT_OF_2, 31.0f); 
	class Sandstorm : public OscBase {
	public:
		float _frequency;
		Saw _saw;
		Square _square;
		Sandstorm(float frequency) : _saw(frequency/2), _square(frequency * SHIFT/2) {
			_frequency = frequency;
			
		}

		virtual float getNextValue() override {
			return (_saw.getNextValue() + _square.getNextValue()) * .5f;
		}
	};

	std::map<std::string, std::function<OscBasePtr(float)>> oscFactory = 
	{
		{ "sine", [](float freq) { return std::make_shared<Sine>(freq); } },
		{ "saw", [](float freq) { return std::make_shared<Saw>(freq); } },
		{ "square", [](float freq) { return std::make_shared<Square>(freq); } },
		{ "supersaw", [](float freq) { return std::make_shared<Supersaw>(freq); } },
		{ "sandstorm", [](float freq) { return std::make_shared<Sandstorm>(freq); } }
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
			return pow((tailLength - _phase) / tailLength, 4.f);
		}
	};

	class DelayLine {
		std::vector<float> _buffer;
		int _writeIndex = 0;
		// length is in sec
	public:
		void init(float duration) {
			_buffer.resize(SAMPLE_RATE * duration, 0.0);
		}
		float process(float in) {
			float before = _buffer[_writeIndex];
			float& out = _buffer[_writeIndex];
			out *= .9;
			out += in;
			_writeIndex++;
			_writeIndex %= _buffer.size();
			return before;
		}
	};
	
	class Reverb {
		std::vector<DelayLine> _delays;
	public:
		void reinit(float decay) {
			_delays.clear();
			const int num = 11;
			for(int i = 0; i < num; i++) {
				float iNorm = i / float(num);
				float delayDuration = decay + 0.04 * iNorm;
				DelayLine delay; delay.init(delayDuration);
				_delays.push_back(delay);
			}
		}

		float process(float in) {
			float out = 0.0;
			for(auto& delay : _delays) {
				out += delay.process(in);
			}
			return 0.55f*in + 0.45f*out / _delays.size();
		}
	};

	class NoteInstance {
	public:
		int _pitchIndex;
		std::shared_ptr<OscBase> _osc;
		Env _env;
		bool _finished = false;

		NoteInstance(int pitchIndex, std::function<OscBasePtr(float)> factory) {
			_osc = factory(220.0f * std::powf(TWELVETH_ROOT_OF_2, pitchIndex));
			this->_pitchIndex = pitchIndex;
		}

		float getNextValue() {
			bool dummy;
			return _osc->getNextValue() * _env.getNextValue(&dummy);
		}
	};
	
	class WorkshopApp : public workshop::IAudioStreamProvider, public workshop::IGuiListener {
	public:
		workshop::AudioSystem _audioSystem;
		workshop::GuiSystem _guiSystem;
		std::vector<NoteInstance*> _notes;
		Reverb _reverb;

		WorkshopApp() {
			_reverb.reinit(0.15f);
			_audioSystem.init(SAMPLE_RATE, this);
			_guiSystem.init(this);
		}

		void update() {
			std::unique_lock<std::mutex> lock(mut);
			if (ImGui::SliderFloat("Reverb length", &settings.echoDelay, 0.1f, 10.0f, "%.1f sec")) {
				_reverb.reinit(settings.echoDelay);
			}
			ImGui::SliderFloat("Supersaw detune", &settings.ssawDetune, 0.005f, 0.02f, "%.4f sec");
			std::vector<std::string> oscNames;
			for(auto& factoryItem : oscFactory) {
				oscNames.push_back(factoryItem.first);
			}
			workshop::imguiRadioGroup("osc", oscNames, settings.oscName);
			//ImGui::Combo("Osc", int *current_item, const char *const *items, int items_count)
			//ImGui::Combo(const char *label, int *current_item, const char *const *items, int items_count)
			std::vector<NoteInstance*> newNotes;
			for(auto& note : _notes) {
				if(note->_finished) {
					delete note;
					note = nullptr;
				} else {
					newNotes.push_back(note);
				}
			}
			_notes = newNotes;
		}

		virtual void getAudioData(float* pOutput, ma_uint32 frameCount) override {
			std::unique_lock<std::mutex> lock(mut);

			for(ma_uint32 i = 0; i < frameCount; i++) {
				pOutput[i] = 0.0f;
				for(auto note : _notes) {
					pOutput[i] += note->getNextValue();
				}
				//pOutput[i] = std::tanh(pOutput[i]*4.0);
				pOutput[i] = _reverb.process(pOutput[i]);
				//pOutput[i] *= .3f;
			}
		}

		virtual void onEvent(sapp_event const* ev) override {
			std::unique_lock<std::mutex> lock(mut);

			if(ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
				if(ev->key_repeat) {
					return;
				}
				if(keyMap.contains(ev->key_code)) {
					_notes.push_back(new NoteInstance(keyMap.at(ev->key_code), oscFactory[settings.oscName]));
				}
			} else if(ev->type == SAPP_EVENTTYPE_KEY_UP) {
				if(ev->key_repeat) {
					return;
				}
				if(keyMap.contains(ev->key_code)) {
					for(auto& note : _notes) {
						if(note->_pitchIndex == keyMap.at(ev->key_code)) {
							note->_finished = true;
						}
					}
				}
			}
		}
	};
}

#endif // WORKSHOPAPP_H