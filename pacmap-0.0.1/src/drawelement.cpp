#include "drawelement.h"

int drawelement::next_id = 0;


void apply_camera_uniforms(shader_ref shader, camera_ref cam) {
	int loc = glGetUniformLocation(gl_shader_object(shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(cam)->col_major);
	loc = glGetUniformLocation(gl_shader_object(shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(cam)->col_major);
	loc = glGetUniformLocation(gl_shader_object(shader), "view_normal");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_normal_matrix_for_view_of(cam)->col_major);
}


void drawelement::apply_default_matrix_uniforms(shader_ref shader) {
	apply_camera_uniforms(shader, current_camera());
	int loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, Modelmatrix()->col_major);
	loc = glGetUniformLocation(gl_shader_object(shader), "model_normal");
	glUniformMatrix4fv(loc, 1, GL_FALSE, normalmatrix.col_major);
}


void drawelement::apply_default_matrix_uniforms(matrix4x4f &model, shader_ref shader) {
	apply_camera_uniforms(shader, current_camera());
	matrix4x4f tmp;
	multiply_matrices4x4f(&tmp, &model, Modelmatrix());
	int loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, tmp.col_major);
	matrix4x4f tmp_inv, tmp_i_t;
	invert_matrix4x4f(&tmp_inv, &tmp);
	transpose_matrix4x4f(&tmp_i_t, &tmp_inv);
	loc = glGetUniformLocation(gl_shader_object(shader), "model_normal");
	glUniformMatrix4fv(loc, 1, GL_FALSE, tmp_i_t.col_major);
}


void drawelement::apply_default_tex_uniforms_and_bind_textures(shader_ref shader) {
	int i = 0;
	for (texlist_t::iterator it = textures.begin(); it != textures.end(); ++it, ++i) {
		bind_texture(it->second, i);
		int loc = glGetUniformLocation(gl_shader_object(shader), it->first.c_str());
		glUniform1i(loc, i);
	}
}


void drawelement::draw_em() {
	for (std::list<mesh_ref>::iterator it = meshes.begin(); it != meshes.end(); ++it) {
		bind_mesh_to_gl(*it);
		draw_mesh(*it);
		unbind_mesh_from_gl(*it);
	}
}


void drawelement::Modelmatrix(const matrix4x4f *m) { 
	modelmatrix = *m; 
	matrix4x4f tmp;
	invert_matrix4x4f(&tmp, m);
	transpose_matrix4x4f(&normalmatrix, &tmp);
}
