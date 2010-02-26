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
// ViewLinearDepthNormals
//
// ViewLinearDepthNormals is intended to be the primary render view that most of the
// everyday rendering will use.  This render view will use the scene graph to
// select the currently visible objects and render them.
//--------------------------------------------------------------------------------
#ifndef ViewLinearDepthNormals_h
#define ViewLinearDepthNormals_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewLinearDepthNormals : public IRenderView
	{
	public:
		ViewLinearDepthNormals( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget );

		virtual void Update( float fTime );
		virtual void Draw( RendererDX11& Renderer );

		void SetBackColor( Vector4f color );
		void SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ );

		virtual void SetRenderParams( RendererDX11& Renderer );
		virtual void SetUsageParams( RendererDX11& Renderer );


		virtual ~ViewLinearDepthNormals();

	protected:
		Vector4f m_vColor;
		//D3DVIEWPORT9 m_viewport;

		ResourcePtr m_RenderTarget;
		ResourcePtr m_DepthTarget;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewLinearDepthNormals_h
