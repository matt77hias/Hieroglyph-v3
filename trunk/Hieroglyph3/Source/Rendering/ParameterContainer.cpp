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
#include "PCH.h"
#include "ParameterContainer.h"
#include "Log.h"
#include "RenderParameterDX11.h"
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ParameterContainer::ParameterContainer()
{
}
//--------------------------------------------------------------------------------
ParameterContainer::~ParameterContainer()
{
	for ( int i = 0; i < m_RenderParameters.count(); i++ )
		delete m_RenderParameters[i];
}
//--------------------------------------------------------------------------------
void ParameterContainer::AddRenderParameter( ParameterWriter* pWriter )
{
	// Add render parameter will take the pointer passed in and add it to the
	// entity's internal list.  Therefore, this must not be from the stack!

	if ( pWriter )
	{
		// Search the list to see if this parameter is already there
		ParameterWriter* pCurr = 0;

		for ( int i = 0; i < m_RenderParameters.count(); i++ )
		{
			if ( pWriter->GetRenderParameterRef()->GetName() == m_RenderParameters[i]->GetRenderParameterRef()->GetName() )
			{
				pCurr = m_RenderParameters[i];
				break;
			}
		}

		if ( !pCurr )
		{
			m_RenderParameters.add( pWriter );
		}
		else
		{
			Log::Get().Write( L"Tried to add a parameter to an entity that was already there!" );
		}
	}
}
//--------------------------------------------------------------------------------
ParameterWriter* ParameterContainer::GetRenderParameter( const std::wstring& name )
{
	ParameterWriter* pResult = NULL;

	for ( int i = 0; i < m_RenderParameters.count(); i++ ) {
		ParameterWriter* pParameterWriter = m_RenderParameters[i];
		
		RenderParameterDX11* pParameter = pParameterWriter->GetRenderParameterRef();
		if ( pParameter != NULL ) {
			if ( name.compare( pParameter->GetName() ) == 0 ) {
				pResult = pParameterWriter;
				break;
			}
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
void ParameterContainer::SetRenderParams( IParameterManager* pParamManager )
{
	// Scroll through each parameter and set it in the renderer
	for ( int i = 0; i < m_RenderParameters.count(); i++ )
		m_RenderParameters[i]->WriteParameter( pParamManager );
}
//--------------------------------------------------------------------------------