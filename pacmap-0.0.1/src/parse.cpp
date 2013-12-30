#include "parse.h"
#include "osm.h"

#include <iostream>
#include <expat.h>
#include <map>

using namespace std;
using namespace std::tr1;

/* TODO
 * In our implementation this file contained the xml parser which created the datastructures used for rendering.
 */


int Eventcnt = 0;

way currentWay;

char Buff[8192];
unordered_map<id_t,p2t::Point> points;
unordered_map<id_t,way> ways;

void
default_hndl(void *data, const char *s, int len) {
  fwrite(s, len, sizeof(char), stdout);
}

void
printcurrent(XML_Parser p) {
  XML_SetDefaultHandler(p, default_hndl);
  XML_DefaultCurrent(p);
  XML_SetDefaultHandler(p, (XML_DefaultHandler) 0);
}  /* End printcurrent */

void
start_hndl(void *data, const char *el, const char **attr) {


  //printf("\n%4d: Start tag %s - ", Eventcnt++, el);
	id_t id = 0;
	double lat= 0;
	double lon= 0;

	if(string(el) == "bounds"){
		for(int i = 0; attr[i]; i+=2){
			if(string(attr[i]) == "minlat"){
				osm::current_bounds.minlat = atof(attr[i+1]);				
			}
			if(string(attr[i]) == "maxlat"){
				osm::current_bounds.maxlat = atof(attr[i+1]);
			}
			if(string(attr[i]) == "minlon"){
				osm::current_bounds.minlon = atof(attr[i+1]);
}
			if(string(attr[i]) == "maxlon"){
				osm::current_bounds.maxlon = atof(attr[i+1]);
}
		}

	}
	
	if(string(el) == "node"){
		/*printcurrent((XML_Parser) data);
		cout << endl;*/
			for(int i = 0; attr[i]; i+=2){
				if(string(attr[i])=="id")
					id = atoll(attr[i+1]);
				if(string(attr[i])=="lat")
                    lat = atof(attr[i+1]);
				if(string(attr[i])=="lon")
                    lon = atof(attr[i+1]);
			}
		
        points[id] = p2t::Point(lat,lon);
	}

	if(string(el) == "nd"){
		if(currentWay.id == -1){
			exit(-1);
		}
		for(int i =0;attr[i];i+=2){
			if(string(attr[i]) == "ref"){
				currentWay.nodes.push_back(atoll(attr[i+1]));
			}			
		}
	}

	if(string(el) == "way"){
		for(int i=0;attr[i];i+=2){
			if(string(attr[i]) == "id")
			currentWay = way(atoll(attr[i+1]));		
		}
	}
 
    if (string(el) == "tag")
    {
        if (string(attr[1]) == "building" && string(attr[3]) == "yes")
        {
            //cout << string(attr[1]) << string(attr[3]) << endl;
            currentWay.building = 1;
        }
    }
	
}  /* End of start_hndl */


void
end_hndl(void *data, const char *el) {
 	if(string(el)=="way"){

		ways[currentWay.id] = currentWay;
        currentWay = way();
	}
}  /* End of end_hndl */

void
char_hndl(void *data, const char *txt, int txtlen) {
  //printf("\n%4d: Text - ", Eventcnt++);

  //fwrite(txt, txtlen, sizeof(char), stdout);
}  /* End char_hndl */

void
proc_hndl(void *data, const char *target, const char *pidata) {
  printf("\n%4d: Processing Instruction - ", Eventcnt++);
  printcurrent((XML_Parser) data);
}  /* End proc_hndl */

/*! \brief a simple manipulator for indentation
 *  use like
 *  <code>
 *  cout << indent(3) << currtag << endl;
 *  </code>
 */
struct indent {
	int depth;
	const std::string tab;
	indent(int depth, const std::string &tab = "\t")
	: depth(depth), tab(tab) {
	}
	friend ostream& operator<<(ostream &out, const indent ind) {
		for (int i = 0; i < ind.depth; ++i)
			out << ind.tab;
		return out;
	}
};

/* A few hints:
 * - map<string,string>
 * - auto (c++11)
 * - atof(3), atoi(3), atoll(3)
 * - string::operator== (don't use string::compare)
 * - ifstream::fail vs ifstream::eof vs ifstream::read
 * - use const string & if possible
 * - exception handling via std::runtime_error
 */

void setup_map(const std::string &filename) {

	FILE * xmlFile;
	xmlFile = fopen(filename.c_str(),"r");
	fgets(Buff,sizeof(Buff), xmlFile);
	XML_Parser p = XML_ParserCreate(NULL);

	XML_UseParserAsHandlerArg(p);
 	XML_SetElementHandler(p, start_hndl, end_hndl);
 //	XML_SetCharacterDataHandler(p, char_hndl);
 	//XML_SetProcessingInstructionHandler(p, proc_hndl);

	if (! p) {
    	fprintf(stderr, "Couldn't allocate memory for parser\n");
    	exit(-1);
  	}

	for(;;){
		fgets(Buff,sizeof(Buff), xmlFile);
		bool done =false;
		int len = strlen(Buff);
	
		done = feof(xmlFile);
		if (! XML_Parse(p, Buff, len, done) &&!done) {
   		    fprintf(stderr, "Parse error at line %d:\n%s\n",
		    XML_GetCurrentLineNumber(p),
			XML_ErrorString(XML_GetErrorCode(p)));
  	   		exit(-1);
  	  }
	
	
		if(done)break;
	}
	fclose(xmlFile);

}


/* vim: set foldmethod=marker: */

