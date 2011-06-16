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
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewSimulation::ViewSimulation( RendererDX11& Renderer, int SizeX )
{
	bOneTimeInit = true;


	m_sParams.iViewType = VT_SIMULATION;

	// Remember what number of thread groups to run in the dispatch call.  This
	// allows the creator of this view to choose the size of buffers to create
	// instead of hard coding it.

	m_iParticleCount = SizeX;


	// Prepare some intial state data for the simulation.  Since the particle 
	// data is added into the simulation dynamically, this data actually won't
	// be used for anything.

	Particle* pData = new Particle[m_iParticleCount];

	for ( int i = 0; i < m_iParticleCount; i++ )
	{
		pData[i].position.MakeZero();
		pData[i].direction = Vector3f( 0.0f, 0.0f, 1.0f );
	}

	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem				= pData;
	InitialData.SysMemPitch			= 0;
	InitialData.SysMemSlicePitch	= 0;


	// Create the resources that will hold the state of the simulation, and initialized
	// to the state specified above.  We create two buffers with the same set of 
	// options to serve as the current and next simulation states, respectively.

	BufferConfigDX11 config;
	config.SetDefaultStructuredBuffer( m_iParticleCount, sizeof( Particle ) );
	config.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );
	config.SetMiscFlags( D3D11_RESOURCE_MISC_BUFFER_STRUCTURED );
	
	ResourcePtr buffer1 = Renderer.CreateStructuredBuffer( &config, &InitialData );
	ResourcePtr buffer2 = Renderer.CreateStructuredBuffer( &config, &InitialData );

	// Release the system memory after buffer creation, since the simulation data is 
	// now on the GPU :)

	delete[] pData;

	
	D3D11_BUFFER_UAV uav;

	// Create a UAV and SRV for the first buffer to allow use both as a source of 
	// data and a destination for updated data.

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


	// Create a UAV and SRV for the second buffer to allow use both as a source of 
	// data and a destination for updated data.

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


	// Create a buffer to be used as the constant buffer for particle counts.  This 
	// is initialized to all zeros, since the particle buffers start out empty.  The 
	// current number of particles is dynamically loaded into the buffer with the 
	// CopyStuctureCount method.

	UINT* pInitArgs = new UINT[4];

	pInitArgs[0] = 0;
	pInitArgs[1] = 0;
	pInitArgs[2] = 0;
	pInitArgs[3] = 0;

	D3D11_SUBRESOURCE_DATA InitArgsData;
	InitArgsData.pSysMem				= pInitArgs;
	InitArgsData.SysMemPitch			= 0;
	InitArgsData.SysMemSlicePitch		= 0;

	BufferConfigDX11 ArgsConfig;
	ArgsConfig.SetByteWidth( 4 * sizeof( UINT ) );
	ArgsConfig.SetBindFlags( D3D11_BIND_CONSTANT_BUFFER );
	ArgsConfig.SetMiscFlags( 0 );
	ArgsConfig.SetStructureByteStride( 0 );
	ArgsConfig.SetUsage( D3D11_USAGE_DEFAULT );
	ArgsConfig.SetCPUAccessFlags( 0 );

	ParticleCountCBBuffer = Renderer.CreateConstantBuffer( &ArgsConfig, &InitArgsData, false );

	Renderer.m_pParamMgr->SetConstantBufferParameter( L"ParticleCount", ParticleCountCBBuffer );


	// Create a buffer to be used as the inidirect arguments buffer.  This is initialized to
	// all zeros, since the particle buffers start out empty.  The current number of particles
	// is dynamically loaded into the buffer with the CopyStuctureCount method.
	//
	// The second argument is set to a 1, since we will be using the DrawInstancedIndirect() method,
	// and it indicates to use one instance of the data in the buffer.

	pInitArgs[0] = 0;
	pInitArgs[1] = 1;
	pInitArgs[2] = 0;
	pInitArgs[3] = 0;

	InitArgsData.pSysMem				= pInitArgs;
	InitArgsData.SysMemPitch			= 0;
	InitArgsData.SysMemSlicePitch		= 0;

	ArgsConfig.SetDefaultIndirectArgsBuffer( 4 * sizeof( UINT ) );
	ArgsConfig.SetUsage( D3D11_USAGE_DEFAULT );
	ParticleCountIABuffer = Renderer.CreateIndirectArgsBuffer( &ArgsConfig, &InitArgsData );


	// Create a buffer to be used as a staging buffer for readback from the GPU.

	pInitArgs[0] = 0;
	pInitArgs[1] = 0;
	pInitArgs[2] = 0;
	pInitArgs[3] = 0;

	BufferConfigDX11 StagingConfig;
	StagingConfig.SetDefaultStagingBuffer( 4 * sizeof( UINT ) );
	ParticleCountSTBuffer = Renderer.CreateIndirectArgsBuffer( &StagingConfig, &InitArgsData );


	// Set up the render effect for actually updating the simulation.

	pParticleUpdate = new RenderEffectDX11();
	pParticleUpdate->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"../Data/Shaders/ParticleSystemUpdateCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	pParticleInsertion = new RenderEffectDX11();
	pParticleInsertion->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"../Data/Shaders/ParticleSystemInsertCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	
	// Get handles to all of the rendering parameters that we will be using.

	pCurrentSimState = Renderer.m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"CurrentSimulationState" ) );
	pNextSimState = Renderer.m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"NewSimulationState" ) );
	pSimState = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"SimulationState" ) );
	pRandomVector = Renderer.m_pParamMgr->GetVectorParameterRef( std::wstring( L"RandomVector" ) );
}
//--------------------------------------------------------------------------------
ViewSimulation::~ViewSimulation()
{
	SAFE_DELETE( pParticleUpdate );
}
//--------------------------------------------------------------------------------
ResourcePtr ViewSimulation::GetParticleCountConstantBuffer()
{
	return( ParticleCountCBBuffer );
}
//--------------------------------------------------------------------------------
ResourcePtr ViewSimulation::GetParticleCountIndirectArgsBuffer()
{
	return( ParticleCountIABuffer );
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
void ViewSimulation::Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{


	//D3D11_MAPPED_SUBRESOURCE mapped;
	//unsigned int* pCount = 0;
	//unsigned int count[6];

	//pPipelineManager->CopyStructureCount( ParticleCountSTBuffer, 0, ParticleStateBuffers[0] );

	//mapped = pPipelineManager->MapResource( ParticleCountSTBuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	//pCount = (unsigned int*)(mapped.pData);
	//count[0] = pCount[0];
	//pPipelineManager->UnMapResource( ParticleCountSTBuffer->m_iResource, 0 );


	//pPipelineManager->CopyStructureCount( ParticleCountSTBuffer, 0, ParticleStateBuffers[1] );

	//mapped = pPipelineManager->MapResource( ParticleCountSTBuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	//pCount = (unsigned int*)(mapped.pData);
	//count[1] = pCount[0];
	//pPipelineManager->UnMapResource( ParticleCountSTBuffer->m_iResource, 0 );

	// Initialize the buffers...
	if ( bOneTimeInit == true )
	{
		// Set this view's render parameters.
		SetRenderParams( pParamManager );

		pPipelineManager->Dispatch( *pParticleUpdate, 1, 1, 1, pParamManager );
	}

	// Set this view's render parameters.
	SetRenderParams( pParamManager );
	

	// Add any new particles here.  For now we simply add one batch of particles every
	// frame.  If there are issues with overflowing the buffer then we would need to 
	// throttle the creation of new particles here.	
	
	pPipelineManager->Dispatch( *pParticleInsertion, 1, 1, 1, pParamManager );

	// Read out the total number of particles for updating into a constant buffer and an
	// indirect argument buffer.

	//pPipelineManager->CopyStructureCount( ParticleCountSTBuffer, 0, ParticleStateBuffers[0] );

	//mapped = pPipelineManager->MapResource( ParticleCountSTBuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	//pCount = (unsigned int*)(mapped.pData);
	//count[2] = pCount[0];
	//pPipelineManager->UnMapResource( ParticleCountSTBuffer->m_iResource, 0 );


	//pPipelineManager->CopyStructureCount( ParticleCountSTBuffer, 0, ParticleStateBuffers[1] );

	//mapped = pPipelineManager->MapResource( ParticleCountSTBuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	//pCount = (unsigned int*)(mapped.pData);
	//count[3] = pCount[0];
	//pPipelineManager->UnMapResource( ParticleCountSTBuffer->m_iResource, 0 );


	pPipelineManager->CopyStructureCount( ParticleCountIABuffer, 0, ParticleStateBuffers[0] );
	pPipelineManager->CopyStructureCount( ParticleCountCBBuffer, 0, ParticleStateBuffers[0] );	


	// Update the particles with a fixed number of threads.  The unused threads will simply
	// not do anything, as they can't read any data from the Append/Consume buffer.

	pPipelineManager->Dispatch( *pParticleUpdate, 512, 1, 1, pParamManager );



	//pPipelineManager->CopyStructureCount( ParticleCountSTBuffer, 0, ParticleStateBuffers[0] );

	//mapped = pPipelineManager->MapResource( ParticleCountSTBuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	//pCount = (unsigned int*)(mapped.pData);
	//count[4] = pCount[0];
	//pPipelineManager->UnMapResource( ParticleCountSTBuffer->m_iResource, 0 );


	//pPipelineManager->CopyStructureCount( ParticleCountSTBuffer, 0, ParticleStateBuffers[1] );

	//mapped = pPipelineManager->MapResource( ParticleCountSTBuffer->m_iResource, 0, D3D11_MAP_READ, 0 );
	//pCount = (unsigned int*)(mapped.pData);
	//count[5] = pCount[0];
	//pPipelineManager->UnMapResource( ParticleCountSTBuffer->m_iResource, 0 );





	// Switch the two resources so that the current state is maintained in slot 0.

	ResourcePtr TempState = ParticleStateBuffers[0];
	ParticleStateBuffers[0] = ParticleStateBuffers[1];
	ParticleStateBuffers[1] = TempState;
}
//--------------------------------------------------------------------------------
void ViewSimulation::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, water state '0' is always considered the current
	// state.

	if ( bOneTimeInit == true )
	{
		pParamManager->SetUnorderedAccessParameter( pCurrentSimState, ParticleStateBuffers[0], 0 );
		pParamManager->SetUnorderedAccessParameter( pNextSimState, ParticleStateBuffers[1], 0 );
		bOneTimeInit = false;
	}
	else
	{
		pParamManager->SetUnorderedAccessParameter( pCurrentSimState, ParticleStateBuffers[0] );
		pParamManager->SetUnorderedAccessParameter( pNextSimState, ParticleStateBuffers[1] );
	}

	
	// Calculate a random vector for rotating the inserted particles.  This gives 
	// a randomized initial rotation to the particles.

	static const float scale = 2.0f;
	float fRandomX = ( (float)rand() / (float)RAND_MAX * scale - scale/2.0f );
	float fRandomY = ( (float)rand() / (float)RAND_MAX * scale - scale/2.0f );
	float fRandomZ = ( (float)rand() / (float)RAND_MAX * scale - scale/2.0f );
	
	Vector3f normalized = Vector3f( fRandomX, fRandomY, fRandomZ );
	normalized.Normalize();

	Vector4f RandomVector = Vector4f( normalized.x, normalized.y, normalized.z, 0.0f );
	pParamManager->SetVectorParameter( pRandomVector, &RandomVector );
}
//--------------------------------------------------------------------------------
void ViewSimulation::SetUsageParams( IParameterManager* pParamManager )
{
	// Set the current state of the simulation for use as particle data while 
	// rendering.

	pParamManager->SetShaderResourceParameter( pSimState, ParticleStateBuffers[0] );
}
//--------------------------------------------------------------------------------
