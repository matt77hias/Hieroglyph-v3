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
// ViewSimulation
//
//--------------------------------------------------------------------------------
#ifndef ViewSimulation_h
#define ViewSimulation_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	struct GridPoint
	{
		float height;
		Vector4f flow;
	};

	class ViewSimulation : public IRenderView
	{
	public:
		ViewSimulation( RendererDX11& Renderer, int SizeX, int SizeY );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager );

		virtual void SetRenderParams( ParameterManagerDX11* pParamManager );
		virtual void SetUsageParams( ParameterManagerDX11* pParamManager );


		virtual ~ViewSimulation();

	protected:
		int ThreadGroupsX;
		int ThreadGroupsY;

		ResourcePtr WaterState[2];
		RenderEffectDX11*	pWaterEffect;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewSimulation_h
