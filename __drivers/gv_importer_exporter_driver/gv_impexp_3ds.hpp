#include "gv_framework_private.h"
#include "gv_importer_exporter.h"

#pragma warning(disable : 4305)
#pragma warning(disable : 4309)
#pragma warning(disable : 4101)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>

namespace detail_3ds
{

/*  ///////////////////////////////////
SAD MAN'S 3DS LOADER             v0.07
LGPL v2    by Caiwan, KitKett and Geri
//////////////////////////////////////
// This loader IS TESTED and IT IS WORKING.
// Supports up to 256 mesh in a model
// Supports texture file names (if a mesh has no textures name, previous will be
cloned to it)
// Normals are currently ignored and calculated automatically by the loader
// SAD MAN'S 3DS LOADER is written in C++, but interfaces are in C language

// THE PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT
// ANY WARRANTY. IT IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
// EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS
// TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM
// PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR
// OR CORRECTION.

// LICENSE: LGPL v2

// Tested compilers: GCC and MSVC

///////////
usage:
///////////

if(!sm3DSLoad(modellfile)){ // loads FILE * modellfile to the memory. WARNING:
use "rb" in fopen instead of "r"!!!
fclose(modellfile);  // we faild for some reason that sould not happend.
return;
}
int meshek=sm3DSget_number_of_mesh(); // tells the number of the meshes
if(meshek>254) meshek=254;
for(int i=0;i<meshek;i++){
char texturaneve[14];
int vtex=sm3DSget_texture_name(i, texturaneve); // store textures name to
texturaneve, and returns 1 if texture name is exist. if no texture, returns 0
float * v=sm3DSget_vtx(i); // returns pointer for geometry
float * t=sm3DSget_tex(i); // returns pointer for uv
float * n=sm3DSget_nrm(i); // returns pointer for normals
int f=sm3DSget_nmbr_faces(i); // returns the number of triangles
if(vtex){
YOUR_GAME_ENGINE_INPUT(modellszam, i, v, t, n, f, texturaneve);
}else{
YOUR_GAME_ENGINE_INPUT(modellszam, i, v, t, n, f, NULL);
}
}
sm3DS_die(); // freeing up everything
fclose(modellfile);
*/

// #define DEBUG_3DS_LOADER
#define snprintf _snprintf

int weld3dsfiled = 0;
int m3dsfile_maxdata = 0;
int number_of_mesh_3ds = 0;
int osszes_polygon_3ds[512];
float* polygon_3ds[512];
float* uv_3ds[512];
float* normal_3ds[512];
char texneve[512][14];
int ukchkFAIL = 0;

#ifndef __CAI_VECTORS_
#define __CAI_VECTORS_
#define VECTOR_ERR_DIV_BY_ZERO 200;

#ifdef DEBUG_3DS_LOADER
#define _MK_DUMP
#endif

template < class T >
class vector3d3dsloaderbb
{
private:
	// ..
public:
	T x, y, z;
	explicit vector3d3dsloaderbb(); // set zero constructor
	vector3d3dsloaderbb(T, T, T);   // set default value constructor
	vector3d3dsloaderbb(const vector3d3dsloaderbb&);
	vector3d3dsloaderbb& operator=(vector3d3dsloaderbb); // overload operators
	vector3d3dsloaderbb operator+(const vector3d3dsloaderbb&);
	vector3d3dsloaderbb operator-(const vector3d3dsloaderbb&);
	vector3d3dsloaderbb operator*(T); // :?
	vector3d3dsloaderbb operator/(T);
	vector3d3dsloaderbb operator+=(T);
	vector3d3dsloaderbb operator-=(T);
	vector3d3dsloaderbb operator*=(T);
	vector3d3dsloaderbb operator/=(T);
	// extra vector fn.s
	T vlen();		// vector length using pythagorean theorem
	void setZero(); // set
};

// ------------------------------------------------
template < class T >
vector3d3dsloaderbb< T >::vector3d3dsloaderbb(void)
{
	x = 0;
	y = 0;
	z = 0;
}
template < class T >
vector3d3dsloaderbb< T >::vector3d3dsloaderbb(T a, T b, T c)
{
	x = a;
	y = b;
	z = c;
}
template < class T >
vector3d3dsloaderbb< T >::vector3d3dsloaderbb(const vector3d3dsloaderbb< T >& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}
template < class T >
vector3d3dsloaderbb< T >& vector3d3dsloaderbb< T >::
operator=(vector3d3dsloaderbb< T > v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::
operator+(const vector3d3dsloaderbb& v)
{
	vector3d3dsloaderbb< T > tmp(*this);
	tmp.x += v.x;
	tmp.y += v.y;
	tmp.z += v.z;
	return tmp;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::
operator-(const vector3d3dsloaderbb& v)
{
	vector3d3dsloaderbb< T > tmp(*this);
	tmp.x -= v.x;
	tmp.y -= v.y;
	tmp.z -= v.z;
	return tmp;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::operator+=(T a)
{
	x += a;
	y += a;
	z += a;
	return *this;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::operator-=(T a)
{
	x -= a;
	y -= a;
	z -= a;
	return *this;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::operator*(T a)
{
	x *= a;
	y *= a;
	z *= a;
	return *this;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::operator/(T a)
{
	if (a == 0)
		a = 1;
	x /= a;
	y /= a;
	z /= a;
	return *this;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::operator*=(T a)
{
	x *= a;
	y *= a;
	z *= a;
	return *this;
}
template < class T >
vector3d3dsloaderbb< T > vector3d3dsloaderbb< T >::operator/=(T a)
{
	if (a == 0)
		a = 1;
	x /= a;
	y /= a;
	z /= a;
	return *this;
}
template < class T >
T vector3d3dsloaderbb< T >::vlen()
{
	return sqrt((x * x) + (y * y) + (z * z)); // pythagorean theorem
}
template < class T >
void vector3d3dsloaderbb< T >::setZero()
{
	x = y = z = 0;
}
#endif
#ifndef __CAI_MODELMGR_
#define __CAI_MODELMGR_
#define FW_CALCEDNORMALS 0x01
namespace FWtexgen
{ // this stucture is mainly defined in FWtexgen namespace in
  // textureGenerator.h
typedef struct
{ // i just copied here
	float r, g, b, a;
} rgbaCol_t;
}
namespace FWmodels
{ // everything is joined in one namespace
typedef vector3d3dsloaderbb< float > verticies_t;
typedef vector3d3dsloaderbb< float > normals_t;
typedef vector3d3dsloaderbb< unsigned short > meshes_t;
typedef struct
{
	float u, v;
} texUV_t;
typedef struct
{
	vector3d3dsloaderbb< float > x1, x2, x3, o;
} localCoord_t;
typedef struct
{
	std::string name;
	std::vector< unsigned short > face;
} materialNfo_t;
// class to sore modles (can store only one)
class model
{
private:
	int err;
	short isNormalsCalced; // bitfields?

public:
	std::vector< verticies_t > vertList; // mesh data
	std::vector< meshes_t > meshList;
	std::vector< normals_t > normList;
	std::vector< FWmodels::texUV_t > uvList;

	localCoord_t localCoord; // local coords	?

	// material data?
	std::vector< materialNfo_t > matList;

	model();
	~model();
	// virtual void loadRawFile(char*);
	void calcNormals();
	void drawPlainModel(); // draw the raw model
	int getError() const
	{
		return this->err;
	}; // is error fv.
};
class material
{ // maybe a struct? :/
protected:
	// float * ambP, diffP, specP;	//pointers for the float arrays
public:
	std::string name;
	char nametex[16];
	FWtexgen::rgbaCol_t ambient, diffusal, specular;

	material();

	void bindMaterial(bool);

	/*float* getAmbientP() const;
  float* getSpecularP() const;
  float* getDiffusalP() const;*/
};
extern material* prev_material; // previously (maybe the actual) binded class pointer
};
#endif
#ifndef _C_3DSMAX_FORMAT_LOADER_
#define _C_3DSMAX_FORMAT_LOADER_

#ifdef _DEBUG // allow dump
//#define _MK_DUMP
#endif
#ifdef _MK_DUMP
#define ASSERT(x) x; // automatic asseriton
#else
#define ASSERT(x)
#endif
/*
some usage nfo will written here.
*/

// The chunk's id numbers
#define M3DS_MAIN3DS 0x4D4D
#define M3DS_MAIN_VERS 0x0002
#define M3DS_EDIT3DS 0x3D3D
#define M3DS_MESH_VERS 0x3D3E
#define M3DS_OBJECT 0x4000
#define M3DS_TRIG_MESH 0x4100
#define M3DS_VERT_LIST 0x4110
#define M3DS_FACE_DESC 0x4120
#define M3DS_FACE_MAT 0x4130
#define M3DS_TEX_VERTS 0x4140
#define M3DS_SMOOTH_GROUP 0x4150
#define M3DS_LOCAL_COORDS 0x4160
#define M3DS_MATERIAL 0xAFFF
#define M3DS_MAT_NAME 0xA000
#define M3DS_MAT_AMBIENT 0xA010
#define M3DS_MAT_DIFFUSE 0xA020
#define M3DS_MAT_SPECULAR 0xA030
#define M3DS_SHINY_PERC 0xA040
#define M3DS_SHINY_STR_PERC 0xA041
#define M3DS_TRANS_PERC 0xA050
#define M3DS_TRANS_FOFF_PERC 0xA052
#define M3DS_REF_BLUR_PERC 0xA053
#define M3DS_RENDER_TYPE 0xA100
#define M3DS_SELF_ILLUM 0xA084
#define M3DS_MAT_SELF_ILPCT 0xA08A
#define M3DS_WIRE_THICKNESS 0xA087
#define M3DS_MAT_TEXMAP 0xA200
#define M3DS_MAT_MAPNAME 0xA300
#define M3DS_ONE_UNIT 0x0100

// we don't need:
#define M3DS_KEYF3DS 0xB000
#define M3DS_FRAMES 0xB008
#define M3DS_MESH_INFO 0xB002
#define M3DS_HIER_POS 0xB030
#define M3DS_HIER_FATHER 0xB010
#define M3DS_PIVOT_PT 0xB013
#define M3DS_TRACK00 0xB020
#define M3DS_TRACK01 0xB021
#define M3DS_TRACK02 0xB022
#define M3DS_COLOR_RGB 0x0010
#define M3DS_COLOR_TRU 0x0011
#define M3DS_COLOR_TRUG 0x0012
#define M3DS_COLOR_RGBG 0x0013
#define M3DS_PERC_INT 0x0030
#define M3DS_PERC_FLOAT 0x0031

namespace FWmodels
{
class model3ds
{
private:
#ifdef _MK_DUMP
	std::fstream log;
#endif
	typedef struct
	{
		unsigned short id;
		unsigned long len;
	} chunkInf;

	unsigned char* data; // file data
	unsigned long fSize;
	unsigned short lVertNum; // local sor for the number vert. and mesh;
	unsigned short lMeshNum; // the vector template also can store everything

	int error; // errcode; value -> pos. of the error; 0 is OK
	bool isResource;
	// chunk processors
	void procObject(unsigned int, model&);
	void procMaterial(unsigned int, material&);

	// chink info grabbers
	chunkInf getChunkInfo(const unsigned int);
	void get3fVector(const unsigned int, float*, float*, float*);
	void get2sVector(const unsigned int, short*, short*);

	void pharseData();

public:
	model3ds(const char*);
	model3ds(const unsigned char*, unsigned long);
	~model3ds();

	void drawModels();

	std::vector< FWmodels::model > objects;
	std::vector< FWmodels::material > materials;

	int getError() const
	{
		return this->error;
	};
};
};
#endif
#ifdef _DEBUG // allow dump
#endif

using namespace std;
using namespace FWmodels; // framework models namespace

model3ds::model3ds(const char* input)
{
	/*	this->error =0; this->isResource = false;

  //if (!fIsExist(input)) {this->error = 1; return; }

  FILE * fp = fopen (input, "rb");

  if (fp == NULL) {this->error = 1; return; }

  fseek(fp, 0, SEEK_END);
  this->fSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  data = new unsigned char [fSize];
  fread(data, fSize, 1, fp);

  fclose(fp);

  this->pharseData();*/
}

model3ds::model3ds(const unsigned char* input, unsigned long len)
{
	this->isResource = true;
	data = const_cast< unsigned char* >(input);
	this->fSize = len;
	this->pharseData();
}

model3ds::~model3ds()
{
#ifdef _MK_DUMP
	log << "end debug file.";
	log.close();
#endif
	if ((data) && (!this->isResource))
		delete[] data;
}
// TODO: make allow to read it form PK file

void model3ds::pharseData()
{

#ifdef _MK_DUMP
	log.open("log.txt", std::ios::out);
	log << "3DS loader runtime dump file." << endl;
	log << ">USE IN DEBUG MODE ONLY !!! <" << endl;
	log << "-----------------------------" << endl;
#endif

	chunkInf mainCh = this->getChunkInfo(0);
	if ((this->fSize != mainCh.len) & (mainCh.id != M3DS_MAIN3DS))
	{
		this->error = 2;
		return;
	}

	unsigned int reloff = 0, offset = 6;
// this->objects = new vector<model>;
// this->materials = new vector<material>;

#ifdef DEBUG_3DS_LOADER
	ASSERT(char asda[1024]);
#endif

	while (offset < this->fSize)
	{
		chunkInf chunk = this->getChunkInfo(offset);

#ifdef DEBUG_3DS_LOADER
		ASSERT(sprintf(asda, "@%06x +%06x $%04x ", offset, chunk.len, chunk.id));
#endif
		switch (chunk.id)
		{
		case M3DS_EDIT3DS:
			offset += 6;
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << " M3DS_EDIT3DS chunk" << endl);
#endif
			break;

		case M3DS_OBJECT:
		{
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << " M3DS_OBJECT Triangle object chunk " << endl);
#endif
			reloff = 6;
			// get pass the header
			while (data[offset + reloff] != '\0')
				reloff++; // here is the name of the mesh, which we skip
			reloff++;

			// create a new model object
			model currentMod;

			// load the data into the object
			procObject(offset + reloff, currentMod); // offset @ current chunk
			currentMod.calcNormals();				 // TODO: add normal loader/ skip this if there's
													 // no normals
			// add this object into the model vector//
			this->objects.push_back(currentMod);

			// printf("object %d\n", this->objects->size());
			offset += chunk.len;
		}
		break;

		case M3DS_MATERIAL:
		{
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << " M3DS_MATERIAL Material chunk" << endl);
#endif

			material currentMat;
			this->procMaterial(offset, currentMat);
			this->materials.push_back(currentMat);
			offset += chunk.len;
		}
		break;

		default:
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << " UNKNOWN chunk " << endl);
#endif
			offset += chunk.len;
			ukchkFAIL++;
			if (ukchkFAIL > 1024)
				return; // fail
			break;
		}
	}
#ifdef DEBUG_3DS_LOADER
	ASSERT(printf("3DS Objects loaded: %d\n", this->objects.size()));
#endif
}

char materialok_nevei[256][16];
char materialok_nevei_tex[256][16];
int ujmat = 0;
void model3ds::procMaterial(unsigned int offs, material& curr)
{
	ujmat++;
	unsigned int offset = offs, reloff = 6;
	chunkInf matchunk = getChunkInfo(offset);

	while (data[offset + reloff] != '\0')
	{
		curr.name += (data[offset + reloff]);
		reloff++; // here is the name of the mesh, what we skip
	}

	for (int i = offset + reloff; i < m3dsfile_maxdata - 3; i++)
	{
		if (data[i] == '.')
		{
			if ((tolower(data[i + 1]) == 'j' && tolower(data[i + 2]) == 'p' &&
				 tolower(data[i + 3]) == 'g') ||
				(tolower(data[i + 1]) == 'b' && tolower(data[i + 2]) == 'm' &&
				 tolower(data[i + 3]) == 'p') ||
				(tolower(data[i + 1]) == 't' && tolower(data[i + 2]) == 'g' &&
				 tolower(data[i + 3]) == 'a') ||
				(tolower(data[i + 1]) == 'p' && tolower(data[i + 2]) == 'n' &&
				 tolower(data[i + 3]) == 'g'))
			{
				// ok megvan a neve a kis gecinek, akkor most az elej?re m?szunk
				for (int j = i; j > 0; j--)
				{
					if (data[j] == '\0')
					{
						j++;
						snprintf(curr.nametex, 13, "%s", &data[j]);
						snprintf(materialok_nevei_tex[ujmat - 1], 13, "%s", &data[j]);
						i = 99999999999ll;
						assert(0);
						break;
					}
				}
			}
		}
	}

	unsigned short val; // bit int
	while (reloff <= matchunk.len)
	{
		chunkInf chunk = getChunkInfo(offs + reloff);
		// material_t mater; memset(&mater, NULL, sizeof(material_t));
		switch (chunk.id)
		{
		case M3DS_MAT_NAME:
		{
			reloff += 6;
			int kezd = offset + reloff;
			while (data[offset + reloff] != '\0')
			{
				curr.name += (data[offset + reloff]);
				reloff++; // here is the name of the mesh, what we skip
			}
			snprintf(materialok_nevei[ujmat - 1], 13, "%s", &data[kezd]);
			reloff++;
		}
		break;

		case M3DS_MAT_AMBIENT:
		case M3DS_MAT_DIFFUSE:
		case M3DS_MAT_SPECULAR:
		{
			FWtexgen::rgbaCol_t temp_col;
			reloff += chunk.len - 3;
			temp_col.r = (float)((int)data[offset + reloff]) / 255;
			temp_col.g = (float)((int)data[offset + reloff + 1]) / 255;
			temp_col.b = (float)((int)data[offset + reloff + 2]) / 255;
			temp_col.a = 1;
			reloff += 3;

			switch (chunk.id)
			{
			case M3DS_MAT_AMBIENT:
				memcpy(&curr.ambient, &temp_col, sizeof(temp_col));
				break;
			case M3DS_MAT_DIFFUSE:
				memcpy(&curr.diffusal, &temp_col, sizeof(temp_col));
				break;
			case M3DS_MAT_SPECULAR:
				memcpy(&curr.specular, &temp_col, sizeof(temp_col));
				break; // lalala :)
			}
		}
		break;
		default:
			reloff += chunk.len;
			break;
		}
	}
}

void model3ds::procObject(const unsigned int offs, model& mobj)
{
	unsigned int offset = offs + 6; // skip the curren ch header
	chunkInf chunk,
		objCh = getChunkInfo(offs); // get info and retun when fail (TODO)

#ifdef DEBUG_3DS_LOADER
	ASSERT(log << "  PROC TRIANGLE MESH:");
#endif
	char asda[1024];

	this->error = 0;
	if (objCh.id != M3DS_TRIG_MESH)
	{ // wrong offset position, do nothing
		this->error = 1;
		return;
	}

#ifdef DEBUG_3DS_LOADER
	ASSERT(log << " ok" << endl);
#endif

	short sVal;						// allocate a sotage for various values
	unsigned int uiVal, reloff = 0; // and relative offset

	while (reloff < objCh.len)
	{
		chunk = getChunkInfo(offset + reloff);
#ifdef DEBUG_3DS_LOADER
		ASSERT(sprintf(asda, "@%06x +%06x $%04x ", offset + reloff, chunk.len,
					   chunk.id));
#endif

		switch (chunk.id)
		{
		case M3DS_VERT_LIST:
		{ // ------ PROCESS VERT LIST ------
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << " Triangle mesh data");
#endif
			verticies_t tmpVert;

			memcpy(&sVal, &data[offset + reloff + 6], 2); // get mesh no.
			reloff += 8;								  // move past reloff 6+2 bytes

#ifdef DEBUG_3DS_LOADER
			ASSERT(log << " - Coords no: " << sVal << endl);
#endif

			this->lVertNum = sVal;
			for (int i = 0; i < this->lVertNum;
				 i++)
			{ // sVal is the current num of meshes
				// memcpy(&this->tmpVert->x, &data[offset+reloff],	  4);	//
				// memory -> vector(xyz)
				// memcpy(&this->tmpVert->y, &data[offset+reloff+4], 4);
				// memcpy(&this->tmpVert->z, &data[offset+reloff+8], 4);
				get3fVector(offset + reloff, &tmpVert.x, &tmpVert.y, &tmpVert.z);
				reloff += 12; // 3*(float) = 12 bytes

				mobj.vertList.push_back(tmpVert);

				// ASSERT(log << "x="<< tmpVert.x << "; y=" << tmpVert.y << "; z=" <<
				// tmpVert.z << ";" << endl);
			}
		}
		break;

		case M3DS_FACE_DESC:
		{ // ------ PROCESS FACE LIST ------
			meshes_t tmpMesh;

#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << "face description list");
#endif

			memcpy(&sVal, &data[offset + reloff + 6], 2); // get mesh no.
			reloff += 8;								  // move past reloff 6+2 bytes

#ifdef DEBUG_3DS_LOADER
			ASSERT(log << " - Coords no: " << sVal << endl);
#endif

			this->lMeshNum = sVal;
			for (int i = 0; i < this->lMeshNum;
				 i++)
			{												   // sVal is the current num of meshes
				memcpy(&tmpMesh.x, &data[offset + reloff], 2); // memory -> vector(xyz)
				memcpy(&tmpMesh.y, &data[offset + reloff + 2], 2);
				memcpy(&tmpMesh.z, &data[offset + reloff + 4], 2);
				memcpy(&sVal, &data[offset + reloff + 6], 2); // face props (drop)
				reloff += 8;								  // 3*(float) = 12 bytes
				// ...

				mobj.meshList.push_back(tmpMesh);

				// ASSERT(log << "x="<< tmpMesh.x << "; y=" << tmpMesh.y << "; z=" <<
				// tmpMesh.z << "; @" << reloff <<endl);
			}
		}
		break;

		case M3DS_TEX_VERTS:
		{ // ------ PROCESS TEXTURE VERTICIES ------
			short len;
			texUV_t uv_tmp;
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << "texture uv nfo" << endl);
#endif
			memcpy(&len, &data[offset + reloff + 6], 2); // face no
			reloff += 8;								 // keep walking

			for (int i = 0; i < len; i++)
			{ // read data
				memcpy(&uv_tmp.u, &data[offset + reloff], 4);
				memcpy(&uv_tmp.v, &data[offset + reloff + 4], 4);
				reloff += 8;
				mobj.uvList.push_back(uv_tmp); // push into vector
			}
		}
		break;

		case M3DS_FACE_MAT: // ------ PROCESS FACE MATERIALS ------
		{

			vector< unsigned short > meshNo;
			FWmodels::materialNfo_t matNfo_tmp;
			reloff += 6; // keep going, skip chunk
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << "material nfo" << endl);
#endif
			while (data[offset + reloff] != '\0')
			{
				matNfo_tmp.name += (data[offset + reloff]);
				reloff++; // here is the name of the mesh, what we skip
			}
			reloff++;

			unsigned short var, var2;
			memcpy(&var, &data[offset + reloff], 2);
			reloff += 2;
			for (int i = 0; i < var; i++)
			{
				memcpy(&var2, &data[offset + reloff], 2);
				matNfo_tmp.face.push_back(var2);
				reloff += 2;
			}
			mobj.matList.push_back(matNfo_tmp);
		}
		break;

		case M3DS_SMOOTH_GROUP: // ------ PROCESS FACE MATERIALS ------
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << "smoothing groups" << endl);
#endif
			reloff += chunk.len;
			break;

		// ------------------------------------------------------------ //
		case M3DS_LOCAL_COORDS: // ------ LOCAL COORDS ------
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << "local coord nfo");
#endif
// memcpy(&mobj.localCoord->x1.x, &data[offset+reloff+6],	4);	// x1 (
// header: 6 bytes)
// memcpy(&mobj.localCoord->x1.y, &data[offset+reloff+10],	4);	//    (
// ..+ 4 bytes float = 10 ... etc)
// memcpy(&mobj.localCoord->x1.z, &data[offset+reloff+14],	4);

// memcpy(&mobj.localCoord->x2.x, &data[offset+reloff+18], 4);	// x2
// memcpy(&mobj.localCoord->x2.y, &data[offset+reloff+22], 4);
// memcpy(&mobj.localCoord->x2.z, &data[offset+reloff+26], 4);

// memcpy(&mobj.localCoord->x3.x, &data[offset+reloff+30], 4);	// x3
// memcpy(&mobj.localCoord->x3.y, &data[offset+reloff+34], 4);
// memcpy(&mobj.localCoord->x3.z, &data[offset+reloff+38], 4);

// memcpy(&mobj.localCoord->o.x,  &data[offset+reloff+42], 4);	// o
// memcpy(&mobj.localCoord->o.y,  &data[offset+reloff+46], 4);
// memcpy(&mobj.localCoord->o.z,  &data[offset+reloff+50], 4);	// ( 6 +
// 4*3*4 float = 6 + 48 = 50 + 4 = 54 bytes long)
//
/*
ASSERT(sprintf(asda, "%f; %f; %f;",&mobj.localCoord->x1.x,
&mobj.localCoord->x1.y, &mobj.localCoord->x1.z); log << " X1("<<asda<<")");
ASSERT(sprintf(asda, "%f; %f; %f;",&mobj.localCoord->x2.x,
&mobj.localCoord->x2.y, &mobj.localCoord->x2.z); log << " X2("<<asda<<")");
ASSERT(sprintf(asda, "%f; %f; %f;",&mobj.localCoord->x3.x,
&mobj.localCoord->x3.y, &mobj.localCoord->x3.z); log << " X3("<<asda<<")");
ASSERT(sprintf(asda, "%f; %f; %f;",&mobj.localCoord->o.x,
&mobj.localCoord->o.y,  &mobj.localCoord->o.z);	 log << " O("<<asda<<")");
*/
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << endl);
#endif

