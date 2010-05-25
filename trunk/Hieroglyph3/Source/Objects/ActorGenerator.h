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
// ActorGenerator
//
//--------------------------------------------------------------------------------
#ifndef ActorGenerator_h
#define ActorGenerator_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "MaterialDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ActorGenerator
	{
	public:
		static Actor* GenerateVisualizationTexture2D( RendererDX11& Renderer, 
			ResourcePtr resource, MaterialDX11* pMaterial );

	private:
		ActorGenerator();
		
	};
};
#endif // ActorGenerator_h


