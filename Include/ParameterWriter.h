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
// ParameterWriter
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
#ifndef ParameterWriter_h
#define ParameterWriter_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ParameterWriter
	{
	public:
		ParameterWriter();
		virtual ~ParameterWriter();

		virtual RenderParameterDX11* GetRenderParameterRef() = 0;

		// Writing a parameter directly sets the value of the referenced parameter,
		// using the provided parameter manager.  This will only be set for that
		// manager, allowing the same parameter object to be used in multiple
		// parameter managers simultaneously (allowing multi-threaded processing).
		
		virtual void WriteParameter( IParameterManager* pParamMgr ) = 0;

		// Initializing a parameter will set this writer's value into all of the
		// slots within the parameter.  Thus this should only be done at a high
		// processing level - i.e. at the scene or application level, since it 
		// will affect the value of the parameter in other parameter managers.

		virtual void InitializeParameter( ) = 0;

	};
};
//--------------------------------------------------------------------------------
#endif // ParameterWriter_h
//--------------------------------------------------------------------------------

