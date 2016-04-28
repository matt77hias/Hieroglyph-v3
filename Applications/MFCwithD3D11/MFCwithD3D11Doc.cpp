//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "stdafx.h"

#include "MFCwithD3D11.h"
#include "MFCwithD3D11Doc.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CMFCwithD3D11Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCwithD3D11Doc, CDocument)
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------
CMFCwithD3D11Doc::CMFCwithD3D11Doc()
{
	pScene = nullptr;
	pActor = nullptr;
}
//--------------------------------------------------------------------------------
CMFCwithD3D11Doc::~CMFCwithD3D11Doc()
{
}
//--------------------------------------------------------------------------------
BOOL CMFCwithD3D11Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	pScene = new Glyph3::Scene();
	pActor = new Glyph3::Actor();

	Glyph3::GeometryPtr pGeometry = Glyph3::GeometryLoaderDX11::loadMS3DFile2( L"box.ms3d" );
    pGeometry->LoadToBuffers();
    pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	pActor->GetBody()->Visual.SetGeometry( pGeometry );
	pActor->GetBody()->Visual.SetMaterial( Glyph3::MaterialGeneratorDX11::GeneratePhong( *Glyph3::RendererDX11::Get() ) );
	pActor->GetBody()->Controllers.Attach( new Glyph3::RotationController<Glyph3::Entity3D>( Glyph3::Vector3f::Random(), 1.0f ) );

	pScene->AddActor( pActor );
	pScene->Update( 0.0f );
	return TRUE;
}
//--------------------------------------------------------------------------------
void CMFCwithD3D11Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
//--------------------------------------------------------------------------------
#ifdef _DEBUG
void CMFCwithD3D11Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCwithD3D11Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
//--------------------------------------------------------------------------------
void CMFCwithD3D11Doc::DeleteContents()
{
	if ( pScene ) {
		delete pScene;
	}

	CDocument::DeleteContents();
}
//--------------------------------------------------------------------------------
CMFCwithD3D11Doc* CMFCwithD3D11Doc::GetDoc()
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	return (CMFCwithD3D11Doc*) pFrame->GetActiveDocument();
}
//--------------------------------------------------------------------------------