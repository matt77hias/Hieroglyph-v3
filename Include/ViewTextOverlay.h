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
// ViewTextOverlay
//
// ViewTextOverlay is intended to render text as an overlay of a given scene.
//--------------------------------------------------------------------------------
#ifndef ViewTextOverlay_h
#define ViewTextOverlay_h
//--------------------------------------------------------------------------------
#include "Task.h"
#include "SpriteFontLoaderDX11.h"
#include "SpriteRendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	struct TextEntry
	{
		std::wstring text;
		Matrix4f xform;
		Vector4f color;
		TextEntry() {};
		TextEntry( std::wstring& text, Matrix4f& xform, Vector4f& color )
		{
			this->text = text;
			this->xform = xform;
			this->color = color;
		};
	};

	class ViewTextOverlay : public Task
	{
	public:
		ViewTextOverlay( RendererDX11& Renderer, ResourcePtr RenderTarget );
		virtual ~ViewTextOverlay();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		void WriteText( std::wstring& text, Matrix4f& xform, Vector4f& color );

		virtual std::wstring GetName();

	protected:
		Vector4f				m_vColor;
		int						m_iViewport;

		ResourcePtr				m_RenderTarget;

		SpriteFontPtr			m_pSpriteFont;
		SpriteRendererDX11*		m_pSpriteRenderer;
		
		std::vector<TextEntry>	m_TextEntries;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewTextOverlay_h
