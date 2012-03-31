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

		// Adding a parameter writer to this container.
		void AddRenderParameter( ParameterWriter* pWriter );

		// Retrieve a parameter writer that was added to this container earlier.
		ParameterWriter* GetRenderParameter( const std::wstring& name );

		// Apply the parameters in this container to a parameter manager.
		void SetRenderParams( IParameterManager* pParamManager );
		
	protected:

		TArray< ParameterWriter* > m_RenderParameters;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterContainer_h