			// TODO: fuck it doesn't work. offset shift is incorrect
			// make an inline vector reader function: 3f and 2s (float n short)

			reloff += chunk.len;
			break;

		// ------------------------------------------------------------ //
		default:
#ifdef DEBUG_3DS_LOADER
			ASSERT(log << asda << " unknown chunk" << endl);
#endif
			reloff += chunk.len;
			ukchkFAIL++;
			if (ukchkFAIL > 1024)
				return; // fail
			break;
		}

		// cp (loaded data -> model data);
	}
#ifdef DEBUG_3DS_LOADER
	ASSERT(log << "  :RET" << endl);
#endif
}

// -----------------------------------------------------------------------------------------

void model3ds::drawModels()
{
	this->error = 0;
	if (this->objects.empty())
	{
		this->error = 1;
		return;
	}
	for (int i = 0; i < this->objects.size(); i++)
		this->objects[i].drawPlainModel();
}

// -----------------------------------------------------------------------------------------
void model3ds::get3fVector(const unsigned int offs, float* x, float* y,
						   float* z)
{
	memcpy(x, &data[offs], 4);
	memcpy(y, &data[offs + 4], 4);
	memcpy(z, &data[offs + 8], 4);
}

model3ds::chunkInf model3ds::getChunkInfo(const unsigned int offset)
{
	chunkInf nfo;
	memcpy(&nfo.id, &this->data[offset], 2);
	memcpy(&nfo.len, &this->data[offset + 2], 4);
	return nfo;
}

