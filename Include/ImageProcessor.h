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
// ImageProcessor
//
//--------------------------------------------------------------------------------
#ifndef ImageProcessor_h
#define ImageProcessor_h
//--------------------------------------------------------------------------------
#include "RenderEffectDX11.h"
#include "RenderParameterDX11.h"
#include "ParameterContainer.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ImageProcessor
	{
	public:
		ImageProcessor();
		~ImageProcessor();

		void GaussianBlur( 
			PipelineManagerDX11* pPipeline, 
			IParameterManager* pParameters,
			unsigned int width,
			unsigned int height,
			ResourcePtr source,
			ResourcePtr dest );

		void LuminanceThreshold(
			PipelineManagerDX11* pPipeline, 
			IParameterManager* pParameters,
			unsigned int width,
			unsigned int height,
			ResourcePtr source,
			ResourcePtr dest,
			float threshold );

		// TODO: Add other operations, like addition, subtraction, etc...


		RenderEffectDX11*		m_pBruteForceGaussian;
		RenderEffectDX11*		m_pSeparableGaussianX;
		RenderEffectDX11*		m_pSeparableGaussianY;
		RenderEffectDX11*		m_pLuminanceThreshold;
	};
};
//--------------------------------------------------------------------------------
#endif // ImageProcessor_h
//--------------------------------------------------------------------------------

