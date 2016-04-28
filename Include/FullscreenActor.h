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
// FullscreenActor
//
//--------------------------------------------------------------------------------
#ifndef FullscreenActor_h
#define FullscreenActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "TexturedVertex.h"
#include "DrawExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class FullscreenActor : public Actor
	{
	public:
		FullscreenActor();
		virtual ~FullscreenActor();

		void SetColor( const Vector4f& color );
		Vector4f GetColor( );

	protected:

		typedef std::shared_ptr<DrawExecutorDX11<TexturedVertex::Vertex>> DrawGeometryPtr;

		DrawGeometryPtr							m_pGeometry;
		MaterialPtr								m_pMaterial;
		Vector4f								m_Color;
	};
};
//--------------------------------------------------------------------------------
#endif // FullscreenActor_h
//--------------------------------------------------------------------------------