//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "App.h"
#include "Log.h"
#include "FileSystem.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RotationController.h"

#include "VectorParameterDX11.h"
#include "BasicVertexDX11.h"
#include "DrawExecutorDX11.h"
#include "MeshSTL.h"
#include "MeshOBJ.h"

#include "TextureActor.h"
#include "SkyboxActor.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 800, 600, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	SetMultiThreadedMode( false );

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.5f, 0.3f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( -3.0f, 12.0f, -15.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.4f, 0.0f ) );

	// Create the actor to hold the immediate geometry.  

	m_pIndexedActor = new GeometryActor();
	m_pScene->AddActor( m_pIndexedActor );
	m_pIndexedActor->GetNode()->Transform.Position() = Vector3f( 3.0, 0.0f, 0.0f );
	m_pIndexedActor->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pIndexedActor->UseTexturedMaterial( m_pRenderer11->LoadTexture( L"EyeOfHorus_128.png" ) );

	// Throw a rotation onto the actor to slowly rotate it about the Y-axis.
	RotationController<Node3D>* pIndexedRotController = new RotationController<Node3D>( Vector3f( 0.0f, 1.0f, 0.0f ), -0.1f );
	m_pIndexedActor->GetNode()->Controllers.Attach( pIndexedRotController );



	m_pGeometryActor = new GeometryActor();
	m_pScene->AddActor( m_pGeometryActor );
	m_pGeometryActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 2.5f, 0.0f );

	m_pGeometryActor->UseTransparentMaterial();
	m_pGeometryActor->SetDiffuse( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pGeometryActor->SetColor( Vector4f( 1.0f, 0.0f, 0.0f, 0.5f ) );
	m_pGeometryActor->DrawSphere( Vector3f( 2.5f, 2.0f, 0.0f ), 1.5f, 16, 24 );
	m_pGeometryActor->SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 1.0f ) );
	m_pGeometryActor->DrawCylinder( Vector3f( -1.5f, -1.0f, 0.0f ), Vector3f( -1.5f, 3.0f, 0.0f ), 1.5f, 0.0f, 8, 24 );
	m_pGeometryActor->SetColor( Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ) );
	m_pGeometryActor->DrawDisc( Vector3f( 0.0f, -3.0f, 0.0f ), Vector3f( 1.0f, 1.0f, 1.0f ), 2.0f, 12 );
	m_pGeometryActor->SetColor( Vector4f( 0.0f, 0.0f, 1.0f, 1.0f ) );
	m_pGeometryActor->DrawBox( Vector3f( 0.0f, 3.0f, 0.0f ), Vector3f( 1.0f, 1.0f, 1.0f ) );

	m_pGeometryActor->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pGeometryActor->DrawArrow( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 5.0f, 0.0f, 0.0f ), 0.5f, 1.0f, 1.0f );

	RotationController<Node3D>* pGeometryRotController = new RotationController<Node3D>( Vector3f( 0.0f, 1.0f, 0.0f ), 0.4f );
	m_pGeometryActor->GetNode()->Controllers.Attach( pGeometryRotController );


	m_pTextActor = new TextActor();
	m_pTextActor->SetTextOrientation( Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
	m_pTextActor->SetText( std::wstring( L"Hello World! This is some \nsample text!" ) );
	
	m_pTextActor->SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 0.5f ) );
	m_pTextActor->NewLine();
	m_pTextActor->SetCharacterHeight( 2.0f );
	m_pTextActor->AppendText( L"Text is colored, scaled, and alpha blended..." );

	m_pTextActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 7.0f, 0.0f );
	m_pScene->AddActor( m_pTextActor );

	// Add a single point light to the scene.
	m_pLight = new PointLight();
	m_pScene->AddLight( m_pLight );

	m_pLight->GetNode()->Controllers.Attach( new RotationController<Node3D>( Vector3f( 0.0f, 1.0f, 0.0f ), -1.0f ) );
	m_pLight->GetNode()->Transform.Position() = Vector3f( 0.0f, 50.0f, 0.0f );
	m_pLight->GetBody()->Transform.Position() = Vector3f( 50.0f, 0.0f, 0.0f );


	// Load an STL file and configure an actor to use it.

	FileSystem fs;
	STL::MeshSTL stl( fs.GetModelsFolder() + L"MeshedReconstruction.stl" );
	OBJ::MeshOBJ obj( fs.GetModelsFolder() + L"Capsule.obj" );

	m_pMeshActor = new Actor();
	m_pScene->AddActor( m_pMeshActor );
	m_pMeshActor->GetBody()->Controllers.Attach( new RotationController<Entity3D>( Vector3f( 0.0f, 1.0f, 0.0f ), -1.0f ) );
	m_pMeshActor->GetNode()->Transform.Position() = Vector3f( 5.0f, 5.0f, 0.0f );


	m_pMeshActor->GetBody()->Visual.SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometrySolidMaterial( *m_pRenderer11) );
	auto pMeshExecutor = std::make_shared<DrawExecutorDX11<BasicVertexDX11::Vertex>>();
	pMeshExecutor->SetLayoutElements( BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements );
	pMeshExecutor->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pMeshExecutor->SetMaxVertexCount( 3 * stl.faces.size() );
	m_pMeshActor->GetBody()->Visual.SetGeometry( pMeshExecutor );


	BasicVertexDX11::Vertex vertex;
	vertex.color = Vector4f( 0.0f, 1.0f, 0.0f, 0.0f );

	for ( auto& face : stl.faces )
	{
		vertex.normal = face.normal;

		vertex.position = face.v0;
		pMeshExecutor->AddVertex( vertex );

		vertex.position = face.v1;
		pMeshExecutor->AddVertex( vertex );

		vertex.position = face.v2;
		pMeshExecutor->AddVertex( vertex );
	}



	Actor* m_pOBJMesh = new Actor();
	m_pScene->AddActor( m_pOBJMesh );
	m_pOBJMesh->GetBody()->Controllers.Attach( new RotationController<Entity3D>( Vector3f( 0.0f, 1.0f, 0.0f ), -1.0f ) );
	m_pOBJMesh->GetNode()->Transform.Position() = Vector3f( 0.0f, 0.0f, -5.0f );


	m_pOBJMesh->GetBody()->Visual.SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometrySolidMaterial( *m_pRenderer11) );
	auto pOBJExecutor = std::make_shared<DrawExecutorDX11<BasicVertexDX11::Vertex>>();
	pOBJExecutor->SetLayoutElements( BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements );
	pOBJExecutor->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pOBJExecutor->SetMaxVertexCount( obj.positions.size() );
	m_pOBJMesh->GetBody()->Visual.SetGeometry( pOBJExecutor );


	BasicVertexDX11::Vertex v;
	v.color = Vector4f( 0.0f, 0.0f, 0.0f, 0.0f );

	for ( auto& o : obj.objects )
	{
		for ( auto& subobject : o.subobjects )
		{
			for ( auto& face : subobject.faces )
			{
				// Only grab faces with 3 vertices - i.e. triangles!
				if ( face.positionIndices.size() == 3 )
				{
					for ( size_t i = 0; i < 3; ++i ) {
						v.position = obj.positions[face.positionIndices[i]];
						v.normal = obj.normals[face.normalIndices[i]];
						v.texcoords = obj.coords[face.coordIndices[i]];
						pOBJExecutor->AddVertex( v );
					}
				}
			}
		}
	}


	BezierCubic curve;
	curve.points[0] = Vector3f( 0.0f, 0.0f, 0.0f );
	curve.points[1] = Vector3f( 5.0f, 5.0f, 0.0f );
	curve.points[2] = Vector3f( 5.0f, 10.0f, 0.0f );
	curve.points[3] = Vector3f( 0.0f, 10.0f, 0.0f );
	
	GeometryActor* curveActor = new GeometryActor();
	curveActor->SetPrimitiveType( D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	curveActor->SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 1.0f ) );
	curveActor->DrawBezierCurve( curve, 0.0f, 1.0f, 200 );
	m_pScene->AddActor( curveActor );


	ResourcePtr skyboxTexture = m_pRenderer11->LoadTexture( L"TropicalSunnyDay.dds" );
	SkyboxActor* skybox = new SkyboxActor( skyboxTexture, 10.0f );
	m_pScene->AddActor( skybox );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Do some immediate rendering here.  These vertex data are applied in object
	// space, so we are essentially redefining our geometry in every frame.  This
	// is normally not the most efficient way, but it is significantly easier to
	// dynamically produce than it is to determine before hand what you need...
	//
	// In this case, I am animating a paraboloid shape as well as its color.
	
	const int GRIDSIZE = 20;
	const float fGRIDSIZE = static_cast<float>( GRIDSIZE );
	const float ELEMSIZE = 1.0f;
	const float fSIZESCALE = 5.0f / fGRIDSIZE;

	Vector3f v0, v1, v2;
	Vector2f uv0, uv1, uv2;
	Vector3f n0, n1, n2;

	float fScaling = 0.25f * sinf( m_pTimer->Runtime() * 0.75f );

	// Do some indexed immediate rendering by creating a grid of vertices first...

	m_pIndexedActor->ResetGeometry();

	for ( int z = 0; z < GRIDSIZE; z++ ) {
		for ( int x = 0; x < GRIDSIZE; x++ ) {
			
			float fX = static_cast<float>( x );
			float fZ = static_cast<float>( z );

			v0.x = fX - GRIDSIZE / 2;
			v0.z = fZ - GRIDSIZE / 2;
			v0.y = ( 5.0f - 0.2f * ( v0.x*v0.x + v0.z*v0.z ) ) * fScaling;
			uv0.x =        ( fX ) / ( fGRIDSIZE-1 );
			uv0.y = 1.0f - ( fZ ) / ( fGRIDSIZE-1 );

			m_pIndexedActor->AddVertex( v0 * fSIZESCALE, uv0 );
		}
	}

	// ... followed by creating the associated group of indices that reference
	// those vertices.

	for ( int z = 0; z < GRIDSIZE-1; z++ ) {
		for ( int x = 0; x < GRIDSIZE-1; x++ ) {

			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + 1 );

			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + 1 );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE );
			m_pIndexedActor->AddIndex( (z*GRIDSIZE + x) + GRIDSIZE + 1 );
		}
	}


	// Print a message to show the framerate and sample name.

	std::wstringstream out;

	out << L"Hieroglyph 3 : " << GetName() << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();
	
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
		Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	
	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( key == '1' ) {
			m_pCamera->SetOffsetProjectionParams( -0.4f, 0.4f, -0.3f, 0.3f, 0.5f, 100.0f );
		}
		if ( key == '2' ) {
			m_pCamera->SetOffsetProjectionParams( 0.0f, 0.8f, -0.3f, 0.3f, 0.5f, 100.0f );
		}
		if ( key == '3' ) {
			m_pCamera->SetOffsetProjectionParams( -0.8f, 0.0f, -0.3f, 0.3f, 0.5f, 100.0f );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"ImmediateRenderer" ) );
}
//--------------------------------------------------------------------------------