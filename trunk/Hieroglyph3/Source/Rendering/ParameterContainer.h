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
// ParameterContainer
//
//--------------------------------------------------------------------------------
#ifndef ParameterContainer_h
#define ParameterContainer_h
//--------------------------------------------------------------------------------
#include "TArray.h"
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class ParameterContainer
	{
	public:
		ParameterContainer( );
		virtual ~ParameterContainer( );

		void AddRenderParameter( ParameterWriter* pWriter );
		//void UpdateRenderParameter( RenderParameterDX11* pParameter );
		void SetRenderParams( IParameterManager* pParamManager );
		
	protected:

		TArray< ParameterWriter* > m_RenderParameters;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterContainer_h
