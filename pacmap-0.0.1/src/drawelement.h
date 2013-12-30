#ifndef __DRAWELEMENT_H__
#define __DRAWELEMENT_H__

#include <string>
#include <list>

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/camera-matrices.h>

#include <gl-version.h>
#include <cgl.h>
#include <glut.h>
#include <shader.h>
#include <mesh.h>
#include <texture.h>
#include <camera.h>

#define row_col(R,C) col_major[((C)*4+(R))]

void apply_camera_uniforms(shader_ref shader, camera_ref cam);

class drawelement {
	protected:
		std::string name;
		shader_ref shader;
		int id;
		static int next_id;
		matrix4x4f modelmatrix;
		matrix4x4f normalmatrix;
	public:
		std::list<mesh_ref> meshes;
		typedef std::list<std::pair<std::string, texture_ref> > texlist_t;
		texlist_t textures;
		texture_ref normal_map;
		vec3f diffuse_color;
	public:
		drawelement(const std::string &name, shader_ref shader = shader_ref()) : name(name), shader(shader), id(next_id++), diffuse_color({1,1,1}) { 
			make_unit_matrix4x4f(&modelmatrix); 
			make_unit_matrix4x4f(&normalmatrix); 
		}
		explicit drawelement(const drawelement &from) {
			name = from.name;
			shader = from.shader;
			meshes = from.meshes;
			modelmatrix = from.modelmatrix;
			textures = from.textures;
			id = next_id++;
		}
		
		void add_mesh(mesh_ref mr) { meshes.push_back(mr); }
		void bind() { bind_shader(shader); }
		void apply_default_matrix_uniforms() { apply_default_matrix_uniforms(shader); }
		void apply_default_matrix_uniforms(shader_ref shader);
		void apply_default_matrix_uniforms(matrix4x4f &model) { apply_default_matrix_uniforms(model, shader); }
		void apply_default_matrix_uniforms(matrix4x4f &model, shader_ref shader); // this is *way* more expensive than the one without the additional matrix
		void apply_default_tex_uniforms_and_bind_textures() { apply_default_tex_uniforms_and_bind_textures(shader);	}
		void apply_default_tex_uniforms_and_bind_textures(shader_ref shader);
		void unbind() {	unbind_shader(shader);	}
		virtual void draw_em();
		void use_shader(shader_ref s) { shader = s; }
		shader_ref Shader() { return shader; }
		void add_texture(const std::string &uniform_name, texture_ref tex) { textures.push_back(make_pair(uniform_name, tex)); }
		void add_normalmap(const std::string &uniform_name, texture_ref tex) { normal_map = tex; }
		const std::string& Name() const { return name; }
		const matrix4x4f* Modelmatrix() const { return &modelmatrix; }
		void Modelmatrix(const matrix4x4f *m);
	};

#endif
