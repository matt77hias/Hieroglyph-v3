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
#include "EntityRenderParams.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EntityRenderParams::EntityRenderParams()
{
	iPass = GEOMETRY;
	WorldMatrix.MakeIdentity();
	pGeometry = 0;
	pMaterial = 0;
}
//--------------------------------------------------------------------------------
EntityRenderParams::~EntityRenderParams()
{
	if ( pGeometry )
		pGeometry->Release();

	if ( pMaterial )
		delete pMaterial;
}
//--------------------------------------------------------------------------------
