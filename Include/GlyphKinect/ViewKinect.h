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
// ViewKinect
//
// This render view is used to obtain data from the Kinect sensor.  It utilizes 
// the KinectManager class to get CPU based copies of color and depth frames, 
// which are then copied into GPU based staging resources.  Those resources are
// finally copied into secondary resources which can be used directly by shader
// programs.
//
// The render view also only performs an image update when new frame data is 
// available, so it can be included in an entity's material as a pre-render view.
// This will automatically set the available resources in the appropriate 
// parameter manager, making it easy to use them.
//
// The view also generates an offset texture based on the Kinect API method for
// mapping from the depth map pixels to the corresponding color map pixels.  This
// can be used for more precise geometry alignment with the color buffer.
//--------------------------------------------------------------------------------
#ifndef ViewKinect_h
#define ViewKinect_h
//--------------------------------------------------------------------------------
//#include "PCH.h"
#include "Task.h"
#include "UnorderedAccessParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "VectorParameterDX11.h"
#include "KinectSkeletonActor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class KinectManager;

	class ViewKinect : public Task
	{
	public:
		ViewKinect( RendererDX11& Renderer );
		virtual ~ViewKinect();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		ResourcePtr GetColorResource();
		ResourcePtr GetDepthResource();

		void SetSkeletonActor( KinectSkeletonActor* pActor );

		virtual std::wstring GetName();

	protected:
		
		KinectManager* pKinectManager;
		ResourcePtr KinectColor;
		ResourcePtr KinectColorStaging;
		ResourcePtr KinectDepth;
		ResourcePtr KinectDepthStaging;
		ResourcePtr KinectOffset;
		ResourcePtr KinectOffsetStaging;

		BYTE* m_pSysMemColor;
		BYTE* m_pSysMemDepth;
		BYTE* m_pSysSkeleton;

		ShaderResourceParameterDX11* m_pKinectDepthBufferParameter;
		ShaderResourceParameterDX11* m_pKinectColorBufferParameter;
		ShaderResourceParameterDX11* m_pKinectOffsetBufferParameter;

		KinectSkeletonActor* m_pSkeletonActor;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewKinect_h
