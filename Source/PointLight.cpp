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
#include "PointLight.h"
#include "Scene.h"
#include "ViewPortDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PointLight::PointLight() :
	m_PositionWriter( nullptr )
{
	// This actor attaches a position extractor to its body, which then updates
	// the 'LightPosition' vector parameter every time the body entity is updated.
	// This ensures that the most up to date position is used, and allows the 
	// App/Scene to manipulate the point light position and have it reflected
	// automatically in the subsequent shading passes.

	m_PositionWriter = Parameters.SetVectorParameter( L"LightPosition", Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	
	PositionExtractorController<Entity3D>* pController = new PositionExtractorController<Entity3D>();
	pController->SetParameterWriter( m_PositionWriter );
	
	GetBody()->Controllers.Attach( pController );
}
//--------------------------------------------------------------------------------
PointLight::~PointLight()
{
}
//--------------------------------------------------------------------------------
