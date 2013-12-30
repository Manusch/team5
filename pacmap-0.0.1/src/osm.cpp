#include "osm.h"
#include "parse.h"
#include <libpoly2tri/poly2tri.h>



using namespace std;
using namespace std::tr1;

extern  unordered_map<id_t,p2t::Point> points;
extern  unordered_map<id_t,way> ways;

//extern unordered_map<id_t,way> ways;

/* TODO
 * In our implementation this file contained the actual geometry generation and rendering code.
 */
	
bool operator==(const vec3f &a, const vec3f &b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
	
//! this namespace is just a suggestion ;)
namespace osm {

bounds current_bounds;

//Singelton
StreetMap* streetMap;


void init()
{
    streetMap = new StreetMap;
    streetMap->make_meshes();
}

StreetMap* get()
{
    return streetMap;
}

unsigned int size;

	
    void StreetMap::make_meshes() {
        size = 0;

        float latQuot =current_bounds.maxlat-current_bounds.minlat;
        float lonQuot =current_bounds.maxlon-current_bounds.minlon;

        for (auto it=points.begin(); it != points.end(); it++)
        {
            it->second.x = (it->second.x-current_bounds.minlat)/latQuot;
            it->second.y = (it->second.y-current_bounds.minlon)/lonQuot;
        }

        triangulate_meshes();

        vector<unsigned int> strip;
        vector<vec3f> vertex;
        vector<vec3f> trivertex;
        vector<unsigned int> triindices;
        tri_index_count =0;
        uint tris = 0;


        cout << current_bounds.maxlat << endl;

        bufferCount = ways.size();
        index_buffers = new GLuint[ways.size()];
        indicesCount = new unsigned int[bufferCount];

        glGenBuffers(ways.size(), index_buffers);

        uint k = 0;
        for(auto it = ways.begin();it !=ways.end();++it){
            strip.clear();
            //cout << "*** new STRIP ***"  << endl;
            for(int i = 0; i<it->second.nodes.size();i++){
                strip.push_back(size);
                id_t id = it->second.nodes[i];

                double lat = points[id].x;
                double lon = points[id].y;

                vertex.push_back(vec3f{(float)lat,0,(float)lon});

                //cout << "***" << endl;
                //cout << id << endl;
                //cout << lon << endl;

                size++;
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[k]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * strip.size(), (unsigned int*) &strip[0], GL_STATIC_DRAW);
            indicesCount[k] = strip.size();
            k++;
            //if (size > 100)
            //    break;

            if (it->second.building == 1 && it->second.triangles.size() != 0)
            {
                for (int i=0; i<it->second.triangles.size(); i++)
                {
                    p2t::Triangle* tri = it->second.triangles[i];
                    //print_triangle(tri);
                    p2t::Point* p = tri->GetPoint(0);
                    trivertex.push_back(vec3f{(float)p->x,0,(float)p->y});
                    p = tri->PointCW(*p);
                    trivertex.push_back(vec3f{(float)p->x,0,(float)p->y});
                    p = tri->PointCW(*p);
                    trivertex.push_back(vec3f{(float)p->x,0,(float)p->y});

                    triindices.push_back(tri_index_count++);
                    triindices.push_back(tri_index_count++);
                    triindices.push_back(tri_index_count++);
                    tris++;
                }
            }

        }

        glGenBuffers(1,&vertex_buffer);
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*size*3,(float *) &vertex[0],GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        //Triangles

        glGenBuffers(1,&triVertexBuffer);
        glGenBuffers(1,&triIndexBuffer);
        glGenVertexArrays(1, &tri_vao_id);
        glBindVertexArray(tri_vao_id);

        cout << tri_index_count << endl;
        cout << tris << endl;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triindices.size(), (unsigned int*) &triindices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,triVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*trivertex.size()*3,(float *) &trivertex[0],GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    }

    void StreetMap::draw_meshes(shader_ref shader) {

        glBindVertexArray(tri_vao_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triIndexBuffer);
        glDrawElements(GL_TRIANGLES, tri_index_count, GL_UNSIGNED_INT, 0);

        glBindVertexArray(vao_id);
        for (int i=0; i< bufferCount; i++)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[i]);
            glDrawElements(GL_LINE_STRIP, indicesCount[i], GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    }

    void StreetMap::triangulate_meshes()
    {
        //p2t::CDT triangulator;
        p2t::CDT* pCDT;

        uint tris = 0;
        int i = 0;
        for(auto it = ways.begin();it !=ways.end();++it)
        {
            if (it->second.building == 1) //6 fails ... no idea, fails at triangulation
            {
                if (i == 6 || i==7|| i>11)
                {
                   i++;
                   continue;
                }
                std::vector<p2t::Point*> polyline;

                for(int k = 0; k<it->second.nodes.size();k++){

                    id_t id = it->second.nodes[k];

                    p2t::Point p = points[id];
                    it->second.points.push_back(p);
                    if (k!=it->second.nodes.size()-1)
                        polyline.push_back(&points[id]);

                    cout.precision(15);
                    //cout << it->second.nodes.size() << " * "<< id << " *** " << points[id].x << " *** " << points[id].y << endl;

                }

                //polyline.pop_back();
                //polyline.pop_back();
                //polyline.pop_back();

                for (auto it = polyline.begin(); it != polyline.end(); it++)
                {
                    //cout << (*it)->x << " *** " << (*it)->y << endl;
                }

                //cout << "step1" << endl;
                //p2t::CDT triangulator(polyline);
                it->second.CDT = new p2t::CDT(polyline);
                //cout << "step2" << endl;
                //triangulator = new p2t::CDT(polyline);
                //triangulator.Triangulate();
                //pCDT->Triangulate();
                it->second.CDT->Triangulate();
                //cout << "step3" << endl;
                //it->second.triangles = triangulator.GetTriangles();
                //it->second.triangles = pCDT->GetTriangles();
                it->second.triangles = it->second.CDT->GetTriangles();

                cout << it->second.triangles.size() << endl;

                for (auto tri = it->second.triangles.begin(); tri != it->second.triangles.end(); tri++)
                {
                    //print_triangle((*tri));
                    tris++;
                }

                i++;
                cout << i << endl;
                //delete(pCDT);
            }
        }

        cout << tris << endl;
        cout << i << endl;
    }


    void StreetMap::print_triangle(p2t::Triangle* tri)
    {
        p2t::Point* p = tri->GetPoint(0);
        cout << p->x << " ** " << p->y << endl;
        p = tri->PointCW(*p);
        cout << p->x << " ** " << p->y << endl;
        p = tri->PointCW(*p);
        cout << p->x << " ** " << p->y << endl;
    }

}



/* vim: set foldmethod=marker: */

