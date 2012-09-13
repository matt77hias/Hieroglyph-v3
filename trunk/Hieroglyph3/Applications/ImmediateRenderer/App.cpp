//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "App.h"
#include "Log.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RotationController.h"


using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 800, 600, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.5f, 0.3f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( -3.0f, 7.0f, -10.0f );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.4f, 0.0f ) );

	// Create the actor to hold the immediate geometry.  

	m_pActor = new Actor();
	m_pScene->AddEntity( m_pActor->GetNode() );
	m_pActor->GetNode()->Position() = Vector3f( -3.0, 0.0f, 0.0f );

	m_pGeometry = ImmediateGeometryPtr( new ImmediateGeometryDX11() );
	m_pGeometry->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	m_pActor->GetBody()->SetGeometry( m_pGeometry );
	//m_pActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometrySolidMaterial( *m_pRenderer11 ) );
	m_pActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometryTexturedMaterial( *m_pRenderer11 ) );

	// Throw a rotation onto the actor to slowly rotate it about the Y-axis.
	RotationController* pRotController = new RotationController();
	m_pActor->GetNode()->AttachController( pRotController );



	m_pIndexedActor = new Actor();
	m_pScene->AddEntity( m_pIndexedActor->GetNode() );
	m_pIndexedActor->GetNode()->Position() = Vector3f( 3.0, 0.0f, 0.0f );

	m_pIndexedGeometry = IndexedImmediateGeometryPtr( new IndexedImmediateGeometryDX11() );
	m_pIndexedGeometry->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pIndexedGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	m_pIndexedActor->GetBody()->SetGeometry( m_pIndexedGeometry );
	//m_pIndexedActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometrySolidMaterial( *m_pRenderer11 ) );
	m_pIndexedActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometryTexturedMaterial( *m_pRenderer11 ) );

	// Throw a rotation onto the actor to slowly rotate it about the Y-axis.
	RotationController* pIndexedRotController = new RotationController( Vector3f( 0.0f, 1.0f, 0.0f ), -0.1f );
	m_pIndexedActor->GetNode()->AttachController( pIndexedRotController );



	m_pGeometryActor = new GeometryActor();
	m_pScene->AddEntity( m_pGeometryActor->GetNode() );
	m_pGeometryActor->GetNode()->Position() = Vector3f( 0.0f, 2.5f, 0.0f );

	m_pGeometryActor->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pGeometryActor->DrawSphere( Vector3f( 1.5f, 1.0f, 0.0f ), 1.5f, 40, 60 );
	m_pGeometryActor->DrawCylinder( Vector3f( -1.5f, -1.0f, 0.0f ), Vector3f( -3.0f, 3.0f, 0.0f ), 1.5f, 1.0f, 2, 20 );

	RotationController* pGeometryRotController = new RotationController( Vector3f( 0.0f, 1.0f, 0.0f ), -0.4f );
	m_pGeometryActor->GetNode()->AttachController( pGeometryRotController );

}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	EventManager::Get()->ProcessEvent( new EvtFrameStart( *m_pTimer ) );


	// Do some immediate rendering here.  These vertex data are applied in object
	// space, so we are essentially redefining our geometry in every frame.  This
	// is normally not the most efficient way, but it is significantly easier to
	// dynamically produce than it is to determine before hand what you need...
	//
	// In this case, I am animating a paraboloid shape as well as its color.
	
	m_pGeometry->ResetGeometry();

	const int GRIDSIZE = 20;
	const float fGRIDSIZE = static_cast<float>( GRIDSIZE );
	const float ELEMSIZE = 1.0f;
	const float fSIZESCALE = 5.0f / fGRIDSIZE;

	Vector3f v0, v1, v2;
	Vector2f uv0, uv1, uv2;

	float fScaling = 0.25f * sinf( m_pTimer->Runtime() * 0.75f );

	Vector4f TopColor = Vector4f( 0.25f, 1.0f, 1.0f, 1.0f );
	Vector4f BotColor = Vector4f( 1.0f, 0.25f, 0.25f, 1.0f );

	float fColorScale = 0.5f * cosf( m_pTimer->Runtime() * 11.0f ) + 0.5f;
	m_pGeometry->SetColor( TopColor * fColorScale + BotColor * ( 1.0f - fColorScale ) );

	for ( int z = 0; z < GRIDSIZE; z++ ) {
		for ( int x = 0; x < GRIDSIZE; x++ ) {
			
			float fX = static_cast<float>( x - GRIDSIZE / 2 );
			float fZ = static_cast<float>( z - GRIDSIZE / 2 );

			v0.x = fX + 0.0f*ELEMSIZE;
			v0.z = fZ + 0.0f*ELEMSIZE;
			v0.y = ( 5.0f - 0.2f * ( v0.x*v0.x + v0.z*v0.z ) ) * fScaling;
			uv0.x =        ( v0.x + ( fGRIDSIZE / 2.0f ) ) / fGRIDSIZE;
			uv0.y = 1.0f - ( v0.z + ( fGRIDSIZE / 2.0f ) ) / fGRIDSIZE;

			v1.x = fX + 0.0f*ELEMSIZE;
			v1.z = fZ + 1.0f*ELEMSIZE;
			v1.y = ( 5.0f - 0.2f * ( v1.x*v1.x + v1.z*v1.z ) ) * fScaling;
			uv1.x =        ( v1.x + ( fGRIDSIZE / 2.0f ) ) / fGRIDSIZE;
			uv1.y = 1.0f - ( v1.z + ( fGRIDSIZE / 2.0f ) ) / fGRIDSIZE;

			v2.x = fX + 1.0f*ELEMSIZE;
			v2.z = fZ + 0.0f*ELEMSIZE;
			v2.y = ( 5.0f - 0.2f * ( v2.x*v2.x + v2.z*v2.z ) ) * fScaling;
			uv2.x =        ( v2.x + ( fGRIDSIZE / 2.0f ) ) / fGRIDSIZE;
			uv2.y = 1.0f - ( v2.z + ( fGRIDSIZE / 2.0f ) ) / fGRIDSIZE;

			m_pGeometry->AddVertex( v0 * fSIZESCALE, uv0 );
			m_pGeometry->AddVertex( v1 * fSIZESCALE, uv1 );
			m_pGeometry->AddVertex( v2 * fSIZESCALE, uv2 );

		}
	}


	// Do some indexed immediate rendering by creating a grid of vertices first...

	m_pIndexedGeometry->ResetGeometry();

	for ( int z = 0; z < GRIDSIZE; z++ ) {
		for ( int x = 0; x < GRIDSIZE; x++ ) {
			
			float fX = static_cast<float>( x );
			float fZ = static_cast<float>( z );

			v0.x = fX - GRIDSIZE / 2;
			v0.z = fZ - GRIDSIZE / 2;
			v0.y = ( 5.0f - 0.2f * ( v0.x*v0.x + v0.z*v0.z ) ) * fScaling;
			uv0.x =        ( fX ) / ( fGRIDSIZE-1 );
			uv0.y = 1.0f - ( fZ ) / ( fGRIDSIZE-1 );

			m_pIndexedGeometry->AddVertex( v0 * fSIZESCALE, uv0 );
		}
	}

	// ... followed by creating the associated group of indices that reference
	// those vertices.

	for ( int z = 0; z < GRIDSIZE-1; z++ ) {
		for ( int x = 0; x < GRIDSIZE-1; x++ ) {

			m_pIndexedGeometry->AddIndex( (z*GRIDSIZE + x) );
			m_pIndexedGeometry->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE );
			m_pIndexedGeometry->AddIndex( (z*GRIDSIZE + x) + 1 );

			m_pIndexedGeometry->AddIndex( (z*GRIDSIZE + x) + 1 );
			m_pIndexedGeometry->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE );
			m_pIndexedGeometry->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE + 1 );
		}
	}


	// Print a message to show the framerate and sample name.

	std::wstringstream out;

	out << L"Hieroglyph 3 : " << GetName() << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();
	
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
		Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	
	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pActor );

	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( IEvent* pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDown* pKeyDown = (EvtKeyDown*)pEvent;

		unsigned int key = pKeyDown->GetCharacterCode();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUp* pKeyUp = (EvtKeyUp*)pEvent;

		unsigned int key = pKeyUp->GetCharacterCode();
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"ImmediateRenderer" ) );
}
//--------------------------------------------------------------------------------