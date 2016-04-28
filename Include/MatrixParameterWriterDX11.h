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
// MatrixParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef MatrixParameterWriterDX11_h
#define MatrixParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixParameterWriterDX11 : public ParameterWriter
	{
	public:
		MatrixParameterWriterDX11();
		virtual ~MatrixParameterWriterDX11();

		void SetRenderParameterRef( MatrixParameterDX11* pParam );
		void SetValue( const Matrix4f& Value );

		virtual void WriteParameter( IParameterManager* pParamMgr );
		virtual void InitializeParameter( );

		virtual RenderParameterDX11* GetRenderParameterRef();

	protected:
		MatrixParameterDX11*			m_pParameter;
		Matrix4f						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // MatrixParameterWriterDX11_h
//--------------------------------------------------------------------------------