//#include <vector>

//#define _LOG_DUMP
#define _AUTOTEXTURE

using namespace std;
using namespace FWmodels;

model::model()
{
	this->err = 0;			   // cle
	this->isNormalsCalced = 0; // clc
}

model::~model()
{
}

// ===============================================================================================================
// //
void model::calcNormals()
{
	// printf("calc normals\n");
	for (int i = 0; i < this->meshList.size(); i++)
	{
		meshes_t tmpMesh(0, 0, 0);
		tmpMesh = meshList[i];

		vector3d3dsloaderbb< float > a, b, c, v1, v2, normal;

		a = this->vertList[tmpMesh.x];
		b = this->vertList[tmpMesh.y];
		c = this->vertList[tmpMesh.z];

		v1 = a - b;								  // AB
		v2 = c - a;								  // AC
		normal.x = (v1.y * v2.z) - (v1.z * v2.y); // determine the normal vector
		normal.y = -((v2.z * v1.x) - (v2.x * v1.z));
		normal.z = (v1.x * v2.y) - (v1.y * v2.x);

		normal /= normal.vlen();

		this->normList.push_back(normal);
	}
}

void model::drawPlainModel()
{
	// custom drawing porcedure inside the class
}

// ===============================================================================================================
// //

material* FWmodels::prev_material; // define here the extern

