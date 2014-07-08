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
#include "FileSystem.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "FirstPersonCamera.h"
#include "RotationController.h"

#include "GlyphRift/RiftController.h"
#include "GlyphRift/ViewRift.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------
App::App()
{
	
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// Create and initialize the rift HMD objects.

	m_pRiftMgr = std::make_shared<RiftManager>();
	m_pRiftHmd = std::make_shared<RiftHMD>( m_pRiftMgr );
	
	// The desired window size is based on the screen size of the Rift display.
	
	if ( !ConfigureRenderingEngineComponents( m_pRiftHmd->HmdDisplayWidth(),
											  m_pRiftHmd->HmdDisplayHeight(), 
											  D3D_FEATURE_LEVEL_11_0 ) )
	{
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	// We want single threaded operation when using the Oculus SDK, since it 
	// manages some aspects of timing in the eye rendering functions.

	SetMultiThreadedMode( false );

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
bool App::ConfigureRenderingSetup()
{
	// Create the render view that will produce the images for the left and right
	// eyes in the Rift.

	ViewRift* pRiftView = new ViewRift( m_pRiftHmd, m_BackBuffer, m_pWindow->GetSwapChain() );
	pRiftView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );
	m_pRenderView = pRiftView;

	// TODO: we need a special way to interact with the text overlay, since
	//       it must be stereo compatible.  Currently this is just overwritten
	//       by the Oculus SDK when it does its distortion rendering.

	m_pTextOverlayView = new ViewTextOverlay( *m_pRenderer11, m_BackBuffer );

	
	// Create a typical first-person camera.  This is used to represent the
	// location of the viewer, while the Rift will be controlling any deviation
	// from that location.  The DK1 tracks orientation changes only, while the
	// later models will track position and orientation both.  Having the camera
	// plus the Rift allows us to use normal WASD movements, plus the natural 
	// reactions from user's head movement.

	m_pCamera = new FirstPersonCamera();
	m_pCamera->SetEventManager( &CameraEventHub );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetOverlayView( m_pTextOverlayView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>( GLYPH_PI ) / 4.0f );

	m_pScene->AddCamera( m_pCamera );

	// Add the additional rift based controller to the *body* of the camera.  This
	// uses the input from the rift headset, but still allows for the traditional
	// FirstPersonCamera controls to be applied to the node of the camera.

	IController* pRiftController = new RiftController( m_pRiftHmd );
	m_pCamera->GetBody()->AttachController( pRiftController );

	return( true );
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.5f, 0.3f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( -1.5f, 6.0f, -7.5f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.4f, 0.0f ) );


	// Create the actor to hold the immediate geometry.  

	m_pIndexedActor = new GeometryActor();
	m_pScene->AddActor( m_pIndexedActor );
	m_pIndexedActor->GetNode()->Position() = Vector3f( 3.0, 0.0f, 0.0f );
	m_pIndexedActor->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pIndexedActor->UseTexturedMaterial( m_pRenderer11->LoadTexture( L"EyeOfHorus_128.png" ) );

	// Throw a rotation onto the actor to slowly rotate it about the Y-axis.
	RotationController* pIndexedRotController = new RotationController( Vector3f( 0.0f, 1.0f, 0.0f ), -0.1f );
	m_pIndexedActor->GetNode()->AttachController( pIndexedRotController );


	m_pGeometryActor = new GeometryActor();
	m_pScene->AddActor( m_pGeometryActor );
	m_pGeometryActor->GetNode()->Position() = Vector3f( 0.0f, 2.5f, 0.0f );

	m_pGeometryActor->SetColor( Vector4f( 1.0f, 0.0f, 0.0f, 1.0f ) );
	m_pGeometryActor->DrawSphere( Vector3f( 2.5f, 2.0f, 0.0f ), 1.5f, 16, 24 );
	m_pGeometryActor->SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 1.0f ) );
	m_pGeometryActor->DrawCylinder( Vector3f( -1.5f, -1.0f, 0.0f ), Vector3f( -1.5f, 3.0f, 0.0f ), 1.5f, 0.0f, 8, 24 );
	m_pGeometryActor->SetColor( Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ) );
	m_pGeometryActor->DrawDisc( Vector3f( 0.0f, -3.0f, 0.0f ), Vector3f( 1.0f, 1.0f, 1.0f ), 2.0f, 12 );
	m_pGeometryActor->SetColor( Vector4f( 0.0f, 0.0f, 1.0f, 1.0f ) );
	m_pGeometryActor->DrawBox( Vector3f( 0.0f, 3.0f, 0.0f ), Vector3f( 1.0f, 1.0f, 1.0f ) );

	RotationController* pGeometryRotController = new RotationController( Vector3f( 0.0f, 1.0f, 0.0f ), 0.4f );
	m_pGeometryActor->GetNode()->AttachController( pGeometryRotController );

	m_pTextActor = new TextActor();
	m_pTextActor->SetTextOrientation( Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
	m_pTextActor->SetText( std::wstring( L"Hello World! This is some \nsample text!" ) );
	
	m_pTextActor->SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 0.5f ) );
	m_pTextActor->NewLine();
	m_pTextActor->SetCharacterHeight( 2.0f );
	m_pTextActor->AppendText( L"Text is colored, scaled, and alpha blended..." );

	m_pTextActor->GetNode()->Position() = Vector3f( 0.0f, 7.0f, 0.0f );
	m_pScene->AddActor( m_pTextActor );

	// Add a single point light to the scene.
	m_pLight = new PointLight();
	m_pScene->AddLight( m_pLight );

	m_pLight->GetNode()->AttachController( new RotationController( Vector3f( 0.0f, 1.0f, 0.0f ), -1.0f ) );
	m_pLight->GetNode()->Position() = Vector3f( 0.0f, 50.0f, 0.0f );
	m_pLight->GetBody()->Position() = Vector3f( 50.0f, 0.0f, 0.0f );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Do some immediate rendering here.  These vertex data are applied in object
	// space, so we are essentially redefining our geometry in every frame.  This
	// is normally not the most efficient way, but it is significantly easier to
	// dynamically produce than it is to determine before hand what you need...
	//
	// In this case, I am animating a paraboloid shape as well as its color.
	
	const int GRIDSIZE = 20;
	const float fGRIDSIZE = static_cast<float>( GRIDSIZE );
	const float ELEMSIZE = 1.0f;
	const float fSIZESCALE = 5.0f / fGRIDSIZE;

	Vector3f v0, v1, v2;
	Vector2f uv0, uv1, uv2;
	Vector3f n0, n1, n2;

	float fScaling = 0.25f * sinf( m_pTimer->Runtime() * 0.75f );

	// Do some indexed immediate rendering by creating a grid of vertices first...

	m_pIndexedActor->ResetGeometry();

	for ( int z = 0; z < GRIDSIZE; z++ ) {
		for ( int x = 0; x < GRIDSIZE; x++ ) {
			
			float fX = static_cast<float>( x );
			float fZ = static_cast<float>( z );

			v0.x = fX - GRIDSIZE / 2;
			v0.z = fZ - GRIDSIZE / 2;
			v0.y = ( 5.0f - 0.2f * ( v0.x*v0.x + v0.z*v0.z ) ) * fScaling;
			uv0.x =        ( fX ) / ( fGRIDSIZE-1 );
			uv0.y = 1.0f - ( fZ ) / ( fGRIDSIZE-1 );

			m_pIndexedActor->AddVertex( v0 * fSIZESCALE, uv0 );
		}
	}

	// ... followed by creating the associated group of indices that reference
	// those vertices.

	for ( int z = 0; z < GRIDSIZE-1; z++ ) {
		for ( int x = 0; x < GRIDSIZE-1; x++ ) {

			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + 1 );

			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + 1 );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE + 1 );
		}
	}


	// Print a message to show the framerate and sample name.

	std::wstringstream out;

	out << L"Hieroglyph 3 : " << GetName() << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();
	
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
		Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	
	// We 'begin the frame' with the function call listed below.  This returns
	// a structure that includes timing info that is in step with the time
	// predicting features of the Rift.  Thus we use the delta time to perform
	// our scene update, and then render the scene.

	float delta = m_pRiftHmd->BeginFrame();
	//ovrFrameTiming frameTiming = ovrHmd_BeginFrame( m_pRiftHmd->GetHMD(), 0 );

	m_pScene->Update( /*frameTiming.DeltaSeconds*/ delta );
	m_pScene->Render( m_pRenderer11 );


	// The presentation to the swap chain would normally go here, but that isn't 
	// needed since the Rift does it for us as part of the distortion rendering
	// process.  We just need to 'end the frame' here.

	m_pRiftHmd->EndFrame();
	//ovrHmd_EndFrame( m_pRiftHmd->GetHMD() );
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
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
	return( std::wstring( L"OculusRiftSample" ) );
}
//--------------------------------------------------------------------------------