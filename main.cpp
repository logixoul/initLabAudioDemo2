#include <miniaudio.h>
#include <sokol_app.h>
#include <stdio.h>
#include <cmath>
#include "miniaudio_init.h"

static int currentSampleIndex = 0;

void audioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    float* fOutput = (float*)pOutput;
    for(int i = 0; i < frameCount; i++) {
        fOutput[i] = 0.2f * std::sin(2 * 3.14159265f * 220 * (float)currentSampleIndex / 48000);
        currentSampleIndex++;
    }
    (void)pInput;   // Unused
}


void init(void) {
    initMiniaudio(&audioDataCallback);
}
void frame(void) {

}
void cleanup(void) {
    uninitMiniaudio();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return sapp_desc{
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 600,
        .window_title = "My Sokol App",
    };
}
