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

#include "ViewSimulation.h"
#include "BufferConfigDX11.h"

#include "IParameterManager.h"
#include "Quaternion.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	Quaternion<float> q;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 800, 600, D3D_FEATURE_LEVEL_10_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	return( true );
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Create and initialize the geometry to be rendered.  This represents a 
	// heightmap that will be displaced with the water state at each vertex.


	const int DispatchSizeX = 16;
	const int DispatchSizeZ = 16;

	GeometryPtr pGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateTexturedPlane( pGeometry, 16 * DispatchSizeX, 16 * DispatchSizeZ );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	

	// Create the material for use by the heightmap.
	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"HeightmapVisualization.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"HeightmapVisualization.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;

	pEffect->m_iRasterizerState = 
		m_pRenderer11->CreateRasterizerState( &RS );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
	pMaterial->Params[VT_PERSPECTIVE].Tasks.push_back( new ViewSimulation( *m_pRenderer11, DispatchSizeX, DispatchSizeZ ) );

	// Initialize parameter values that will be needed by the shaders used above.

	Vector4f DispatchSize = Vector4f( (float)DispatchSizeX, (float)DispatchSizeZ, (float)DispatchSizeX * 16, (float)DispatchSizeZ * 16 );
	m_pDispatchSize = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"DispatchSize" ) );
	m_pDispatchSize->InitializeParameterData( &DispatchSize );

	Vector4f FinalColor = Vector4f( 0.5f, 1.0f, 0.5f, 1.0f );
	m_pFinalColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"FinalColor" ) );
	m_pFinalColor->InitializeParameterData( &m_pFinalColor );

	// The camera is already created, we just need to specify where it is.  We also set
	// the back color of the render view.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.307f, 0.707f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( -100.0f, 30.5f, -100.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );

	// Create the desired scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated automatically via the scene interface instead of 
	// manually manipulating it.

	m_pActor = new Actor();
	m_pActor->GetBody()->Visual.SetGeometry( pGeometry );
	m_pActor->GetBody()->Visual.SetMaterial( pMaterial );
	m_pActor->GetBody()->Transform.Position() = Vector3f( -8.0f * DispatchSizeX, 0.0f, -8.0f * DispatchSizeZ );  

	m_pScene->AddActor( m_pActor );
	

	// Get a handle to the render parameters that the application will be setting every
	// frame.  This allows for fast parameter setting.

	m_pTimeFactors = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"TimeFactors" ) );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Create a series of time factors for use in the simulation.  The factors 
	// are as follows:
	// x: Time in seconds since the last frame.
	// y: Current framerate, which is updated once per second.
	// z: Time in seconds since application startup.
	// w: Current frame number since application startup.

	Vector4f TimeFactors = Vector4f( m_pTimer->Elapsed()*2.0f, (float)m_pTimer->Framerate(), 
		m_pTimer->Runtime(), (float)m_pTimer->FrameCount() );

	m_pTimeFactors->InitializeParameterData( &TimeFactors );


	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Manipulate the scene here - simply rotate the root of the scene proportionally to
	// the amount of time that has passed between frames.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() * 0.2f );
	m_pActor->GetNode()->Transform.Rotation() *= rotation;


	// Update the scene, and then render all cameras within the scene.

	// Render the onscreen text.
	std::wstringstream out;
	out << L"Hieroglyph 3 : Water Simulation\nFPS: " << m_pTimer->Framerate();
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 10.0f, 10.0f, 0.0f ), Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ) );


	//m_pRenderer11->StartPipelineStatistics();

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );

	//m_pRenderer11->EndPipelineStatistics();
	//Log::Get().Write( m_pRenderer11->PrintPipelineStatistics() );



	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

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
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
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
	return( std::wstring( L"WaterSimulation" ) );
}
//--------------------------------------------------------------------------------