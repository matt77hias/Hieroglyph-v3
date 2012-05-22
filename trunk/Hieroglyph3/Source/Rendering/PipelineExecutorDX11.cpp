//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineExecutorDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PipelineExecutorDX11::PipelineExecutorDX11( )
{
}
//--------------------------------------------------------------------------------
PipelineExecutorDX11::~PipelineExecutorDX11()
{
	std::map<int, InputLayoutKey*>::iterator it = m_InputLayouts.begin();
	for( ; it != m_InputLayouts.end(); it++ )
        SAFE_DELETE( (*it).second );
}
//--------------------------------------------------------------------------------
int PipelineExecutorDX11::GetInputLayout( int ShaderID )
{
	int layout = -1;

	// Automatically generate the layout if it doesn't already exist.

	if ( m_InputLayouts[ShaderID] == 0 )
		GenerateInputLayout( ShaderID );
	
	// The layout should be created now...
	layout = m_InputLayouts[ShaderID]->layout;

	return( layout );
}
//--------------------------------------------------------------------------------