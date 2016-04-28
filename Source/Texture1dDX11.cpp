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
#include "Texture1dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Texture1dDX11::Texture1dDX11( Microsoft::WRL::ComPtr<ID3D11Texture1D> pTex )
{
	m_pTexture = pTex;

	ZeroMemory( &m_DesiredDesc, sizeof( D3D11_TEXTURE1D_DESC ) );
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_TEXTURE1D_DESC ) );
}
//--------------------------------------------------------------------------------
Texture1dDX11::~Texture1dDX11()
{
}
//--------------------------------------------------------------------------------
ResourceType Texture1dDX11::GetType()
{
	return( RT_TEXTURE1D );
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
ID3D11Resource* Texture1dDX11::GetResource()
{
	return( m_pTexture.Get() );
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