material::material()
{
	memset(&this->ambient, 0, sizeof(this->ambient));
	memset(&this->diffusal, 0, sizeof(this->diffusal));
	memset(&this->specular, 0, sizeof(this->specular));

	/*this->ambP = new float[3];
  this->specP = new float[3];
  this->diffP= new float[3];*/
}

void material::bindMaterial(bool force = true)
{
	//...
}

// float* material::getAmbientP(){
//	//...
//}
//
// float* material::getDiffusalP(){
//	//...
//}
//
// float* material::getSpecularP(){
//	//...
//}

int sm3DSLoad(FILE* fail, int WELD = 0)
{
	weld3dsfiled = WELD;
	unsigned char* filedata;
	if (!fail)
		return 0;
	FILE* fp = fail;
	fseek(fp, 0, SEEK_END);
	long flen = ftell(fp);
	m3dsfile_maxdata = flen;
	fseek(fp, 0, SEEK_SET);
	filedata = new unsigned char[flen];
	fread(filedata, flen, 1, fp);
	FWmodels::model3ds test(filedata, flen);
	//	FWmodels::model3ds * test=new model3ds (filedata, flen);
	delete[] filedata;

	number_of_mesh_3ds = 0; // null?z?s, h?tha elk?rta a compiler
	for (int i = 0; i < 511; i++)
	{
		osszes_polygon_3ds[i] = 0;
		polygon_3ds[i] = NULL;
		uv_3ds[i] = NULL;
		normal_3ds[i] = NULL;
		texneve[i][0] = '\0';
	}

	for (int i = 0; i < test.objects.size(); i++)
	{
		polygon_3ds[number_of_mesh_3ds] = (float*)new char[(
			(test.objects[i]).meshList.size() * sizeof(float) * 9)];
		uv_3ds[number_of_mesh_3ds] = (float*)new char[(
			(test.objects[i]).meshList.size() * sizeof(float) * 6)];
		normal_3ds[number_of_mesh_3ds] = (float*)new char[(
			(test.objects[i]).meshList.size() * sizeof(float) * 9)];

		if ((test.objects[i]).matList.size() > 0)
		{
			const char* material_neve = (test.objects[i]).matList[0].name.c_str();
			if (!material_neve)
			{ // ezen nincs text?ra, ejnye
				texneve[number_of_mesh_3ds][0] = '\0';
			}
			else
			{   // ezen van text?ra, meg kell keresni n?v alapj?n a hordoz?
				// materialt.
				if (ujmat > 254)
					ujmat = 254;
				for (int q = 0; q < ujmat; q++)
				{
					if (strstr(materialok_nevei[q], material_neve) &&
						strstr(material_neve, materialok_nevei[q]))
					{
						snprintf(texneve[number_of_mesh_3ds], 13, "%s",
								 materialok_nevei_tex[q]);
						//					printf("megtalalva %s\n",
						//texneve[i]);
					}
				}
			}
		}
		else
		{
			texneve[number_of_mesh_3ds][0] = '\0';
		}

		for (int j = 0; j < (test.objects[i]).meshList.size(); j++)
		{
			osszes_polygon_3ds[number_of_mesh_3ds]++;

			if (!(test.objects[i]).vertList.empty())
			{
				polygon_3ds[number_of_mesh_3ds][j * 9 + 0] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].x].x;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 1] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].x].y;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 2] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].x].z;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 3] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].y].x;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 4] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].y].y;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 5] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].y].z;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 6] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].z].x;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 7] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].z].y;
				polygon_3ds[number_of_mesh_3ds][j * 9 + 8] =
					(test.objects[i]).vertList[(test.objects[i]).meshList[j].z].z;
			}

			if (!(test.objects[i]).uvList.empty())
			{
				uv_3ds[number_of_mesh_3ds][j * 6 + 0] =
					(test.objects[i]).uvList[(test.objects[i]).meshList[j].x].u;
				uv_3ds[number_of_mesh_3ds][j * 6 + 1] =
					(test.objects[i]).uvList[(test.objects[i]).meshList[j].x].v;
				uv_3ds[number_of_mesh_3ds][j * 6 + 2] =
					(test.objects[i]).uvList[(test.objects[i]).meshList[j].y].u;
				uv_3ds[number_of_mesh_3ds][j * 6 + 3] =
					(test.objects[i]).uvList[(test.objects[i]).meshList[j].y].v;
				uv_3ds[number_of_mesh_3ds][j * 6 + 4] =
					(test.objects[i]).uvList[(test.objects[i]).meshList[j].z].u;
				uv_3ds[number_of_mesh_3ds][j * 6 + 5] =
					(test.objects[i]).uvList[(test.objects[i]).meshList[j].z].v;
			}

			if (!(test.objects[i]).normList.empty())
			{
				/*				normal_3ds[number_of_mesh_3ds][j*9
        +0]=(test.objects[i]).normList[(test.objects[i]).meshList[j].x].x;
        normal_3ds[number_of_mesh_3ds][j*9
        +1]=(test.objects[i]).normList[(test.objects[i]).meshList[j].x].y;
        normal_3ds[number_of_mesh_3ds][j*9
        +2]=(test.objects[i]).normList[(test.objects[i]).meshList[j].x].z;
        normal_3ds[number_of_mesh_3ds][j*9
        +3]=(test.objects[i]).normList[(test.objects[i]).meshList[j].y].x;
        normal_3ds[number_of_mesh_3ds][j*9
        +4]=(test.objects[i]).normList[(test.objects[i]).meshList[j].y].y;
        normal_3ds[number_of_mesh_3ds][j*9
        +5]=(test.objects[i]).normList[(test.objects[i]).meshList[j].y].z;
        normal_3ds[number_of_mesh_3ds][j*9
        +6]=(test.objects[i]).normList[(test.objects[i]).meshList[j].z].x;
        normal_3ds[number_of_mesh_3ds][j*9
        +7]=(test.objects[i]).normList[(test.objects[i]).meshList[j].z].y;
        normal_3ds[number_of_mesh_3ds][j*9
        +8]=(test.objects[i]).normList[(test.objects[i]).meshList[j].z].z;*/

				/*				normal_3ds[number_of_mesh_3ds][j*9
        +0]=polygon_3ds[number_of_mesh_3ds][j*9  +0] -
        polygon_3ds[number_of_mesh_3ds][j*9  +3] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +6] +
        polygon_3ds[number_of_mesh_3ds][j*9  +0]);
        normal_3ds[number_of_mesh_3ds][j*9
        +1]=polygon_3ds[number_of_mesh_3ds][j*9  +1] -
        polygon_3ds[number_of_mesh_3ds][j*9  +4] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +7] +
        polygon_3ds[number_of_mesh_3ds][j*9  +1]);
        normal_3ds[number_of_mesh_3ds][j*9
        +2]=polygon_3ds[number_of_mesh_3ds][j*9  +2] -
        polygon_3ds[number_of_mesh_3ds][j*9  +5] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +8] +
        polygon_3ds[number_of_mesh_3ds][j*9  +2]);
        normal_3ds[number_of_mesh_3ds][j*9
        +3]=polygon_3ds[number_of_mesh_3ds][j*9  +3] -
        polygon_3ds[number_of_mesh_3ds][j*9  +6] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +0] +
        polygon_3ds[number_of_mesh_3ds][j*9  +3]);
        normal_3ds[number_of_mesh_3ds][j*9
        +4]=polygon_3ds[number_of_mesh_3ds][j*9  +4] -
        polygon_3ds[number_of_mesh_3ds][j*9  +7] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +1] +
        polygon_3ds[number_of_mesh_3ds][j*9  +4]);
        normal_3ds[number_of_mesh_3ds][j*9
        +5]=polygon_3ds[number_of_mesh_3ds][j*9  +5] -
        polygon_3ds[number_of_mesh_3ds][j*9  +8] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +2] +
        polygon_3ds[number_of_mesh_3ds][j*9  +5]);
        normal_3ds[number_of_mesh_3ds][j*9
        +6]=polygon_3ds[number_of_mesh_3ds][j*9  +6] -
        polygon_3ds[number_of_mesh_3ds][j*9  +0] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +3] +
        polygon_3ds[number_of_mesh_3ds][j*9  +6]);
        normal_3ds[number_of_mesh_3ds][j*9
        +7]=polygon_3ds[number_of_mesh_3ds][j*9  +7] -
        polygon_3ds[number_of_mesh_3ds][j*9  +1] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +4] +
        polygon_3ds[number_of_mesh_3ds][j*9  +7]);
        normal_3ds[number_of_mesh_3ds][j*9
        +8]=polygon_3ds[number_of_mesh_3ds][j*9  +8] -
        polygon_3ds[number_of_mesh_3ds][j*9  +2] -
        (polygon_3ds[number_of_mesh_3ds][j*9  +5] +
        polygon_3ds[number_of_mesh_3ds][j*9  +8]);

        float nrm;
        nrm=1.0f/sqrtf(normal_3ds[number_of_mesh_3ds][j*9
        +0]*normal_3ds[number_of_mesh_3ds][j*9
        +0]+normal_3ds[number_of_mesh_3ds][j*9
        +1]*normal_3ds[number_of_mesh_3ds][j*9
        +1]+normal_3ds[number_of_mesh_3ds][j*9
        +2]*normal_3ds[number_of_mesh_3ds][j*9  +2]);
        normal_3ds[number_of_mesh_3ds][j*9  +0]*=nrm;
        normal_3ds[number_of_mesh_3ds][j*9  +1]*=nrm;
        normal_3ds[number_of_mesh_3ds][j*9  +2]*=nrm;

        nrm=1.0f/sqrtf(normal_3ds[number_of_mesh_3ds][j*9
        +3]*normal_3ds[number_of_mesh_3ds][j*9
        +3]+normal_3ds[number_of_mesh_3ds][j*9
        +4]*normal_3ds[number_of_mesh_3ds][j*9
        +4]+normal_3ds[number_of_mesh_3ds][j*9
        +5]*normal_3ds[number_of_mesh_3ds][j*9  +5]);
        normal_3ds[number_of_mesh_3ds][j*9  +3]*=nrm;
        normal_3ds[number_of_mesh_3ds][j*9  +4]*=nrm;
        normal_3ds[number_of_mesh_3ds][j*9  +5]*=nrm;

        nrm=1.0f/sqrtf(normal_3ds[number_of_mesh_3ds][j*9
        +6]*normal_3ds[number_of_mesh_3ds][j*9
        +6]+normal_3ds[number_of_mesh_3ds][j*9
        +7]*normal_3ds[number_of_mesh_3ds][j*9
        +7]+normal_3ds[number_of_mesh_3ds][j*9
        +8]*normal_3ds[number_of_mesh_3ds][j*9  +8]);
        normal_3ds[number_of_mesh_3ds][j*9  +6]*=nrm;
        normal_3ds[number_of_mesh_3ds][j*9  +7]*=nrm;
        normal_3ds[number_of_mesh_3ds][j*9  +8]*=nrm;*/

				float v1_x = polygon_3ds[number_of_mesh_3ds][j * 9 + 3] -
							 polygon_3ds[number_of_mesh_3ds][j * 9 + 0];
				float v1_y = polygon_3ds[number_of_mesh_3ds][j * 9 + 4] -
							 polygon_3ds[number_of_mesh_3ds][j * 9 + 1];
				float v1_z = polygon_3ds[number_of_mesh_3ds][j * 9 + 5] -
							 polygon_3ds[number_of_mesh_3ds][j * 9 + 2];
				float v2_x = polygon_3ds[number_of_mesh_3ds][j * 9 + 6] -
							 polygon_3ds[number_of_mesh_3ds][j * 9 + 0];
				float v2_y = polygon_3ds[number_of_mesh_3ds][j * 9 + 7] -
							 polygon_3ds[number_of_mesh_3ds][j * 9 + 1];
				float v2_z = polygon_3ds[number_of_mesh_3ds][j * 9 + 8] -
							 polygon_3ds[number_of_mesh_3ds][j * 9 + 2];

				float cx = v1_y * v2_z - v2_y * v1_z;
				float cy = v1_z * v2_x - v2_z * v1_x;
				float cz = v1_x * v2_y - v2_x * v1_y;

				normal_3ds[number_of_mesh_3ds][j * 9 + 0] = cx;
				normal_3ds[number_of_mesh_3ds][j * 9 + 1] = cy;
				normal_3ds[number_of_mesh_3ds][j * 9 + 2] = cz;

				normal_3ds[number_of_mesh_3ds][j * 9 + 3] = cx;
				normal_3ds[number_of_mesh_3ds][j * 9 + 4] = cy;
				normal_3ds[number_of_mesh_3ds][j * 9 + 5] = cz;

				normal_3ds[number_of_mesh_3ds][j * 9 + 6] = cx;
				normal_3ds[number_of_mesh_3ds][j * 9 + 7] = cy;
				normal_3ds[number_of_mesh_3ds][j * 9 + 8] = cz;

				float nrm;
				nrm = 1.0f / sqrtf(normal_3ds[number_of_mesh_3ds][j * 9 + 0] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 0] +
								   normal_3ds[number_of_mesh_3ds][j * 9 + 1] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 1] +
								   normal_3ds[number_of_mesh_3ds][j * 9 + 2] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 2]);
				normal_3ds[number_of_mesh_3ds][j * 9 + 0] *= nrm;
				normal_3ds[number_of_mesh_3ds][j * 9 + 1] *= nrm;
				normal_3ds[number_of_mesh_3ds][j * 9 + 2] *= nrm;

				nrm = 1.0f / sqrtf(normal_3ds[number_of_mesh_3ds][j * 9 + 3] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 3] +
								   normal_3ds[number_of_mesh_3ds][j * 9 + 4] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 4] +
								   normal_3ds[number_of_mesh_3ds][j * 9 + 5] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 5]);
				normal_3ds[number_of_mesh_3ds][j * 9 + 3] *= nrm;
				normal_3ds[number_of_mesh_3ds][j * 9 + 4] *= nrm;
				normal_3ds[number_of_mesh_3ds][j * 9 + 5] *= nrm;

				nrm = 1.0f / sqrtf(normal_3ds[number_of_mesh_3ds][j * 9 + 6] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 6] +
								   normal_3ds[number_of_mesh_3ds][j * 9 + 7] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 7] +
								   normal_3ds[number_of_mesh_3ds][j * 9 + 8] *
									   normal_3ds[number_of_mesh_3ds][j * 9 + 8]);
				normal_3ds[number_of_mesh_3ds][j * 9 + 6] *= nrm;
				normal_3ds[number_of_mesh_3ds][j * 9 + 7] *= nrm;
				normal_3ds[number_of_mesh_3ds][j * 9 + 8] *= nrm;
			}
		}
		// normalok posztprocesszalasa
		if (weld3dsfiled)
		{
			register int ipopt = osszes_polygon_3ds[number_of_mesh_3ds];
			for (register int i = 0; i < ipopt; i++)
			{
				for (register int j = 0; j < 3; j++)
				{
					for (register int i2 = 0; i2 < ipopt; i2++)
					{
						for (register int j2 = 0; j2 < 3; j2++)
						{
							register float t21 =
								polygon_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 + 0];
							register float t1 =
								polygon_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 0];
							if (t21 == t1)
							{
								float t22 =
									polygon_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 + 1];
								float t2 = polygon_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 1];
								if (t22 == t2)
								{
									float t23 =
										polygon_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 + 2];
									float t3 = polygon_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 2];
									if (t23 == t3)
									{
										// weld!
										normal_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 + 0] =
											normal_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 0] =
												(normal_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 0] +
												 normal_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 +
																				0]) *
												0.5;
										normal_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 + 1] =
											normal_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 1] =
												(normal_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 1] +
												 normal_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 +
																				1]) *
												0.5;
										normal_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 + 2] =
											normal_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 2] =
												(normal_3ds[number_of_mesh_3ds][i * 9 + j * 3 + 2] +
												 normal_3ds[number_of_mesh_3ds][i2 * 9 + j2 * 3 +
																				2]) *
												0.5;
									}
								}
							}
						}
					}
				}
			}
		}
		number_of_mesh_3ds++;
		if (number_of_mesh_3ds > 253)
		{ // h?jnye, sok lesz
			return 1;
		}
	}
	return 1;
}

