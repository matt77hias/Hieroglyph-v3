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
// ConstantBufferParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef ConstantBufferParameterWriterDX11_h
#define ConstantBufferParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ConstantBufferParameterWriterDX11 : public ParameterWriter
	{
	public:
		ConstantBufferParameterWriterDX11();
		virtual ~ConstantBufferParameterWriterDX11();

		void SetRenderParameterRef( ConstantBufferParameterDX11* pParam );
		void SetValue( ResourcePtr Value );

		virtual void WriteParameter( IParameterManager* pParamMgr );
		virtual void InitializeParameter( );

		virtual RenderParameterDX11* GetRenderParameterRef();

	protected:
		ConstantBufferParameterDX11*	m_pParameter;
		ResourcePtr						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterWriter_h
//--------------------------------------------------------------------------------

