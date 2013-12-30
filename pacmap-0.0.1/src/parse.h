#ifndef __PARSE_H__ 
#define __PARSE_H__ 

#include <string>
#include <string.h>
#include <tr1/unordered_map>
#include <vector>
#include <libpoly2tri/poly2tri.h>

void setup_map(const std::string &filename);




/*struct point{
	double lat;
	double lon;
	point():lat(0),lon(0){}
	point(double lat,double lon):lat(lat),lon(lon){}
};*/

struct way{
	id_t id;
	std::vector<id_t> nodes;
    std::vector<p2t::Triangle*> triangles;
    std::vector<p2t::Point> points;
    p2t::CDT* CDT;
    int building;
    way():id(-1),nodes(0), building(0){}
	way(id_t id):id(id){}
	
};





#endif

