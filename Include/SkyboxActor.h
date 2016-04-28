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
// SkyboxActor
//
//--------------------------------------------------------------------------------
#ifndef SkyboxActor_h
#define SkyboxActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "TexturedVertex.h"
#include "DrawIndexedExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SkyboxActor : public Actor
	{
	public:
		SkyboxActor( const ResourcePtr& texture, float scale = 1000.0f );
		virtual ~SkyboxActor();

		void SetTexture( const ResourcePtr& texture, float scale = 1000.0f );

	protected:

		typedef std::shared_ptr<DrawIndexedExecutorDX11<TexturedVertex::Vertex>> DrawGeometryPtr;

		DrawGeometryPtr							m_pGeometry;
		MaterialPtr								m_pMaterial;
		ResourcePtr								m_Texture;
		float									m_scale;
	};
};
//--------------------------------------------------------------------------------
#endif // SkyboxActor_h
//--------------------------------------------------------------------------------