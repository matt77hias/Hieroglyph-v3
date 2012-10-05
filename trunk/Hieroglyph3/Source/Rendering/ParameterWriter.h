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
// ParameterWriter
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
#ifndef ParameterWriter_h
#define ParameterWriter_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ParameterWriter
	{
	public:
		ParameterWriter();
		virtual ~ParameterWriter();

		//void SetRenderParameterRef( RenderParameterDX11* pParam );
		RenderParameterDX11* GetRenderParameterRef();

		virtual void WriteParameter( IParameterManager* pParamMgr ) = 0;

	protected:
		RenderParameterDX11*	m_pParameter;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterWriter_h
//--------------------------------------------------------------------------------

