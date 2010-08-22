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
#include "RendererDX11.h"
#include "RenderEffectDX11.h"
//--------------------------------------------------------------------------------
#ifndef SpriteRendererDX11_h
#define SpriteRendererDX11_h
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

		struct SpriteDrawRect
		{
			float X;
			float Y;
			float Width;
			float Height;
		};

		struct SpriteDrawData
		{
			Matrix4f Transform;
			Vector4f Color;
			SpriteDrawRect DrawRect;
		};

		static const UINT MaxBatchSize = 1000;

		SpriteRendererDX11();
		~SpriteRendererDX11();

		bool Initialize();

		void Render(	PipelineManagerDX11* pipeline,
						ParameterManagerDX11* parameters,
						ResourcePtr texture,
						const SpriteDrawData* drawData,
						UINT numSprites,
						FilterMode filterMode = Linear );

		void Render(	PipelineManagerDX11* pipeline,
						ParameterManagerDX11* parameters,
						ResourcePtr texture,
						const Matrix4f& transform,
						const Vector4f& Color = Vector4f( 1, 1, 1, 1 ),
						FilterMode filterMode = Linear,
						const SpriteDrawRect* drawRect = NULL );

		void RenderText(	PipelineManagerDX11* pipeline,
							ParameterManagerDX11* parameters,
							const SpriteFontDX11& font,
							const wchar_t* text,
							const Matrix4f& transform,
							const Vector4f& color = Vector4f( 1, 1, 1, 1 ) );

	protected:

		void RenderCommon( ResourcePtr texture );

		RenderEffectDX11 m_effect;

		ResourcePtr m_pVertexBuffer;
		ResourcePtr m_pIndexBuffer;
		ResourcePtr m_pInstanceDataBuffer;
		int m_iInputLayout;

		int m_iLinearSamplerState;
		int m_iPointSamplerState;

		bool m_bInitialized;

		SpriteDrawData m_TextDrawData [MaxBatchSize];

		struct SpriteVertex
		{
			Vector2f Position;
			Vector2f TexCoord;
		};

	};
}

//--------------------------------------------------------------------------------
#endif // SpriteRendererDX11_h
//--------------------------------------------------------------------------------