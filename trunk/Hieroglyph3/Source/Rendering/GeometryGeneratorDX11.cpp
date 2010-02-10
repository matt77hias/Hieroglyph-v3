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
#include "GeometryGeneratorDX11.h"
#include "VertexElementDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GeometryGeneratorDX11::GeometryGeneratorDX11( )
{
}
//--------------------------------------------------------------------------------
void GeometryGeneratorDX11::GenerateFullScreenQuad( GeometryDX11* pGeometry )
{
	if ( pGeometry )
	{
		VertexElementDX11* pClipPositions = new VertexElementDX11( 4, 4 );
		pClipPositions->m_SemanticName = "POSITION";
		pClipPositions->m_uiSemanticIndex = 0;
		pClipPositions->m_Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pClipPositions->m_uiInputSlot = 0;
		pClipPositions->m_uiAlignedByteOffset = 0;
		pClipPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pClipPositions->m_uiInstanceDataStepRate = 0;

		VertexElementDX11* pTexCoords = new VertexElementDX11( 2, 4 );
		pTexCoords->m_SemanticName = "TEXCOORDS";
		pTexCoords->m_uiSemanticIndex = 0;
		pTexCoords->m_Format = DXGI_FORMAT_R32G32_FLOAT;
		pTexCoords->m_uiInputSlot = 0;
		pTexCoords->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pTexCoords->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pTexCoords->m_uiInstanceDataStepRate = 0;

		Vector4f* pClipPos = pClipPositions->Get4f( 0 );
		Vector2f* pTex = pTexCoords->Get2f( 0 );

		// Set the clip positions of each vertex in the near clipping plane
		pClipPos[0] = Vector4f( -1.0f, 1.0f, 0.0f, 1.0f );		// Upper left
		pClipPos[1] = Vector4f( -1.0f, -1.0f, 0.0f, 1.0f );	// Lower left
		pClipPos[2] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );		// Upper right
		pClipPos[3] = Vector4f( 1.0f, -1.0f, 0.0f, 1.0f );		// Lower right

		pTex[0] = Vector2f( 0.0f, 0.0f );
		pTex[1] = Vector2f( 0.0f, 1.0f );
		pTex[2] = Vector2f( 1.0f, 0.0f );
		pTex[3] = Vector2f( 1.0f, 1.0f );

		TriangleIndices face;

		pGeometry->AddElement( pClipPositions );
		pGeometry->AddElement( pTexCoords );

		face = TriangleIndices( 0, 2, 1 );
		pGeometry->AddFace( face );

		face = TriangleIndices( 1, 2, 3 );
		pGeometry->AddFace( face );

		//pGeometry->GenerateVertexDeclaration();
		//pGeometry->LoadToBuffers();	
	}
}
//--------------------------------------------------------------------------------