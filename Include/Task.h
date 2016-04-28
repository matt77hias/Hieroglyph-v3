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
// Task
//
//--------------------------------------------------------------------------------
#ifndef Task_h
#define Task_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "PipelineManagerDX11.h"
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Task
	{

	public:
		Task( );
		virtual ~Task( );

		// Update and draw functions.  The update function allows the view to use
		// time varying parameters.  Any animation or time varying quantities should
		// be updated here.  The draw function will be used to set the necessary
		// render states and render the geometry that is stored in the object list.

		virtual void Update( float fTime ) = 0;
		virtual void QueuePreTasks( RendererDX11* pRenderer ) = 0;
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager ) = 0;

		// The render view must set all of its rendering parameters needed to 
		// execute itself with the SetRenderParams function.  The SetUsageParams
		// is used to set semantics that are needed to use the output of the 
		// render view by other objects.
		
		virtual void SetRenderParams( IParameterManager* pParamManager ) = 0;
		virtual void SetUsageParams( IParameterManager* pParamManager ) = 0;

		virtual std::wstring GetName( ) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // Task_h
