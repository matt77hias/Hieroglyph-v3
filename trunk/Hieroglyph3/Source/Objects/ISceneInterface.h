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
// ISceneInterface
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef ISceneInterface_h
#define ISceneInterface_h
//--------------------------------------------------------------------------------
#include "Node3D.h"
#include "ICameraInterface.h"
#include "TArray.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ISceneInterface
	{
	public:
		ISceneInterface();
		virtual ~ISceneInterface();

		virtual void Update( float time );
		virtual void Render( );

		void AddCamera( ICameraInterface* camera );
		void AddEntity( Entity3D* entity );

		// Geometric queries
		void BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record );

		Node3D* GetRoot();

	protected:
		Node3D* m_pRoot;
		TArray< ICameraInterface* > m_vCameras;
	};
};
//--------------------------------------------------------------------------------
#endif // ISceneInterface_h
