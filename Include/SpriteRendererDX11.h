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
// SpriteRendererDX11
//
//--------------------------------------------------------------------------------
#ifndef SpriteRendererDX11_h
#define SpriteRendererDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "RenderEffectDX11.h"
#include "DrawIndexedInstancedExecutorDX11.h"
#include "SpriteVertexDX11.h"
#include "SpriteFontDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;

	typedef std::shared_ptr<DrawIndexedInstancedExecutorDX11<SpriteVertexDX11::VertexData,SpriteVertexDX11::InstanceData>> SpriteGeometryPtr;

	class SpriteRendererDX11
	{

	public:

		enum FilterMode
		{
			DontSet = 0,
			Linear = 1,
			Point = 2
		};

		static const UINT MaxBatchSize = 1000;

		SpriteRendererDX11();
		~SpriteRendererDX11();

		bool Initialize();

		void Render(	PipelineManagerDX11* pipeline,
						IParameterManager* parameters,
						ResourcePtr texture,
						const SpriteVertexDX11::InstanceData* drawData,
						UINT numSprites,
						FilterMode filterMode = Linear );

		void Render(	PipelineManagerDX11* pipeline,
						IParameterManager* parameters,
						ResourcePtr texture,
						const Matrix4f& transform,
						const Vector4f& Color = Vector4f( 1, 1, 1, 1 ),
						FilterMode filterMode = Linear,
						const SpriteVertexDX11::SpriteDrawRect* drawRect = NULL );

		void RenderText(	PipelineManagerDX11* pipeline,
							IParameterManager* parameters,
							SpriteFontPtr pFont,
							const wchar_t* text,
							const Matrix4f& transform,
							const Vector4f& color = Vector4f( 1, 1, 1, 1 ) );

	protected:

		void RenderCommon( ResourcePtr texture );

		RenderEffectDX11 m_effect;

		SpriteGeometryPtr m_pGeometry;

		int m_iLinearSamplerState;
		int m_iPointSamplerState;

		bool m_bInitialized;

	};
}

//--------------------------------------------------------------------------------
#endif // SpriteRendererDX11_h
//--------------------------------------------------------------------------------