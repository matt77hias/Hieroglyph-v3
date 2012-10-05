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
// IndexedImmediateGeometryDX11
//
// This class specializes the ImmediateGeometryDX11 class by adding indices to
// its capability, and subsequently uses indexed rendering instead of standard
// rendering calls.  The interface for the indexed portion of the geometry
// functions in a similar manner to the vertex capability from 
// ImmediateGeometryDX11 - you simply add indices which reference the vertices
// that have either already been added to the object or will be added (i.e. this
// class doesn't perform any range checking of the indices - you need to add
// correct indices to your object!).
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
#include "ImmediateGeometryDX11.h"
#include "ResourceProxyDX11.h"
#include "TGrowableIndexBufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexBufferDX11;

	class IndexedImmediateGeometryDX11 : public ImmediateGeometryDX11
	{
	public:
		IndexedImmediateGeometryDX11( );
		virtual ~IndexedImmediateGeometryDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		virtual void ResetGeometry();

		void AddIndex( const unsigned int index );
		void AddIndices( const unsigned int i1, const unsigned int i2 );
		void AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 );

		unsigned int GetIndexCount();
		virtual unsigned int GetPrimitiveCount();

	protected:
		
		TGrowableIndexBufferDX11<unsigned int> IndexBuffer;
	};

	typedef std::shared_ptr<IndexedImmediateGeometryDX11> IndexedImmediateGeometryPtr;
};
//--------------------------------------------------------------------------------
#endif // IndexedImmediateGeometryDX11_h
