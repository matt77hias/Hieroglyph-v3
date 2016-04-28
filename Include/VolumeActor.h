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
// VolumeActor
//
//--------------------------------------------------------------------------------
#ifndef VolumeActor_h
#define VolumeActor_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Actor.h"
#include "DrawIndexedExecutorDX11.h"
#include "VolumeTextureVertexDX11.h"
#include "TextureSpaceCameraPositionWriter.h"
#include "TextureSpaceLightPositionWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	typedef std::shared_ptr<DrawIndexedExecutorDX11<VolumeTextureVertexDX11::Vertex>> VolumeGeometryPtr;

	class VolumeActor : public Actor
	{
	public:
		VolumeActor();
		virtual ~VolumeActor();

		ResourcePtr							m_VolumeTexture;
		VolumeGeometryPtr					m_pGeometry;
	};
};
//--------------------------------------------------------------------------------
#endif // VolumeActor_h
