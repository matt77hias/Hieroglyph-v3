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
#include "TexturedVertex.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
D3D11_INPUT_ELEMENT_DESC TexturedVertex::Elements[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
//--------------------------------------------------------------------------------
TexturedVertex::TexturedVertex()
{
}
//--------------------------------------------------------------------------------
TexturedVertex::~TexturedVertex()
{
}
//--------------------------------------------------------------------------------
unsigned int TexturedVertex::GetElementCount()
{
	return( sizeof( Elements ) / sizeof( Elements[0] ) );
}
//--------------------------------------------------------------------------------
