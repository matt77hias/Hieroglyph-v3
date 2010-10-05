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
#include "GeometryLoaderDX11.h"
#include "GeometryDX11.h"
#include "VertexElementDX11.h"
#include "ms3dspec.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Log.h"
#include "GlyphString.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GeometryLoaderDX11::GeometryLoaderDX11( )
{
}
//--------------------------------------------------------------------------------
//CGeometryDX9* CGeometryLoaderDX9::loadPlyFile( std::wstring filename )
//{
//
//	std::wstring nextLine;
//	std::ifstream iFile( filename.c_str() );
//	std::wstring command;
//	std::wstring subcommand;
//
//	int iNumVertices;
//	int iNumFaces;
//	
//	CVertexElementDX9* pPositions = NULL;
//	Vector3f* pPos = NULL;
//
//	getline( iFile, nextLine, '\n' );
//
//	std::istringstream In(nextLine);
//
//	while ( command != "end_header" )
//	{
//		getline( iFile, nextLine, '\n' );
//		std::istringstream thisline(nextLine);
//		thisline >> command;
//
//		if ( command == "element" )
//		{
//			while ( thisline )
//			{
//				std::wstring test;
//				thisline >> test;
//				removeWhiteSpace( test );
//
//				if ( test.length() > 0 )
//				{
//					std::istringstream subcommandstream(test);
//					subcommandstream >> subcommand;
//
//					if ( subcommand == "vertex" )
//					{
//						thisline >> test;
//						removeWhiteSpace( test );
//						
//						if ( test.length() > 0 )
//						{
//							std::istringstream final( test );
//							final >> iNumVertices;
//
//							pPositions = new CVertexElementDX9( 3, iNumVertices );
//							pPositions->m_Name = "POSITION";
//							pPositions->m_Type = FLOAT3;
//							pPositions->m_Method = DEFAULT;
//							pPositions->m_Usage = POSITION;
//							pPositions->m_UsageIndex = 0;
//							pPos = (Vector3f*)((*pPositions)[0]);
//						}
//					}
//					if ( subcommand == "face" )
//					{
//						thisline >> test;
//						removeWhiteSpace( test );
//
//						if ( test.length() > 0 )
//						{
//							std::istringstream final( test );
//							final >> iNumFaces;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	for ( int i = 0; i < iNumVertices; i++ )
//	{
//		getline( iFile, nextLine, '\n' );
//		std::istringstream vertexData(nextLine);
//		vertexData >> pPos[i].x() >> pPos[i].y() >> pPos[i].z();
//	}
//
//	CGeometryDX9* pMesh = new CGeometryDX9();
//	pMesh->AddElement( pPositions );
//
//	for ( int i = 0; i < iNumFaces; i++ )
//	{
//		getline( iFile, nextLine, '\n' );
//		std::istringstream faceData(nextLine);
//		
//		int indexCount;
//		faceData >> indexCount;
//
//		TriangleIndices face;
//		faceData >> face.P1() >> face.P2() >> face.P3();
//
//		pMesh->AddFace( face );
//	}
//
//	iFile.close();
//
//	return pMesh;
//}
//--------------------------------------------------------------------------------
//GeometryDX11* GeometryLoaderDX11::loadMS3DFile( std::wstring filename )
//{
//	// Temporary Milkshape structures
//	unsigned short usVertexCount = 0;
//	unsigned short usTriangleCount = 0;
//	unsigned short usGroupCount = 0;
//	unsigned short usMaterialCount = 0;
//	MS3DVertex* pMS3DVertices = NULL;
//	MS3DTriangle* pMS3DTriangles = NULL;
//	MS3DGroup* pMS3DGroups = NULL;
//	MS3DMaterial* pMS3DMaterials = NULL;
//
//	int i;
//	std::ifstream fin;
//	MS3DHeader header;
//
//	// Open the file and read the MS3D header data
//	fin.open( filename.c_str(),std::ios::binary );
//	fin.read((char*)(&(header.id)), sizeof(header.id));
//	fin.read((char*)(&(header.version)), sizeof(header.version));
//	if (header.version!=3 && header.version!=4)
//		return NULL;
//
//	// Load all the vertices
//	fin.read((char*)(&usVertexCount), sizeof(unsigned short));
//	pMS3DVertices = new MS3DVertex[usVertexCount];
//	for (i=0; i < usVertexCount; i++)
//	{
//		fin.read((char*)&(pMS3DVertices[i].flags), sizeof(unsigned char));
//		fin.read((char*)&(pMS3DVertices[i].vertex[0]), sizeof(float));
//		fin.read((char*)&(pMS3DVertices[i].vertex[1]), sizeof(float));
//		fin.read((char*)&(pMS3DVertices[i].vertex[2]), sizeof(float));
//		fin.read((char*)&(pMS3DVertices[i].boneId), sizeof(char));
//		fin.read((char*)&(pMS3DVertices[i].referenceCount), sizeof(unsigned char));
//	}
//
//	// Load all the triangle indices
//	fin.read((char*)(&usTriangleCount), sizeof(unsigned short));
//	pMS3DTriangles = new MS3DTriangle[usTriangleCount];
//	for (i=0; i < usTriangleCount; i++)
//	{
//		fin.read((char*)&(pMS3DTriangles[i].flags),sizeof(unsigned short));
//		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[0]), sizeof(unsigned short)); //3*sizeof(unsigned short));
//		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[1]), sizeof(unsigned short)); //3*sizeof(unsigned short));
//		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[2]), sizeof(unsigned short)); //3*sizeof(unsigned short));
//		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[0]), 3*sizeof(float));
//		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[1]), 3*sizeof(float));
//		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[2]), 3*sizeof(float));
//		fin.read((char*)&(pMS3DTriangles[i].s), 3*sizeof(float));
//		fin.read((char*)&(pMS3DTriangles[i].t), 3*sizeof(float));
//		fin.read((char*)&(pMS3DTriangles[i].smoothingGroup), sizeof(unsigned char));
//		fin.read((char*)&(pMS3DTriangles[i].groupIndex), sizeof(unsigned char));
//	}
//
//	// Load all the group information
//	fin.read((char*)(&usGroupCount), sizeof(unsigned short));
//	pMS3DGroups = new MS3DGroup[usGroupCount];
//	for (i=0; i < usGroupCount; i++)
//	{
//		fin.read((char*)&(pMS3DGroups[i].flags), sizeof(unsigned char));
//		fin.read((char*)&(pMS3DGroups[i].name), sizeof(char[32]));
//		fin.read((char*)&(pMS3DGroups[i].numtriangles), sizeof(unsigned short));
//		unsigned short triCount = pMS3DGroups[i].numtriangles;
//		pMS3DGroups[i].triangleIndices = new unsigned short[triCount];
//		fin.read((char*)(pMS3DGroups[i].triangleIndices), sizeof(unsigned short) * triCount);
//		fin.read((char*)&(pMS3DGroups[i].materialIndex), sizeof(char));
//	}
//
//	// Load all the material information
//	fin.read((char*)(&usMaterialCount),sizeof(unsigned short));
//	pMS3DMaterials = new MS3DMaterial[usMaterialCount];
//	for (i=0; i < usMaterialCount; i++)
//	{
//		fin.read((char*)&(pMS3DMaterials[i].name), sizeof(char[32]));
//		fin.read((char*)&(pMS3DMaterials[i].ambient), 4 * sizeof(float));
//		fin.read((char*)&(pMS3DMaterials[i].diffuse), 4 * sizeof(float));
//		fin.read((char*)&(pMS3DMaterials[i].specular), 4 * sizeof(float));
//		fin.read((char*)&(pMS3DMaterials[i].emissive), 4 * sizeof(float));
//		fin.read((char*)&(pMS3DMaterials[i].shininess), sizeof(float));
//		fin.read((char*)&(pMS3DMaterials[i].transparency), sizeof(float));
//		fin.read((char*)&(pMS3DMaterials[i].mode), sizeof(char));
//		fin.read((char*)&(pMS3DMaterials[i].texture), sizeof(char[128]));
//		fin.read((char*)&(pMS3DMaterials[i].alphamap), sizeof(char[128]));
//	}
//
//	// Close the file (remaining file data unused)
//	fin.close();
//
//
//	// create the vertex element streams
//	CVertexElementDX9* pPositions = new CVertexElementDX9( 3, usVertexCount );
//	pPositions->m_Name = "POSITION";
//	pPositions->m_Type = FLOAT3;
//	pPositions->m_Method = DEFAULT;
//	pPositions->m_Usage = POSITION;
//	pPositions->m_UsageIndex = 0;
//
//	CVertexElementDX9* pTexcoords = new CVertexElementDX9( 2, usVertexCount );
//	pTexcoords->m_Name = "TEXCOORDS0";
//	pTexcoords->m_Type = FLOAT2;
//	pTexcoords->m_Method = DEFAULT;
//	pTexcoords->m_Usage = TEXCOORD;
//	pTexcoords->m_UsageIndex = 0;
//
//	CVertexElementDX9* pNormals = new CVertexElementDX9( 3, pPositions->Count() );
//	pNormals->m_Name = "NORMAL";
//	pNormals->m_Type = FLOAT3;
//	pNormals->m_Method = DEFAULT;
//	pNormals->m_Usage = NORMAL;
//	pNormals->m_UsageIndex = 0;
//
//	Vector3f* pPos = (Vector3f*)((*pPositions)[0]);
//	Vector3f* pNrm = (Vector3f*)((*pNormals)[0]);
//	Vector2f* pTex = (Vector2f*)((*pTexcoords)[0]);
//
//	CGeometryDX9* pMesh = new CGeometryDX9();
//
//	for ( int i = 0; i < usVertexCount; i++ )
//	{
//		pPos[i].x() = pMS3DVertices[i].vertex[0];
//		pPos[i].y() = pMS3DVertices[i].vertex[1];
//		pPos[i].z() = -pMS3DVertices[i].vertex[2];
//		pNrm[i].MakeZero();
//	}
//
//	TriangleIndices face;
//
//	for ( int i = 0; i < usTriangleCount; i++ )
//	{
//		face.P1() = pMS3DTriangles[i].vertexIndices[0];
//		face.P2() = pMS3DTriangles[i].vertexIndices[1];
//		face.P3() = pMS3DTriangles[i].vertexIndices[2];
//
//		pNrm[face.P1()].x() += pMS3DTriangles[i].vertexNormals[0][0];
//		pNrm[face.P1()].y() += pMS3DTriangles[i].vertexNormals[0][1];
//		pNrm[face.P1()].z() += pMS3DTriangles[i].vertexNormals[0][2];
//
//		pNrm[face.P2()].x() += pMS3DTriangles[i].vertexNormals[1][0];
//		pNrm[face.P2()].y() += pMS3DTriangles[i].vertexNormals[1][1];
//		pNrm[face.P2()].z() += pMS3DTriangles[i].vertexNormals[1][2];
//
//		pNrm[face.P3()].x() += pMS3DTriangles[i].vertexNormals[2][0];
//		pNrm[face.P3()].y() += pMS3DTriangles[i].vertexNormals[2][1];
//		pNrm[face.P3()].z() += pMS3DTriangles[i].vertexNormals[2][2];
//
//		pTex[face.P1()].x() = pMS3DTriangles[i].s[0];
//		pTex[face.P1()].y() = pMS3DTriangles[i].t[0];
//		pTex[face.P2()].x() = pMS3DTriangles[i].s[1];
//		pTex[face.P2()].y() = pMS3DTriangles[i].t[1];
//		pTex[face.P3()].x() = pMS3DTriangles[i].s[2];
//		pTex[face.P3()].y() = pMS3DTriangles[i].t[2];
//
//		pMesh->AddFace( face );
//	}
//
//	for ( int i = 0; i < usVertexCount; i++ )
//	{
//		pNrm[i].Normalize();
//		pNrm[i] *= -1.0f;
//	}
//
//	pMesh->AddElement( pPositions );
//	pMesh->AddElement( pTexcoords );
//	pMesh->AddElement( pNormals );
//
//	// Delete temporary materials
//	if (pMS3DMaterials != NULL)
//	{
//		delete[] pMS3DMaterials;
//		pMS3DMaterials = NULL;
//	}
//
//	// Delete temporary groups and their indices
//	if (pMS3DGroups != NULL)
//	{
//		for (i = 0; i < usGroupCount; i++)
//		{
//			if (pMS3DGroups[i].triangleIndices != NULL)
//			{
//				delete[] pMS3DGroups[i].triangleIndices;
//				pMS3DGroups[i].triangleIndices = NULL;
//			}
//		}
//		delete[] pMS3DGroups;
//		pMS3DGroups = NULL;
//	}
//
//	// Delete temporary triangles
//	if (pMS3DTriangles != NULL)
//	{
//		delete[] pMS3DTriangles;
//		pMS3DTriangles = NULL;
//	}
//
//	// Delete temporary vertices
//	if (pMS3DVertices != NULL)
//	{
//        delete[] pMS3DVertices;
//		pMS3DVertices = NULL;
//	}
//
//	pMesh->GenerateVertexDeclaration();
//	pMesh->LoadToBuffers();
//
//	return pMesh;
//}
//--------------------------------------------------------------------------------
GeometryDX11* GeometryLoaderDX11::loadMS3DFile2( std::wstring filename )
{
	// Temporary Milkshape structures
	unsigned short usVertexCount = 0;
	unsigned short usTriangleCount = 0;
	unsigned short usGroupCount = 0;
	unsigned short usMaterialCount = 0;
	MS3DVertex* pMS3DVertices = NULL;
	MS3DTriangle* pMS3DTriangles = NULL;
	MS3DGroup* pMS3DGroups = NULL;
	MS3DMaterial* pMS3DMaterials = NULL;

	int i;
	std::ifstream fin;
	MS3DHeader header;

	// Open the file and read the MS3D header data
	fin.open( filename.c_str(),std::ios::binary );
	fin.read((char*)(&(header.id)), sizeof(header.id));
	fin.read((char*)(&(header.version)), sizeof(header.version));
	if (header.version!=3 && header.version!=4)
		return NULL;

	// Load all the vertices
	fin.read((char*)(&usVertexCount), sizeof(unsigned short));
	pMS3DVertices = new MS3DVertex[usVertexCount];
	for (i=0; i < usVertexCount; i++)
	{
		fin.read((char*)&(pMS3DVertices[i].flags), sizeof(unsigned char));
		fin.read((char*)&(pMS3DVertices[i].vertex[0]), sizeof(float));
		fin.read((char*)&(pMS3DVertices[i].vertex[1]), sizeof(float));
		fin.read((char*)&(pMS3DVertices[i].vertex[2]), sizeof(float));
		fin.read((char*)&(pMS3DVertices[i].boneId), sizeof(char));
		fin.read((char*)&(pMS3DVertices[i].referenceCount), sizeof(unsigned char));
	}

	// Load all the triangle indices
	fin.read((char*)(&usTriangleCount), sizeof(unsigned short));
	pMS3DTriangles = new MS3DTriangle[usTriangleCount];
	for (i=0; i < usTriangleCount; i++)
	{
		fin.read((char*)&(pMS3DTriangles[i].flags),sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[0]), sizeof(unsigned short)); //3*sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[1]), sizeof(unsigned short)); //3*sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[2]), sizeof(unsigned short)); //3*sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[0]), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[1]), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[2]), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].s), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].t), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].smoothingGroup), sizeof(unsigned char));
		fin.read((char*)&(pMS3DTriangles[i].groupIndex), sizeof(unsigned char));
	}

	// Load all the group information
	fin.read((char*)(&usGroupCount), sizeof(unsigned short));
	pMS3DGroups = new MS3DGroup[usGroupCount];
	for (i=0; i < usGroupCount; i++)
	{
		fin.read((char*)&(pMS3DGroups[i].flags), sizeof(unsigned char));
		fin.read((char*)&(pMS3DGroups[i].name), sizeof(char[32]));
		fin.read((char*)&(pMS3DGroups[i].numtriangles), sizeof(unsigned short));
		unsigned short triCount = pMS3DGroups[i].numtriangles;
		pMS3DGroups[i].triangleIndices = new unsigned short[triCount];
		fin.read((char*)(pMS3DGroups[i].triangleIndices), sizeof(unsigned short) * triCount);
		fin.read((char*)&(pMS3DGroups[i].materialIndex), sizeof(char));
	}

	// Load all the material information
	fin.read((char*)(&usMaterialCount),sizeof(unsigned short));
	pMS3DMaterials = new MS3DMaterial[usMaterialCount];
	for (i=0; i < usMaterialCount; i++)
	{
		fin.read((char*)&(pMS3DMaterials[i].name), sizeof(char[32]));
		fin.read((char*)&(pMS3DMaterials[i].ambient), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].diffuse), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].specular), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].emissive), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].shininess), sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].transparency), sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].mode), sizeof(char));
		fin.read((char*)&(pMS3DMaterials[i].texture), sizeof(char[128]));
		fin.read((char*)&(pMS3DMaterials[i].alphamap), sizeof(char[128]));
	}

	// Close the file (remaining file data unused)
	fin.close();


	// create the vertex element streams
	VertexElementDX11* pPositions = new VertexElementDX11( 3, usTriangleCount*3 );
    pPositions->m_SemanticName = VertexElementDX11::PositionSemantic;
	pPositions->m_uiSemanticIndex = 0;
	pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pPositions->m_uiInputSlot = 0;
	pPositions->m_uiAlignedByteOffset = 0;
	pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pPositions->m_uiInstanceDataStepRate = 0;

	VertexElementDX11* pTexcoords = new VertexElementDX11( 2, usTriangleCount*3 );
	pTexcoords->m_SemanticName = VertexElementDX11::TexCoordSemantic;
	pTexcoords->m_uiSemanticIndex = 0;
	pTexcoords->m_Format = DXGI_FORMAT_R32G32_FLOAT;
	pTexcoords->m_uiInputSlot = 0;
	pTexcoords->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pTexcoords->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pTexcoords->m_uiInstanceDataStepRate = 0;

	VertexElementDX11* pNormals = new VertexElementDX11( 3, usTriangleCount*3 );
	pNormals->m_SemanticName = VertexElementDX11::NormalSemantic;
	pNormals->m_uiSemanticIndex = 0;
	pNormals->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pNormals->m_uiInputSlot = 0;
	pNormals->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pNormals->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pNormals->m_uiInstanceDataStepRate = 0;

	Vector3f* pPos = (Vector3f*)((*pPositions)[0]);
	Vector3f* pNrm = (Vector3f*)((*pNormals)[0]);
	Vector2f* pTex = (Vector2f*)((*pTexcoords)[0]);

	GeometryDX11* pMesh = new GeometryDX11();

	//for ( int i = 0; i < usVertexCount; i++ )
	//{
	//	pPos[i].x() = pMS3DVertices[i].vertex[0];
	//	pPos[i].y() = pMS3DVertices[i].vertex[1];
	//	pPos[i].z() = -pMS3DVertices[i].vertex[2];
	//	pNrm[i].MakeZero();
	//}

	TriangleIndices face;

	for ( int i = 0; i < usTriangleCount; i++ )
	{

		face.P1() = 3*i+0;
		face.P2() = 3*i+2;
		face.P3() = 3*i+1;

		pPos[3*i+0].x = pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].vertex[0];
		pPos[3*i+0].y = pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].vertex[1];
		pPos[3*i+0].z = -pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].vertex[2];
		pPos[3*i+1].x = pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].vertex[0];
		pPos[3*i+1].y = pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].vertex[1];
		pPos[3*i+1].z = -pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].vertex[2];
		pPos[3*i+2].x = pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].vertex[0];
		pPos[3*i+2].y = pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].vertex[1];
		pPos[3*i+2].z = -pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].vertex[2];

		pNrm[3*i+0].x = pMS3DTriangles[i].vertexNormals[0][0];
		pNrm[3*i+0].y = pMS3DTriangles[i].vertexNormals[0][1];
		pNrm[3*i+0].z = -pMS3DTriangles[i].vertexNormals[0][2];
		pNrm[3*i+1].x = pMS3DTriangles[i].vertexNormals[1][0];
		pNrm[3*i+1].y = pMS3DTriangles[i].vertexNormals[1][1];
		pNrm[3*i+1].z = -pMS3DTriangles[i].vertexNormals[1][2];
		pNrm[3*i+2].x = pMS3DTriangles[i].vertexNormals[2][0];
		pNrm[3*i+2].y = pMS3DTriangles[i].vertexNormals[2][1];
		pNrm[3*i+2].z = -pMS3DTriangles[i].vertexNormals[2][2];

		pTex[3*i+0].x = pMS3DTriangles[i].s[0];
		pTex[3*i+0].y = pMS3DTriangles[i].t[0];
		pTex[3*i+1].x = pMS3DTriangles[i].s[1];
		pTex[3*i+1].y = pMS3DTriangles[i].t[1];
		pTex[3*i+2].x = pMS3DTriangles[i].s[2];
		pTex[3*i+2].y = pMS3DTriangles[i].t[2];

		pMesh->AddFace( face );
	}

	for ( int i = 0; i < usVertexCount; i++ )
	{
		pNrm[i].Normalize();
	}

	pMesh->AddElement( pPositions );
	pMesh->AddElement( pTexcoords );
	pMesh->AddElement( pNormals );

	// Delete temporary materials
	if (pMS3DMaterials != NULL)
	{
		delete[] pMS3DMaterials;
		pMS3DMaterials = NULL;
	}

	// Delete temporary groups and their indices
	if (pMS3DGroups != NULL)
	{
		for (i = 0; i < usGroupCount; i++)
		{
			if (pMS3DGroups[i].triangleIndices != NULL)
			{
				delete[] pMS3DGroups[i].triangleIndices;
				pMS3DGroups[i].triangleIndices = NULL;
			}
		}
		delete[] pMS3DGroups;
		pMS3DGroups = NULL;
	}

	// Delete temporary triangles
	if (pMS3DTriangles != NULL)
	{
		delete[] pMS3DTriangles;
		pMS3DTriangles = NULL;
	}

	// Delete temporary vertices
	if (pMS3DVertices != NULL)
	{
        delete[] pMS3DVertices;
		pMS3DVertices = NULL;
	}

	//pMesh->GenerateVertexDeclaration();
	//pMesh->LoadToBuffers();

	return( pMesh );
}
//--------------------------------------------------------------------------------
GeometryDX11* GeometryLoaderDX11::loadMS3DFileWithAnimation( std::wstring filename, SkinnedActor* pActor )
{
	// Temporary Milkshape structures
	unsigned short usVertexCount = 0;
	unsigned short usTriangleCount = 0;
	unsigned short usGroupCount = 0;
	unsigned short usMaterialCount = 0;
	MS3DVertex* pMS3DVertices = NULL;
	MS3DTriangle* pMS3DTriangles = NULL;
	MS3DGroup* pMS3DGroups = NULL;
	MS3DMaterial* pMS3DMaterials = NULL;

	int i;
	std::ifstream fin;
	MS3DHeader header;

	// Open the file and read the MS3D header data
	fin.open( filename.c_str(),std::ios::binary );
	fin.read((char*)(&(header.id)), sizeof(header.id));
	fin.read((char*)(&(header.version)), sizeof(header.version));
	if (header.version!=3 && header.version!=4)
		return NULL;

	// Load all the vertices
	fin.read((char*)(&usVertexCount), sizeof(unsigned short));
	pMS3DVertices = new MS3DVertex[usVertexCount];
	for (i=0; i < usVertexCount; i++)
	{
		fin.read((char*)&(pMS3DVertices[i].flags), sizeof(unsigned char));
		fin.read((char*)&(pMS3DVertices[i].vertex[0]), sizeof(float));
		fin.read((char*)&(pMS3DVertices[i].vertex[1]), sizeof(float));
		fin.read((char*)&(pMS3DVertices[i].vertex[2]), sizeof(float));
		fin.read((char*)&(pMS3DVertices[i].boneId), sizeof(char));
		fin.read((char*)&(pMS3DVertices[i].referenceCount), sizeof(unsigned char));
	}

	// Load all the triangle indices
	fin.read((char*)(&usTriangleCount), sizeof(unsigned short));
	pMS3DTriangles = new MS3DTriangle[usTriangleCount];
	for (i=0; i < usTriangleCount; i++)
	{
		fin.read((char*)&(pMS3DTriangles[i].flags),sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[0]), sizeof(unsigned short)); //3*sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[1]), sizeof(unsigned short)); //3*sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexIndices[2]), sizeof(unsigned short)); //3*sizeof(unsigned short));
		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[0]), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[1]), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].vertexNormals[2]), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].s), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].t), 3*sizeof(float));
		fin.read((char*)&(pMS3DTriangles[i].smoothingGroup), sizeof(unsigned char));
		fin.read((char*)&(pMS3DTriangles[i].groupIndex), sizeof(unsigned char));
	}

	// Load all the group information
	fin.read((char*)(&usGroupCount), sizeof(unsigned short));
	pMS3DGroups = new MS3DGroup[usGroupCount];
	for (i=0; i < usGroupCount; i++)
	{
		fin.read((char*)&(pMS3DGroups[i].flags), sizeof(unsigned char));
		fin.read((char*)&(pMS3DGroups[i].name), sizeof(char[32]));
		fin.read((char*)&(pMS3DGroups[i].numtriangles), sizeof(unsigned short));
		unsigned short triCount = pMS3DGroups[i].numtriangles;
		pMS3DGroups[i].triangleIndices = new unsigned short[triCount];
		fin.read((char*)(pMS3DGroups[i].triangleIndices), sizeof(unsigned short) * triCount);
		fin.read((char*)&(pMS3DGroups[i].materialIndex), sizeof(char));
	}

	// Load all the material information
	fin.read((char*)(&usMaterialCount),sizeof(unsigned short));
	pMS3DMaterials = new MS3DMaterial[usMaterialCount];
	for (i=0; i < usMaterialCount; i++)
	{
		fin.read((char*)&(pMS3DMaterials[i].name), sizeof(char[32]));
		fin.read((char*)&(pMS3DMaterials[i].ambient), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].diffuse), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].specular), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].emissive), 4 * sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].shininess), sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].transparency), sizeof(float));
		fin.read((char*)&(pMS3DMaterials[i].mode), sizeof(char));
		fin.read((char*)&(pMS3DMaterials[i].texture), sizeof(char[128]));
		fin.read((char*)&(pMS3DMaterials[i].alphamap), sizeof(char[128]));
	}

	float fAnimationFPS; // 4 bytes
	float fCurrentTime; // 4 bytes
	int iTotalFrames; // 4 bytes
	unsigned short nNumJoints; // 2 bytes

	fin.read((char*)&(fAnimationFPS), sizeof(float));
	fin.read((char*)&(fCurrentTime), sizeof(float));
	fin.read((char*)&(iTotalFrames), sizeof(int));
	fin.read((char*)&(nNumJoints), sizeof(unsigned short));


	MS3DKeyframeJoint* pMS3DJoints = 0;

	if ( nNumJoints > 0 )
		pMS3DJoints = new MS3DKeyframeJoint[nNumJoints];

	for ( i = 0; i < nNumJoints; i++ )
	{
		fin.read((char*)&(pMS3DJoints[i].flags), sizeof(unsigned char));
		fin.read((char*)&(pMS3DJoints[i].name), sizeof(char[32]));
		fin.read((char*)&(pMS3DJoints[i].parentName), sizeof(char[32]));
		fin.read((char*)&(pMS3DJoints[i].rotation[0]), 3 * sizeof(float));
		fin.read((char*)&(pMS3DJoints[i].position[0]), 3 * sizeof(float));
		fin.read((char*)&(pMS3DJoints[i].numKeyFramesRot), sizeof(unsigned short));
		fin.read((char*)&(pMS3DJoints[i].numKeyFramesTrans), sizeof(unsigned short));

		pMS3DJoints[i].keyFramesRot = new MS3DKeyframeRotation[pMS3DJoints[i].numKeyFramesRot];
		pMS3DJoints[i].keyFramesTrans = new MS3DKeyframePosition[pMS3DJoints[i].numKeyFramesTrans];

		for ( int j = 0; j < pMS3DJoints[i].numKeyFramesRot; j++ )
		{
			fin.read((char*)&(pMS3DJoints[i].keyFramesRot[j].time), 1 * sizeof(float));
			fin.read((char*)&(pMS3DJoints[i].keyFramesRot[j].rotation), 3 * sizeof(float));
		}

		for ( int j = 0; j < pMS3DJoints[i].numKeyFramesTrans; j++ )
		{
			fin.read((char*)&(pMS3DJoints[i].keyFramesTrans[j].time), 1 * sizeof(float));
			fin.read((char*)&(pMS3DJoints[i].keyFramesTrans[j].position), 3 * sizeof(float));
		}

		//std::wstringstream s;
		//s << GlyphString::ToUnicode( std::string( pMS3DJoints[i].name ) ) << std::endl;
		//for ( int z = 0; z < pMS3DJoints[i].numKeyFramesRot; z++ )
		//	s << L"rotation: " << pMS3DJoints[i].keyFramesRot[z].time << L" " <<pMS3DJoints[i].keyFramesRot[z].rotation[0] << L" " << pMS3DJoints[i].keyFramesRot[z].rotation[1] << L" " << pMS3DJoints[i].keyFramesRot[z].rotation[2] << std::endl;

		//for ( int z = 0; z < pMS3DJoints[i].numKeyFramesTrans; z++ )
		//	s << L"position: " << pMS3DJoints[i].keyFramesTrans[z].time << L" " <<pMS3DJoints[i].keyFramesTrans[z].position[0] << L" " << pMS3DJoints[i].keyFramesTrans[z].position[1] << L" " << pMS3DJoints[i].keyFramesTrans[z].position[2] << std::endl;

		//Log::Get().Write( s.str() );
	}



	// Close the file (remaining file data unused)
	fin.close();


	// create the vertex element streams
	VertexElementDX11* pPositions = new VertexElementDX11( 3, usTriangleCount*3 );
	pPositions->m_SemanticName = VertexElementDX11::PositionSemantic;
	pPositions->m_uiSemanticIndex = 0;
	pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pPositions->m_uiInputSlot = 0;
	pPositions->m_uiAlignedByteOffset = 0;
	pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pPositions->m_uiInstanceDataStepRate = 0;

	VertexElementDX11* pBoneIDs = new VertexElementDX11( 1, usTriangleCount*3 );
	pBoneIDs->m_SemanticName = VertexElementDX11::BoneIDSemantic;
	pBoneIDs->m_uiSemanticIndex = 0;
	pBoneIDs->m_Format = DXGI_FORMAT_R32_SINT;
	pBoneIDs->m_uiInputSlot = 0;
	pBoneIDs->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pBoneIDs->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pBoneIDs->m_uiInstanceDataStepRate = 0;

	VertexElementDX11* pTexcoords = new VertexElementDX11( 2, usTriangleCount*3 );
	pTexcoords->m_SemanticName = VertexElementDX11::TexCoordSemantic;
	pTexcoords->m_uiSemanticIndex = 0;
	pTexcoords->m_Format = DXGI_FORMAT_R32G32_FLOAT;
	pTexcoords->m_uiInputSlot = 0;
	pTexcoords->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pTexcoords->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pTexcoords->m_uiInstanceDataStepRate = 0;

	VertexElementDX11* pNormals = new VertexElementDX11( 3, usTriangleCount*3 );
	pNormals->m_SemanticName = VertexElementDX11::NormalSemantic;
	pNormals->m_uiSemanticIndex = 0;
	pNormals->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pNormals->m_uiInputSlot = 0;
	pNormals->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pNormals->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pNormals->m_uiInstanceDataStepRate = 0;

	Vector3f* pPos = pPositions->Get3f( 0 );
	int* pIds = pBoneIDs->Get1i( 0 );
	Vector3f* pNrm = pNormals->Get3f( 0 );
	Vector2f* pTex = pTexcoords->Get2f( 0 );

	// Create the geometry object, and fill it with the data read from the file.

	GeometryDX11* pMesh = new GeometryDX11();

	TriangleIndices face;

	for ( int i = 0; i < usTriangleCount; i++ )
	{

		face.P1() = 3*i+0;
		face.P2() = 3*i+2;
		face.P3() = 3*i+1;

		pPos[3*i+0].x = pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].vertex[0];
		pPos[3*i+0].y = pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].vertex[1];
		pPos[3*i+0].z = -pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].vertex[2];
		pPos[3*i+1].x = pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].vertex[0];
		pPos[3*i+1].y = pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].vertex[1];
		pPos[3*i+1].z = -pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].vertex[2];
		pPos[3*i+2].x = pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].vertex[0];
		pPos[3*i+2].y = pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].vertex[1];
		pPos[3*i+2].z = -pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].vertex[2];

		pIds[3*i+0] = pMS3DVertices[pMS3DTriangles[i].vertexIndices[0]].boneId;
		pIds[3*i+1] = pMS3DVertices[pMS3DTriangles[i].vertexIndices[1]].boneId;
		pIds[3*i+2] = pMS3DVertices[pMS3DTriangles[i].vertexIndices[2]].boneId;

		pNrm[3*i+0].x = pMS3DTriangles[i].vertexNormals[0][0];
		pNrm[3*i+0].y = pMS3DTriangles[i].vertexNormals[0][1];
		pNrm[3*i+0].z = -pMS3DTriangles[i].vertexNormals[0][2];
		pNrm[3*i+1].x = pMS3DTriangles[i].vertexNormals[1][0];
		pNrm[3*i+1].y = pMS3DTriangles[i].vertexNormals[1][1];
		pNrm[3*i+1].z = -pMS3DTriangles[i].vertexNormals[1][2];
		pNrm[3*i+2].x = pMS3DTriangles[i].vertexNormals[2][0];
		pNrm[3*i+2].y = pMS3DTriangles[i].vertexNormals[2][1];
		pNrm[3*i+2].z = -pMS3DTriangles[i].vertexNormals[2][2];

		pTex[3*i+0].x = pMS3DTriangles[i].s[0];
		pTex[3*i+0].y = pMS3DTriangles[i].t[0];
		pTex[3*i+1].x = pMS3DTriangles[i].s[1];
		pTex[3*i+1].y = pMS3DTriangles[i].t[1];
		pTex[3*i+2].x = pMS3DTriangles[i].s[2];
		pTex[3*i+2].y = pMS3DTriangles[i].t[2];

		pMesh->AddFace( face );
	}

	for ( int i = 0; i < usVertexCount; i++ )
	{
		pNrm[i].Normalize();
	}

	pMesh->AddElement( pPositions );
	pMesh->AddElement( pBoneIDs );
	pMesh->AddElement( pTexcoords );
	pMesh->AddElement( pNormals );

	// Now set the geometry in the SkinnedActor, and create the bones
	// and add them to the SkinnedActor.

	if ( pActor )
	{
		// Set the geometry in the body of the actor
		pActor->GetBody()->SetGeometry( pMesh );

		// Create an array of nodes, one for each joint.
		std::map<std::string,Node3D*> JointNodes;

		for ( int i = 0; i < nNumJoints; i++ )
		{
			Node3D* pBone = new Node3D();

			Vector3f BindPosition = Vector3f( pMS3DJoints[i].position[0],
			 									pMS3DJoints[i].position[1],
												pMS3DJoints[i].position[2] );

			AnimationStream<Vector3f>* pPosFrames = new AnimationStream<Vector3f>();

			for ( int j = 0; j < pMS3DJoints[i].numKeyFramesTrans; j++ )
			{
				Vector3f p = Vector3f( pMS3DJoints[i].keyFramesTrans[j].position[0],
					pMS3DJoints[i].keyFramesTrans[j].position[1],
					pMS3DJoints[i].keyFramesTrans[j].position[2] );

				pPosFrames->AddState( AnimationState<Vector3f>( pMS3DJoints[i].keyFramesTrans[j].time, p ) ); 
			}

			AnimationStream<Vector3f>* pRotFrames = new AnimationStream<Vector3f>();
			
			Vector3f BindRotation = Vector3f( pMS3DJoints[i].rotation[0] + 6.28f, pMS3DJoints[i].rotation[1] + 6.28f, pMS3DJoints[i].rotation[2] + 6.28f );

			for ( int j = 0; j < pMS3DJoints[i].numKeyFramesRot; j++ )
			{
				Vector3f p = Vector3f( pMS3DJoints[i].keyFramesRot[j].rotation[0] + 6.28f,
					pMS3DJoints[i].keyFramesRot[j].rotation[1] + 6.28f,
					pMS3DJoints[i].keyFramesRot[j].rotation[2] + 6.28f );

				pRotFrames->AddState( AnimationState<Vector3f>( pMS3DJoints[i].keyFramesRot[j].time, p ) ); 
			}

			pActor->AddBoneNode( pBone, BindPosition, BindRotation, pPosFrames, pRotFrames );

			JointNodes[std::string(pMS3DJoints[i].name)] = pBone;
		}

		// Connect up the bones to form the skeleton.
		for ( int i = 0; i < nNumJoints; i++ )
		{
			Node3D* pParent = JointNodes[std::string(pMS3DJoints[i].parentName)];
			Node3D* pChild = JointNodes[std::string(pMS3DJoints[i].name)];

			// If the node has a parent, link them
			if ( pParent && pChild )
				pParent->AttachChild( pChild );

			// If the node has no parent, link it to the root of the skinned actor (for connection
			// to the scene graph).
			if ( !pParent && pChild )
				pActor->GetNode()->AttachChild( pChild );
		}
	}


	// Delete temporary joint information
	if ( pMS3DJoints )
	{
		for ( int i = 0; i < nNumJoints; i++ )
		{
			delete[] pMS3DJoints[i].keyFramesRot;
			delete[] pMS3DJoints[i].keyFramesTrans;
		}
		delete[] pMS3DJoints;
	}


	// Delete temporary materials
	if (pMS3DMaterials != NULL)
	{
		delete[] pMS3DMaterials;
		pMS3DMaterials = NULL;
	}

	// Delete temporary groups and their indices
	if (pMS3DGroups != NULL)
	{
		for (i = 0; i < usGroupCount; i++)
		{
			if (pMS3DGroups[i].triangleIndices != NULL)
			{
				delete[] pMS3DGroups[i].triangleIndices;
				pMS3DGroups[i].triangleIndices = NULL;
			}
		}
		delete[] pMS3DGroups;
		pMS3DGroups = NULL;
	}

	// Delete temporary triangles
	if (pMS3DTriangles != NULL)
	{
		delete[] pMS3DTriangles;
		pMS3DTriangles = NULL;
	}

	// Delete temporary vertices
	if (pMS3DVertices != NULL)
	{
        delete[] pMS3DVertices;
		pMS3DVertices = NULL;
	}

	//pMesh->GenerateVertexDeclaration();
	pMesh->LoadToBuffers();

	return( pMesh );
}
//--------------------------------------------------------------------------------
//void CGeometryLoaderDX9::removeWhiteSpace( std::wstring& s )
//{
//	unsigned int i = 0;
//	while ( i < s.length() )
//	{
//		switch ( s[i] )
//		{
//			case '\t':
//			case '\n':
//			case '\f':
//			case '\r':
//				s = s.substr( 0, i ) + s.substr( i+1, s.length() );
//				break;
//			default:
//				i++;
//		}
//	}
//}
//--------------------------------------------------------------------------------
//CGeometryDX9* CGeometryLoader::loadXFile( std::wstring filename )
//{
//	CGeometryDX9* pTriMesh = NULL;
//
//	ID3DXMesh* pMesh = NULL;
//	ID3DXBuffer* pAdjacency = NULL;
//	ID3DXBuffer* pMaterials = NULL;
//	ID3DXBuffer* pEffects = NULL;
//
//	DWORD Options = 0;
//	Options |= D3DXMESH_32BIT;
//	Options |= D3DXMESH_VB_MANAGED;
//	Options |= D3DXMESH_IB_MANAGED;
//
//	DWORD dMaterials = 0;
//
//	if ( D3D_OK == D3DXLoadMeshFromX( 
//		filename.c_str(),
//		Options,
//		CRenderer::GetRenderer()->GetDevice(),
//		&pAdjacency,
//		&pMaterials,
//		&pEffects,
//		&dMaterials,
//		&pMesh ) )
//	{
//		IDirect3DVertexBuffer9* pVB = NULL;
//		IDirect3DIndexBuffer9* pIB = NULL;
//		D3DVERTEXELEMENT9 pDeclaration[MAX_FVF_DECL_SIZE];
//	
//		DWORD dVertices = pMesh->GetNumVertices();
//		DWORD dFaces = pMesh->GetNumFaces();
//		DWORD dSize = pMesh->GetNumBytesPerVertex(); 
//
//		pMesh->GetVertexBuffer( &pVB );
//		pMesh->GetIndexBuffer( &pIB );
//		pMesh->GetDeclaration( pDeclaration );
//
//		pTriMesh = new CTriMesh();
//
//		std::ostringstream out;
//
//		int i = 0;
//		while ( pDeclaration[i].Stream != 0xff )
//		{
//			int iElementSize = 0;
//			if ( pDeclaration[i].Type == D3DDECLTYPE_FLOAT1 )
//				iElementSize = 1;
//			else if ( pDeclaration[i].Type == D3DDECLTYPE_FLOAT2 )
//				iElementSize = 2;
//			else if ( pDeclaration[i].Type == D3DDECLTYPE_FLOAT3 )
//				iElementSize = 3;
//			else if ( pDeclaration[i].Type == D3DDECLTYPE_FLOAT4 )
//				iElementSize = 4;
//
//			CVertexElement* pElement = new CVertexElement( iElementSize, dVertices );
//			pElement->m_Name = CGeometryLoader::getElementName( pDeclaration[i].Usage, pDeclaration[i].UsageIndex );
//			pElement->m_Type = (Glyph::VertexElementType)pDeclaration[i].Type;
//			pElement->m_Method = (Glyph::VertexElementMethod)pDeclaration[i].Method;
//			pElement->m_Usage = (Glyph::VertexElementUsage)pDeclaration[i].Usage;
//			pElement->m_UsageIndex = pDeclaration[i].UsageIndex;
//
//			BYTE* pbVertices;
//			pVB->Lock(0,0,(void**)(&pbVertices),0);
//
//			BYTE* pbStartVertex = pbVertices + pDeclaration[i].Offset;
//			BYTE* pbCurrentVertex = pbStartVertex;
//
//			for ( unsigned int v = 0; v < dVertices; v++ )
//			{
//				memcpy( (void*)((*pElement)[v]), pbCurrentVertex, pElement->size() * sizeof(float));
//				pbCurrentVertex += dSize;
//			}
//
//			pVB->Unlock();
//
//			pTriMesh->addElement( pElement );
//			i++;
//		}
//
//		TriangleIndices face;
//
//		unsigned int* pIndices = NULL;
//		pIndices = new unsigned int[dFaces*3];
//
//	    void* pbIndices = NULL;
//	    pIB->Lock( 0, dFaces * 3 * sizeof(unsigned int), &pbIndices, 0 );
//		memcpy( pIndices, pbIndices, dFaces * 3 * sizeof(unsigned int) );
//		pIB->Unlock();
//
//		for ( unsigned int i = 0; i < dFaces; i++ )
//		{
//			face.P1() = pIndices[3*i+0];
//			face.P2() = pIndices[3*i+1];
//			face.P3() = pIndices[3*i+2];
//			pTriMesh->addFace( face );
//		}
//
//		delete [] pIndices;
//
//		// create vertex elements depending on the vertex declaration *
//		// for each element, lock it and copy from the vertex buffer *
//		// add each element to the CTriMesh *
//		// for each face, create a TriangleIndices and add it to CTriMesh *
//		// clean up and release all temporary objects
//
//		pVB->Release();
//		pIB->Release();
//		pMesh->Release();
//	}
//
//
//	return( pTriMesh );
//}
//--------------------------------------------------------------------------------
//std::wstring CGeometryLoaderDX9::getElementName( int usage, int index )
//{
//	std::wstring name; 
//
//	switch( usage )
//	{
//		case D3DDECLUSAGE_POSITION:
//			name = "POSITION";
//			break;
//		case D3DDECLUSAGE_BLENDWEIGHT:
//			name = "BLENDWEIGHT";
//			break;
//		case D3DDECLUSAGE_BLENDINDICES:
//			name = "BLENDINDICES";
//			break;
//		case D3DDECLUSAGE_NORMAL:
//			name = "NORMAL";
//			break;
//		case D3DDECLUSAGE_PSIZE:
//			name = "PSIZE";
//			break;
//		case D3DDECLUSAGE_TEXCOORD:
//			switch( index )
//			{
//			case 0:
//				name = "TEXCOORD0";
//				break;
//			case 1:
//				name = "TEXCOORD1";
//				break;
//			case 2:
//				name = "TEXCOORD2";
//				break;
//			case 3:
//				name = "TEXCOORD3";
//				break;
//			case 4:
//				name = "TEXCOORD4";
//				break;
//			case 5:
//				name = "TEXCOORD5";
//				break;
//			case 6:
//				name = "TEXCOORD6";
//				break;
//			case 7:
//				name = "TEXCOORD7";
//				break;
//			}
//			break;
//		case D3DDECLUSAGE_TANGENT:
//			name = "TANGENT";
//			break;
//		case D3DDECLUSAGE_BINORMAL:
//			name = "BINORMAL";
//			break;
//		case D3DDECLUSAGE_TESSFACTOR:
//			name = "TESSFACTOR";
//			break;
//		case D3DDECLUSAGE_POSITIONT:
//			name = "TPOSITION";
//			break;
//		case D3DDECLUSAGE_COLOR:
//			switch( index )
//			{
//			case 0:
//				name = "DIFFUSE";
//				break;
//			case 1:
//				name = "SPECULAR";
//				break;
//			}
//			break;
//		case D3DDECLUSAGE_FOG:
//			name = "FOG";
//			break;
//		case D3DDECLUSAGE_DEPTH:
//			name = "DEPTH";
//			break;
//		case D3DDECLUSAGE_SAMPLE:
//			name = "SAMPLE";
//			break;
//
//	}
//
//	return( name );
//}



