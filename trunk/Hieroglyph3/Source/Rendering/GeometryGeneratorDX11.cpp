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
#include "PCH.h"
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
	}
}
//--------------------------------------------------------------------------------
void GeometryGeneratorDX11::GenerateTexturedPlane( GeometryDX11* pGeometry, int SizeX, int SizeY )
{
	if ( pGeometry )
	{
		VertexElementDX11* pPositions = new VertexElementDX11( 3, SizeX * SizeY );
		pPositions->m_SemanticName = "POSITION";
		pPositions->m_uiSemanticIndex = 0;
		pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pPositions->m_uiInputSlot = 0;
		pPositions->m_uiAlignedByteOffset = 0;
		pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pPositions->m_uiInstanceDataStepRate = 0;

		VertexElementDX11* pTexCoords = new VertexElementDX11( 2, SizeX * SizeY );
		pTexCoords->m_SemanticName = "TEXCOORDS";
		pTexCoords->m_uiSemanticIndex = 0;
		pTexCoords->m_Format = DXGI_FORMAT_R32G32_FLOAT;
		pTexCoords->m_uiInputSlot = 0;
		pTexCoords->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pTexCoords->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pTexCoords->m_uiInstanceDataStepRate = 0;

		Vector3f* pPos = pPositions->Get3f( 0 );
		Vector2f* pTex = pTexCoords->Get2f( 0 );

		// Set the locations and texture coordinates first.
		for ( int y = 0; y < SizeY; y++ )
		{
			for ( int x = 0; x < SizeX; x++ )
			{
				pPos[y*SizeX+x] = Vector3f( (float)x, 0.0f, (float)y );		// Upper left
				pTex[y*SizeX+x] = Vector2f( (float)x, (float)y );
			}
		}

		// Generate the triangle faces of the plane.
		for ( int j = 0; j < SizeY-1; j++ )
		{
			for ( int i = 0; i < SizeX-1; i++ )
			{
				TriangleIndices f1;
				TriangleIndices f2;

				f1.P1() = j*SizeX + i;
				f1.P2() = (j*SizeX + i) + SizeX;
				f1.P3() = (j*SizeX + i) + 1;

				f2.P1() = (j*SizeX + i) + 1;
				f2.P2() = (j*SizeX + i) + SizeX;
				f2.P3() = (j*SizeX + i) + SizeX + 1;

				pGeometry->AddFace( f1 );
				pGeometry->AddFace( f2 );
			}
		}

		// Add the vertex elements to the geometry object.
		pGeometry->AddElement( pPositions );
		pGeometry->AddElement( pTexCoords );
	}
}
//--------------------------------------------------------------------------------
void GeometryGeneratorDX11::GenerateSkinnedBiped( GeometryDX11* pGeometry )
{
	// The skinned geometry requires to have at least positions, plus bone ID's.
	// Other vertex attributes can be added in addition to these if needed.

	if ( pGeometry )
	{
		VertexElementDX11* pPositions = new VertexElementDX11( 3, 8 );
		pPositions->m_SemanticName = "POSITION";
		pPositions->m_uiSemanticIndex = 0;
		pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pPositions->m_uiInputSlot = 0;
		pPositions->m_uiAlignedByteOffset = 0;
		pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pPositions->m_uiInstanceDataStepRate = 0;

		VertexElementDX11* pBoneIDs = new VertexElementDX11( 1, 8 );
		pBoneIDs->m_SemanticName = "BONEIDS";
		pBoneIDs->m_uiSemanticIndex = 0;
		pBoneIDs->m_Format = DXGI_FORMAT_R32_SINT;
		pBoneIDs->m_uiInputSlot = 0;
		pBoneIDs->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pBoneIDs->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pBoneIDs->m_uiInstanceDataStepRate = 0;

		Vector3f* pPos = pPositions->Get3f( 0 );
		int* pIDs = pBoneIDs->Get1i( 0 );


		// Specify the base bone as the starting point.
		pPos[0] = Vector3f( 0.0f, 0.0f, 0.0f );
		pIDs[0] = 0;

		// Next is the waist bone.
		pPos[1] = Vector3f( 0.0f, 1.0f, 0.0f );
		pIDs[1] = 1;

		// Next is the left leg.
		pPos[2] = Vector3f( -1.0f, 0.0f, 0.0f );
		pIDs[2] = 2;

		// Next is the right leg.
		pPos[3] = Vector3f( 1.0f, 0.0f, 0.0f );
		pIDs[3] = 3;

		// Next is the shoulder bone.
		pPos[4] = Vector3f( 0.0f, 2.0f, 0.0f );
		pIDs[4] = 4;

		// Next is the left arm.
		pPos[5] = Vector3f( -1.5f, 2.5f, 0.0f );
		pIDs[5] = 5;

		// Next is the right arm.
		pPos[6] = Vector3f( 1.5f, 2.5f, 0.0f );
		pIDs[6] = 6;

		// Next is the head.
		pPos[7] = Vector3f( 0.0f, 3.0f, 0.5f );
		pIDs[7] = 7;

		// Generate and add each line segment.
		LineIndices line;
		
		// Base to waist
		line.P1() = 0;
		line.P2() = 1;
		pGeometry->AddLine( line );

		// Waist to left leg
		line.P1() = 1;
		line.P2() = 2;
		pGeometry->AddLine( line );

		// Waist to right leg
		line.P1() = 1;
		line.P2() = 3;
		pGeometry->AddLine( line );

		// Waist to shoulder
		line.P1() = 1;
		line.P2() = 4;
		pGeometry->AddLine( line );

		// Shoulder to left arm
		line.P1() = 4;
		line.P2() = 5;
		pGeometry->AddLine( line );

		// Shoulder to right arm
		line.P1() = 4;
		line.P2() = 6;
		pGeometry->AddLine( line );

		// Shoulder to head
		line.P1() = 4;
		line.P2() = 7;
		pGeometry->AddLine( line );

		// Add the vertex elements to the geometry object.
		pGeometry->AddElement( pPositions );
		pGeometry->AddElement( pBoneIDs );
	}	
}
//--------------------------------------------------------------------------------
void GeometryGeneratorDX11::GenerateAxisGeometry( GeometryDX11* pGeometry )
{
	// The axis geometry requires to have at positions plus colors.

	if ( pGeometry )
	{
		VertexElementDX11* pPositions = new VertexElementDX11( 3, 5*3 );
		pPositions->m_SemanticName = "POSITION";
		pPositions->m_uiSemanticIndex = 0;
		pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pPositions->m_uiInputSlot = 0;
		pPositions->m_uiAlignedByteOffset = 0;
		pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pPositions->m_uiInstanceDataStepRate = 0;

		VertexElementDX11* pColors = new VertexElementDX11( 4, 5*3 );
		pColors->m_SemanticName = "COLOR";
		pColors->m_uiSemanticIndex = 0;
		pColors->m_Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pColors->m_uiInputSlot = 0;
		pColors->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pColors->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pColors->m_uiInstanceDataStepRate = 0;

		Vector3f* pPos = pPositions->Get3f( 0 );
		Vector4f* pCols = pColors->Get4f( 0 );


		// Create the X-Axis
		pPos[0] = Vector3f( 0.0f, 0.5f, 0.5f );
		pCols[0] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[1] = Vector3f( 0.0f, -0.5f, 0.5f );
		pCols[1] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[2] = Vector3f( 0.0f, -0.5f, -0.5f );
		pCols[2] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[3] = Vector3f( 0.0f, 0.5f, -0.5f );
		pCols[3] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[4] = Vector3f( 5.0f, 0.0f, 0.0f );
		pCols[4] = Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );

		// Next is the Y-Axis.
		pPos[5] = Vector3f( 0.5f, 0.0f, 0.5f );
		pCols[5] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[6] = Vector3f( -0.5f, 0.0f, 0.5f );
		pCols[6] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[7] = Vector3f( -0.5f, 0.0f, -0.5f );
		pCols[7] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[8] = Vector3f( 0.5f, 0.0f, -0.5f );
		pCols[8] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[9] = Vector3f( 0.0f, 5.0f, 0.0f );
		pCols[9] = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );

		// Next is the Z-Axis.
		pPos[10] = Vector3f( 0.5f, 0.5f, 0.0f );
		pCols[10] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[11] = Vector3f( -0.5f, 0.5f, 0.0f );
		pCols[11] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[12] = Vector3f( -0.5f, -0.5f, 0.0f );
		pCols[12] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[13] = Vector3f( 0.5f, -0.5f, 0.0f );
		pCols[13] = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
		pPos[14] = Vector3f( 0.0f, 0.0f, 5.0f );
		pCols[14] = Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );

		// Generate and add each line segment.
		TriangleIndices tri;
		
		tri.P1() = 0;
		tri.P2() = 1;
		tri.P3() = 4;
		pGeometry->AddFace( tri );

		tri.P1() = 1;
		tri.P2() = 2;
		tri.P3() = 4;
		pGeometry->AddFace( tri );

		tri.P1() = 2;
		tri.P2() = 3;
		tri.P3() = 4;
		pGeometry->AddFace( tri );

		tri.P1() = 3;
		tri.P2() = 0;
		tri.P3() = 4;
		pGeometry->AddFace( tri );


		tri.P1() = 5;
		tri.P2() = 6;
		tri.P3() = 9;
		pGeometry->AddFace( tri );

		tri.P1() = 6;
		tri.P2() = 7;
		tri.P3() = 9;
		pGeometry->AddFace( tri );

		tri.P1() = 7;
		tri.P2() = 8;
		tri.P3() = 9;
		pGeometry->AddFace( tri );

		tri.P1() = 8;
		tri.P2() = 5;
		tri.P3() = 9;
		pGeometry->AddFace( tri );


		tri.P1() = 10;
		tri.P2() = 11;
		tri.P3() = 14;
		pGeometry->AddFace( tri );

		tri.P1() = 11;
		tri.P2() = 12;
		tri.P3() = 14;
		pGeometry->AddFace( tri );

		tri.P1() = 12;
		tri.P2() = 13;
		tri.P3() = 14;
		pGeometry->AddFace( tri );

		tri.P1() = 13;
		tri.P2() = 10;
		tri.P3() = 14;
		pGeometry->AddFace( tri );

		// Add the vertex elements to the geometry object.
		pGeometry->AddElement( pPositions );
		pGeometry->AddElement( pColors );

		//pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	}	
}
//--------------------------------------------------------------------------------
