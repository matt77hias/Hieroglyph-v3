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
// D3DEnumConversion
//
// This module includes free functions for converting D3D enumeration values into
// strings of human readable text.
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
#ifndef D3DEnumConversion_h
#define D3DEnumConversion_h
//--------------------------------------------------------------------------------
namespace Glyph3
{

	// These functions are used to convert the enumerated values to human readable text.
	// This is convenient for logging or displaying various states.

	std::wstring TO_STRING_D3D_SHADER_VARIABLE_CLASS( D3D_SHADER_VARIABLE_CLASS varclass );
	std::wstring TO_STRING_D3D_SHADER_VARIABLE_FLAGS( D3D_SHADER_VARIABLE_FLAGS varflags );
	std::wstring TO_STRING_D3D_SHADER_VARIABLE_TYPE( D3D_SHADER_VARIABLE_TYPE vartype );
	std::wstring TO_STRING_D3D_SHADER_INPUT_FLAGS( D3D_SHADER_INPUT_FLAGS inputflags );
	std::wstring TO_STRING_D3D_SHADER_INPUT_TYPE( D3D_SHADER_INPUT_TYPE type );
	std::wstring TO_STRING_D3D_SHADER_CBUFFER_FLAGS( D3D_SHADER_CBUFFER_FLAGS flags );
	std::wstring TO_STRING_D3D_CBUFFER_TYPE( D3D_CBUFFER_TYPE type );
	std::wstring TO_STRING_D3D_NAME( D3D_NAME name );
	std::wstring TO_STRING_D3D_RESOURCE_RETURN_TYPE( D3D_RESOURCE_RETURN_TYPE type );
	std::wstring TO_STRING_D3D_REGISTER_COMPONENT_TYPE( D3D_REGISTER_COMPONENT_TYPE type );
	std::wstring TO_STRING_D3D_TESSELLATOR_DOMAIN( D3D_TESSELLATOR_DOMAIN domain );
	std::wstring TO_STRING_D3D_TESSELLATOR_PARTITIONING( D3D_TESSELLATOR_PARTITIONING partitioning );
	std::wstring TO_STRING_D3D_TESSELLATOR_OUTPUT_PRIMITIVE( D3D_TESSELLATOR_OUTPUT_PRIMITIVE primitive );
	std::wstring TO_STRING_D3D_PRIMITIVE_TOPOLOGY( D3D_PRIMITIVE_TOPOLOGY topology );
	std::wstring TO_STRING_D3D_PRIMITIVE( D3D_PRIMITIVE topology );

	std::wstring TO_STRING_D3D11_SHADER_DESC( D3D11_SHADER_DESC description );
};
//--------------------------------------------------------------------------------
#endif // IRenderer_h
//--------------------------------------------------------------------------------

