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
// IRenderView
//
//--------------------------------------------------------------------------------
#ifndef IRenderView_h
#define IRenderView_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "PipelineManagerDX11.h"
#include "IParameterManager.h"
#include "Timer.h"
#include "IEventListener.h"
#include "ViewRenderParams.h"
#include <vector>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;
	class Node3D;

	class IRenderView : public IEventListener
	{
	public:

		// Virtual destructor for abstract base class.

		IRenderView( );
		virtual ~IRenderView( );

		// All of the render views must provide a view type for identifying
		// themselves to the renderer.  This is used during sorting to determine
		// which views are rendered first.

		VIEWTYPE GetType( );
		
		// Update and draw functions.  The update function allows the view to use
		// time varying parameters.  Any animation or time varying quantities should
		// be updated here.  The draw function will be used to set the necesary
		// render states and render the geometry that is stored in the object list.

		virtual void Update( float fTime ) = 0;
		virtual void PreDraw( RendererDX11* pRenderer ) = 0;
		virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager ) = 0;
		virtual void Resize( UINT width, UINT height ){};

		// A reference is provided to allow views to reference it's entity's data.

		virtual void SetEntity( Entity3D* pEntity );

		// A second reference is used to specify the root node of the entities that
		// the view will be rendering from.

		virtual void SetRoot( Node3D* pRoot );

		// The render view must set all of its rendering parameters needed to 
		// execute itself with the SetRenderParams function.  The SetUsageParams
		// is used to set semantics that are needed to use the output of the 
		// render view by other objects.
		
		virtual void SetRenderParams( IParameterManager* pParamManager ) = 0;
		virtual void SetUsageParams( IParameterManager* pParamManager ) = 0;

		void SetName( std::wstring& name );

		virtual std::wstring GetName( );
		virtual bool HandleEvent( IEvent* pEvent );

		int GetRTID( int index );

		void SetMaxRecurrence( int max );
		int GetMaxRecurrence( );

		virtual void SetViewMatrix( const Matrix4f& matrix );
		Matrix4f GetViewMatrix( );
		virtual void SetProjMatrix( const Matrix4f& matrix );
		Matrix4f GetProjMatrix( );

	protected:

		ViewRenderParams m_sParams;
		std::wstring m_sName;

		Entity3D* m_pEntity;
		Node3D* m_pRoot;

		Matrix4f ViewMatrix;
		Matrix4f ProjMatrix;

		int		m_iMaxRecurrence;
		int		m_iCurrRecurrence;
	};
};
//--------------------------------------------------------------------------------
#endif // IRenderView_h
