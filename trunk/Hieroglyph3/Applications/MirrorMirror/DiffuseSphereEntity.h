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
// DiffuseSphereEntity
//
//--------------------------------------------------------------------------------
#ifndef DiffuseSphereEntity_h
#define DiffuseSphereEntity_h
//--------------------------------------------------------------------------------
#include "Entity3D.h"
#include "ViewParaboloidEnvMap.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
    class ShaderResourceParameterDX11;

	class DiffuseSphereEntity : public Entity3D
	{
	public:
		DiffuseSphereEntity( );

        static void LoadResources( );

	public:
		static ResourcePtr ColorTexture;
        static ShaderResourceParameterDX11* TextureParameter;
		static int LinearSampler;
        static RenderEffectDX11* RenderEffect;
        static RenderEffectDX11* ParabolaEffect;
        static GeometryPtr SphereGeometry;
	};
};
//--------------------------------------------------------------------------------
#endif // DiffuseSphereEntity_h
