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
// Light
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef Light_h
#define Light_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "ParameterContainer.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Light : public Actor
	{
	public:
		Light();
		virtual ~Light();

		void SetDiffuse( const Vector4f& color );
		void SetAmbient( const Vector4f& color );
		void SetSpecular( const Vector4f& color );

		Vector4f GetDiffuse( ) const;
		Vector4f GetAmbient( ) const;
		Vector4f GetSpecular( ) const;

		ParameterContainer Parameters;

	protected:
		VectorParameterWriterDX11* m_DiffuseWriter;
		VectorParameterWriterDX11* m_AmbientWriter;
		VectorParameterWriterDX11* m_SpecularWriter;
		VectorParameterWriterDX11* m_PositionWriter;
	};
};
//--------------------------------------------------------------------------------
#endif // Light_h
//--------------------------------------------------------------------------------
