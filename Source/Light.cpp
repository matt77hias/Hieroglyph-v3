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
#include "Light.h"
#include "Scene.h"
#include "ViewPortDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Light::Light() :
	Parameters( ),
	m_DiffuseWriter( nullptr ),
	m_AmbientWriter( nullptr ),
	m_SpecularWriter( nullptr )
{
	// By initializing these parameters, we gain a handle to the underlying 
	// parameter writer.  We can hang on to the pointer for referencing later on
	// when someone wants to change the color of the light or whatever.

	m_DiffuseWriter = Parameters.SetVectorParameter( L"Id", Vector4f( 0.5f, 0.5f, 0.5f, 1.0f ) );
	m_AmbientWriter = Parameters.SetVectorParameter( L"Ia", Vector4f( 0.25f, 0.25f, 0.25f, 0.25f ) );
	m_SpecularWriter = Parameters.SetVectorParameter( L"Is", Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

}
//--------------------------------------------------------------------------------
Light::~Light()
{
}
//--------------------------------------------------------------------------------
void Light::SetDiffuse( const Vector4f& color )
{
	m_DiffuseWriter->SetValue( color );
}
//--------------------------------------------------------------------------------
void Light::SetAmbient( const Vector4f& color )
{
	m_AmbientWriter->SetValue( color );
}
//--------------------------------------------------------------------------------
void Light::SetSpecular( const Vector4f& color )
{
	m_SpecularWriter->SetValue( color );
}
//--------------------------------------------------------------------------------
Vector4f Light::GetDiffuse( ) const
{
	return( m_DiffuseWriter->GetValue() );
}
//--------------------------------------------------------------------------------
Vector4f Light::GetAmbient( ) const
{
	return( m_AmbientWriter->GetValue() );
}
//--------------------------------------------------------------------------------
Vector4f Light::GetSpecular( ) const
{
	return( m_SpecularWriter->GetValue() );
}
//--------------------------------------------------------------------------------
