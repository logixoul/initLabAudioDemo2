#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <imgui.h>
#include <sokol_imgui.h>
#include <vector>
#include <string>

namespace workshop {
	class IGuiListener {
	public:
		virtual void onEvent(sapp_event const* ev) = 0;

		virtual ~IGuiListener() = default;
	};

	class GuiSystem {
	public:
		IGuiListener* _listener;

		void init(IGuiListener* listener) {
			sg_setup(sg_desc{ .environment = sglue_environment() });
        	simgui_setup(simgui_desc_t{});
			_listener = listener;
		}

		~GuiSystem() {
			simgui_shutdown();
			sg_shutdown();
		}

		void handleEvent(const sapp_event* ev) {
			bool imguiHandledIt = simgui_handle_event(ev);
			if(!imguiHandledIt) {
				_listener->onEvent(ev);
			}
		}
		void onBeginFrame() {
			simgui_new_frame({ sapp_width(), sapp_height(), sapp_frame_duration(), sapp_dpi_scale() });

			ImGui::Begin("Synth");
		}
		void onEndFrame() {
			ImGui::End();

			simgui_flush();
			sg_begin_pass(sg_pass{ .swapchain = sglue_swapchain() });
			simgui_draw();
			sg_end_pass();
			sg_commit();
		}
	};

	void imguiRadioGroup(std::string name, std::vector<std::string> options, std::string& choice) {
		if (ImGui::BeginListBox(name.c_str())) {
			for (auto& opt : options) {
				bool isSelected = (opt == choice);
				if (ImGui::Selectable(opt.c_str(), isSelected))
					choice = opt;
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
}

#endif // GUISYSTEM_H