int sm3DSget_number_of_mesh()
{
	return number_of_mesh_3ds;
}

int sm3DSget_texture_name(int mesh, char* a)
{
	/*	for(int i=0;i<???;i++){
  if(objects[i].matlist.???==???){
  snprintf(a, 13, "%s", objects[i].matlist.???.nametex);
  return 1;
  }
  }*/
	if (strlen(texneve[mesh]) > 0)
	{
		snprintf(a, 13, "%s", texneve[mesh]);
		return 1;
	}
	return 0;
}

float* sm3DSget_vtx(int mesh)
{
	return polygon_3ds[mesh];
}

float* sm3DSget_tex(int mesh)
{
	return uv_3ds[mesh];
}

float* sm3DSget_nrm(int mesh)
{
	return normal_3ds[mesh];
}

void sm3DS_die()
{
	number_of_mesh_3ds = 0;
	for (int i = 0; i < 511; i++)
	{
		osszes_polygon_3ds[i] = 0;
		if (polygon_3ds[i])
			free(polygon_3ds[i]);
		polygon_3ds[i] = NULL;
		if (uv_3ds[i])
			free(uv_3ds[i]);
		uv_3ds[i] = NULL;
		if (normal_3ds[i])
			free(normal_3ds[i]);
		normal_3ds[i] = NULL;
		texneve[i][0] = '\0';
	}
	ukchkFAIL = 0;
	ujmat = 0;
	return;
}

