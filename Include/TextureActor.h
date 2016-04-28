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
// TextureActor
//
//--------------------------------------------------------------------------------
#ifndef TextureActor_h
#define TextureActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "TexturedVertex.h"
#include "DrawExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TextureActor : public Actor
	{
	public:
		TextureActor( const ResourcePtr& texture, float scale = 1.0f );
		virtual ~TextureActor();

		void SetTexture( const ResourcePtr& texture, float scale = 1.0f );

		void SetColor( const Vector4f& color );
		Vector4f GetColor( );

	protected:

		typedef std::shared_ptr<DrawExecutorDX11<TexturedVertex::Vertex>> DrawGeometryPtr;

		DrawGeometryPtr							m_pGeometry;
		MaterialPtr								m_pMaterial;
		ResourcePtr								m_Texture;
		Vector4f								m_Color;
		float									m_scale;
	};
};
//--------------------------------------------------------------------------------
#endif // TextureActor_h
//--------------------------------------------------------------------------------