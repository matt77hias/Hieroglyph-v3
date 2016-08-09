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
#include "MaterialTemplate.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MaterialTemplate::MaterialTemplate() :
material( std::make_shared<MaterialDX11>() )
{
}
//--------------------------------------------------------------------------------
MaterialTemplate::~MaterialTemplate()
{
}
//--------------------------------------------------------------------------------
void MaterialTemplate::SetColorMode( ColorMode mode )
{
	if ( mode == ColorMode::ObjectColor )
	{
	
	}
	else if ( mode == ColorMode::VertexColor )
	{
	
	}
	else if ( mode == ColorMode::TexturedColor )
	{

	}
}
//--------------------------------------------------------------------------------
void MaterialTemplate::SetLightingMode( LightingMode mode )
{
	if ( mode == LightingMode::Flat )
	{
	}
	else if ( mode == LightingMode::Point )
	{
	}
}
//--------------------------------------------------------------------------------
void MaterialTemplate::SetTransparencyMode( TransparencyMode mode )
{
	if ( mode == TransparencyMode::Off )
	{
	
	}
	else if ( mode == TransparencyMode::Alpha )
	{
	
	}
}
//--------------------------------------------------------------------------------