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
#include "PCH.h"
#include "SceneRenderTask.h"
#include "EventManager.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Log.h"
#include "BoundsVisualizerActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SceneRenderTask::SceneRenderTask( ) :
	m_pEntity( nullptr ),
	m_pScene( nullptr ),
	m_bDebugViewEnabled( false ),
	m_pDebugVisualizer( new BoundsVisualizerActor() ),
	m_vColor( ),
	m_iViewports(),
	m_uiViewportCount( 1 ),
	ViewMatrix(),
	ProjMatrix()
{
}
//--------------------------------------------------------------------------------
SceneRenderTask::~SceneRenderTask( )
{
	SAFE_DELETE( m_pDebugVisualizer );
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetRenderParams( IParameterManager* pParamManager )
{
	pParamManager->SetViewMatrixParameter( &ViewMatrix );
	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetUsageParams( IParameterManager* pParamManager )
{

}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetEntity( Entity3D* pEntity )
{
	m_pEntity = pEntity;
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetScene( Scene* pScene )
{
	m_pScene = pScene;
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetViewMatrix( const Matrix4f& matrix )
{
	ViewMatrix = matrix;
}
//--------------------------------------------------------------------------------
Matrix4f SceneRenderTask::GetViewMatrix( )
{
	return( ViewMatrix );
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetProjMatrix( const Matrix4f& matrix )
{
	ProjMatrix = matrix;
}
//--------------------------------------------------------------------------------
Matrix4f SceneRenderTask::GetProjMatrix( )
{
	return( ProjMatrix );
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetBackColor( const Vector4f& color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetViewPort( int viewport, unsigned int index )
{
	assert( index < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE );

	m_iViewports[index] = viewport;
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetViewPortCount( unsigned int count )
{
	assert( count < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE );

	m_uiViewportCount = count;
}
//--------------------------------------------------------------------------------
void SceneRenderTask::ConfigureViewports( PipelineManagerDX11* pPipeline )
{
	// Indicate the number of viewports to set.
	pPipeline->RasterizerStage.DesiredState.ViewportCount.SetState( m_uiViewportCount );

	// Provide the actual viewport indices.
	for ( unsigned int i = 0; i < m_uiViewportCount; i++ ) {
		pPipeline->RasterizerStage.DesiredState.Viewports.SetState( i, m_iViewports[i] );
	}
}
//--------------------------------------------------------------------------------
const ViewPortDX11& SceneRenderTask::GetViewPort( unsigned int index )
{
	assert( index < m_uiViewportCount );

	return( RendererDX11::Get()->GetViewPort( m_iViewports[index] ) );
}
//--------------------------------------------------------------------------------
void SceneRenderTask::SetDebugViewEnabled( bool debug )
{
	m_bDebugViewEnabled = debug;
}
//--------------------------------------------------------------------------------
bool SceneRenderTask::IsDebugViewEnabled()
{
	return( m_bDebugViewEnabled );
}
//--------------------------------------------------------------------------------