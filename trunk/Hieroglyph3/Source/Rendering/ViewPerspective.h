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
// ViewPerspective
//
// ViewPerspective is intended to be the primary render view that most of the
// everyday rendering will use.  This render view will use the scene graph to
// select the currently visible objects and render them.
//--------------------------------------------------------------------------------
#ifndef ViewPerspective_h
#define ViewPerspective_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewPerspective : public IRenderView
	{
	public:
		ViewPerspective( RendererDX11& Renderer, int Width, int Height );
		ViewPerspective( RendererDX11& Renderer, int RenderTargetID );

		virtual void Update( Timer& Timer );
		virtual void Draw( RendererDX11& Renderer );

		void SetBackColor( DWORD color );
		void SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ );

		virtual void SetRenderParams( RendererDX11& Renderer );
		virtual void SetUsageParams( RendererDX11& Renderer );


		virtual ~ViewPerspective();

	protected:
		DWORD m_dColor;
		//D3DVIEWPORT9 m_viewport;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewPerspective_h
