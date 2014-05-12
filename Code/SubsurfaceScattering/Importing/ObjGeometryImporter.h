#ifndef GEOMETRYIMPORTER_OBJ_H
#define GEOMETRYIMPORTER_OBJ_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>



class ObjGeometryImporter		//:public GeometryImporter
{
public:
	union obj_float3
	{
		struct { float x, y, z; };
		float v[3];
		obj_float3(){ x = 0; y = 0; z = 0; }
		obj_float3(float _x, float _y, float _z){ x = _x; y = _y; z = _z; }
	};
	union obj_float2
	{
		struct{ float x, y; };
		float v[2];
		obj_float2(){ x = 0; y = 0; }
		obj_float2(float _x, float _y){ x = _x; y = _y; }
	};
	struct Material
	{
		std::string			Name;
		int					Illum;
		obj_float3			Kd;
		obj_float3			Ka;
		obj_float3			Ks;
		obj_float3			Tf;
		float				Ns;
		float				Ni;
		std::string			map_Kd;
		std::string			map_Ka;
		std::string			map_Ks;
		std::string			bump;
		std::string			disp;
		std::string			occlusion;
	};

	struct VertexVNT
	{
		obj_float3 v;
		obj_float3 vn;
		obj_float2 uv;
	};

private:
	struct VertexData
	{
		std::vector<obj_float3> v;
		std::vector<obj_float3> vn;
		std::vector<obj_float2> vt;
	};
	//Stores an edge by its vertices index and force an order between them
	struct Edge
	{
		Edge(unsigned int _a, unsigned int _b)
		{
			//assert(_a != _b);

			if (_a < _b)
			{
				a = _a;
				b = _b;
			}
			else
			{
				a = _b;
				b = _a;
			}
		}

		/*void Print()
		{
		printf("Edge %d %d\n", a, b);
		}*/

		unsigned int a;
		unsigned int b;
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

		unsigned int GetOppositeIndex(const Edge& e) const
		{
			for (unsigned int i = 0; i < 3; i++) {
				unsigned int index = vertecies[i].vIndex;

				if (index != e.a && index != e.b) {
					return index;
				}
			}
			return 0;
		}
	};
	
	struct Neighbors
	{
		unsigned int n1;
		unsigned int n2;

		Neighbors()
		{
			n1 = n2 = (unsigned int)-1;
		}

		void AddNeigbor(unsigned int n)
		{
			if (n1 == -1) {
				n1 = n;
			}
			else if (n2 == -1) {
				n2 = n;
			}
			/*else {
				assert(0);
			}*/
		}

		unsigned int GetOther(unsigned int me) const
		{
			if (n1 == me) {
				return n2;
			}
			else if (n2 == me) {
				return n1;
			}
			/*else {
				assert(0);
			}*/

			return 0;
		}
	};

	struct CompareEdges
	{
		bool operator()(const Edge& Edge1, const Edge& Edge2)
		{
			if (Edge1.a < Edge2.a) {
				return true;
			}
			else if (Edge1.a == Edge2.a) {
				return (Edge1.b < Edge2.b);
			}
			else {
				return false;
			}
		}
	};

public:
	ObjGeometryImporter();
	virtual~ObjGeometryImporter();

	bool LoadGeometry(std::string filename, std::vector<VertexVNT>& out_floatData, std::vector<unsigned int>&out_indices, unsigned int& nrOfIndices, unsigned int& nrOfVertecies, unsigned int& vertexStride, std::vector<Material>& out_material);

private:
	void processPosition();
	void processNormal();
	void processTexCoord();
	void processFace();
	void processMaterial();
	bool processMtlFile(std::string& currentDir, std::vector<Material>& newMat);

	std::string read(std::ifstream&);
	std::string readLine(std::ifstream&);

	void Tidy();

private:
	std::ifstream		inStream;
	std::string			strInData;
	VertexData			vertecies;
	std::vector<Face>	faces;
	
	std::map<Edge, Neighbors, CompareEdges> indexMap;
};

#endif