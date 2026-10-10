#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_NO_ENCODING
#include "miniaudio.h"

#define SOKOL_IMPL
#define SOKOL_GLCORE
#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <imgui.h>
#include <sokol_imgui.h>
