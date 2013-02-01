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
// ViewSimulation
//
//--------------------------------------------------------------------------------
#ifndef ViewSimulation_h
#define ViewSimulation_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
#include "UnorderedAccessParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "VectorParameterDX11.h"
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
		virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );


		virtual ~ViewSimulation();

	protected:
		int ThreadGroupsX;
		int ThreadGroupsY;

		ResourcePtr WaterState[2];
		RenderEffectDX11*	pWaterEffect;

		ShaderResourceParameterDX11* m_pCurrentWaterState;
		UnorderedAccessParameterDX11* m_pNewWaterState;
		VectorParameterDX11* m_pDispatchSize;

	};
};
//--------------------------------------------------------------------------------
#endif // ViewSimulation_h
