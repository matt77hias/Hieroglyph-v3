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
#include "IRenderView.h"
#include "EventManager.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IRenderView::IRenderView( )
{
	m_pEntity = 0;
	m_pRoot = 0;

	m_iMaxRecurrence = 1;
	m_iCurrRecurrence = m_iMaxRecurrence;
	
	EventManager* pEventManager = EventManager::Get( );
	pEventManager->AddEventListener( RENDER_FRAME_START, this );
}
//--------------------------------------------------------------------------------
IRenderView::~IRenderView( )
{
}
//--------------------------------------------------------------------------------
VIEWTYPE IRenderView::GetType( )
{
	return( m_sParams.iViewType );
}
//--------------------------------------------------------------------------------
void IRenderView::SetEntity( Entity3D* pEntity )
{
	m_pEntity = pEntity;
}
//--------------------------------------------------------------------------------
int IRenderView::GetRTID( int index )
{
	if ( ( index >= 0 ) && ( index <= 8 ) )
		return( m_sParams.iRenderTargets[index] );
	else
		return( 0 );
}
//--------------------------------------------------------------------------------
void IRenderView::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the render parameters for this view.  Note that the view and projection
	// matrices are not set, since this will vary depending on the render view
	// type being rendered.

	//Renderer.SetVectorParameter( std::string("ViewParm00"), &(m_sParams.vParameters[0]) );
	//Renderer.SetVectorParameter( std::string("ViewParm01"), &(m_sParams.vParameters[1]) );
	//Renderer.SetVectorParameter( std::string("ViewParm02"), &(m_sParams.vParameters[2]) );
	//Renderer.SetVectorParameter( std::string("ViewParm03"), &(m_sParams.vParameters[3]) );
	//Renderer.SetVectorParameter( std::string("ViewParm04"), &(m_sParams.vParameters[4]) );
	//Renderer.SetVectorParameter( std::string("ViewParm05"), &(m_sParams.vParameters[5]) );
	//Renderer.SetVectorParameter( std::string("ViewParm06"), &(m_sParams.vParameters[6]) );
	//Renderer.SetVectorParameter( std::string("ViewParm07"), &(m_sParams.vParameters[7]) );

	//Renderer.SetTextureParameter( std::string("ViewTex00"), m_sParams.iTextures[0] );
	//Renderer.SetTextureParameter( std::string("ViewTex01"), m_sParams.iTextures[1] );
	//Renderer.SetTextureParameter( std::string("ViewTex02"), m_sParams.iTextures[2] );
	//Renderer.SetTextureParameter( std::string("ViewTex03"), m_sParams.iTextures[3] );
	//Renderer.SetTextureParameter( std::string("ViewTex04"), m_sParams.iTextures[4] );
	//Renderer.SetTextureParameter( std::string("ViewTex05"), m_sParams.iTextures[5] );
	//Renderer.SetTextureParameter( std::string("ViewTex06"), m_sParams.iTextures[6] );
	//Renderer.SetTextureParameter( std::string("ViewTex07"), m_sParams.iTextures[7] );
}
//--------------------------------------------------------------------------------
void IRenderView::SetName( std::wstring& name )
{
	m_sName = name;
}
//--------------------------------------------------------------------------------
std::wstring IRenderView::GetName( )
{
	return( m_sName );
}
//--------------------------------------------------------------------------------
bool IRenderView::HandleEvent( IEvent* pEvent )
{
	eEVENT e = pEvent->GetEventType();

	// Start of a rendering frame
	if ( e == RENDER_FRAME_START )
	{
		// Reset the allowed recurrence for this frame
		m_iCurrRecurrence = m_iMaxRecurrence;

		// Return false to ensure other views can reset as well
		return( false );
	}

	return( false );
}
//--------------------------------------------------------------------------------
void IRenderView::SetMaxRecurrence( int max )
{
	m_iMaxRecurrence = max;
}
//--------------------------------------------------------------------------------
int IRenderView::GetMaxRecurrence( )
{
	return( m_iMaxRecurrence );
}
//--------------------------------------------------------------------------------
void IRenderView::SetRoot( Node3D* pRoot )
{
	m_pRoot = pRoot;
}
//--------------------------------------------------------------------------------
void IRenderView::SetViewMatrix( const Matrix4f& matrix )
{
	ViewMatrix = matrix;
}
//--------------------------------------------------------------------------------
Matrix4f IRenderView::GetViewMatrix( )
{
	return( ViewMatrix );
}
//--------------------------------------------------------------------------------
void IRenderView::SetProjMatrix( const Matrix4f& matrix )
{
	ProjMatrix = matrix;
}
//--------------------------------------------------------------------------------
Matrix4f IRenderView::GetProjMatrix( )
{
	return( ProjMatrix );
}
//--------------------------------------------------------------------------------
void IRenderView::SetBackColor( Vector4f color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
void IRenderView::SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ )
{
	//m_viewport.X = x;
	//m_viewport.Y = y;
	//m_viewport.Width = w;
	//m_viewport.Height = h;
	//m_viewport.MinZ = MinZ;
	//m_viewport.MaxZ = MaxZ;
}
//--------------------------------------------------------------------------------
void IRenderView::SetViewPort( int viewport )
{
	m_iViewport = viewport;
}
//--------------------------------------------------------------------------------