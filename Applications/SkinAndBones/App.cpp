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

#include "IParameterManager.h"

#include "GeometryGeneratorDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
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
	// Create the light parameters for use with this effect

	Vector4f LightParams = Vector4f( 0.2f, 0.7f, 0.2f, 0.7f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &LightParams );

	Vector4f LightPosition = Vector4f( -1000.0f, 200.0f, 0.0f, 0.0f );
	m_pLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	m_pLightPosition->InitializeParameterData( &LightPosition );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.7f, 0.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 50.0f, -20.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 0.0f ) );



	// Create the displaced skinned actor

	m_pDisplacedActor = new SkinnedActor();
	GeometryPtr pGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateWeightedSkinnedCone( pGeometry, 16, 20, 2.0f, 40.0f, 6, m_pDisplacedActor );
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
	m_pDisplacedActor->GetBody()->Visual.SetMaterial( MaterialGeneratorDX11::GenerateSkinnedSolid( *m_pRenderer11 ) );

	RotationController<Node3D>* pRotController1 = new RotationController<Node3D>();
	m_pDisplacedActor->GetNode()->Controllers.Attach( pRotController1 );

	
	// Create the skinned actor without displacement

	m_pSkinnedActor = new SkinnedActor();
	GeometryPtr pSkinnedGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateWeightedSkinnedCone( pSkinnedGeometry, 16, 20, 2.0f, 40.0f, 6, m_pSkinnedActor );
	pSkinnedGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pSkinnedActor->GetBody()->Visual.SetMaterial( MaterialGeneratorDX11::GenerateSkinnedTextured( *m_pRenderer11 ) );

	RotationController<Node3D>* pRotController2 = new RotationController<Node3D>();
	m_pSkinnedActor->GetNode()->Controllers.Attach( pRotController2 );

	
	// Generate the static mesh, and attach a texture to its entity

	m_pStaticActor = new Actor();
	GeometryPtr pStaticGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"box.ms3d" ) );
	pStaticGeometry->LoadToBuffers();
	MaterialPtr pStaticMaterial = MaterialGeneratorDX11::GenerateStaticTextured( *m_pRenderer11 );
	m_pStaticActor->GetBody()->Visual.SetGeometry( pStaticGeometry );
	m_pStaticActor->GetBody()->Visual.SetMaterial( pStaticMaterial );

	RotationController<Entity3D>* pRotController3 = new RotationController<Entity3D>();
	m_pStaticActor->GetBody()->Controllers.Attach( pRotController3);

	
	ResourcePtr ColorTexture = RendererDX11::Get()->LoadTexture( L"Tiles.png" );
	ShaderResourceParameterWriterDX11* pTextureParameter = new ShaderResourceParameterWriterDX11();
	m_pStaticActor->GetBody()->Parameters.SetShaderResourceParameter( L"ColorTexture", ColorTexture );
	

	// Attach the actors to the scene, so that they will be rendered all together.

	m_pScene->AddActor( m_pDisplacedActor );
	m_pScene->AddActor( m_pSkinnedActor );
	m_pScene->AddActor( m_pStaticActor );


	// Setup the skinned actors' bind poses and start their animations.

	m_pDisplacedActor->SetBindPose();
	m_pDisplacedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pDisplacedActor->PlayAllAnimations();

	m_pSkinnedActor->SetBindPose();
	m_pSkinnedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pSkinnedActor->PlayAllAnimations();

	m_pStaticActor->GetBody()->Transform.Position() = Vector3f( -20.0f, 10.0f, 15.0f );
	m_pSkinnedActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 0.0f, 20.0f );
	m_pDisplacedActor->GetNode()->Transform.Position() = Vector3f( 20.0f, 0.0f, 20.0f );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();


	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	std::wstringstream out;
	out << L"Hieroglyph 3 : Skin and Bones" << std::endl;
	out << L"The static mesh represents rigid geometry, while skinned meshes can be animated." << std::endl;
	out << L"Each node in the skinned meshes displays its coordinate axes." << std::endl;
	out << L"To start the animations press 'A'" << std::endl;

	m_pTextOverlayView->WriteText( out.str(), Matrix4f::Identity(), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );

	m_pDisplacedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pSkinnedActor->SetSkinningMatrices( *m_pRenderer11 );

	m_pScene->Render( m_pRenderer11 );


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

		if ( 'A' == key ) // 'A' Key - restart animations
		{
			m_pDisplacedActor->PlayAllAnimations();
			m_pSkinnedActor->PlayAllAnimations();
			return( true );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"SkinAndBones" ) );
}
//--------------------------------------------------------------------------------