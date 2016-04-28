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
// SamplerParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef SamplerParameterWriterDX11_h
#define SamplerParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SamplerParameterWriterDX11 : public ParameterWriter
	{
	public:
		SamplerParameterWriterDX11();
		virtual ~SamplerParameterWriterDX11();

		void SetRenderParameterRef( SamplerParameterDX11* pParam );
		void SetValue( int Value );

		virtual void WriteParameter( IParameterManager* pParamMgr );
		virtual void InitializeParameter( );

		virtual RenderParameterDX11* GetRenderParameterRef();

	protected:
		SamplerParameterDX11*	m_pParameter;
		int						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // SamplerParameterWriterDX11_h
//--------------------------------------------------------------------------------

