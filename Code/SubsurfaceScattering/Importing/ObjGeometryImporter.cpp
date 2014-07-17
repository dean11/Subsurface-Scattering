#include "ObjGeometryImporter.h"

#include <algorithm>
#include <sstream>
#include <ctime>
#include <string>
#include "..\Utilities\Util.h"

using namespace std;
int temCarret = 0;

static void stringSplit(std::string& p_sstring, char p_cDel, std::vector<std::string>& outData, bool ignoreEmpty = true)
{
	int start = 0;
	int endIndex = (int)p_sstring.length() - 1;
	int charCount = 0;
	std::string temp = "";

	int k = 0;
	for (int i = 0; i <= endIndex; i)
	{
		for (k = i; k <= endIndex && p_sstring[k] != p_cDel; k++)
		{
			temp += p_sstring[k];
		}
		if (temp != "" || !ignoreEmpty)
		{
			outData.push_back(temp);
			temp = "";
		}
		i = k + 1;
	}
}

ObjGeometryImporter::obj_float3 AlphaToFloat3(std::string val)
{
	static std::vector<std::string> part;
	part.clear();

	ObjGeometryImporter::obj_float3 v(0.0f, 0.0f, 0.0f);

	stringSplit(val, ' ', part);
	for (size_t i = 0; i<part.size(); i++)
	{
		switch (i)
		{
		case 0:	//x
			v.x = (float)atof(part[i].c_str());
			break;
		case 1:	//y
			v.y = (float)atof(part[i].c_str());
			break;
		case 2:	//z
			v.z = (float)atof(part[i].c_str());
			break;
		}
	}
	return v;
}

ObjGeometryImporter::ObjGeometryImporter()
{ 
this->carret = 0;
}
ObjGeometryImporter::~ObjGeometryImporter() 
{
}	
void ObjGeometryImporter::Tidy()
{
	this->faces.clear();
	this->faces.resize(0);

	this->vertecies.v.clear();
	this->vertecies.vn.clear();
	this->vertecies.vt.clear();

	this->vertecies.v.resize(0);
	this->vertecies.vn.resize(0);
	this->vertecies.vt.resize(0);
	
	this->strInData = "";

	this->inStream.clear();

}
bool ReadSomeData(std::string& data, std::string fname)
{
	bool result = true;
	size_t bytesTotal = 0;
	size_t bytesRead = 0;
	FILE *stream;

	wchar_t buff[255] = {0};
	_wgetcwd(buff, 255);
	errno_t err = 0;
	
	if ((err = fopen_s(&stream, fname.c_str(), "r+t")) == 0)
	{
		//Get size of the file
		fseek(stream, 0L, SEEK_END);
		bytesTotal = ftell(stream);
		fseek(stream, 0L, SEEK_SET);
		fflush(stream);

		//Sanity check
		if(bytesTotal == 0) return false;

		//Create the new byte buffer
		char *buff = new char[bytesTotal + 1];

		//Read the bytes to the end
		bytesRead = fread_s( buff, bytesTotal, sizeof(char), bytesTotal ,stream );
		fclose( stream );
		data = buff;
		
		//Did we read enough bytes (Get the bytes if we read with ANSI since the hidden characters is ignored)
		//if(bytesRead != bytesTotal)	return false;

		data.resize(bytesRead);
		memcpy(&data[0], &buff[0], bytesRead);
	
		delete [] buff;
	}
	else
		return false;

	return true;
}
bool  ObjGeometryImporter::LoadGeometry(std::string filename, std::vector<float>& out_floatData, unsigned int& nrOfVertecies, unsigned int& vertexStride, std::vector<Material>& out_material)
{
	if(!ReadSomeData(this->inStream, filename)) return false;
	
	int count = 0;

	while (this->carret < this->inStream.size())
	{
		this->strInData = read(this->inStream, this->carret);

			 if (this->strInData == "#" || this->strInData == "s")	{ readLine			(this->inStream, this->carret); 	}
		else if (this->strInData == "v"	)							{ processPosition	();	}
		else if (this->strInData == "vt"	)						{ processTexCoord	();	}
		else if (this->strInData == "vn"	)						{ processNormal		();	}
		else if (this->strInData == "f"	)							{ processFace		();	}
		else if (this->strInData == "mtllib")						{ if (!processMtlFile(filename, out_material)) { Tidy(); return false; } }
		count ++;
	}
	nrOfVertecies = 0;
	for (size_t i = 0; i < this->faces.size(); i += 1)
	{
		for (size_t k = 0; k < 3; k++)
		{
			nrOfVertecies++;
			//V
			out_floatData.push_back(this->vertecies.v[this->faces[i].vertecies[k].vIndex].x);
			out_floatData.push_back(this->vertecies.v[this->faces[i].vertecies[k].vIndex].y);
			out_floatData.push_back(this->vertecies.v[this->faces[i].vertecies[k].vIndex].z);
			//VN
			out_floatData.push_back(this->vertecies.vn[this->faces[i].vertecies[k].vnIndex].x);
			out_floatData.push_back(this->vertecies.vn[this->faces[i].vertecies[k].vnIndex].y);
			out_floatData.push_back(this->vertecies.vn[this->faces[i].vertecies[k].vnIndex].z);
			//VT
			out_floatData.push_back(this->vertecies.vt[this->faces[i].vertecies[k].vtIndex].x);
			out_floatData.push_back(this->vertecies.vt[this->faces[i].vertecies[k].vtIndex].y);
		}
	}

	//nrOfVertecies = this->vertecies.v.size();
	vertexStride = sizeof(float)* 8;
	Tidy();

	return true;
}


