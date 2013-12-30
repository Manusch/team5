#ifndef __OBJLOADER_ADDON_H__ 
#define __OBJLOADER_ADDON_H__ 

#if HAVE_LIBPLYLOADER == 1
#include <libplyloader/plyloader.h>
#endif

#include <libcgl-0.0.1/objloader.h>

#include <string>
#include <vector>

#include "drawelement.h"

class ObjLoader {
	obj_data objdata;
	vec3f diffuse;
public:
	ObjLoader(const std::string &objname, const std::string &filename);

	void GenerateMeshesAndDrawelements(std::vector<drawelement*> &drawelements);
	void GenerateNonsharingMeshesAndDrawElements(std::vector<drawelement*> &drawelements);

	void BoundingBox(vec3f &bb_min, vec3f &bb_max);
	void ScaleVertexDataToFit(vec3f &min, vec3f &max);
	void ScaleVertexData(const vec3f &by);
	void TranslateToOrigin();
	void TranslateTo(vec3f &pos);

	shader_ref default_shader, pos_and_norm_shader, pos_norm_and_tc_shader;
};

#if HAVE_LIBPLYLOADER == 1
class PlyLoader {
	::ply ply;
	std::string name;
	vec3f *vertex_data;
	int vertices;
	bool has_normals, has_colors;
	vec3f *normal_data, *color_data;
public:
	PlyLoader(const std::string &objname, const std::string &filename);
	
	void GenerateMeshAndDrawelement(std::vector<drawelement*> &drawelements);

	void BoundingBox(vec3f &bb_min, vec3f &bb_max);
	void ScaleVertexDataToFit(vec3f &min, vec3f &max);
	void TranslateToOrigin();
	void TranslateTo(vec3f &pos);

	shader_ref default_shader, pos_and_norm_shader, pos_norm_and_tc_shader, pos_and_col_shader, pos_norm_and_col_shader;
};
#endif


#endif

