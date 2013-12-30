#include <iostream>

#include <mesh.h>

#include "objloader.h"

using namespace std;

ObjLoader::ObjLoader(const std::string &objname, const std::string &filename) {
	cout << "loading " << filename << endl;
	load_objfile(objname.c_str(), filename.c_str(), &objdata, false, false, 0);	// XXX
	default_shader = make_invalid_shader();
	pos_and_norm_shader = make_invalid_shader();
	pos_norm_and_tc_shader = make_invalid_shader();
	diffuse = { 1,1,1 };
}

void ObjLoader::GenerateMeshesAndDrawelements(std::vector<drawelement*> &drawelements) {
	// todo: vertex-buffer sharing
	for (int i = 0; i < objdata.number_of_groups; ++i) {
		cout << "group " << i << endl;
		obj_group *group = objdata.groups + i;
		
		mesh_ref m = make_mesh(group->name, group->t_ids ? 3 : 2);
		bind_mesh_to_gl(m);
		
		add_vertex_buffer_to_mesh(m, "in_pos", GL_FLOAT, objdata.vertices, 3, (float *) objdata.vertex_data, GL_STATIC_DRAW);
		add_vertex_buffer_to_mesh(m, "in_norm", GL_FLOAT, objdata.vertices, 3, (float *) objdata.normal_data, GL_STATIC_DRAW);
		if (group->t_ids) add_vertex_buffer_to_mesh(m, "in_tc", GL_FLOAT, objdata.vertices, 2, (float *) objdata.texcoord_data, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(m, group->triangles * 3, (unsigned int *) group->v_ids, GL_STATIC_DRAW);
		unbind_mesh_from_gl(m);

		drawelement *de = new drawelement(group->name + string("-de"));
		shader_ref shader;
		bool has_tex = false;
				
		if (group->mtl) {
			tex_params_t p = default_tex_params();
				
			if (group->mtl->tex_d) {
				de->add_texture("diffuse_tex", make_texture_ub("diffuse_tex", group->mtl->tex_d, GL_TEXTURE_2D, &p));
				has_tex = true;
			}
			if (group->mtl->tex_bump) {
				p = default_fbo_tex_params();
				de->add_normalmap("normalmap", make_texture_ub("normalmap", group->mtl->tex_bump, GL_TEXTURE_2D, &p));
				has_tex = true;
			}
			vec3f c = {group->mtl->col_diff.x, group->mtl->col_diff.y, group->mtl->col_diff.z};
			de->diffuse_color = c;
		}
		
		if (group->t_ids && valid_shader_ref(pos_norm_and_tc_shader))
			shader = pos_norm_and_tc_shader;
		else if (valid_shader_ref(pos_and_norm_shader) && has_tex)
			shader = pos_and_norm_shader;
		else if (valid_shader_ref(default_shader))
			shader = default_shader;

		cout << de->Name() << ", shader " << shader.id << endl;
		de->use_shader(shader);
		de->add_mesh(m);
		drawelements.push_back(de);
	}
}

void ObjLoader::GenerateNonsharingMeshesAndDrawElements(std::vector<drawelement*> &drawelements) {
	for (int i = 0; i < objdata.number_of_groups; ++i) {
		obj_group *group = objdata.groups + i;
		
		mesh_ref m = make_mesh(group->name, group->t_ids ? 3 : 2);
		bind_mesh_to_gl(m);
		
		int verts = group->triangles*3;
		vec3f *v = new vec3f[verts];
		vec3f *n = new vec3f[verts];
		vec2f *t = group->t_ids ? new vec2f[verts] : 0;
		unsigned int *indices = new unsigned int[verts];
		for (int tri = 0; tri < group->triangles; ++tri) {
			v[3*tri+0] = objdata.vertex_data[group->v_ids[tri].x];
			v[3*tri+1] = objdata.vertex_data[group->v_ids[tri].y];
			v[3*tri+2] = objdata.vertex_data[group->v_ids[tri].z];
			n[3*tri+0] = objdata.normal_data[group->n_ids[tri].x];
			n[3*tri+1] = objdata.normal_data[group->n_ids[tri].y];
			n[3*tri+2] = objdata.normal_data[group->n_ids[tri].z];
			if (t) {
				t[3*tri+0] = objdata.texcoord_data[group->t_ids[tri].x];
				t[3*tri+1] = objdata.texcoord_data[group->t_ids[tri].y];
				t[3*tri+2] = objdata.texcoord_data[group->t_ids[tri].z];
			}
			indices[3*tri+0] = 3*tri+0;
			indices[3*tri+1] = 3*tri+1;
			indices[3*tri+2] = 3*tri+2;
		}
		
		add_vertex_buffer_to_mesh(m, "in_pos", GL_FLOAT, verts, 3, (float *) v, GL_STATIC_DRAW);
		add_vertex_buffer_to_mesh(m, "in_norm", GL_FLOAT, verts, 3, (float *) n, GL_STATIC_DRAW);
		if (t) add_vertex_buffer_to_mesh(m, "in_tc", GL_FLOAT, verts, 2, (float *) t, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(m, verts, indices, GL_STATIC_DRAW);
		unbind_mesh_from_gl(m);
		
		drawelement *de = new drawelement(group->name + string("-de"));
		bool has_tex = false;
	
		if (group->mtl) {
			tex_params_t p = default_tex_params();
				
			if (group->mtl->tex_d) {
				de->add_texture("diffuse_tex", make_texture_ub("diffuse_tex", group->mtl->tex_d, GL_TEXTURE_2D, &p));
				has_tex = true;
			}
			if (group->mtl->tex_bump) {
				p = default_fbo_tex_params();
				de->add_normalmap("normalmap", make_texture_ub("normalmap", group->mtl->tex_bump, GL_TEXTURE_2D, &p));
				has_tex = true;
			}
			vec3f c = {group->mtl->col_diff.x, group->mtl->col_diff.y, group->mtl->col_diff.z};
			de->diffuse_color = c;
		}

		if (group->t_ids && valid_shader_ref(pos_norm_and_tc_shader) && has_tex)
			de->use_shader(pos_norm_and_tc_shader);
		else if (valid_shader_ref(pos_and_norm_shader))
			de->use_shader(pos_and_norm_shader);
		else if (valid_shader_ref(default_shader))
			de->use_shader(default_shader);
		else {
			cout << "no shader found for drawelement " << de->Name() << endl
				 << "prov data: "; 
			
			if (group->t_ids) 	cout << "3";
			else 				cout << "2";
			
			cout << endl 
			
				 << "valid pntc: " << valid_shader_ref(pos_norm_and_tc_shader) << endl
				 << "has tex:    " << has_tex << endl
				 << "valid pns:  " << valid_shader_ref(pos_and_norm_shader) << endl
				 << "valid defs: " << valid_shader_ref(default_shader) << endl;
		}
		
		cout << de->Name() << ", shader " << de->Shader().id << " (" << shader_name(de->Shader()) << ")" << endl;

		de->add_mesh(m);
		drawelements.push_back(de);

		delete [] v;
		delete [] n;
		delete [] t;
		delete [] indices;
	}
}

void ObjLoader::BoundingBox(vec3f &bb_min, vec3f &bb_max) {
	bb_min = objdata.vertex_data[0]; 
	bb_max = objdata.vertex_data[0];
	for (int i = 0; i < objdata.vertices; ++i) {
		if (objdata.vertex_data[i].x < bb_min.x) bb_min.x = objdata.vertex_data[i].x;
		if (objdata.vertex_data[i].y < bb_min.y) bb_min.y = objdata.vertex_data[i].y;
		if (objdata.vertex_data[i].z < bb_min.z) bb_min.z = objdata.vertex_data[i].z;
		if (objdata.vertex_data[i].x > bb_max.x) bb_max.x = objdata.vertex_data[i].x;
		if (objdata.vertex_data[i].y > bb_max.y) bb_max.y = objdata.vertex_data[i].y;
		if (objdata.vertex_data[i].z > bb_max.z) bb_max.z = objdata.vertex_data[i].z;
	}
}

void ObjLoader::ScaleVertexDataToFit(vec3f &min, vec3f &max) {
	vec3f size;
	sub_components_vec3f(&size, &max, &min);
	vec3f bb_size, bb_min, bb_max;
	BoundingBox(bb_min, bb_max);
	sub_components_vec3f(&bb_size, &bb_max, &bb_min);
	vec3f scale_factors;
	div_components_vec3f(&scale_factors, &size, &bb_size);
	float scale = std::min(scale_factors.x, std::min(scale_factors.y, scale_factors.z));
	for (int i = 0; i < objdata.vertices; ++i)
		mul_vec3f_by_scalar(objdata.vertex_data + i, objdata.vertex_data + i, scale);
}

void ObjLoader::ScaleVertexData(const vec3f &by) {
	for (int i = 0; i < objdata.vertices; ++i)
		mul_components_vec3f(objdata.vertex_data + i, objdata.vertex_data + i, &by);
}

void ObjLoader::TranslateToOrigin() {
	vec3f bb_size, bb_min, bb_max, offset;
	BoundingBox(bb_min, bb_max);
	sub_components_vec3f(&bb_size, &bb_max, &bb_min);
	div_vec3f_by_scalar(&bb_size, &bb_size, 2);
	add_components_vec3f(&offset, &bb_size, &bb_min);
	for (int i = 0; i < objdata.vertices; ++i)
		sub_components_vec3f(objdata.vertex_data+i, objdata.vertex_data+i, &offset);
}

void ObjLoader::TranslateTo(vec3f &pos) {
	for (int i = 0; i < objdata.vertices; ++i)
		add_components_vec3f(objdata.vertex_data+i, objdata.vertex_data+i, &pos);
}





#if HAVE_LIBPLYLOADER == 1

PlyLoader::PlyLoader(const std::string &objname, const std::string &filename)
{
	cout << "loading " << filename << endl;
	name = objname;
	load_ply_file(filename.c_str(), &ply);
	default_shader = make_invalid_shader();
	pos_and_norm_shader = make_invalid_shader();
	pos_norm_and_tc_shader = make_invalid_shader();
	pos_and_col_shader = make_invalid_shader();
	pos_norm_and_col_shader = make_invalid_shader();

	vertex_data = ply.flat_triangle_data("x", "y", "z", [](float f){ return f;});
	ply::element_t *elem = ply.element("vertex");
	vertices = ply.faces() * 3;

	has_normals = elem->property("nx") && elem->property("ny") && elem->property("nz");
	if (has_normals)
		normal_data = ply.flat_triangle_data("nx", "ny", "nz", [](float f){ return f;});

	has_colors = elem->property("red") && elem->property("green") && elem->property("blue");
	if (has_colors)
		color_data = ply.flat_triangle_data("red", "green", "blue", [](float f){ return f/255.0f;});

// 		ply.add_comment("just a copy");
// 		ply.allocate_new_vertex_data({"red", "green", "blue"}, "vertex");
// 		save_ply_file("/tmp/plyout.ply", &ply);
}

void PlyLoader::GenerateMeshAndDrawelement(std::vector<drawelement*> &drawelements) {
	
		auto inputs = stringlist("in_pos");
		if (has_normals) inputs.push_back("in_norm");
		if (has_colors)  inputs.push_back("in_col");
		mesh *m = new mesh(name, inputs);
		m->bind();
		m->add_vertex_data("in_pos", vertices, vertex_data);
		if (has_normals)
			m->add_vertex_data("in_norm", vertices, normal_data);
		if (has_colors)
			m->add_vertex_data("in_col", vertices, color_data);
		for (int i = 0; i < 3; ++i)
			cout << normal_data[i].x << "\t" << normal_data[i].y << "\t" << normal_data[i].z << endl;
		cout << endl;
		for (int i = 0; i < 3; ++i)
			cout << color_data[i].x << "\t" << color_data[i].y << "\t" << color_data[i].z << endl;
		cout << endl;
// 			const int tris = vertices;
		const int tris = ply.face_verts.size();
		unsigned int *idx = new unsigned int[tris];
		for (int i = 0; i < tris; ++i)
			idx[i] = i;//ply.face_verts[i];
// 			if (group->t_ids) m->add_vertex_data("in_tc", objdata.vertices, objdata.texcoord_data);
		m->add_index_data(tris, idx);	// todo: vec3ui, vec3i in mcm
		m->unbind();
		delete [] idx;

		drawelement *de = new drawelement(name + string("-de"));
		shader *shader;
		cout << "N: " << has_normals << ", C: " << has_colors << endl;
		if (has_normals && !has_colors)
			shader = new shader(pos_and_norm_shader);
		else if (has_colors && !has_normals)
			shader = new shader(pos_and_col_shader);
		else if (has_colors && has_normals)
			shader = new shader(pos_norm_and_col_shader);
		else if (valid_shader_ref(default_shader))
			shader = new shader(default_shader);
					

		cout << de->Name() << ", shader " << shader->cgl_ref().id << endl;
		de->use_shader(shader_ptr(shader));
		de->add_mesh(m);
		drawelements.push_back(de);
}

void PlyLoader::BoundingBox(vec3f &bb_min, vec3f &bb_max) {
	bb_min = vertex_data[0]; 
	bb_max = vertex_data[0];
	for (int i = 0; i < vertices; ++i) {
		if (vertex_data[i].x < bb_min.x) bb_min.x = vertex_data[i].x;
		if (vertex_data[i].y < bb_min.y) bb_min.y = vertex_data[i].y;
		if (vertex_data[i].z < bb_min.z) bb_min.z = vertex_data[i].z;
		if (vertex_data[i].x > bb_max.x) bb_max.x = vertex_data[i].x;
		if (vertex_data[i].y > bb_max.y) bb_max.y = vertex_data[i].y;
		if (vertex_data[i].z > bb_max.z) bb_max.z = vertex_data[i].z;
	}
}

void PlyLoader::ScaleVertexDataToFit(vec3f &min, vec3f &max) {
	vec3f size;
	sub_components_vec3f(&size, &max, &min);
	vec3f bb_size, bb_min, bb_max;
	BoundingBox(bb_min, bb_max);
	sub_components_vec3f(&bb_size, &bb_max, &bb_min);
	vec3f scale_factors;
	div_components_vec3f(&scale_factors, &size, &bb_size);
	float scale = std::min(scale_factors.x, std::min(scale_factors.y, scale_factors.z));
	for (int i = 0; i < vertices; ++i)
		mul_vec3f_by_scalar(vertex_data + i, vertex_data + i, scale);
}

void PlyLoader::TranslateToOrigin() {
	vec3f bb_size, bb_min, bb_max, offset;
	BoundingBox(bb_min, bb_max);
	sub_components_vec3f(&bb_size, &bb_max, &bb_min);
	div_vec3f_by_scalar(&bb_size, &bb_size, 2);
	add_components_vec3f(&offset, &bb_size, &bb_min);
	for (int i = 0; i < vertices; ++i)
		sub_components_vec3f(vertex_data+i, vertex_data+i, &offset);
}

void PlyLoader::TranslateTo(vec3f &pos) {
	for (int i = 0; i < vertices; ++i)
		add_components_vec3f(vertex_data+i, vertex_data+i, &pos);
}

#endif



/* vim: set foldmethod=marker: */


