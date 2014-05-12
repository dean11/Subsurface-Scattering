#ifndef HEMGEOMETRYIMPORTER_H
#define HEMGEOMETRYIMPORTER_H
#include <iostream>
#include <fstream>
#include <vector>


/*
# Format:
#    Triangle, edge, and vertex counts (3 lines)
#    Vertices  ("v <id> <position x,y,z> <normal x,y,z>")
#    Edges     ("e <id> <vertID #1> <vertID #2>")
#    Triangles ("t <id> <vertID #1> <vertID #2> <vertID #3> <edgeID #1> <edgeID #2> <edgeID #3>")
# For edges, vertID1 < vertID2.  For triangles, vertID #1, #2, and #3 are specified in order, with
#    edgeID #1 corresponding to the edge between vertID #1 and #2.
*/
class HEMGeometryImporter
{
public:
	union hem_float3
	{
		struct { float x, y, z; };
		float v[3];
		hem_float3(){ x = 0; y = 0; z = 0; }
		hem_float3(float _x, float _y, float _z){ x = _x; y = _y; z = _z; }
	};
	union hem_float2
	{
		struct{ float x, y; };
		float v[2];
		hem_float2(){ x = 0; y = 0; }
		hem_float2(float _x, float _y){ x = _x; y = _y; }
	};


private:
	struct VertexData
	{
		std::vector<hem_float3> v;
		std::vector<hem_float3> vn;
		std::vector<hem_float2> vt;
	};
	struct Face
	{
		struct VertIndex
		{
			int vIndex;
			int vnIndex;
			int vtIndex;
		};

		VertIndex vertecies[3];
	};



};

#endif