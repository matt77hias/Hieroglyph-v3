//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
// This is a simple loader for OBJ ascii files.  The usage concept is that the
// raw data gets loaded into vectors, and the application can then use the data as
// it sees fit.  This simplifies the loading of the files, while not making 
// decisions for the developer about how to use the data.
//
// The loader also will load and interpret the associated MTL files, which are
// commonly used to indicate material properties for a model.  You can find more
// information about OBJ and MTL files on their wikipedia page:
// http://en.wikipedia.org/wiki/Wavefront_.obj_file and also on Paul Bourke's site
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
Vector3f toVec3( const std::vector<std::string>& tokens )
{
	assert( tokens.size() >= 4 );

	return Vector3f( static_cast<float>( atof( tokens[1].c_str() ) ),
					 static_cast<float>( atof( tokens[2].c_str() ) ),
					 static_cast<float>( atof( tokens[3].c_str() ) ) );
}
//--------------------------------------------------------------------------------
Vector2f toVec2( const std::vector<std::string>& tokens )
{
	assert( tokens.size() >= 3 );
	return Vector2f( static_cast<float>( atof( tokens[1].c_str() ) ),
					 static_cast<float>( atof( tokens[2].c_str() ) ) );
}
//--------------------------------------------------------------------------------
std::array<int,3> toIndexTriple( const std::string s )
{
	// Split the string according to '/' into tokens
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, '/')) {
        elems.push_back(item);
    }

	// We need to have at least one index to do anything.
	assert( elems.size() >= 1 && elems.size() <= 3 );

	// initialize to 0, then fill in indices with the available data.
	std::array<int,3> triple = {0,0,0};
	
	for ( size_t i = 0; i < elems.size(); ++i ) {
		if ( elems[i].size() > 0 ) {
			triple[i] = atoi( elems[i].c_str() ) - 1; // Indices start at 1, so shift down one to match our vector format.
		}
	}

	return triple;
}
//--------------------------------------------------------------------------------
int relativeOffset( int index, int refVal )
{
	if ( index < 0 ) 
		index = index + refVal + 1; // With negative indices, add the current size of the arrays for relative offsets.

	return index;
}
//--------------------------------------------------------------------------------
class MeshOBJ
{
public:
	MeshOBJ( const std::wstring& filename )
	{
		// Open the file in text mode.  If there is an issue opening the file, 
		// just exit since nothing has been allocated.

		std::ifstream objFile( filename, std::ios::in );
		if ( !objFile.is_open() ) { return; }

		// We will process the file one line at a time.
		std::string line;

		// Initialize to a single group in case none are declared within the file.
		size_t current_group = 0;
		objects.push_back( object_t() );

		while( std::getline( objFile, line ) )
		{
			std::stringstream   lineStream( line );
			std::string         token;
			std::vector<std::string> tokenList;

			while ( lineStream >> token ) {
				tokenList.push_back( token );
			}

			// Check for an empty line
			if ( tokenList.size() != 0 ) {
				if ( tokenList[0] == "v" ) {
					positions.emplace_back( toVec3( tokenList ) );
				} else if ( tokenList[0] == "vn" ) {
					normals.emplace_back( toVec3( tokenList ) );
				} else if ( tokenList[0] == "vt" ) {
					coords.emplace_back( toVec2( tokenList ) );
				} else if ( tokenList[0] == "f" ) {
					face_t f;

					for ( size_t i = 0; i < 3; ++i ) {
						auto triple = toIndexTriple( tokenList[i+1] );
						f.positionIndices[i] = relativeOffset( triple[0], positions.size() );
						f.coordIndices[i] = relativeOffset( triple[1], coords.size() );
						f.normalIndices[i] = relativeOffset( triple[2], normals.size() );
					}

					objects.back().faces.emplace_back( f );
				} else if ( tokenList[0] == "o" ) {
					// Check if the default object is currently in use. If so,
					// put this name on that object, otherwise we create a new one.
					if ( objects.back().faces.size() == 0 ) {
						objects.back().name = tokenList[1];
					} else {
						objects.push_back( object_t() );
						objects.back().name = tokenList[1];
					}
				} else if ( tokenList[0] == "mtllib" ) {
					material_libs.push_back( tokenList[1] );
				}
			}
		}

		// The file will close on its own when it goes out of scope.
	}

	typedef struct
	{
		std::array<int,3> positionIndices;
		std::array<int,3> normalIndices;
		std::array<int,3> coordIndices;
	} face_t;

	typedef struct {
		std::string name;
		std::string material_name;
		std::vector<face_t> faces;
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