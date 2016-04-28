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
// FullscreenTexturedActor
//
//--------------------------------------------------------------------------------
#ifndef FullscreenTexturedActor_h
#define FullscreenTexturedActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "TexturedVertex.h"
#include "DrawExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class FullscreenTexturedActor : public Actor
	{
	public:
		FullscreenTexturedActor( ResourcePtr texture );
		virtual ~FullscreenTexturedActor();

		void SetTexture( ResourcePtr texture );
		ResourcePtr GetTexture( );

	protected:

		typedef std::shared_ptr<DrawExecutorDX11<TexturedVertex::Vertex>> DrawGeometryPtr;

		DrawGeometryPtr							m_pGeometry;
		MaterialPtr								m_pMaterial;
		ResourcePtr								m_Texture;
	};
};
//--------------------------------------------------------------------------------
#endif // FullscreenTexturedActor_h
//--------------------------------------------------------------------------------