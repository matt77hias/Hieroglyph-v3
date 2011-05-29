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
// EntityParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Entity3D.h"
//--------------------------------------------------------------------------------
#ifndef EntityParameterDX11_h
#define EntityParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EntityParameterDX11 : public RenderParameterDX11
	{
	public:
		EntityParameterDX11();
		EntityParameterDX11( EntityParameterDX11& copy );
		virtual ~EntityParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		virtual ParameterType GetParameterType();
		Entity3D* GetEntity();

		void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		Entity3D*	m_pEntity;
	};
};
//--------------------------------------------------------------------------------
#endif // EntityParameterDX11_h
//--------------------------------------------------------------------------------

