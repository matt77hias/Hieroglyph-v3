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
// SpriteRendererDX11
//
//--------------------------------------------------------------------------------
#ifndef SpriteRendererDX11_h
#define SpriteRendererDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "RenderEffectDX11.h"
#include "InstancedQuadGeometryDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SpriteFontDX11;
	class PipelineManagerDX11;

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
						const SpriteDrawData* drawData,
						UINT numSprites,
						FilterMode filterMode = Linear );

		void Render(	PipelineManagerDX11* pipeline,
						IParameterManager* parameters,
						ResourcePtr texture,
						const Matrix4f& transform,
						const Vector4f& Color = Vector4f( 1, 1, 1, 1 ),
						FilterMode filterMode = Linear,
						const SpriteDrawRect* drawRect = NULL );

		void RenderText(	PipelineManagerDX11* pipeline,
							IParameterManager* parameters,
							const SpriteFontDX11& font,
							const wchar_t* text,
							const Matrix4f& transform,
							const Vector4f& color = Vector4f( 1, 1, 1, 1 ) );

	protected:

		void RenderCommon( ResourcePtr texture );

		RenderEffectDX11 m_effect;

		InstancedQuadGeometryPtr m_pGeometry;

		int m_iLinearSamplerState;
		int m_iPointSamplerState;

		bool m_bInitialized;

	};
}

//--------------------------------------------------------------------------------
#endif // SpriteRendererDX11_h
//--------------------------------------------------------------------------------