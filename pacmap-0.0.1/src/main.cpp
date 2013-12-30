#include "cmdline.h"
#include "osm.h"
#include "wall-timer.h"
#include "rendering.h"
#include "objloader.h"

#include <iostream>
#include <libguile.h>
#include <mesh.h>
#include <shader.h>
#include <camera.h>
#include <framebuffer.h>
#include <glut.h>

#include <iomanip>

using namespace std;

wall_time_timer wtt;
wall_time_timer fps;

void key(unsigned char k, int x, int y) {
	float factor = 0.00001;
	switch (k) {
		default: standard_keyboard(k,x,y);
	}
}


void render_scene(shader_ref shader) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	camera_ref cam = current_camera();

	bind_shader(shader);
	int loc = glGetUniformLocation(gl_shader_object(shader), "proj");	
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(cam)->col_major); 
	loc = glGetUniformLocation(gl_shader_object(shader), "view");	
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(cam)->col_major);
	
    //osm::draw_meshes(shader);
    osm::get()->draw_meshes(shader);

	unbind_shader(shader);
	
	if (wtt.look() >= wall_time_timer::msec(50)) {
		wtt.restart();
	}
}


void display() {
	fps.restart();
	
	glClearColor(0.863, 0.863, 0.863, 1);
	render_scene(find_shader("shader"));
	
	swap_buffers();
	check_for_gl_errors("display");
	
// 	cout << setprecision(1) << fixed << 1000.f / fps.look() << "\r";
}

extern "C" {
	void load_configfile(const char*);
	void stop_debug_output();
}

void gl_error(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void*) {
	check_for_gl_errors("OpenGL Error Callback (gl_error)\n$ gdb src/bbm\n$ break gl_error\n$ run");
}

void actual_main() {
#if CGL_GL_VERSION == GL3
	glDebugMessageCallbackARB(gl_error, 0);
#endif
	stop_debug_output();

	register_display_function(display);
	register_idle_function(display);
	register_keyboard_function(key);
	
	load_configfile("cfg.scm");
	
	extern void setup_map(const std::string &);
	setup_map("map/map.osm");
    //osm::make_meshes();
    osm::init();
    //camera_ref make_orthographic_cam(char *name, vec3f *pos, vec3f *dir, vec3f *up,
    //                                 float right, float left, float top, float bottom, float near, float far);
    vec3f pos = vec3f{0.0,5.0,0.0};
    vec3f dir = vec3f{0.0,-1.0,0.0};
    vec3f up = vec3f{1.0,0.0,0.0};

    make_orthographic_cam("cam2d", &pos, &dir, &up, -1.0F,  1.0F, -1.0F, 1.0F, -0, 200);

    //use_camera(find_camera("cam2d"));
    use_camera(find_camera("cam"));

	glEnable(GL_DEPTH_TEST);

	float data[3*3] = { 0, 0, 0,
	                    10, 0, 0,
						0, 10, 0 };

	enter_glut_main_loop();
}


int main(int argc, char **argv)
{	
	parse_cmdline(argc, argv);
 	int guile_mode = guile_cfg_only;
// 	append_image_path("./render-data/images/");
//	int guile_mode = with_guile;
	
	enable_glut_multisampling();
	startup_cgl("OpenStreetMap Viewer", 3, 3, argc, argv, render_settings::screenres_x, render_settings::screenres_y, actual_main, guile_mode, true, 0);
	return 0;
}


