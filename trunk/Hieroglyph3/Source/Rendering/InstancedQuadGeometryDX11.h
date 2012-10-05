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
// InstancedQuadGeometryDX11
//
// This class specializes PipelineExecutorDX11 to provide an easy to use quad
// renderer, using instanced drawing.
//--------------------------------------------------------------------------------
#ifndef InstancedQuadGeometryDX11_h
#define InstancedQuadGeometryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineExecutorDX11.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
#include "ResourceProxyDX11.h"
#include "TArray.h"
#include "TGrowableVertexBufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexBufferDX11;

	struct SpriteDrawRect
	{
		float X;
		float Y;
		float Width;
		float Height;
	};

	struct SpriteDrawData
	{
		Matrix4f Transform;
		Vector4f Color;
		SpriteDrawRect DrawRect;
	};


	class InstancedQuadGeometryDX11 : public PipelineExecutorDX11
	{
	public:
		InstancedQuadGeometryDX11( );
		virtual ~InstancedQuadGeometryDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		virtual void ResetGeometry();

		void AddQuad( const SpriteDrawData& data );

		unsigned int GetQuadCount();

	protected:
		
		ResourcePtr m_pVertexBuffer;
		ResourcePtr m_pIndexBuffer;
		TGrowableVertexBufferDX11<SpriteDrawData> QuadBuffer;

		struct SpriteVertex
		{
			Vector2f Position;
			Vector2f TexCoord;
		};
	};

	typedef std::shared_ptr<InstancedQuadGeometryDX11> InstancedQuadGeometryPtr;
};
//--------------------------------------------------------------------------------
#endif // InstancedQuadGeometryDX11_h