int sm3DSget_nmbr_faces(int mesh)
{
	return osszes_polygon_3ds[mesh];
}

//             *     ,MMM8&&&.            *
//                  MMMM88&&&&&    .
//                 MMMM88&&&&&&&
//     *           MMM88&&&&&&&&
//                 MMM88&&&&&&&&
//                 'MMM88&&&&&&'
//                   'MMM8&&&'      *
//          |\___/|
//          )     (             .              '
//         =\     /=
//           )===(       *
//          /     \
//          |     |
//         /       \
//         \       /
//  _/\_/\_/\__  _/_/\_/\_/\_/\_/\_/\_/\_/\_/\_
//  |  |  |  |( (  |  |  |  |  |  |  |  |  |  |
//  |  |  |  | ) ) |  |  |  |  |  |  |  |  |  |
//  |  |  |  |(_(  |  |  |  |  |  |  |  |  |  |
//  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
//  jgs|  |  |  |  |  |  |  |  |  |  |  |  |  |

//=========================================================
}

#include "gv_importer_exporter.h"

#include "..\renderer\gv_image.h"
#include "..\renderer\gv_texture.h"
#include "..\renderer\gv_material.h"
#include "..\renderer\gv_index_buffer.h"
#include "..\renderer\gv_vertex_buffer.h"
#include "..\renderer\gv_static_mesh.h"
#include "..\renderer\gv_skeletal_mesh.h"

