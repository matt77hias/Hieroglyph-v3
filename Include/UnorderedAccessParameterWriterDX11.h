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
// UnorderedAccessParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef UnorderedAccessParameterWriterDX11_h
#define UnorderedAccessParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class UnorderedAccessParameterWriterDX11 : public ParameterWriter
	{
	public:
		UnorderedAccessParameterWriterDX11();
		virtual ~UnorderedAccessParameterWriterDX11();

		void SetRenderParameterRef( UnorderedAccessParameterDX11* pParam );
		void SetValue( ResourcePtr Value );
		void SetInitCount( unsigned int count );

		virtual void WriteParameter( IParameterManager* pParamMgr );
		virtual void InitializeParameter( );

		virtual RenderParameterDX11* GetRenderParameterRef();

	protected:
		UnorderedAccessParameterDX11*	m_pParameter;
		ResourcePtr						m_Value;
		unsigned int					m_InitCount;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterWriter_h
//--------------------------------------------------------------------------------

