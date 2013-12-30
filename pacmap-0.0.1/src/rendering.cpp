#include "rendering.h"

#include "wall-timer.h"

#include <framebuffer.h>

#include <cstdlib>

using namespace std;

namespace render_settings {
	int screenres_x = 1024, screenres_y = 768;
};

float random_float() {
	return ((rand() % 32768) / 16384.0f) - 1.0f;
}

