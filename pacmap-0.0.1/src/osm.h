#ifndef __OSM_H__
#define __OSM_H__ 

#include <tr1/unordered_map>

#include <iostream>
#include <list>

#include <mesh.h>
#include <shader.h>

#include "rendering.h"
#include "objloader.h"
#include <libpoly2tri/poly2tri.h>

namespace osm {
	typedef unsigned long long id_t;    //!< HINT: use me.
	typedef float angle_t;              //!< HINT: use me.
    typedef unsigned int uint;
	
	// 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//

	struct bounds {
		angle_t minlat, maxlat, minlon, maxlon;
	};
	extern bounds current_bounds;

	// 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//

	// 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//
    class StreetMap;

    void init();
    StreetMap* get();

    class StreetMap
    {
    public:
        void make_meshes();
        void draw_meshes(shader_ref shader);

    private:
        void triangulate_meshes();
        void print_triangle(p2t::Triangle*);

    private:
        GLuint vertex_buffer;
        GLuint* index_buffers;
        GLuint vao_id;
        uint bufferCount;
        uint* indicesCount;

        GLuint tri_vao_id;
        GLuint triIndexBuffer;
        GLuint triVertexBuffer;
        uint tri_index_count;
    };
}

#endif

