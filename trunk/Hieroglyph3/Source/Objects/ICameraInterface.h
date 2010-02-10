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
// ICameraInterface
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef ICameraInterface_h
#define ICameraInterface_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ISceneInterface;

	class ICameraInterface : public Actor
	{
	public:
		ICameraInterface();
		virtual ~ICameraInterface();

		void SetCameraView( IRenderView* pView );
		void SetScene( ISceneInterface* pScene );
		void RenderFrame();

	protected:
		IRenderView* m_pCameraView;
		ISceneInterface* m_pScene;
	};
};
//--------------------------------------------------------------------------------
#endif // ICameraInterface_h
