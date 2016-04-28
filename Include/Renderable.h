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
// Renderable
//
// The object rendering parameter structure defines all of the variable 
// parameters that could potentially change from object to object.  This
// structure represents all of the ways that an object can communicate
// with the renderer.
//
// The iPass variable is an identifier that specifies if an object has a
// special case associated with it.  This would then allow a specific
// render view to sort the objects according to these types if there
// is a need.  For example, a skybox could be rendered before or after
// normal geometry for a particular effect.
//--------------------------------------------------------------------------------
#ifndef Renderable_h
#define Renderable_h
//--------------------------------------------------------------------------------
#include "PipelineExecutorDX11.h"
#include "MaterialDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// The render pass is used to allow an object to define what type of
	// object it actually is.  This can then be used by a render view to
	// provide a special consideration when selecting its object rendering 
	// order.

	class Renderable
	{
	public:
		enum ENTITYTYPE
		{
			GUI_TEXT,
			GUI,
			ALPHA,
			SKY,
			BACKGROUND,
			GEOMETRY,
			NUMPASSES
		};

		Renderable();
		~Renderable();

		void SetMaterial( MaterialPtr pMaterial );
		MaterialPtr GetMaterial( );

		void SetGeometry( ExecutorPtr pExecutor );
		ExecutorPtr GetGeometry( );


		ENTITYTYPE				iPass;
		ExecutorPtr				Executor;
		MaterialPtr				Material;
	};
};
//--------------------------------------------------------------------------------
#endif // Renderable_h
//--------------------------------------------------------------------------------