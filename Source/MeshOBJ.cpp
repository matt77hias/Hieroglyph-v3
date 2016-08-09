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
#include "PCH.h"
#include "MeshOBJ.h"
#include "GlyphString.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
using namespace Glyph3::OBJ;
//--------------------------------------------------------------------------------
Vector3f toVec3(const std::vector<std::string>& tokens)
{
	assert(tokens.size() >= 4);

	return Vector3f(
		std::stof(tokens[1]),
		std::stof(tokens[2]),
		std::stof(tokens[3]) );
}
//--------------------------------------------------------------------------------
Vector2f toVec2(const std::vector<std::string>& tokens)
{
	assert(tokens.size() >= 3);
	return Vector2f(
		std::stof(tokens[1]),
		std::stof(tokens[2]));
}
//--------------------------------------------------------------------------------
std::array<int, 3> toIndexTriple(const std::string s)
{
	// Split the string according to '/' into tokens
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;

	elems = GlyphString::split(s, '/');

	// We need to have at least one index to do anything.
	assert(elems.size() >= 1 && elems.size() <= 3);

	// initialize to 0, then fill in indices with the available data.
	std::array<int, 3> triple = { 0, 0, 0 };

	for (size_t i = 0; i < elems.size(); ++i) {
		if (elems[i].size() > 0) {
			triple[i] = atoi(elems[i].c_str()) - 1; // Indices start at 1, so shift down one to match our vector format.
		}
	}

	return triple;
}
//--------------------------------------------------------------------------------
int relativeOffset(int index, int refVal)
{
	if (index < 0)
		index = index + refVal + 1; // With negative indices, add the current size of the arrays for relative offsets.

	return index;
}
//--------------------------------------------------------------------------------
MeshOBJ::MeshOBJ(const std::wstring& filename)
{
	// Open the file in text mode.  If there is an issue opening the file, 
	// just exit since nothing has been allocated.

	std::ifstream objFile(filename, std::ios::in);
	if (!objFile.is_open()) { return; }

	// We will process the file one line at a time.
	std::string line;

	// Initialize to a single group in case none are declared within the file.
	//objects.push_back( object_t() );

	while (std::getline(objFile, line))
	{
		std::stringstream   lineStream(line);
		std::string         token;
		std::vector<std::string> tokenList;

		while (lineStream >> token) {
			tokenList.push_back(token);
		}

		// Check for an empty line
		if (tokenList.size() != 0) {
			if (tokenList[0] == "v") {
				positions.emplace_back(toVec3(tokenList));
			}
			else if (tokenList[0] == "vn") {
				normals.emplace_back(toVec3(tokenList));
			}
			else if (tokenList[0] == "vt") {
				coords.emplace_back(toVec2(tokenList));
			}
			else if (tokenList[0] == "f" ||
				     tokenList[0] == "l" ||
					 tokenList[0] == "p" ) {

				face_t f;

				for ( size_t i = 1; i < tokenList.size(); ++i ) {
					auto triple = toIndexTriple( tokenList[i] );
					f.positionIndices.push_back( relativeOffset( triple[0], positions.size() ) );
					f.coordIndices.push_back( relativeOffset( triple[1], coords.size() ) );
					f.normalIndices.push_back( relativeOffset( triple[2], normals.size() ) );
				}

				// If no object has been declared yet, then add one along with
				// a subobject.  Otherwise, ensure that the current object has
				// at least one subobject already with which to add the face to.
				if ( objects.size() == 0 ) { 
					objects.push_back(object_t());
				} 

				if ( objects.back().subobjects.size() == 0 ) {
					objects.back().subobjects.push_back(subobject_t());
				}
				
				objects.back().subobjects.back().faces.emplace_back(f);
			}
			else if (tokenList[0] == "o") {
				// Check if the default object is currently in use. If so,
				// put this name on that object, otherwise we create a new one.
				if (objects.size() == 1 && objects.back().subobjects.back().faces.size() == 0) {
					objects.back().name = tokenList[1];
				}
				else {
					objects.push_back(object_t());
					objects.back().name = tokenList[1];
				}
			}
			else if (tokenList[0] == "mtllib") {
				material_libs.push_back(line.substr(7));
			}
			else if (tokenList[0] == "usemtl") {
				if (objects.size() == 0) { objects.push_back(object_t()); }
				objects.back().subobjects.push_back(subobject_t());
				objects.back().subobjects.back().material_name = line.substr(7);
			}
		}
	}

	// The file will close on its own when it goes out of scope.
}
//--------------------------------------------------------------------------------
