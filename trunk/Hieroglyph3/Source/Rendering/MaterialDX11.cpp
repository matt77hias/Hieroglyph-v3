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
#include "MaterialDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MaterialDX11::MaterialDX11()
{
	for ( int i = 0; i < VT_NUM_VIEW_TYPES; i++ )
	{
		Params[i].bRender = false;
		Params[i].pEffect = 0;

		// Loop until no views remain in the current param set
		while ( Params[i].vViews.count() > 0 )
		{
			// Get the view and delete it
			IRenderView* pView = Params[i].vViews[0];
			delete pView;

			// Remove any additional instances of the view in other sets of params
			for ( int j = i; j < VT_NUM_VIEW_TYPES; j++ )
			{
				Params[j].vViews.remove( Params[j].vViews.find( pView ) );
			}
		}
	}
	
	m_pEntity = 0;
}
//--------------------------------------------------------------------------------
MaterialDX11::~MaterialDX11()
{
	// Delete the effects that have been added to this material

	for ( int i = 0; i < VT_NUM_VIEW_TYPES; i++ )
		SAFE_DELETE( Params[i].pEffect );

	// Delete the parameters that have been added to this material.

	for ( int i = 0; i < m_RenderParameters.count(); i++ )
		SAFE_DELETE( m_RenderParameters[i] );
}
//--------------------------------------------------------------------------------
void MaterialDX11::PreRender( RendererDX11& Renderer, VIEWTYPE type )
{
	// Update the render views that are needed before a view of 'type' can be
	// processed.  We will always try to update the view, but it may limit if it
	// is updated based on the maximum recurrence allowed. 

	for ( int i = 0; i < Params[type].vViews.count(); i++ )
		Params[type].vViews[i]->Draw( Renderer );
}
//--------------------------------------------------------------------------------
void MaterialDX11::AddRenderParameter( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{

		// Search the list to see if this parameter is already there
		RenderParameterDX11* pCurr = 0;

		for ( int i = 0; i < m_RenderParameters.count(); i++ )
		{
			if ( pParameter->GetName() == m_RenderParameters[i]->GetName() )
			{
				pCurr = m_RenderParameters[i];
				break;
			}
		}

		if ( !pCurr )
		{
			// Add the parameter to the list.  We make a copy since we don't know
			// who created the input object and can't take ownership of it.  It 
			// may even be created on the stack, leading to disaster at the first
			// use if we just took a copy of the pointer...
			RenderParameterDX11* pCopy = pParameter->CreateCopy();
			m_RenderParameters.add( pCopy );
		}
		else
		{
			pCurr->UpdateValue( pParameter ); 
		}
	}
}
//--------------------------------------------------------------------------------
void MaterialDX11::SetRenderParams( RendererDX11& Renderer, VIEWTYPE type )
{
	if ( m_pEntity )
	{
		// TODO:
		// Set the object specific render parameters here, like object position,
		// orientation, etc...
	}

	// Set any special render view parameters that may be needed for the effect
	// to use the results of the render view in its rendering pass.

	for ( int i = 0; i < Params[type].vViews.count(); i++ )
		Params[type].vViews[i]->SetUsageParams( Renderer );

	// Set the additional render parameters that have been added to the material.

	for ( int i = 0; i < m_RenderParameters.count(); i++ )
		Renderer.SetParameter( m_RenderParameters[i] );
}
//--------------------------------------------------------------------------------
void MaterialDX11::SetEntity( Entity3D* pEntity )
{
	m_pEntity = pEntity;

	for ( int i = 0; i < VT_NUM_VIEW_TYPES; i++ )
	{
		for ( int j = 0; j < Params[i].vViews.count(); j++ )
		{
			Params[i].vViews[j]->SetEntity( pEntity );
		}
	}
}
//--------------------------------------------------------------------------------