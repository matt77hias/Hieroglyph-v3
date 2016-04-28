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

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "ViewSimulation.h"

#include "IParameterManager.h"

#include "ShaderResourceParameterWriterDX11.h"

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

	//m_pRenderer11->SetMultiThreadingState( false );

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

	m_pCamera->Spatial().SetRotation( Vector3f( 0.307f, 0.707f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( -100.0f, 60.5f, -100.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pActor = new ParticleSystemActor();

	m_pScene->AddActor( m_pActor );


	m_pTimeFactors = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"TimeFactors" ) );
	m_pEmitterLocation = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"EmitterLocation" ) );
	m_pConsumerLocation = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"ConsumerLocation" ) );
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

	Vector4f TimeFactors = Vector4f( m_pTimer->Elapsed(), (float)m_pTimer->Framerate(), 
		m_pTimer->Runtime(), (float)m_pTimer->FrameCount() );
	Vector4f EmitterLocation = Vector4f( -50.0f, 10.0f, 0.0f, 0.0f );
	Vector4f ConsumerLocation = Vector4f( 50.0f, 0.0f, 0.0f, 0.0f );

	m_pTimeFactors->InitializeParameterData( &TimeFactors );
	m_pEmitterLocation->InitializeParameterData( &EmitterLocation );	
	m_pConsumerLocation->InitializeParameterData( &ConsumerLocation );

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );

	// The following code can be used to read out the structure count data from
	// the staging buffer of the ViewSimulation class.  This requires the copy
	// methods to be un-commented in the classes draw method.
	//
	/*
	D3D11_MAPPED_SUBRESOURCE mapped;
	unsigned int* pCount = 0;
	unsigned int count[8];

	ResourcePtr stagingbuffer = ((ParticleSystemEntity*)m_pEntity)->GetSimulationView()->GetParticleCountStagingBuffer();
	mapped = m_pRenderer11->pImmPipeline->MapResource( stagingbuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	pCount = (unsigned int*)(mapped.pData);

	std::wstringstream debugout;
	debugout << L"Values: ";

	for ( int i = 0; i < 8; i++ )
	{
		count[i] = pCount[i];
		debugout << count[i] << L", ";
	}
	m_pRenderer11->pImmPipeline->UnMapResource( stagingbuffer->m_iResource, 0 );

	Log::Get().Write( debugout.str() );
	*/

	std::wstringstream out;
	out << L"Hieroglyph 3 : " << GetName() << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
		Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );


	// Update the scene, and then render all cameras within the scene.

	//m_pRenderer11->pImmPipeline->StartPipelineStatistics();

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );

	//m_pRenderer11->pImmPipeline->EndPipelineStatistics();
	//Log::Get().Write( m_pRenderer11->pImmPipeline->PrintPipelineStatistics() );



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
	return( std::wstring( L"ParticleStorm" ) );
}
//--------------------------------------------------------------------------------