void ObjGeometryImporter::processPosition()
{
	this->strInData = read(this->inStream, this->carret);

	float x = (float)atof(this->strInData.c_str());
	this->strInData = read(this->inStream, this->carret);
	float y = (float)atof(this->strInData.c_str());
	this->strInData = read(this->inStream, this->carret);
	float z = (float)atof(this->strInData.c_str());


	this->vertecies.v.push_back(obj_float3(x, y, (-1.0f * z)));
}
void ObjGeometryImporter::processNormal()			   
{
	this->strInData = read(this->inStream, this->carret);
	float x = (float)atof(this->strInData.c_str());
	this->strInData = read(this->inStream, this->carret);
	float y = (float)atof(this->strInData.c_str());
	this->strInData = read(this->inStream, this->carret);
	float z = (float)atof(this->strInData.c_str());

	this->vertecies.vn.push_back(obj_float3(x, y, (-1 * z)));
}
void ObjGeometryImporter::processTexCoord()
{
	this->strInData = read(this->inStream, this->carret);
	float x = (float)atof(this->strInData.c_str());
	this->strInData = read(this->inStream, this->carret);
	float y = 1.0f - (float)atof(this->strInData.c_str());
	
	this->vertecies.vt.push_back(obj_float2(x, y));

}
void ObjGeometryImporter::processFace()			   
{
	static std::vector<std::string> facePart;
	facePart.clear();
	this->strInData = readLine(this->inStream, this->carret);
	//this->strInData.erase(this->strInData.length() - 1, this->strInData.length() - 1);
	stringSplit(this->strInData, ' ', facePart);
	
	Face f;
	Face::VertIndex v;
 	std::vector<std::string> values;

	for (int i = 0; i < (int)facePart.size(); i++)
	{
		values.clear();
		stringSplit(facePart[i], '/', values);
	
		if((int)facePart.size() > 0)
		{
			if(values[0] != "")
				v.vIndex = atoi(values[0].c_str()) - 1; 
	
			if(values.size() < 3)
			{
				if(this->vertecies.vt.size() > 0)
					v.vtIndex = atoi(values[1].c_str()) - 1;
				else if(this->vertecies.vn.size() > 0)
					v.vnIndex = atoi(values[1].c_str()) - 1;
			}
			else
			{
				if(values[1] != "")
					v.vtIndex = atoi(values[1].c_str()) - 1;
				if(values[2] != "")	
					v.vnIndex = atoi(values[2].c_str()) - 1;
			}
		}
	
		f.vertecies[i] = v;
	}
	this->faces.push_back(f);
}
bool ObjGeometryImporter::processMtlFile(std::string& currentDir, std::vector<Material>& material)
{
	int p = currentDir.rfind('\\');
	p = p ? p : currentDir.rfind('/');
	
	this->strInData = readLine(this->inStream, this->carret);
	//this->strInData.erase(this->strInData.length() - 1, this->strInData.length() - 1);
	std::string dir = currentDir.substr(0, p+1);
	std::string mtlFilePath = dir;

	mtlFilePath.append(this->strInData);

	int pos = -1;
	int namelessCount = 0;

	
	std::string mtl;
	int tempCarret = 0;
	
	if(ReadSomeData(mtl, mtlFilePath))
	{
		Material newMat;

		while (tempCarret < mtl.size())
		{
			this->strInData = read(mtl, tempCarret);
			if (tempCarret >= mtl.size())
				break;
	
			if(this->strInData == "newmtl") 
			{
				if ((this->strInData = readLine(mtl, tempCarret)) != "")
				{
					newMat.Name = this->strInData;
					material.push_back(newMat);
					pos = (int)material.size() - 1;
				}
				else
				{
					newMat.Name = "default_" + std::to_string((_LONGLONG)namelessCount++);
					material.push_back(newMat);
					pos = (int)material.size() - 1;
				}
			}
			else if (this->strInData == "#")			{ readLine(mtl, tempCarret); }
			else if (this->strInData == "Ka")			{ material[pos].Ka			= AlphaToFloat3(readLine(mtl, tempCarret)); }
			else if (this->strInData == "Kd")			{ material[pos].Kd			= AlphaToFloat3(readLine(mtl, tempCarret)); }
			else if (this->strInData == "Ks")			{ material[pos].Ks			= AlphaToFloat3(readLine(mtl, tempCarret)); }
			else if (this->strInData == "Tf")			{ material[pos].Tf			= AlphaToFloat3(readLine(mtl, tempCarret)); }
			else if (this->strInData == "illum")		{ material[pos].Illum		= atoi(readLine(mtl, tempCarret).c_str()); }
			else if (this->strInData == "Ni")			{ material[pos].Ni			= (float)atof(read(mtl, tempCarret).c_str()); }
			else if (this->strInData == "Ns")			{ material[pos].Ns			= (float)atof(read(mtl, tempCarret).c_str()); }
			else if (this->strInData == "map_Ka")		{ material[pos].map_Ka		= readLine(mtl, tempCarret); }
			else if (this->strInData == "map_Td")		{ material[pos].map_Td		= readLine(mtl, tempCarret); }
			else if (this->strInData == "map_Kd")		{ material[pos].map_Kd		= readLine(mtl, tempCarret); }
			else if (this->strInData == "map_Ks")		{ material[pos].map_Ks		= readLine(mtl, tempCarret); }
			else if (this->strInData == "disp")			{ material[pos].disp		= readLine(mtl, tempCarret); }
			else if (this->strInData == "bump")			{ material[pos].bump		= readLine(mtl, tempCarret); }
			else if (this->strInData == "occlusion")	{ material[pos].occlusion	= readLine(mtl, tempCarret); }
			
		}
		mtl.clear();
	}
	else
	{
		return false;
	}
	
	return true;
}
std::string ObjGeometryImporter::read(std::string& inStream, int& crt)
{
	std::string str;
	char tmp = '\0';

	if (crt >= inStream.size())
		return str;

	do
	{
		str.append(&inStream[crt++], 1 );

	} while (crt < inStream.size() && inStream[crt] != ' ' && inStream[crt] != '\n');

	while (inStream[crt] == ' ' || inStream[crt] == '\n')
	{
		crt++;
	}

	return str;
}
std::string ObjGeometryImporter::readLine(std::string& inStream, int& carret)
{
	std::string out = "";
	while (carret < inStream.size() && inStream[carret] != '\n')
	{
		out += inStream[carret++];
	}

	//Set at right index
	while (inStream[carret] == '\n' || inStream[carret] == ' ')
		carret++;

	return out;

	//char line[255];
	//inStream.getline(line, 255, '\n');
	//
	//this->strInData = line;
	//
	////Remove junk, aka spacing
	//int left = 0;
	//int right = (int)this->strInData.length()-1;
	//while(left < right && this->strInData[left] == ' ')
	//	left++;
	//while(right > left && this->strInData[right] == ' ') // !(isalpha(this->strInData[right]) || isdigit(this->strInData[right]))) 
	//	right--;
	//	
	//
	//if(left > 0 && left < (int)this->strInData.length() - 1)
	//	this->strInData.erase(0, left);
	//if(right > 0 && right < (int)this->strInData.length())
	//	this->strInData.erase(right, this->strInData.length() - 1);
	//
	//return this->strInData;
}


