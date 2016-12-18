//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ImageProcessor.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ImageProcessor::ImageProcessor()
{
	m_pBruteForceGaussian = new RenderEffectDX11();
	m_pBruteForceGaussian->SetComputeShader( RendererDX11::Get()->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianBruteForceCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pSeparableGaussianX = new RenderEffectDX11();
	m_pSeparableGaussianX->SetComputeShader( RendererDX11::Get()->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianSeparableCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pSeparableGaussianY = new RenderEffectDX11();
	m_pSeparableGaussianY->SetComputeShader( RendererDX11::Get()->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianSeparableCS.hlsl" ),
		std::wstring( L"CSMAINY" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pLuminanceThreshold = new RenderEffectDX11();
	m_pLuminanceThreshold->SetComputeShader( RendererDX11::Get()->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"LuminanceThresholdCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) ) );
}
//--------------------------------------------------------------------------------
ImageProcessor::~ImageProcessor()
{
	delete m_pBruteForceGaussian;
	delete m_pSeparableGaussianX;
	delete m_pSeparableGaussianY;
	delete m_pLuminanceThreshold;
}
//--------------------------------------------------------------------------------
void ImageProcessor::GaussianBlur( PipelineManagerDX11* pPipeline, IParameterManager* pParameters, unsigned int width, unsigned int height, ResourcePtr source, ResourcePtr dest )
{
	UINT DispatchX = (UINT)( ceil( width / 32.0f ) );
	UINT DispatchY = (UINT)( ceil( height / 32.0f ) );

	pParameters->SetShaderResourceParameter( L"InputMap", source );
	pParameters->SetUnorderedAccessParameter( L"OutputMap", dest );

	pPipeline->Dispatch( *m_pBruteForceGaussian, DispatchX, DispatchY, 1, pParameters );

	pPipeline->ClearPipelineResources();
	pPipeline->ApplyPipelineResources();
}
//--------------------------------------------------------------------------------
void ImageProcessor::LuminanceThreshold( PipelineManagerDX11* pPipeline, IParameterManager* pParameters, unsigned int width, unsigned int height, ResourcePtr source, ResourcePtr dest, float threshold )
{
	UINT DispatchX = (UINT)( ceil( width / 32.0f ) );
	UINT DispatchY = (UINT)( ceil( height / 32.0f ) );

	pParameters->SetShaderResourceParameter( L"InputMap", source );
	pParameters->SetUnorderedAccessParameter( L"OutputMap", dest );

	Vector4f threshold_values( threshold, 0.0f, 0.0f, 0.0f );
	pParameters->SetVectorParameter( L"Thresholds", &threshold_values );

	pPipeline->Dispatch( *m_pLuminanceThreshold, DispatchX, DispatchY, 1, pParameters );

	pPipeline->ClearPipelineResources();
	pPipeline->ApplyPipelineResources();
}
//--------------------------------------------------------------------------------
