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
#include "Texture1dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Texture1dDX11::Texture1dDX11( ID3D11Texture1D* pTex )
{
	m_pTexture = pTex;

	ZeroMemory( &m_DesiredDesc, sizeof( D3D11_TEXTURE1D_DESC ) );
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_TEXTURE1D_DESC ) );
}
//--------------------------------------------------------------------------------
Texture1dDX11::~Texture1dDX11()
{
	SAFE_RELEASE( m_pTexture );
}
//--------------------------------------------------------------------------------
D3D11_TEXTURE1D_DESC Texture1dDX11::GetActualDescription()
{
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_TEXTURE1D_DESC ) );

	if ( m_pTexture )
		m_pTexture->GetDesc( &m_ActualDesc );

	return( m_ActualDesc );
}
//--------------------------------------------------------------------------------
D3D11_TEXTURE1D_DESC Texture1dDX11::GetDesiredDescription()
{
	return( m_DesiredDesc );
}
//--------------------------------------------------------------------------------
void Texture1dDX11::SetDesiredDescription( D3D11_TEXTURE1D_DESC description )
{
	m_DesiredDesc = description;
}
//--------------------------------------------------------------------------------
D3D11_RESOURCE_DIMENSION Texture1dDX11::GetType()
{
	D3D11_RESOURCE_DIMENSION dim;
	ZeroMemory( &dim, sizeof( D3D11_RESOURCE_DIMENSION ) );
	
	if ( m_pTexture )
		m_pTexture->GetType( &dim );

	return( dim );
}
//--------------------------------------------------------------------------------
ID3D11Resource* Texture1dDX11::GetResource()
{
	return( m_pTexture );
}
//--------------------------------------------------------------------------------
UINT Texture1dDX11::GetEvictionPriority()
{
	UINT priority = 0;

	if ( m_pTexture )
		priority = m_pTexture->GetEvictionPriority();

	return( priority );
}
//--------------------------------------------------------------------------------
void Texture1dDX11::SetEvictionPriority( UINT EvictionPriority )
{
	if ( m_pTexture )
		m_pTexture->SetEvictionPriority( EvictionPriority );
}
//--------------------------------------------------------------------------------