/*
#include "ObjGeometryImporter.h"

#include <algorithm>
#include <sstream>
#include <ctime>


static std::vector<std::string>& stringSplit(std::string& p_sstring, char p_cDel, bool ignoreEmpty = true)
{
	static std::vector<std::string> parts;
	parts.clear();

	int start = 0;
	int endIndex = (int)p_sstring.length() - 1;
	int charCount = 0;
	std::string temp = "";

	int k = 0;
	for (int i = 0; i <= endIndex; i)
	{
		for (k = i; k <= endIndex && p_sstring[k] != p_cDel; k++)
		{
			temp += p_sstring[k];
		}
		if (temp != "" || !ignoreEmpty)
		{
			parts.push_back(temp);
			temp = "";
		}
		i = k + 1;
	}

	return parts;
}
ObjGeometryImporter::obj_float3 AlphaToFloat3(std::string val)
{
	ObjGeometryImporter::obj_float3 v(0.0f, 0.0f, 0.0f);

	std::vector<std::string> part = stringSplit(val, ' ');
	for (size_t i = 0; i<part.size(); i++)
	{
		switch (i)
		{
		case 0:	//x
			v.x = (float)atof(part[i].c_str());
			break;
		case 1:	//y
			v.y = (float)atof(part[i].c_str());
			break;
		case 2:	//z
			v.z = (float)atof(part[i].c_str());
			break;
		}
	}
	return v;
}

ObjGeometryImporter::ObjGeometryImporter()
{ }
ObjGeometryImporter::~ObjGeometryImporter() 
{
	if(this->inStream.is_open())
		this->inStream.close();
}	
void ObjGeometryImporter::Tidy()
{
	this->faces.clear();
	this->faces.resize(0);

	this->vertecies.v.clear();
	this->vertecies.vn.clear();
	this->vertecies.vt.clear();

	this->vertecies.v.resize(0);
	this->vertecies.vn.resize(0);
	this->vertecies.vt.resize(0);
	
	this->strInData = "";

	if (this->inStream.is_open())
		this->inStream.close();
	this->inStream.clear();

}

bool  ObjGeometryImporter::LoadGeometry(std::string filename, std::vector<float>& out_floatData, unsigned int& nrOfVertecies, unsigned int& vertexStride, std::vector<Material>& out_material)
{
	bool result = true;

	this->inStream.open(filename);

	if(!this->inStream.is_open())
	{ return false; }
	std::stringstream ss;
	
	while (!this->inStream.eof())
	{
		read(this->inStream);

		if (this->strInData == "#" || this->strInData == "s")	{ readLine			(this->inStream);	}
		else if (this->strInData == "v"	)						{ processPosition	();	}
		else if (this->strInData == "vt"	)					{ processTexCoord	();	}
		else if (this->strInData == "vn"	)					{ processNormal		();	}
		else if (this->strInData == "f"	)						{ processFace		();	}
		else if (this->strInData == "mtllib")					{ if (!processMtlFile(filename, out_material)) { Tidy(); return false; } }
	}
	nrOfVertecies = 0;
	for (size_t i = 0; i < this->faces.size(); i += 1)
	{
		for (size_t k = 0; k < 3; k++)
		{
			nrOfVertecies++;
			//V
			out_floatData.push_back(this->vertecies.v[this->faces[i].vertecies[k].vIndex].x);
			out_floatData.push_back(this->vertecies.v[this->faces[i].vertecies[k].vIndex].y);
			out_floatData.push_back(this->vertecies.v[this->faces[i].vertecies[k].vIndex].z);
			//VN
			out_floatData.push_back(this->vertecies.vn[this->faces[i].vertecies[k].vnIndex].x);
			out_floatData.push_back(this->vertecies.vn[this->faces[i].vertecies[k].vnIndex].y);
			out_floatData.push_back(this->vertecies.vn[this->faces[i].vertecies[k].vnIndex].z);
			//VT
			out_floatData.push_back(this->vertecies.vt[this->faces[i].vertecies[k].vtIndex].x);
			out_floatData.push_back(this->vertecies.vt[this->faces[i].vertecies[k].vtIndex].y);
		}
	}

	//nrOfVertecies = this->vertecies.v.size();
	vertexStride = sizeof(float)* 8;
	Tidy();

	return result;
}


void ObjGeometryImporter::processPosition()
{
	read(this->inStream);
	float x = (float)atof(this->strInData.c_str());
	read(this->inStream);
	float y = (float)atof(this->strInData.c_str());
	read(this->inStream);
	float z = (float)atof(this->strInData.c_str());


	this->vertecies.v.push_back(obj_float3(x, y, (-1.0f * z)));
}
void ObjGeometryImporter::processNormal()			   
{
	read(this->inStream);
	float x = (float)atof(this->strInData.c_str());
	read(this->inStream);
	float y = (float)atof(strInData.c_str());
	read(this->inStream);
	float z = (float)atof(strInData.c_str());

	this->vertecies.vn.push_back(obj_float3(x, y, (-1 * z)));
}
void ObjGeometryImporter::processTexCoord()
{
	read(this->inStream);
	float x = (float)atof(this->strInData.c_str());
	read(this->inStream);
	float y = 1.0f - (float)atof(this->strInData.c_str());
	
	this->vertecies.vt.push_back(obj_float2(x, y));

}
void ObjGeometryImporter::processFace()			   
{
	std::vector<std::string> facePart = stringSplit(readLine(this->inStream), ' ');
	
	Face f;
	Face::VertIndex v;
 	std::vector<std::string> values;

	for (int i = 0; i < (int)facePart.size(); i++)
	{
		values = stringSplit(facePart[i], '/');
	
		if((int)facePart.size() > 0)
		{
			if(values[0] != "")
				v.vIndex = atoi(values[0].c_str()) - 1; 
	
			if(values.size() < 3)
			{
				if(this->vertecies.vt.size() > 0)
					v.vtIndex = atoi(values[1].c_str()) - 1;
				else if(this->vertecies.vn.size() > 0)
					v.vnIndex = atoi(values[1].c_str()) - 1;
			}
			else
			{
				if(values[1] != "")
					v.vtIndex = atoi(values[1].c_str()) - 1;
				if(values[2] != "")	
					v.vnIndex = atoi(values[2].c_str()) - 1;
			}
		}
	
		f.vertecies[i] = v;
	}
	this->faces.push_back(f);
}
bool ObjGeometryImporter::processMtlFile(std::string& currentDir, std::vector<Material>& material)
{
	readLine(this->inStream);
	std::string dir = currentDir.substr(0, currentDir.find_last_of('/') + 1);
	if (dir.size() == 0)
		dir = currentDir.substr(0, currentDir.find_last_of('\\') + 1);
	std::string mtlFilePath = dir + this->strInData;
	int pos = -1;
	int namelessCount = 0;
	
	std::ifstream mtl;
	mtl.open(mtlFilePath);
	if(mtl.is_open())
	{
		Material newMat;

		while (!mtl.eof())
		{
			read(mtl);
			if(mtl.eof())
				break;
	
			if(this->strInData == "newmtl") 
			{
				if (readLine(mtl) != "")
				{
					newMat.Name = this->strInData;
					material.push_back(newMat);
					pos = (int)material.size() - 1;
				}
				else
				{
					newMat.Name = "default_" + std::to_string((_LONGLONG)namelessCount++);
					material.push_back(newMat);
					pos = (int)material.size() - 1;
				}
			}
			else if (this->strInData == "#")				{ readLine(mtl); }
			else if (this->strInData == "Ka")			{ material[pos].Ka			= AlphaToFloat3(readLine(mtl)); }
			else if (this->strInData == "Kd")			{ material[pos].Kd			= AlphaToFloat3(readLine(mtl)); }
			else if (this->strInData == "Ks")			{ material[pos].Ks			= AlphaToFloat3(readLine(mtl)); }
			else if (this->strInData == "Tf")			{ material[pos].Tf			= AlphaToFloat3(readLine(mtl)); }
			else if (this->strInData == "illum")		{ material[pos].Illum		= atoi (readLine(mtl).c_str()); }
			else if (this->strInData == "Ni")			{ material[pos].Ni			= (float)atof(read(mtl).c_str()); }
			else if (this->strInData == "Ns")			{ material[pos].Ns			= (float)atof(read(mtl).c_str()); }
			else if (this->strInData == "map_Ka")		{ material[pos].map_Ka		= readLine(mtl); }
			else if (this->strInData == "map_Td")		{ material[pos].map_Td		= readLine(mtl); }
			else if (this->strInData == "map_Kd")		{ material[pos].map_Kd		= readLine(mtl); }
			else if (this->strInData == "map_Ks")		{ material[pos].map_Ks		= readLine(mtl); }
			else if (this->strInData == "disp")			{ material[pos].disp		= readLine(mtl); }
			else if (this->strInData == "bump")			{ material[pos].bump		= readLine(mtl); }
			else if (this->strInData == "occlusion")	{ material[pos].occlusion	= readLine(mtl); }
			
		}
		mtl.close();
	}
	else
	{
		return false;
	}
	
	return true;
}
std::string ObjGeometryImporter::read(std::ifstream& inStream)
{
	inStream >> this->strInData;
	return this->strInData;
}
std::string ObjGeometryImporter::readLine(std::ifstream& inStream)		   
{
	char line[255];
	inStream.getline(line, 255, '\n');
	this->strInData = line;
	
	//Remove junk, aka spacing
	int left = 0;
	int right = (int)this->strInData.length()-1;
	while(left < right && this->strInData[left] == ' ')
		left++;
	while(right > left && this->strInData[right] == ' ')
		right--;
		
	
	if(left > 0 && left < (int)this->strInData.length() - 1)
		this->strInData.erase(0, left);
	if(right > 0 && right < (int)this->strInData.length() - 1)
		this->strInData.erase(right, this->strInData.length() - 1);

	return this->strInData;
}

*/