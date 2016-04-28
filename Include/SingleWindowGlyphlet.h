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
// SingleWindowGlyphlet
//
// This glyphlet is used to generate a rendered view of a scene with a traditional
// perspective style camera.  The results of the rendered scene are
// then available for use by the owner of this object. 
//
// In general, this Glyphlet closely resembles the typical RenderApplication based
// sample program.  All of the basic items are provided (scene, camera, render 
// view, etc...), and a single output rendering is produced.  By specializing this
// class, we can produce other Glyphlets that provide the same functionality as
// the other stand-alone sample programs - except that the results of this 
// rendering are passed to any render target that is provided to it (as opposed to
// a traditional app, which only renders into an actual window).
//--------------------------------------------------------------------------------
#ifndef SingleWindowGlyphlet_h
#define SingleWindowGlyphlet_h
//--------------------------------------------------------------------------------
#include "Glyphlet.h"
#include "FirstPersonCamera.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SingleWindowGlyphlet : public Glyphlet
	{
	public:
		SingleWindowGlyphlet( );
		virtual ~SingleWindowGlyphlet();

		void Setup( ResourcePtr target );

		virtual void Initialize();
		virtual void Update( float dt );
		virtual void Shutdown();
		virtual bool HandleEvent( EventPtr pEvent );

		EventManager			EvtManager;

	protected:

		Scene*					m_pScene;
		FirstPersonCamera*		m_pCamera;
		SceneRenderTask*		m_pRenderView;
		ResourcePtr				m_RenderTarget;

		unsigned int			m_iWidth;
		unsigned int			m_iHeight;
	};

	typedef std::shared_ptr<SingleWindowGlyphlet> SingleWindowGlyphletPtr;
};
//--------------------------------------------------------------------------------
#endif // SingleWindowGlyphlet_h
