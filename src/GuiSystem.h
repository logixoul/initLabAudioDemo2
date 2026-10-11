#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <imgui.h>
#include <sokol_imgui.h>

namespace workshop {
	class GuiSystem {
	public:
		GuiSystem() {
		}
		void init() {
			sg_setup(sg_desc{ .environment = sglue_environment() });
        	simgui_setup(simgui_desc_t{});
		}

		~GuiSystem() {
			simgui_shutdown();
			sg_shutdown();
		}

		void handleEvent(const sapp_event* ev) {
			simgui_handle_event(ev);
			
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
}

#endif // GUISYSTEM_H