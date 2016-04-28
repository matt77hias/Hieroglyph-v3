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
// ViewRiftHighlight
//
//--------------------------------------------------------------------------------
#ifndef ViewRiftHighlight_h
#define ViewRiftHighlight_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "FullscreenActor.h"
#include "RiftHMD.h"
#include <array>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewRiftHighlight : public SceneRenderTask
	{
	public:
		ViewRiftHighlight( RiftHMDPtr hmd, ResourcePtr RenderTarget, int SwapChain );
		virtual ~ViewRiftHighlight();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual std::wstring GetName();

	private:

		static const unsigned int eye_count = 2;

		std::array<Matrix4f,eye_count>			m_view;
		std::array<Matrix4f,eye_count>			m_proj;

		ResourcePtr								m_DepthTarget;

		ResourcePtr								m_SilhouetteTarget;
		FullscreenActor							m_SilhouetteActor;

		RiftHMDPtr m_pHmd;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewRiftHighlight_h
//--------------------------------------------------------------------------------