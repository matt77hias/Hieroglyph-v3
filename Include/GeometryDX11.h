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
// GeometryDX11
//
//
//--------------------------------------------------------------------------------
#ifndef GeometryDX11_h
#define GeometryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ResourceProxyDX11.h"
#include "VertexElementDX11.h"
#include "TriangleIndices.h"
#include "LineIndices.h"
#include "PointIndices.h"
#include "PipelineExecutorDX11.h"
#include "InputAssemblerStateDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexBufferDX11;
	class IndexBufferDX11;

	class GeometryDX11 : public PipelineExecutorDX11
	{
	public:
		GeometryDX11( );
		virtual ~GeometryDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );

		void AddElement( VertexElementDX11* element );
		void AddFace( TriangleIndices& face );
		void AddLine( LineIndices& line );
		void AddPoint( PointIndices& point );
		void AddIndex( UINT index );

        VertexElementDX11* GetElement( std::string name );
		VertexElementDX11* GetElement( std::wstring name );
		VertexElementDX11* GetElement( int index );

		UINT GetIndex( unsigned int index );

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType();
		void SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY type );

		int GetPrimitiveCount();
		UINT GetIndexCount();
		
		int GetVertexCount();
		int GetElementCount();
		int GetVertexSize();

		int CalculateVertexSize();
		int CalculateVertexCount();

		void GenerateInputLayout( int ShaderID );

		void LoadToBuffers( );

        bool ComputeTangentFrame( std::string positionSemantic = VertexElementDX11::PositionSemantic,
                                  std::string normalSemantic = VertexElementDX11::NormalSemantic,
                                  std::string texCoordSemantic = VertexElementDX11::TexCoordSemantic, 
                                  std::string tangentSemantic = VertexElementDX11::TangentSemantic );

		std::vector<VertexElementDX11*>		m_vElements;
		std::vector<UINT>					m_vIndices;
		
		ResourcePtr m_VB;
		ResourcePtr m_IB;

		// The size 
		int m_iVertexSize;
		int m_iVertexCount;

		// The type of primitives listed in the index buffer
		D3D11_PRIMITIVE_TOPOLOGY m_ePrimType;
	};

	typedef std::shared_ptr<GeometryDX11> GeometryPtr;
};
//--------------------------------------------------------------------------------
#endif // GeometryDX11_h
