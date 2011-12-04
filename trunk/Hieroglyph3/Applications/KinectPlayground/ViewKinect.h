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
// ViewKinect
//
//--------------------------------------------------------------------------------
#ifndef ViewKinect_h
#define ViewKinect_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "IRenderView.h"
#include "UnorderedAccessParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class KinectManager;

	class ViewKinect : public IRenderView
	{
	public:
		ViewKinect( RendererDX11& Renderer );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		ResourcePtr GetColorResource();
		ResourcePtr GetDepthResource();

		virtual ~ViewKinect();

	protected:
		
		KinectManager* pKinectManager;
		ResourcePtr KinectColor;
		ResourcePtr KinectColorStaging;
		ResourcePtr KinectDepth;
		ResourcePtr KinectDepthStaging;

		BYTE* m_pSysMemColor;
		BYTE* m_pSysMemDepth;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewKinect_h
