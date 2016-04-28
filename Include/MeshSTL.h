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
#ifndef MeshSTL_h
#define MeshSTL_h
//--------------------------------------------------------------------------------
#include <vector>
#include <fstream>
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3 { namespace STL {

template<typename T>
void read( std::ifstream& s, T& item )
{
	s.read( reinterpret_cast<char*>(&item), sizeof(item) );
}
//--------------------------------------------------------------------------------
class MeshSTL
{
public:
	MeshSTL( const std::wstring& filename ) : faces()
	{
		unsigned int faceCount = 0; 

		// Open the file for input, in binary mode, and put the marker at the end.
		// This let's us grab the file size by reading the 'get' marker location.
		// If the file doesn't open, simply return without loading.

		std::ifstream stlFile( filename, std::ios::in | std::ios::ate | std::ios::binary );
		if ( !stlFile.is_open() ) { return; }

		unsigned int fileSize = static_cast<unsigned int>( stlFile.tellg() );

		// Skip the header of the STL file, and read in the number of faces.  We
		// then ensure that the file is actually large enough to handle that many
		// faces before we proceed.

		stlFile.seekg( 80 );
		read( stlFile, faceCount );

		if ( fileSize < 84 + faceCount * FILE_FACE_SIZE ) { return; }

		// Now we read the face data in, and add it to our vector of faces.  We
		// provided an ifstream constructor for our face to allow constructing
		// the vector elements in place.  Before starting the loop, we reserve
		// enough space in the vector to ensure we don't need to reallocate while
		// loading (and skip all of the unneeded copying...).

		faces.reserve( faceCount );

		for ( unsigned int i = 0; i < faceCount; ++i ) {
			faces.emplace_back( stlFile );
		}
	}

public:

	struct Face
	{
		Face( std::ifstream& s ) 
		{
			read( s, normal );	// Read normal vector
			read( s, v0 );		// Read vertex 0
			read( s, v1 );		// Read vertex 1
			read( s, v2 );		// Read vertex 2
			s.seekg( 2, std::ios_base::cur ); // Skip 2 bytes for unused data
		}

		Vector3f normal;
		Vector3f v0;
		Vector3f v1;
		Vector3f v2;
	};

	static const unsigned int FILE_FACE_SIZE = sizeof(Vector3f)*4 + sizeof(unsigned short);

	std::vector<Face> faces;
};

} }
//--------------------------------------------------------------------------------
#endif // MeshSTL_h
//--------------------------------------------------------------------------------