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
// OculusSDK v0.3.1, and uses its C-API.
//--------------------------------------------------------------------------------
#ifndef ViewRift_h
#define ViewRift_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "RiftHMD.h"
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

	protected:

		ResourcePtr		m_LeftTarget;
		ResourcePtr		m_RightTarget;
		ResourcePtr		m_DepthTarget;

		int m_LeftEyeViewport;
		int m_RightEyeViewport;

		RiftHMDPtr m_pHmd;

		Matrix4f leftView;
		Matrix4f rightView;
		Matrix4f leftProj;
		Matrix4f rightProj;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewRift_h
//--------------------------------------------------------------------------------