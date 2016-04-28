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
// DrawIndexedExecutorDX11
//
// This class specializes the DrawExecutorDX11 class by adding indices to
// its capability, and subsequently uses indexed rendering instead of standard
// rendering calls.  The interface for the indexed portion of the geometry
// functions in a similar manner to the vertex capability from 
// DrawExecutorDX11 - you simply add indices which reference the vertices
// that have either already been added to the object or will be added (i.e. this
// class doesn't perform any range checking of the indices - you need to add
// correct indices yourself!).
//
// With vertices and indices available, you can use this class to create any type
// of primitive for submission to the pipeline.  The only thing that needs to be
// done is to ensure that the geometry being passed to the pipeline matches the
// expected geometry from the shader programs.
//--------------------------------------------------------------------------------
#ifndef IndexedImmediateGeometryDX11_h
#define IndexedImmediateGeometryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "DrawExecutorDX11.h"
#include "TGrowableIndexBufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class TVertex>
	class DrawIndexedExecutorDX11 : public DrawExecutorDX11<TVertex>
	{
	public:
		DrawIndexedExecutorDX11( );
		virtual ~DrawIndexedExecutorDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		virtual void ResetGeometry();
		virtual void ResetIndices();

		void AddIndex( const unsigned int index );
		void AddIndices( const unsigned int i1, const unsigned int i2 );
		void AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 );

		unsigned int GetIndexCount();
		virtual unsigned int GetPrimitiveCount();

		void SetMaxIndexCount( unsigned int count );

	protected:
		
		TGrowableIndexBufferDX11<unsigned int> IndexBuffer;
	};

	#include "DrawIndexedExecutorDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // IndexedImmediateGeometryDX11_h
