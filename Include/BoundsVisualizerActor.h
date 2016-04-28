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
// BoundsVisualizerActor
//
//--------------------------------------------------------------------------------
#ifndef BoundsVisualizerActor_h
#define BoundsVisualizerActor_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Actor.h"
#include "DrawIndexedInstancedExecutorDX11.h"
#include "VisualizerVertexDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	typedef std::shared_ptr<DrawIndexedInstancedExecutorDX11<VisualizerVertexDX11::VertexData, VisualizerVertexDX11::InstanceData>> VisualizerGeometryPtr;

	class BoundsVisualizerActor : public Actor
	{
	public:
		BoundsVisualizerActor();
		virtual ~BoundsVisualizerActor();

		void UpdateBoundsData( std::vector<Entity3D*>& entities );

		VisualizerGeometryPtr				m_pGeometry;
	};
};
//--------------------------------------------------------------------------------
#endif // BoundsVisualizerActor_h
