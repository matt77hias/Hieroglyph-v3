//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "ParticleStormGlyphlet.h"
#include "ViewPerspective.h"
#include "FirstPersonCamera.h"
#include "Texture2dDX11.h"
#include "EvtFrameStart.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ParticleStormGlyphlet::ParticleStormGlyphlet()
{
}
//--------------------------------------------------------------------------------
ParticleStormGlyphlet::~ParticleStormGlyphlet()
{
}
//--------------------------------------------------------------------------------
void ParticleStormGlyphlet::Initialize()
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

	RendererDX11* pRenderer = RendererDX11::Get();
	m_pTimeFactors = pRenderer->m_pParamMgr->GetVectorParameterRef( std::wstring( L"TimeFactors" ) );
	m_pEmitterLocation = pRenderer->m_pParamMgr->GetVectorParameterRef( std::wstring( L"EmitterLocation" ) );
	m_pConsumerLocation = pRenderer->m_pParamMgr->GetVectorParameterRef( std::wstring( L"ConsumerLocation" ) );
}
//--------------------------------------------------------------------------------
void ParticleStormGlyphlet::Update( float dt )
{
	// Create a series of time factors for use in the simulation.  The factors 
	// are as follows:
	// x: Time in seconds since the last frame.
	// y: Current framerate, which is updated once per second.
	// z: Time in seconds since application startup.
	// w: Current frame number since application startup.

	Vector4f TimeFactors = Vector4f( dt, 0.0f, 0.0f, 0.0f );
	Vector4f EmitterLocation = Vector4f( -50.0f, 10.0f, 0.0f, 0.0f );
	Vector4f ConsumerLocation = Vector4f( 50.0f, 0.0f, 0.0f, 0.0f );

	m_pTimeFactors->InitializeParameterData( &TimeFactors );
	m_pEmitterLocation->InitializeParameterData( &EmitterLocation );	
	m_pConsumerLocation->InitializeParameterData( &ConsumerLocation );

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( dt ) ) );


	// After the application specific update, call the base class update method
	// to perform the rendering.

	SingleWindowGlyphlet::Update( dt );
}
//--------------------------------------------------------------------------------
void ParticleStormGlyphlet::Shutdown()
{
	// Clean up your scene here...  
}
//--------------------------------------------------------------------------------
bool ParticleStormGlyphlet::HandleEvent( EventPtr pEvent )
{
	// Handle events for your scene here...  No specific events are handled by 
	// this glyphlet, meaning that we can just return false (which means we didn't
	// handle pEvent).

	return( false );
}
//--------------------------------------------------------------------------------
std::wstring ParticleStormGlyphlet::GetName()
{
	return( L"ParticleStormGlyphlet" );
}
//--------------------------------------------------------------------------------