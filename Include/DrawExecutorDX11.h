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
// DrawExecutorDX11
//
// This class specializes PipelineExecutorDX11 to provide an easy to use immediate
// mode rendering interface.  The basic configuration allows you to add vertices
// to a growable vertex buffer, which is later sent to the input assembler.  What 
// primitives those vertices will create in the IA depend on what topology is
// selected.
//
// It is important to consider that this pipeline executor does not use indices,
// and its geometry needs to be submitted and uploaded each time it changes.
// Because of this, it may be more efficient to use the GeometryDX11 class 
// instead if your geometry doesn't change very often.
//
// The vertex type is a template parameter for this class.  The only requirements 
// are that you provide a struct to represent the data, and that you provide the
// vertex layout information in a call to DrawExecutorDX11::SetLayoutElements.
// The existing engine usage of this template provides both the struct and the
// layout in a single object (see BasicVertexDX11 for an example) but that doesn't
// need to be the case.  You can provide your own vertices/layouts to match the 
// shader programs that you are using - it should be no problem to customize the
// vertex types!
//--------------------------------------------------------------------------------
#ifndef ImmediateGeometryDX11_h
#define ImmediateGeometryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineExecutorDX11.h"
#include "TGrowableVertexBufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class TVertex>
	class DrawExecutorDX11 : public PipelineExecutorDX11
	{
	public:
		DrawExecutorDX11( );
		virtual ~DrawExecutorDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		virtual void ResetGeometry();
		virtual void ResetVertices();

		void AddVertex( const TVertex& vertex );

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType();
		void SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY type );

		unsigned int GetVertexCount();
		virtual unsigned int GetPrimitiveCount();

		void SetMaxVertexCount( unsigned int count );

	protected:

		// The type of primitives listed in the index buffer
		D3D11_PRIMITIVE_TOPOLOGY m_ePrimType;

		// Use a growable vertex buffer to hold the vertex data.
		TGrowableVertexBufferDX11<TVertex> VertexBuffer;
	};

	#include "DrawExecutorDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // ImmediateGeometryDX11_h
