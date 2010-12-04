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
#include "ViewSimulation.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
#include "ParameterManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewSimulation::ViewSimulation( RendererDX11& Renderer, int SizeX )
{
	bOneTimeInit = true;

	m_sParams.iViewType = VT_SIMULATION;

	// Remember what number of thread groups to run in the dispatch call.

	m_iParticleCount = SizeX;

	// Prepare some intial state data for the simulation.

	Particle* pData = new Particle[m_iParticleCount];

	srand( 1 );

	const float scale = 100.0f;
	const float vel_scale = 10.0f;
	for ( int i = 0; i < m_iParticleCount; i++ )
	{
		float fRandomX = ( (float)rand() / (float)RAND_MAX * scale - scale/2.0f );
		float fRandomY = ( (float)rand() / (float)RAND_MAX * scale - scale/2.0f );
		float fRandomZ = ( (float)rand() / (float)RAND_MAX * scale - scale/2.0f );
		pData[i].position = Vector3f( fRandomX, fRandomY, fRandomZ );

		float fRandomRX = ( (float)rand() / (float)RAND_MAX * vel_scale - vel_scale/2.0f );
		float fRandomRY = ( (float)rand() / (float)RAND_MAX * vel_scale - vel_scale/2.0f );
		float fRandomRZ = ( (float)rand() / (float)RAND_MAX * vel_scale - vel_scale/2.0f );

		pData[i].direction = Vector3f( fRandomRX, fRandomRY, fRandomRZ );
		//pData[i].direction.Normalize();

		pData[i].status.MakeZero();
	}

	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem				= pData;
	InitialData.SysMemPitch			= 0;
	InitialData.SysMemSlicePitch	= 0;

	// Create the resources that will hold the state of the simulation, and initialized
	// to the state specified above.

	BufferConfigDX11 config;
	config.SetDefaultStructuredBuffer( m_iParticleCount, sizeof( Particle ) );
	config.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );
	config.SetMiscFlags( D3D11_RESOURCE_MISC_BUFFER_STRUCTURED );
	

	ResourcePtr buffer1 = Renderer.CreateStructuredBuffer( &config, &InitialData );
	ResourcePtr buffer2 = Renderer.CreateStructuredBuffer( &config, &InitialData );

	// Release the system memory since the simulation data is now on the GPU :)

	delete[] pData;


	
	D3D11_BUFFER_UAV uav;

	// Create a UAV for the first half of the buffer
	uav.FirstElement = 0;
	uav.NumElements = m_iParticleCount;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	UnorderedAccessViewConfigDX11 firstHalfUAV;
	firstHalfUAV.SetViewDimensions( D3D11_UAV_DIMENSION_BUFFER );
	firstHalfUAV.SetBuffer( uav );

	D3D11_BUFFER_SRV srv;
	srv.FirstElement = 0;
	srv.NumElements = m_iParticleCount;
	
	ShaderResourceViewConfigDX11 firstHalfSRV;
	firstHalfSRV.SetViewDimensions( D3D11_SRV_DIMENSION_BUFFER );
	firstHalfSRV.SetBuffer( srv );

	ParticleStateBuffers[0] = ResourcePtr( new ResourceProxyDX11( buffer1->m_iResource, 
																&config,
																&Renderer,
                                                                &firstHalfSRV, 
																NULL,
                                                                &firstHalfUAV ) );


	// Create a UAV for the second half of the buffer
	uav.FirstElement = 0;
	uav.NumElements = m_iParticleCount;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	UnorderedAccessViewConfigDX11 secondHalfUAV;
	secondHalfUAV.SetViewDimensions( D3D11_UAV_DIMENSION_BUFFER );
	secondHalfUAV.SetBuffer( uav );

	srv.FirstElement = 0;
	srv.NumElements = m_iParticleCount;
	
	ShaderResourceViewConfigDX11 secondHalfSRV;
	secondHalfSRV.SetViewDimensions( D3D11_SRV_DIMENSION_BUFFER );
	secondHalfSRV.SetBuffer( srv );

	ParticleStateBuffers[1] = ResourcePtr( new ResourceProxyDX11( buffer2->m_iResource, 
																&config,
																&Renderer,
																&secondHalfSRV,
																NULL,
                                                                &secondHalfUAV ) );

	

	// Set up the render effect for actually updating the simulation.

	pParticleUpdate = new RenderEffectDX11();
	pParticleUpdate->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"../Data/Shaders/ParticleSystemUpdateCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );
}
//--------------------------------------------------------------------------------
ViewSimulation::~ViewSimulation()
{
	SAFE_DELETE( pParticleUpdate );
}
//--------------------------------------------------------------------------------
void ViewSimulation::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewSimulation::PreDraw( RendererDX11* pRenderer )
{
	// Queue this view into the renderer for processing.  Since this is a 
	// simulation style view, there is no root and hence no additional recursive
	// 'PreDraw'ing required.
	pRenderer->QueueRenderView( this );
}
//--------------------------------------------------------------------------------
void ViewSimulation::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
{
	// Set this view's render parameters.
	SetRenderParams( pParamManager );

	// Perform the dispatch call to update the simulation state.
	pPipelineManager->Dispatch( *pParticleUpdate, 1, 1, 1, pParamManager );

	// Switch the two resources so that the current state is maintained in slot 0.
	ResourcePtr TempState = ParticleStateBuffers[0];
	ParticleStateBuffers[0] = ParticleStateBuffers[1];
	ParticleStateBuffers[1] = TempState;
}
//--------------------------------------------------------------------------------
void ViewSimulation::SetRenderParams( ParameterManagerDX11* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, water state '0' is always considered the current
	// state.

	if ( bOneTimeInit == true )
	{
		//pParamManager->SetUnorderedAccessParameter( L"CurrentSimulationState", ParticleStateBuffers[0], m_iParticleCount / 2 );
		pParamManager->SetUnorderedAccessParameter( L"CurrentSimulationState", ParticleStateBuffers[0], m_iParticleCount );
		pParamManager->SetUnorderedAccessParameter( L"NewSimulationState", ParticleStateBuffers[1], 0 );
		bOneTimeInit = false;
	}
	else
	{
		pParamManager->SetUnorderedAccessParameter( L"CurrentSimulationState", ParticleStateBuffers[0] );
		pParamManager->SetUnorderedAccessParameter( L"NewSimulationState", ParticleStateBuffers[1] );
	}

	Vector4f DispatchSize = Vector4f( 16.0f, 16.0f, 16.0f * 16.0f, 16.0f * 16.0f );
	pParamManager->SetVectorParameter( std::wstring( L"DispatchSize" ), &DispatchSize );
}
//--------------------------------------------------------------------------------
void ViewSimulation::SetUsageParams( ParameterManagerDX11* pParamManager )
{
	// Set the parameters for allowing an application to use the current state
	// as a height map via a shader resource view.

	Vector4f DispatchSize = Vector4f( 16.0f, 16.0f, 16.0f * 16.0f, 16.0f * 16.0f );
	pParamManager->SetVectorParameter( std::wstring( L"DispatchSize" ), &DispatchSize );

	pParamManager->SetShaderResourceParameter( L"SimulationState", ParticleStateBuffers[0] );
}
//--------------------------------------------------------------------------------
