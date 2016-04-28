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
// ViewRift
//
// ViewRift provides a rendering task for producing the stereoscopic view for use
// with an Oculus Rift Head Mounted Display (HMD).  This code is based on the 
// OculusSDK v0.4.2, and uses its C-API.
//--------------------------------------------------------------------------------
#ifndef ViewRift_h
#define ViewRift_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "RiftHMD.h"
#include <array>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewRift : public SceneRenderTask
	{
	public:
		ViewRift( RiftHMDPtr hmd, ResourcePtr RenderTarget, int SwapChain );
		virtual ~ViewRift();

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

		RiftHMDPtr m_pHmd;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewRift_h
//--------------------------------------------------------------------------------