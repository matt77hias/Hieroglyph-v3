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
#include "D3DEnumConversion.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_SHADER_VARIABLE_CLASS( D3D_SHADER_VARIABLE_CLASS varclass )
{
	std::wstringstream s;

	switch( varclass )
	{
	case D3D_SVC_SCALAR:
		s << L"SCALAR"; break;
	case D3D_SVC_VECTOR:
		s << L"VECTOR"; break;
	case D3D_SVC_MATRIX_ROWS:
		s << L"MATRIX_ROWS"; break;
	case D3D_SVC_MATRIX_COLUMNS:
		s << L"MATRIX_COLUMNS"; break;
	case D3D_SVC_OBJECT:
		s << L"OBJECT"; break;
	case D3D_SVC_STRUCT:
		s << L"STRUCT"; break;
	case D3D_SVC_INTERFACE_CLASS:
		s << L"INTERFACE_CLASS"; break;
	case D3D_SVC_INTERFACE_POINTER:
		s << L"INTERFACE_POINTER"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_SHADER_VARIABLE_FLAGS( D3D_SHADER_VARIABLE_FLAGS varflags )
{
	std::wstringstream s;

	switch( varflags )
	{
	case D3D_SVF_USERPACKED:
		s << L"USERPACKED"; break;
	case D3D_SVF_USED:
		s << L"USED"; break;
	case D3D_SVF_INTERFACE_POINTER:
		s << L"INTERFACE_POINTER"; break;
	case D3D_SVF_INTERFACE_PARAMETER:
		s << L"INTERFACE_PARAMETER"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_SHADER_VARIABLE_TYPE( D3D_SHADER_VARIABLE_TYPE vartype )
{
	std::wstringstream s;

	switch( vartype )
	{
	case D3D_SVT_VOID:
		s << L"VOID"; break;
	case D3D_SVT_BOOL:
		s << L"BOOL"; break;
	case D3D_SVT_INT:
		s << L"INTEGER"; break;
	case D3D_SVT_FLOAT:
		s << L"FLOAT"; break;
	case D3D_SVT_STRING:
		s << L"STRING"; break;
	case D3D_SVT_TEXTURE:
		s << L"TEXTURE"; break;
	case D3D_SVT_TEXTURE1D:
		s << L"TEXTURE1D"; break;
	case D3D_SVT_TEXTURE2D:
		s << L"TEXTURE2D"; break;
	case D3D_SVT_TEXTURE3D:
		s << L"TEXTURE3D"; break;
	case D3D_SVT_TEXTURECUBE:
		s << L"TEXTURECUBE"; break;
	case D3D_SVT_SAMPLER:
		s << L"SAMPLER"; break;
	case D3D_SVT_SAMPLER1D:
		s << L"SAMPLER1D"; break;
	case D3D_SVT_SAMPLER2D:
		s << L"SAMPLER2D"; break;
	case D3D_SVT_SAMPLER3D:
		s << L"SAMPLER3D"; break;
	case D3D_SVT_SAMPLERCUBE:
		s << L"SAMPLERCUBE"; break;
	case D3D_SVT_PIXELSHADER:
		s << L"PIXELSHADER"; break;
	case D3D_SVT_VERTEXSHADER:
		s << L"VERTEXSHADER"; break;
	case D3D_SVT_PIXELFRAGMENT:
		s << L"PIXELFRAGMENT"; break;
	case D3D_SVT_VERTEXFRAGMENT:
		s << L"VERTEXFRAGMENT"; break;
	case D3D_SVT_UINT:
		s << L"UINT"; break;
	case D3D_SVT_UINT8:
		s << L"UINT8"; break;
	case D3D_SVT_GEOMETRYSHADER:
		s << L"GEOMETRYSHADER"; break;
	case D3D_SVT_RASTERIZER:
		s << L"RASTERIZER"; break;
	case D3D_SVT_DEPTHSTENCIL:
		s << L"DEPTHSTENCIL"; break;
	case D3D_SVT_BLEND:
		s << L"BLEND"; break;
	case D3D_SVT_BUFFER:
		s << L"BUFFER"; break;
	case D3D_SVT_CBUFFER:
		s << L"CBUFFER"; break;
	case D3D_SVT_TBUFFER:
		s << L"TBUFFER"; break;
	case D3D_SVT_TEXTURE1DARRAY:
		s << L"TEXTURE1DARRAY"; break;
	case D3D_SVT_TEXTURE2DARRAY:
		s << L"TEXTURE2DARRAY"; break;
	case D3D_SVT_RENDERTARGETVIEW:
		s << L"RENDERTARGETVIEW"; break;
	case D3D_SVT_DEPTHSTENCILVIEW:
		s << L"DEPTHSTENCILVIEW"; break;
	case D3D_SVT_TEXTURE2DMS:
		s << L"TEXTURE2DMS"; break;
	case D3D_SVT_TEXTURE2DMSARRAY:
		s << L"TEXTURE2DMSARRAY"; break;
	case D3D_SVT_TEXTURECUBEARRAY:
		s << L"TEXTURECUBEARRAY"; break;
	case D3D_SVT_HULLSHADER:
		s << L"HULLSHADER"; break;
	case D3D_SVT_DOMAINSHADER:
		s << L"DOMAINSHADER"; break;
	case D3D_SVT_INTERFACE_POINTER:
		s << L"INTERFACE_POINTER"; break;
	case D3D_SVT_COMPUTESHADER:
		s << L"COMPUTESHADER"; break;
	case D3D_SVT_DOUBLE:
		s << L"DOUBLE"; break;
	case D3D_SVT_RWTEXTURE1D:
		s << L"RWTEXTURE1D"; break;
	case D3D_SVT_RWTEXTURE1DARRAY:
		s << L"RWTEXTURE1DARRAY"; break;
	case D3D_SVT_RWTEXTURE2D:
		s << L"RWTEXTURE2D"; break;
	case D3D_SVT_RWTEXTURE2DARRAY:
		s << L"RWTEXTURE2DARRAY"; break;
	case D3D_SVT_RWTEXTURE3D:
		s << L"RWTEXTURE3D"; break;
	case D3D_SVT_RWBUFFER:
		s << L"RWBUFFER"; break;
	case D3D_SVT_BYTEADDRESS_BUFFER:
		s << L"BYTEADDRESS_BUFFER"; break;
	case D3D_SVT_RWBYTEADDRESS_BUFFER:
		s << L"RWBYTEADDRESS_BUFFER"; break;
	case D3D_SVT_STRUCTURED_BUFFER:
		s << L"STRUCTURED_BUFFER"; break;
	case D3D_SVT_RWSTRUCTURED_BUFFER:
		s << L"RWSTRUCTURED_BUFFER"; break;
	case D3D_SVT_APPEND_STRUCTURED_BUFFER:
		s << L"APPEND_STRUCTURED_BUFFER"; break;
	case D3D_SVT_CONSUME_STRUCTURED_BUFFER:
		s << L"CONSUME_STRUCTURED_BUFFER"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_SHADER_INPUT_FLAGS( D3D_SHADER_INPUT_FLAGS inputflags )
{
	std::wstringstream s;

	switch( inputflags )
	{
	case D3D_SIF_USERPACKED:
		s << L"USERPACKED"; break;
	case D3D_SIF_COMPARISON_SAMPLER:
		s << L"COMPARISON_SAMPLER"; break;
	case D3D_SIF_TEXTURE_COMPONENT_0:
		s << L"TEXTURE_COMPONENT_0"; break;
	case D3D_SIF_TEXTURE_COMPONENT_1:
		s << L"TEXTURE_COMPONENT_1"; break;
	case D3D_SIF_TEXTURE_COMPONENTS:
		s << L"TEXTURE_COMPONENTS"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_SHADER_INPUT_TYPE( D3D_SHADER_INPUT_TYPE type )
{
	std::wstringstream s;

	switch( type )
	{
	case D3D_SIT_CBUFFER:
		s << L"CBUFFER"; break;
	case D3D_SIT_TBUFFER:
		s << L"TBUFFER"; break;
	case D3D_SIT_TEXTURE:
		s << L"TEXTURE"; break;
	case D3D_SIT_SAMPLER:
		s << L"SAMPLER"; break;
	case D3D_SIT_UAV_RWTYPED:
		s << L"UAV_RW_TYPED"; break;
	case D3D_SIT_STRUCTURED:
		s << L"STRUCTURED"; break;
	case D3D_SIT_UAV_RWSTRUCTURED:
		s << L"UAV_RW_STRUCTURED"; break;
	case D3D_SIT_BYTEADDRESS:
		s << L"UAV_BYTE_ADDRESS"; break;
	case D3D_SIT_UAV_RWBYTEADDRESS:
		s << L"UAV_RW_BYTE_ADDRESS"; break;
	case D3D_SIT_UAV_APPEND_STRUCTURED:
		s << L"UAV_APPEND_STRUCTURED"; break;
	case D3D_SIT_UAV_CONSUME_STRUCTURED:
		s << L"UAV_CONSUME_STRUCTURED"; break;
	case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
		s << L"UAV_RWSTRUCTURED_WITH_COUNTER"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_SHADER_CBUFFER_FLAGS( D3D_SHADER_CBUFFER_FLAGS flags )
{
	std::wstringstream s;

	switch( flags )
	{
	case D3D_CBF_USERPACKED:
		s << L"USERPACKED"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_CBUFFER_TYPE( D3D_CBUFFER_TYPE type )
{
	std::wstringstream s;

	switch( type )
	{
	case D3D_CT_CBUFFER:
		s << L"CBUFFER"; break;
	case D3D_CT_TBUFFER:
		s << L"TBUFFER"; break;
	case D3D_CT_INTERFACE_POINTERS:
		s << L"INTERFACE_POINTERS"; break;
	case D3D_CT_RESOURCE_BIND_INFO:
		s << L"RESOURCE_BIND_INFO"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_NAME( D3D_NAME name )
{
	std::wstringstream s;

	switch( name )
	{
	case D3D_NAME_UNDEFINED:
		s << L"UNDEFINED"; break;
	case D3D_NAME_POSITION:
		s << L"POSITION"; break;
	case D3D_NAME_CLIP_DISTANCE:
		s << L"CLIP_DISTANCE"; break;
	case D3D_NAME_CULL_DISTANCE:
		s << L"CULL_DISTANCE"; break;
	case D3D_NAME_RENDER_TARGET_ARRAY_INDEX:
		s << L"RENDER_TARGET_ARRAY_INDEX"; break;
	case D3D_NAME_VIEWPORT_ARRAY_INDEX:
		s << L"VIEWPORT_ARRAY_INDEX"; break;
	case D3D_NAME_VERTEX_ID:
		s << L"VERTEX_ID"; break;
	case D3D_NAME_PRIMITIVE_ID:
		s << L"PRIMITIVE_ID"; break;
	case D3D_NAME_INSTANCE_ID:
		s << L"INSTANCE_ID"; break;
	case D3D_NAME_IS_FRONT_FACE:
		s << L"IS_FRONT_FACE"; break;
	case D3D_NAME_SAMPLE_INDEX:
		s << L"SAMPLE_INDEX"; break;
	case D3D_NAME_FINAL_QUAD_EDGE_TESSFACTOR:
		s << L"FINAL_QUAD_EDGE_TESSFACTOR"; break;
	case D3D_NAME_FINAL_QUAD_INSIDE_TESSFACTOR:
		s << L"FINAL_QUAD_INSIDE_TESSFACTOR"; break;
	case D3D_NAME_FINAL_TRI_EDGE_TESSFACTOR:
		s << L"FINAL_TRI_EDGE_TESSFACTOR"; break;
	case D3D_NAME_FINAL_TRI_INSIDE_TESSFACTOR:
		s << L"FINAL_TRI_INSIDE_TESSFACTOR"; break;
	case D3D_NAME_FINAL_LINE_DETAIL_TESSFACTOR:
		s << L"FINAL_LINE_DETAIL_TESSFACTOR"; break;
	case D3D_NAME_FINAL_LINE_DENSITY_TESSFACTOR:
		s << L"FINAL_LINE_DENSITY_TESSFACTOR"; break;
	case D3D_NAME_TARGET:
		s << L"TARGET"; break;
	case D3D_NAME_DEPTH:
		s << L"DEPTH"; break;
	case D3D_NAME_COVERAGE:
		s << L"COVERAGE"; break;
	case D3D_NAME_DEPTH_GREATER_EQUAL:
		s << L"DEPTH_GREATER_EQUAL"; break;
	case D3D_NAME_DEPTH_LESS_EQUAL:
		s << L"DEPTH_LESS_EQUAL"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_RESOURCE_RETURN_TYPE( D3D_RESOURCE_RETURN_TYPE type )
{
	std::wstringstream s;
	
	switch( type )
	{
	case D3D_RETURN_TYPE_UNORM:
		s << L"UNORM"; break;
	case D3D_RETURN_TYPE_SNORM:
		s << L"SNORM"; break;
	case D3D_RETURN_TYPE_SINT:
		s << L"SINT"; break;
	case D3D_RETURN_TYPE_UINT:
		s << L"UINT"; break;
	case D3D_RETURN_TYPE_FLOAT:
		s << L"FLOAT"; break;
	case D3D_RETURN_TYPE_MIXED:
		s << L"MIXED"; break;
	case D3D_RETURN_TYPE_DOUBLE:
		s << L"DOUBLE"; break;
	case D3D_RETURN_TYPE_CONTINUED:
		s << L"CONTINUED"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_REGISTER_COMPONENT_TYPE( D3D_REGISTER_COMPONENT_TYPE type )
{
	std::wstringstream s;

	switch( type )
	{
	case D3D_REGISTER_COMPONENT_UNKNOWN:
		s << L"UNKNOWN"; break;
	case D3D_REGISTER_COMPONENT_UINT32:
		s << L"UINT32"; break;
	case D3D_REGISTER_COMPONENT_SINT32:
		s << L"SINT32"; break;
	case D3D_REGISTER_COMPONENT_FLOAT32:
		s << L"FLOAT32"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_TESSELLATOR_DOMAIN( D3D_TESSELLATOR_DOMAIN domain )
{
	std::wstringstream s;

	switch ( domain )
	{
	case D3D_TESSELLATOR_DOMAIN_UNDEFINED:
		s << L"UNDEFINED"; break;
	case D3D_TESSELLATOR_DOMAIN_ISOLINE:
		s << L"ISOLINE"; break;
	case D3D_TESSELLATOR_DOMAIN_TRI:
		s << L"TRIANGLE"; break;
	case D3D_TESSELLATOR_DOMAIN_QUAD:
		s << L"QUAD"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_TESSELLATOR_PARTITIONING( D3D_TESSELLATOR_PARTITIONING partitioning )
{
	std::wstringstream s;

	switch ( partitioning )
	{
	case D3D_TESSELLATOR_PARTITIONING_UNDEFINED:
		s << L"UNDEFINED"; break;
	case D3D_TESSELLATOR_PARTITIONING_INTEGER:
		s << L"INTEGER"; break;
	case D3D_TESSELLATOR_PARTITIONING_POW2:
		s << L"POW2"; break;
	case D3D_TESSELLATOR_PARTITIONING_FRACTIONAL_ODD:
		s << L"FRACTIONAL ODD"; break;
	case D3D_TESSELLATOR_PARTITIONING_FRACTIONAL_EVEN:
		s << L"FRACTIONAL EVEN"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_TESSELLATOR_OUTPUT_PRIMITIVE( D3D_TESSELLATOR_OUTPUT_PRIMITIVE primitive )
{
	std::wstringstream s;

	switch ( primitive )
	{
	case D3D_TESSELLATOR_OUTPUT_UNDEFINED:
		s << L"UNDEFINED"; break;
	case D3D_TESSELLATOR_OUTPUT_POINT:
		s << L"POINT"; break;
	case D3D_TESSELLATOR_OUTPUT_LINE:
		s << L"LINE"; break;
	case D3D_TESSELLATOR_OUTPUT_TRIANGLE_CW:
		s << L"TRIANGLE CLOCKWISE"; break;
	case D3D_TESSELLATOR_OUTPUT_TRIANGLE_CCW:
		s << L"TRIANGLE COUNTER CLOCKWISE"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_PRIMITIVE_TOPOLOGY( D3D_PRIMITIVE_TOPOLOGY topology )
{
	std::wstringstream s;

	switch ( topology )
	{
	case D3D_PRIMITIVE_TOPOLOGY_UNDEFINED:
		s << L"UNDEFINED"; break;
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		s << L"POINT LIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
		s << L"LINE LIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
		s << L"LINE STRIP"; break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		s << L"TRIANGLE LIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		s << L"TRIANGLE STRIP"; break;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		s << L"LINE LIST w/ADJACENCY"; break;
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		s << L"LINE STRIP w/ADJACENCY"; break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		s << L"TRIANGLE LIST w/ADJACENCY"; break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		s << L"TRIANGLE STRIP w/ADJACENCY"; break;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		s << L"1 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		s << L"2 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		s << L"3 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		s << L"4 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		s << L"5 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		s << L"6 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		s << L"7 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		s << L"9 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		s << L"10 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		s << L"11 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		s << L"12 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		s << L"13 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		s << L"14 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		s << L"15 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		s << L"16 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		s << L"17 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		s << L"18 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		s << L"19 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		s << L"20 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		s << L"21 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		s << L"22 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		s << L"23 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		s << L"24 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		s << L"25 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		s << L"26 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		s << L"27 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		s << L"28 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		s << L"29 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		s << L"30 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		s << L"31 CONTROL POINT PATCHLIST"; break;
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		s << L"32 CONTROL POINT PATCHLIST"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D_PRIMITIVE( D3D_PRIMITIVE topology )
{
	std::wstringstream s;

	switch ( topology )
	{
	case D3D_PRIMITIVE_UNDEFINED:
		s << L"UNDEFINED"; break;
	case D3D_PRIMITIVE_POINT:
		s << L"POINT"; break;
	case D3D_PRIMITIVE_LINE:
		s << L"LINE"; break;
	case D3D_PRIMITIVE_TRIANGLE:
		s << L"TRIANGLE"; break;
	case D3D_PRIMITIVE_LINE_ADJ:
		s << L"LINE w/ADJACENCY"; break;
	case D3D_PRIMITIVE_TRIANGLE_ADJ:
		s << L"TRIANGLE w/ADJACENCY"; break;
	case D3D_PRIMITIVE_1_CONTROL_POINT_PATCH:
		s << L"1 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_2_CONTROL_POINT_PATCH:
		s << L"2 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_3_CONTROL_POINT_PATCH:
		s << L"3 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_4_CONTROL_POINT_PATCH:
		s << L"4 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_5_CONTROL_POINT_PATCH:
		s << L"5 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_6_CONTROL_POINT_PATCH:
		s << L"6 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_7_CONTROL_POINT_PATCH:
		s << L"7 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_8_CONTROL_POINT_PATCH:
		s << L"9 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_10_CONTROL_POINT_PATCH:
		s << L"10 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_11_CONTROL_POINT_PATCH:
		s << L"11 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_12_CONTROL_POINT_PATCH:
		s << L"12 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_13_CONTROL_POINT_PATCH:
		s << L"13 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_14_CONTROL_POINT_PATCH:
		s << L"14 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_15_CONTROL_POINT_PATCH:
		s << L"15 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_16_CONTROL_POINT_PATCH:
		s << L"16 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_17_CONTROL_POINT_PATCH:
		s << L"17 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_18_CONTROL_POINT_PATCH:
		s << L"18 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_19_CONTROL_POINT_PATCH:
		s << L"19 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_20_CONTROL_POINT_PATCH:
		s << L"20 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_21_CONTROL_POINT_PATCH:
		s << L"21 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_22_CONTROL_POINT_PATCH:
		s << L"22 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_23_CONTROL_POINT_PATCH:
		s << L"23 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_24_CONTROL_POINT_PATCH:
		s << L"24 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_25_CONTROL_POINT_PATCH:
		s << L"25 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_26_CONTROL_POINT_PATCH:
		s << L"26 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_27_CONTROL_POINT_PATCH:
		s << L"27 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_28_CONTROL_POINT_PATCH:
		s << L"28 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_29_CONTROL_POINT_PATCH:
		s << L"29 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_30_CONTROL_POINT_PATCH:
		s << L"30 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_31_CONTROL_POINT_PATCH:
		s << L"31 CONTROL POINT PATCH"; break;
	case D3D_PRIMITIVE_32_CONTROL_POINT_PATCH:
		s << L"32 CONTROL POINT PATCH"; break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::TO_STRING_D3D11_SHADER_DESC( D3D11_SHADER_DESC description )
{
	std::wstringstream s;

	s << L"Version: " << description.Version << std::endl;
	s << L"Creator: " << description.Creator << std::endl;
	s << L"Flags: " << description.Flags << std::endl;
	s << L"ConstantBuffers: " << description.ConstantBuffers << std::endl;
	s << L"BoundResources: " << description.BoundResources << std::endl;
	s << L"InputParameters: " << description.InputParameters << std::endl;
	s << L"OutputParameters: " << description.OutputParameters << std::endl;
	s << L"InstructionCount: " << description.InstructionCount << std::endl;
	s << L"TempRegisterCount: " << description.TempRegisterCount << std::endl;
	s << L"TempArrayCount: " << description.TempArrayCount << std::endl;
	s << L"DefCount: " << description.DefCount << std::endl;
	s << L"DclCount: " << description.DclCount << std::endl;
	s << L"TextureNormalInstructions: " << description.TextureNormalInstructions << std::endl;
	s << L"TextureLoadInstructions: " << description.TextureLoadInstructions << std::endl;
	s << L"TextureCompInstructions: " << description.TextureCompInstructions << std::endl;
	s << L"TextureBiasInstructions: " << description.TextureBiasInstructions << std::endl;
	s << L"TextureGradientInstructions: " << description.TextureGradientInstructions << std::endl;
	s << L"FloatInstructionCount: " << description.FloatInstructionCount << std::endl;
	s << L"IntInstructionCount: " << description.IntInstructionCount << std::endl;
	s << L"UintInstructionCount: " << description.UintInstructionCount << std::endl;
	s << L"StaticFlowControlCount: " << description.StaticFlowControlCount << std::endl;
	s << L"DynamicFlowControlCount: " << description.DynamicFlowControlCount << std::endl;
	s << L"MacroInstructionCount: " << description.MacroInstructionCount << std::endl;
	s << L"ArrayInstructionCount: " << description.ArrayInstructionCount << std::endl;
	s << L"CutInstructionCount: " << description.CutInstructionCount << std::endl;
	s << L"EmitInstructionCount: " << description.EmitInstructionCount << std::endl;
	s << L"GSOutputTopology: " << TO_STRING_D3D_PRIMITIVE_TOPOLOGY( description.GSOutputTopology ) << std::endl;
	s << L"GSMaxOutputVertexCount: " << description.GSMaxOutputVertexCount << std::endl;
	s << L"InputPrimitive: " << TO_STRING_D3D_PRIMITIVE( description.InputPrimitive ) << std::endl;
	s << L"PatchConstantParameters: " << description.PatchConstantParameters << std::endl;
	s << L"cGSInstanceCount: " << description.cGSInstanceCount << std::endl;
	s << L"cControlPoints: " << description.cControlPoints << std::endl;
	s << L"HSOutputPrimitive: " << TO_STRING_D3D_TESSELLATOR_OUTPUT_PRIMITIVE( description.HSOutputPrimitive ) << std::endl;
	s << L"HSPartitioning: " << TO_STRING_D3D_TESSELLATOR_PARTITIONING( description.HSPartitioning ) << std::endl;
	s << L"TessellatorDomain: " << TO_STRING_D3D_TESSELLATOR_DOMAIN( description.TessellatorDomain ) << std::endl;
	s << L"cBarrierInstructions: " << description.cBarrierInstructions << std::endl;
	s << L"cInterlockedInstructions: " << description.cInterlockedInstructions << std::endl;
	s << L"cTextureStoreInstructions: " << description.cTextureStoreInstructions << std::endl;

	return( s.str() );
}
//--------------------------------------------------------------------------------
