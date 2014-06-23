//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
// This is a simple loader for STL binary files.  The usage concept is that the
// face data gets loaded into a vector, and the application can then use the face
// data as it sees fit.  This simplifies the loading of the files, while not 
// making decisions for the developer about how to use the data.
//
// Our face representation eliminates the unused AttributeByteCount to allow each
// face to align to 4 byte boundaries.  More information about the STL file format 
// can be found on the wikipedia page:
// http://en.wikipedia.org/wiki/STL_%28file_format%29.
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
namespace Glyph3 {
//--------------------------------------------------------------------------------
Vector3f toVec3( const std::vector<std::string>& tokens )
{
	assert( tokens.size() >= 4 );

	return Vector3f( atof( tokens[1].c_str() ),
					 atof( tokens[2].c_str() ),
					 atof( tokens[3].c_str() ) );
}
//--------------------------------------------------------------------------------
Vector2f toVec2( const std::vector<std::string>& tokens )
{
	assert( tokens.size() >= 3 );
	return Vector2f( atof( tokens[1].c_str() ),
					 atof( tokens[2].c_str() ) );
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
	MeshOBJ( const std::wstring& filename ) : faces()
	{
		// Open the file in text mode.  If there is an issue opening the file, 
		// just exit since nothing has been allocated.

		std::ifstream objFile( filename, std::ios::in );
		if ( !objFile.is_open() ) { return; }

		// We will process the file one line at a time.
		std::string line;

		while( std::getline( objFile, line ) )
		{
			std::stringstream   lineStream( line );
			std::string         token;
			std::vector<std::string> tokenList;

			while ( lineStream >> token ) {
				tokenList.push_back( token );
			}

			if ( tokenList[0] == "v" ) {
				positions.emplace_back( toVec3( tokenList ) );
			} else if ( tokenList[0] == "vn" ) {
				normals.emplace_back( toVec3( tokenList ) );
			} else if ( tokenList[0] == "vt" ) {
				coords.emplace_back( toVec2( tokenList ) );
			} else if ( tokenList[0] == "f" ) {
				Face f;

				for ( size_t i = 0; i < 3; ++i ) {
					auto triple = toIndexTriple( tokenList[i+1] );
					f.positionIndices[i] = relativeOffset( triple[0], positions.size() );
					f.coordIndices[i] = relativeOffset( triple[1], coords.size() );
					f.normalIndices[i] = relativeOffset( triple[2], normals.size() );
				}

				faces.emplace_back( f );
			}
		}

		// The file will close on its own when it goes out of scope.
	}

	struct Face
	{
		std::array<int,3> positionIndices;
		std::array<int,3> normalIndices;
		std::array<int,3> coordIndices;
	};

	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> coords;
	std::vector<Face> faces;
};

}
//--------------------------------------------------------------------------------
#endif // MeshOBJ_h
//--------------------------------------------------------------------------------