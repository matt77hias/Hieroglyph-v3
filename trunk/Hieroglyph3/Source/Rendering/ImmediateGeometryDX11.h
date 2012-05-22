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
// ImmediateGeometryDX11
//
// This class specializes PipelineExecutorDX11 to provide an easy to use immediate
// mode rendering interface.  The basic configuration allows you to add vertices
// to a dynamic buffer, which is later sent to the input assembler.  What geometry
// those vertices will create in the IA depend on what topology is selected.
//
// It is important to consider that this pipeline executor does not use indices,
// and its geometry needs to be submitted and uploaded each time it changes.
// Because of this, it may be more efficient to use the GeometryDX11 class 
// instead if your geometry doesn't change very often.
//
// The vertices are provided with a fixed set of contents.  This includes the 
// following components:
//
// float3 position
// float4 color
// float2 texcoords
//
// The class is designed to be used with its matching material, but there is no 
// reason that you couldn't switch to use a customized material.  As long as its
// pipeline configuration can be applied to this input layout, then there should
// be no problem.
//--------------------------------------------------------------------------------
#ifndef ImmediateGeometryDX11_h
#define ImmediateGeometryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineExecutorDX11.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "ResourceProxyDX11.h"
#include "TArray.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexBufferDX11;

	struct ImmediateVertexDX11 {
		Vector3f position;
		Vector4f color;
		Vector2f texcoords;
	};

	class ImmediateGeometryDX11 : public PipelineExecutorDX11
	{
	public:
		ImmediateGeometryDX11( );
		virtual ~ImmediateGeometryDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );

		void AddVertex( const ImmediateVertexDX11& vertex );
		void AddVertex( const Vector3f& position );
		void AddVertex( const Vector3f& position, const Vector4f& color );
		void AddVertex( const Vector3f& position, const Vector2f& texcoords );
		void AddVertex( const Vector3f& position, const Vector4f& color, const Vector2f& texcoords );

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType();
		void SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY type );

		void SetColor( const Vector4f& color );
		Vector4f GetColor( );

		void SetMaxSize( unsigned int maxVertices );
		unsigned int GetMaxSize();

		unsigned int GetVertexCount();
		unsigned int GetPrimitiveCount();

		void GenerateInputLayout( int ShaderID );

	protected:
		
		void UploadData( PipelineManagerDX11* pPipeline );

		ResourcePtr m_VB;

		// The sizes
		unsigned int m_uiMaxSize;
		unsigned int m_uiVertexCount;

		Vector4f m_Color;

		// The type of primitives listed in the index buffer
		D3D11_PRIMITIVE_TOPOLOGY m_ePrimType;

		// A description of our vertex elements
		TArray<D3D11_INPUT_ELEMENT_DESC> m_elements;

		// The pointer to our array of vertex data
		ImmediateVertexDX11* m_pVertexArray;
	};

	typedef std::shared_ptr<ImmediateGeometryDX11> ImmediateGeometryPtr;
};
//--------------------------------------------------------------------------------
#endif // ImmediateGeometryDX11_h