#include "../animation/gv_animation.h"
#include "../renderer/gv_model.h"

namespace gv
{

class gv_impexp_3ds : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_3ds, gv_importer_exporter);
	gv_impexp_3ds()
	{
		link_class(gv_impexp_3ds::static_class());
	};
	~gv_impexp_3ds(){};

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_3ds);
	};

	gv_class_info* get_import_target_class()
	{
		return gv_model::static_class();
	}

	bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{
		GV_PROFILE_EVENT(gv_impexp_3ds__do_import, 0)
		gv_string ext = gv_global::fm->get_extension(*file_name);
		;
		ext.to_upper();
		if (ext != ".3DS")
			return false;

		using namespace detail_3ds;
		gv_model* pmodel = gvt_cast< gv_model >(target);
		if (!pmodel)
			return false;
		FILE* modellfile = fopen(*file_name, "rb");
		if (!modellfile)
			return false;
		if (!sm3DSLoad(modellfile))
		{						// loads FILE * modellfile to the memory.
								// WARNING: use "rb" in fopen instead of
								// "r"!!!
			fclose(modellfile); // we faild for some reason that sould not happend.
			return false;
		}
		int meshek = sm3DSget_number_of_mesh(); // tells the number of the meshes
		if (meshek > 254)
			meshek = 254;
		for (int i = 0; i < meshek; i++)
		{
			gv_static_mesh* pmesh =
				pmodel->get_sandbox()->create_object< gv_static_mesh >(pmodel);
			pmesh->merge_optimize_vertex();
			pmodel->m_static_mesh.push_back(pmesh);

			gv_vertex_buffer* pvb =
				pmesh->get_sandbox()->create_object< gv_vertex_buffer >(pmesh);
			pmesh->m_vertex_buffer = pvb;
			gv_index_buffer* pib =
				pmesh->get_sandbox()->create_object< gv_index_buffer >(pmesh);
			pmesh->m_index_buffer = pib;

			char texturaneve[256];
			int vtex = sm3DSget_texture_name(i, texturaneve); // store textures name
															  // to texturaneve, and
															  // returns 1 if texture
															  // name is exist. if no
															  // texture, returns 0
			float* v = sm3DSget_vtx(i);						  // returns pointer for geometry
			float* t = sm3DSget_tex(i);						  // returns pointer for uv
			float* n = sm3DSget_nrm(i);						  // returns pointer for normals
			int f = sm3DSget_nmbr_faces(i);					  // returns the number of triangles

			if (v)
			{
				pvb->m_raw_pos.init((gv_vector3*)v, f * 3);
			}
			// if (n)		{	pvb->m_raw_normal.init(
			// (gv_vector3*)n,f*3);	}
			if (t)
			{
				pvb->m_raw_texcoord0.init((gv_vector2*)t, f * 3);
			}
			if (vtex)
			{
				pmesh->m_diffuse_texture =
					pmodel->get_sandbox()->create_object< gv_texture >(pmesh);
				pmesh->m_diffuse_texture->set_file_name(texturaneve);
			}
			pib->m_raw_index_buffer.reserve(f * 3);
			for (int i = 0; i < f * 3; i++)
				pib->m_raw_index_buffer.push_back(i);
			pmesh->merge_optimize_vertex();
		}
		sm3DS_die(); // freeing up everything
		fclose(modellfile);
		return true;
	};

	void get_export_extension(gvt_array< gv_id >& result){

	};
	gv_class_info* get_export_source_class()
	{
		return NULL;
	};
	bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{
		return false;
	};
};

GVM_IMP_CLASS(gv_impexp_3ds, gv_importer_exporter)
GVM_END_CLASS
}