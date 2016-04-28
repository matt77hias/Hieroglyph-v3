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
// ConstantBufferDX11 
//
//--------------------------------------------------------------------------------
#ifndef ConstantBufferDX11_h
#define ConstantBufferDX11_h
//--------------------------------------------------------------------------------
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct ConstantBufferMapping
	{
		RenderParameterDX11*		pParameter;
		unsigned int				offset;
		unsigned int				size;
		D3D_SHADER_VARIABLE_CLASS	varclass;
		unsigned int				elements;
		unsigned int				valueID;
	};



	class ConstantBufferDX11 : public BufferDX11
	{
	public:
		ConstantBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer );
		virtual ~ConstantBufferDX11();

		virtual ResourceType				GetType( );

		void						AddMapping( ConstantBufferMapping& mapping );
		void						EmptyMappings( );
		void						EvaluateMappings( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		bool						ContainsMapping( int index, const ConstantBufferMapping& mapping );

		void						SetAutoUpdate( bool enable );
		bool						GetAutoUpdate( );

	protected:
		bool									m_bAutoUpdate;
		std::vector< ConstantBufferMapping >	m_Mappings;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ConstantBufferDX11_h
