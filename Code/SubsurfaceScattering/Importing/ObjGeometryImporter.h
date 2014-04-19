#ifndef GEOMETRYIMPORTER_OBJ_H
#define GEOMETRYIMPORTER_OBJ_H

#include <iostream>
#include <fstream>
#include <vector>

#include "ObjGeometryImporter.h"


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

private:
	struct VertexData
	{
		std::vector<obj_float3> v;
		std::vector<obj_float3> vn;
		std::vector<obj_float2> vt;
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



public:
	ObjGeometryImporter();
	virtual~ObjGeometryImporter();

	bool LoadGeometry(std::string filename, std::vector<float>& out_floatData, unsigned int& nrOfVertecies, unsigned int& vertexStride, std::vector<Material>& out_material);

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
};

#endif