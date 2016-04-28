//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "App.h"
#include "Log.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "ScriptManager.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "IParameterManager.h"

#include "ShaderResourceParameterWriterDX11.h"
#include "RotationController.h"

#include "ActorGenerator.h"

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
	if ( !ConfigureRenderingEngineComponents( 1024, 800, D3D_FEATURE_LEVEL_11_0 ) ) {
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

	// Create the light parameters for use with this effect

	Vector4f LightParams = Vector4f( 0.2f, 0.2f, 0.2f, 0.7f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &LightParams );


	// Configure the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetTranslation( Vector3f( 7.0f, 7.0f, -10.0f ) );
	m_pCamera->Spatial().SetRotation( Vector3f( 0.3f, -0.4f, 0.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 0.0f ) );


	// Create a Kinect render view, and then attach it to the color and depth
	// visualization actors.  This will ensure that the render view is executed
	// prior to being used by either actor.

	m_pKinectView = new ViewKinect( *m_pRenderer11 );

	m_pColorActor = ActorGenerator::GenerateVisualizationTexture2D( 
		*m_pRenderer11, m_pKinectView->GetColorResource(), 0 );
	m_pDepthActor = ActorGenerator::GenerateVisualizationTexture2D( 
		*m_pRenderer11, m_pKinectView->GetDepthResource(),
		MaterialGeneratorDX11::GenerateKinectDepthBufferMaterial(*m_pRenderer11) );
	
	m_pColorActor->GetBody()->Visual.GetMaterial()->Params[VT_PERSPECTIVE].Tasks.push_back( m_pKinectView );
	m_pDepthActor->GetBody()->Visual.GetMaterial()->Params[VT_PERSPECTIVE].Tasks.push_back( m_pKinectView );
	
	m_pColorActor->GetNode()->Transform.Position() = Vector3f( -5.5f, 5.5f, 5.0f );
	m_pColorActor->GetNode()->Transform.Rotation().RotationY( -0.7f );
	m_pDepthActor->GetNode()->Transform.Position() = Vector3f( -5.5f, 0.0f, 5.0f );
	m_pDepthActor->GetNode()->Transform.Rotation().RotationY( -0.7f );



	// Create an actor to represent the 3D reconstruction of the scene.

	Actor* m_pDepthMapViewer = new Actor();

	GeometryPtr pGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateTexturedPlane( pGeometry, 320, 240 );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	MaterialPtr pMaterial = MaterialGeneratorDX11::GenerateKinectReconstructionMaterial( *m_pRenderer11 );

	m_pDepthMapViewer->GetBody()->Visual.SetGeometry( pGeometry );
	m_pDepthMapViewer->GetBody()->Visual.SetMaterial( pMaterial );
	m_pDepthMapViewer->GetBody()->Visual.GetMaterial()->Params[VT_PERSPECTIVE].Tasks.push_back( m_pKinectView );

	m_pDepthMapViewer->GetNode()->Transform.Position() = Vector3f( 1.0f, 0.0f, 2.0f );
	m_pDepthMapViewer->GetNode()->Transform.Rotation().RotationX( 0.0f );
	m_pDepthMapViewer->GetNode()->Transform.Scale() = Vector3f( 7.0f, 7.0f, 7.0f );



	// Create a skeleton actor to show the joint tracking in the 3D representation.
	// It is then bound to the kinect render view for updating its values from the
	// SDK calculated joints.  It is then connected to the depth map viewer so that
	// any scaling applied matched between both of them.

	m_pSkeletonActor = new KinectSkeletonActor();
	m_pKinectView->SetSkeletonActor( m_pSkeletonActor );
	m_pDepthMapViewer->GetNode()->AttachChild( m_pSkeletonActor->GetNode() );


	// Add all our objects to the scene for rendering.

	m_pScene->AddActor( m_pColorActor );
	m_pScene->AddActor( m_pDepthActor );
	m_pScene->AddActor( m_pDepthMapViewer );

	SetMultiThreadedMode( false );

	// Do a one time initialization of the scene lighting parameters here
	Vector4f Ka( 0.2f, 0.2f, 0.2f, 0.2f );
	VectorParameterDX11* FactorKa = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ka" ) );
	FactorKa->InitializeParameterData( &Ka );

	Vector4f Kd( 0.5f, 0.5f, 0.5f, 0.5f );
	VectorParameterDX11* FactorKd = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Kd" ) );
	FactorKd->InitializeParameterData( &Kd );

	Vector4f Ks( 1.0f, 1.0f, 1.0f, 1.0f );
	VectorParameterDX11* FactorKs = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ks" ) );
	FactorKs->InitializeParameterData( &Ks );

	Vector4f LightPosition( 100.0f, 100.0f, -100.0f, 0.0f );
	VectorParameterDX11* FactorLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPosition" ) );
	FactorLightPosition->InitializeParameterData( &LightPosition );

	Vector4f Ia( 0.25f, 0.25f, 0.25f, 0.25f );
	VectorParameterDX11* FactorIa = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ia" ) );
	FactorIa->InitializeParameterData( &Ia );

	Vector4f Id( 0.5f, 0.5f, 0.5f, 1.0f );
	VectorParameterDX11* FactorId = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Id" ) );
	FactorId->InitializeParameterData( &Id );

	Vector4f Is( 1.0f, 1.0f, 1.0f, 1.0f );
	VectorParameterDX11* FactorIs = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Is" ) );
	FactorIs->InitializeParameterData( &Is );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();


	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	std::wstringstream out;
	out << L"Hieroglyph 3 : Kinect Playground" << std::endl;
	out << L"Color and depth are represented on the left, with a 3D reconstruction on the right." << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();

	m_pTextOverlayView->WriteText( out.str(), Matrix4f::Identity(), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pKinectView );

	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"KinectPlayground" ) );
}
//--------------------------------------------------------------------------------