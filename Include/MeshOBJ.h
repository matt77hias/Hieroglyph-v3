//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// This is a simple loader for OBJ ascii files.  The usage concept is that the
// raw data gets loaded into vectors, and the application can then use the data as
// it sees fit.  This simplifies the loading of the files, while not making
// decisions for the developer about how to use the data.
//
// The loader also will load and interpret the associated MTL files, which are
// commonly used to indicate material properties for a model.  You can find more
// information about OBJ and MTL files on their wikipedia pages:
// http://en.wikipedia.org/wiki/Wavefront_.obj_file
// http://paulbourke.net/dataformats/obj/.
//--------------------------------------------------------------------------------


#ifndef MeshOBJ_h
#define MeshOBJ_h
//--------------------------------------------------------------------------------
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "Vector2f.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3 { namespace OBJ {
//--------------------------------------------------------------------------------
class MeshOBJ
{
public:
	MeshOBJ(const std::wstring& filename);

	typedef struct
	{
		std::vector<int> positionIndices;
		std::vector<int> normalIndices;
		std::vector<int> coordIndices;
	} face_t;

	typedef struct
	{
		std::string material_name;
		std::vector<face_t> faces;
	} subobject_t;

	typedef struct {
		std::string name;
		std::vector<subobject_t> subobjects;
	} object_t;

	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> coords;
	std::vector<object_t> objects;
	std::vector<std::string> material_libs;
};

} }
//--------------------------------------------------------------------------------
#endif // MeshOBJ_h
//--------------------------------------------------------------------------------