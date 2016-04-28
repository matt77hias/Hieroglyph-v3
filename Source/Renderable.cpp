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
#include "Renderable.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Renderable::Renderable() :
	iPass( GEOMETRY ),
	Executor( nullptr ),
	Material( nullptr ) 
{
}
//--------------------------------------------------------------------------------
Renderable::~Renderable()
{
	Executor = nullptr;
	Material = nullptr;
}
//--------------------------------------------------------------------------------
void Renderable::SetMaterial( MaterialPtr pMaterial )
{
	Material = pMaterial;

	// Query all of the vertex shader IDs that will be used so that the pipeline
	// executor class can pre-create them.

	if ( Material != nullptr && Executor != nullptr )
	{
		std::vector<int> idlist;
		Material->GetAllVertexShaderIDs( idlist );

		for ( auto ID : idlist ) {
			Executor->GenerateInputLayout( ID );
		}
	}
}
//--------------------------------------------------------------------------------
MaterialPtr Renderable::GetMaterial( )
{
	return( Material );
}
//--------------------------------------------------------------------------------
void Renderable::SetGeometry( ExecutorPtr pExecutor )
{
	Executor = pExecutor;

	// Query all of the vertex shader IDs that will be used so that the pipeline
	// executor class can pre-create them.

	if ( Material != nullptr && Executor != nullptr )
	{
		std::vector<int> idlist;
		Material->GetAllVertexShaderIDs( idlist );

		for ( auto ID : idlist ) {
			Executor->GenerateInputLayout( ID );
		}
	}
}
//--------------------------------------------------------------------------------
ExecutorPtr Renderable::GetGeometry( )
{
	return( Executor );
}
//--------------------------------------------------------------------------------
