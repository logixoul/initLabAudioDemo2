#include <sokol_app.h>
#include "WorkshopApp.h"

namespace workshop {
	WorkshopApp* app;

	void initApp() {
		app = new WorkshopApp();
	}
	void cleanupApp() {
		delete app;
	}
	void updateApp() {
		app->guiSystem.onBeginFrame();
		app->readControls();
		app->guiSystem.onEndFrame();
	}
	void appHandleEvent(const sapp_event* ev) {
		app->guiSystem.handleEvent(ev);
	}
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return sapp_desc{
        .init_cb = &workshop::initApp,
        .frame_cb = &workshop::updateApp,
		.cleanup_cb = &workshop::cleanupApp,
		.event_cb = &workshop::appHandleEvent,
    };
}
