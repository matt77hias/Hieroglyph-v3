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
// MaterialDX11
//
//--------------------------------------------------------------------------------
#ifndef MaterialDX11_h
#define MaterialDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "IRenderView.h"
#include "RenderEffectDX11.h"
#include "RenderParameterDX11.h"
#include "ParameterContainer.h"
#include <memory>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct MaterialParams
	{
		bool							bRender;
		RenderEffectDX11*				pEffect;
		TArray<RenderParameterDX11*>	vParameters;
		TArray<IRenderView*>			vViews;
	};


	class MaterialDX11
	{
	public:
		MaterialDX11();
		virtual ~MaterialDX11();

		void PreRender( RendererDX11* pRenderer, VIEWTYPE type );
		void SetEntity( Entity3D* pEntity );
		void SetRenderParams( IParameterManager* pParamManager, VIEWTYPE type );

	public:
		MaterialParams				Params[VT_NUM_VIEW_TYPES];
		ParameterContainer			Parameters;
		Entity3D*					m_pEntity;
	};
	typedef std::shared_ptr<MaterialDX11> MaterialPtr;
};
//--------------------------------------------------------------------------------
#endif // MaterialDX11_h
//--------------------------------------------------------------------------------

