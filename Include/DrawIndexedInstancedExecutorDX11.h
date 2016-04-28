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
// DrawIndexedInstancedExecutorDX11
//
// This class specializes further specializes the DrawIndexedExecutorDX11 to use
// instanced drawing.  This is performed in a simple vertex configuration with 
// one vertex buffer for providing per-vertex data, one vertex buffer for 
// providing per-instance data, and then the index buffer for providing the index
// data.
//--------------------------------------------------------------------------------
#ifndef DrawIndexedInstancedExecutorDX11_h
#define DrawIndexedInstancedExecutorDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "DrawIndexedExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class TVertex, class TInstance>
	class DrawIndexedInstancedExecutorDX11 : public DrawIndexedExecutorDX11<TVertex>
	{
	public:
		DrawIndexedInstancedExecutorDX11( );
		virtual ~DrawIndexedInstancedExecutorDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		virtual void ResetGeometry();
		virtual void ResetInstances();

		void AddInstance( const TInstance& data );
		unsigned int GetInstanceCount();

		void SetInstanceRange( unsigned int start, unsigned int end );

	protected:
		
		TGrowableVertexBufferDX11<TInstance> InstanceBuffer;
		unsigned int m_uiStart;
		unsigned int m_uiCount;
	};

	#include "DrawIndexedInstancedExecutorDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // DrawIndexedInstancedExecutorDX11_h
//--------------------------------------------------------------------------------
