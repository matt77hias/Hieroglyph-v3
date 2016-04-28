//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
// This is a simple loader for MTL ascii files.  This is a companion file format
// for OBJ files.  More information can be found on the OBJ wikipedia page:
// http://en.wikipedia.org/wiki/Wavefront_.obj_file and on Paul Bourke's site
// http://paulbourke.net/dataformats/mtl/.
//--------------------------------------------------------------------------------
#ifndef MeshMTL_h
#define MeshMTL_h
//--------------------------------------------------------------------------------
#include <map>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "Vector2f.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3 { namespace MTL {
//--------------------------------------------------------------------------------
Vector3f toVec3( const std::vector<std::string>& tokens )
{
	assert( tokens.size() >= 4 );

	return Vector3f( std::stof( tokens[1] ),
					 std::stof( tokens[2] ),
					 std::stof( tokens[3] ) );
}
//--------------------------------------------------------------------------------
int toIllumModel( const std::vector<std::string>& tokens )
{
	assert( tokens.size() == 2 );

	if ( tokens[1] == "illum_0" ) {
		return 0;
	} else if ( tokens[1] == "illum_1" ) {
		return 1;
	} else if ( tokens[1] == "illum_2" ) {
		return 2;
	} else if ( tokens[1] == "illum_3" ) {
		return 3;
	} else if ( tokens[1] == "illum_4" ) {
		return 4;
	} else if ( tokens[1] == "illum_5" ) {
		return 5;
	} else if ( tokens[1] == "illum_6" ) {
		return 6;
	} else if ( tokens[1] == "illum_7" ) {
		return 7;
	} else if ( tokens[1] == "illum_8" ) {
		return 8;
	} else if ( tokens[1] == "illum_9" ) {
		return 9;
	} else if ( tokens[1] == "illum_10" ) {
		return 10;
	}

	return -1;
}
//--------------------------------------------------------------------------------
class MeshMTL
{
public:
	MeshMTL( const std::wstring& filename )
	{
		// Open the file in text mode.  If there is an issue opening the file, 
		// just exit since nothing has been allocated.

		std::ifstream mtlFile( filename, std::ios::in );
		if ( !mtlFile.is_open() ) { return; }

		// We will process the file one line at a time.
		std::string line;
		std::string current;

		while( std::getline( mtlFile, line ) )
		{
			std::stringstream   lineStream( line );
			std::string         token;
			std::vector<std::string> tokenList;

			while ( lineStream >> token ) {
				tokenList.push_back( token );
			}

			// Check for an empty line
			if ( tokenList.size() != 0 ) {
				if ( tokenList[0] == "newmtl" ) {
					materials.insert( make_pair(tokenList[1], material_t() ) );
					current = tokenList[1];
				} else if ( tokenList[0] == "Ka" ) {
					materials[current].ambient = toVec3( tokenList );
				} else if ( tokenList[0] == "Kd" ) {
					materials[current].diffuse = toVec3( tokenList );
				} else if ( tokenList[0] == "Ks" ) {
					materials[current].specular = toVec3( tokenList );
				} else if ( tokenList[0] == "Tf" ) {
					materials[current].transmittance = toVec3( tokenList );
				} else if ( tokenList[0] == "Ns" ) {
					materials[current].shininess = std::stof( tokenList[1] );
				} else if ( tokenList[0] == "Ni" ) {
					materials[current].ior = std::stof( tokenList[1] );
				} else if ( tokenList[0] == "d" ) {
					materials[current].dissolve = std::stof( tokenList[1] );
				} else if ( tokenList[0] == "illum" ) {
					materials[current].illumination = toIllumModel( tokenList );
				} else if ( tokenList[0] == "map_Ka" ) {
					materials[current].ambient_texname = line.substr( 7 );
				} else if ( tokenList[0] == "map_Kd" ) {
					materials[current].diffuse_texname = line.substr( 7 );
				} else if ( tokenList[0] == "map_Ks" ) {
					materials[current].specular_texname = line.substr( 7 );
				} else if ( tokenList[0] == "map_Ns" ) {
					materials[current].shininess_texname = line.substr( 7 );
				}
			}
		}

		// The file will close on its own when it goes out of scope.
	}

	struct material_t
	{
		material_t() : 
			ambient( 0.0f, 0.0f, 0.0f ),
			diffuse( 0.0f, 0.0f, 0.0f ),
			specular( 0.0f, 0.0f, 0.0f ),
			transmittance( 0.0f, 0.0f, 0.0f ),
			shininess( 1.0f ),
			ior( 1.0f ),
			dissolve( 1.0f ),
			illumination( 0 )
		{
		}

		Vector3f ambient;
		Vector3f diffuse;
		Vector3f specular;
		Vector3f transmittance;
		float shininess;
		float ior;
		float dissolve;
		int illumination;

		std::string ambient_texname;
		std::string diffuse_texname;
		std::string specular_texname;
		std::string shininess_texname;
	};

	std::map<std::string,material_t> materials;
};

} }
//--------------------------------------------------------------------------------
#endif // MeshMTL_h
//--------------------------------------------------------------------------------