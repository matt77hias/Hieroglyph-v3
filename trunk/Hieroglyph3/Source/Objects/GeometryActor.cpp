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
#include "GeometryActor.h"
#include "MaterialGeneratorDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "VertexEvaluator2f.h"
#include "SphereAttributes.h"
#include "ConeAttributes.h"
#include "GridTessellator2f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GeometryActor::GeometryActor() :
	m_Color( 1.0f, 1.0f, 1.0f, 1.0f )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	m_pGeometry = IndexedImmediateGeometryPtr( new DrawIndexedExecutorDX11<BasicVertexDX11::Vertex>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pGeometry->SetLayoutElements( BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements );
	
	SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	m_pSolidMaterial = MaterialGeneratorDX11::GenerateImmediateGeometrySolidMaterial( *pRenderer );
	m_pTexturedMaterial = MaterialGeneratorDX11::GenerateImmediateGeometryTexturedMaterial( *pRenderer );

	GetBody()->SetGeometry( m_pGeometry );

	UseSolidMaterial();
}
//--------------------------------------------------------------------------------
GeometryActor::~GeometryActor()
{
}
//--------------------------------------------------------------------------------
void GeometryActor::ResetGeometry()
{
	m_pGeometry->ResetGeometry();
}
//--------------------------------------------------------------------------------
void GeometryActor::SetColor( const Vector4f& color )
{
	m_Color = color;
}
//--------------------------------------------------------------------------------
Vector4f GeometryActor::GetColor( )
{
	return( m_Color );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = m_Color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );

	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector4f& color )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector2f& texcoords )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = m_Color;
	vertex.texcoords = texcoords;
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector4f& color, const Vector2f& texcoords )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = color;
	vertex.texcoords = texcoords;
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector3f& normal )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = m_Color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector3f& normal, const Vector4f& color )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector3f& normal, const Vector2f& texcoords )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = m_Color;
	vertex.texcoords = texcoords;
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddVertex( const Vector3f& position, const Vector3f& normal, const Vector4f& color, const Vector2f& texcoords )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = color;
	vertex.texcoords = texcoords;
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddIndex( const unsigned int index )
{
	// This method does not check the value that is passed as an index against
	// the number of vertices that are out there. User's are allowed to do what they
	// want with this data, but have to ensure that the indices are correct!

	m_pGeometry->AddIndex( index );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddIndices( const unsigned int i1, const unsigned int i2 )
{
	AddIndex( i1 );
	AddIndex( i2 );
}
//--------------------------------------------------------------------------------
void GeometryActor::AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 )
{
	AddIndex( i1 );
	AddIndex( i2 );
	AddIndex( i3 );
}
//--------------------------------------------------------------------------------
void GeometryActor::DrawSphere( const Vector3f& center, float radius, unsigned int stacks, unsigned int slices )
{
	Sphere3f sphere( center, radius );

	if ( stacks < 2 ) stacks = 2;
	if ( slices < 4 ) slices = 4;

	unsigned int baseVertex = m_pGeometry->GetVertexCount();

	// Build up a vertex evaluator, which will allow us to 'sample' a vertex's
	// attributes in a particular location.  In this case, we are defining an
	// evaluator that works in a 2D space.  In addition, we are provide the
	// evaluator with a Sphere3f to be it's 'model' and we tell it its vertex
	// type - both through template parameters.

	VertexEvaluator2f< Sphere3f, BasicVertexDX11::Vertex > evaluator;

	// Once the evaluator is created, then we add some AttributeEvaluators to 
	// it.  These objects are also templated according to the model and vertex
	// types, and they provide an 'evaluate' method to calculate a single vertex 
	// attribute that attribute is written directly into a passed in vertex.  We
	// add them to a list, and just iterate over it to create a complete vertex.

	evaluator.Evaluators.push_back( std::make_shared<SurfacePositionFromSphere<BasicVertexDX11::Vertex>>() );
	evaluator.Evaluators.push_back( std::make_shared<SurfaceNormalFromSphere<BasicVertexDX11::Vertex>>() );
	evaluator.Evaluators.push_back( std::make_shared<TexcoordsFromSphere<BasicVertexDX11::Vertex>>() );
	evaluator.Evaluators.push_back( std::make_shared<ConstantColorFromSphere<BasicVertexDX11::Vertex>>( m_Color ) );

	// Now we provide the evaluator with the instance of its model type.  This will
	// be passed to the attribute evaluators so they can determine what value they
	// should have.

	evaluator.SetModel( sphere );

	// Next we create a tessellator object, and provide our vertex evaluator type
	// and the receiving object type as template parameters.  These two types 
	// basically describe the 'vertex producer' and the 'vertex consumer'.  The
	// tessellator also takes a few parameters to define how we want it to do the
	// sampling over its space.

	GridTessellator2f<
		VertexEvaluator2f< Sphere3f, BasicVertexDX11::Vertex >, 
		DrawIndexedExecutorDX11<BasicVertexDX11::Vertex>
	> tessellator( stacks, slices );
	
	// The tessellator then fires off its activities using the supplied vertex
	// attribute evaluator and containers.  We simply specify the parameters 
	// indicating the ranges we want to sweep through the 2D manifold and the
	// rest is done for us.  In this case, we are tessellating a sphere, so we
	// pass in [0,2*pi] for theta and [0,pi] for phi.

	tessellator.TessellateTriGrid<BasicVertexDX11::Vertex>( 
		evaluator, *m_pGeometry,
		0.0f, static_cast<float>( 2.0 * GLYPH_PI ),
		0.0f, static_cast<float>( 1.0 * GLYPH_PI ) );

}
//--------------------------------------------------------------------------------
void GeometryActor::DrawDisc( const Vector3f& center, const Vector3f& normal, float radius, unsigned int slices )
{
	if ( slices < 4 ) slices = 4;
 
	// Define the axis through the center of the circle
	Vector3f vnorm = Vector3f::Normalize( normal );
 
	Vector3f up = Vector3f( 0.0f, 1.0f, 0.0f );

	// Find the unit vector perpendicular to the axis, that is pointing closest to the (0,1,0) direction.
	// If the axis points in the (0,1,0) direction, then use (1,0,0) as the unit vector.
 
	Vector3f unit;
 
	if ( vnorm == up || vnorm == -up ) {
		unit = Vector3f( 1.0f, 0.0f, 0.0f );
	} else {
		Vector3f temp = Vector3f::Cross( vnorm, up );
		unit = Vector3f::Cross( temp, vnorm );
		unit.Normalize();
	}

	float sliceStep = static_cast<float>( 2.0 * GLYPH_PI ) / static_cast<float>( slices );
 
	//float uStep = 2.0f / static_cast<float>( slices );
	//float vStep = 2.0f / static_cast<float>( stacks );

	// Generate all of the vertices according to the specified input parameters.

	unsigned int baseVertex = m_pGeometry->GetVertexCount();

	Matrix3f r;

	// Add the center vertex first...
 
	AddVertex( center, vnorm );

	for ( unsigned int slice = 0; slice <= slices; slice++ ) {
 
		float theta = sliceStep * static_cast<float>( slice );

		r.RotationEuler( vnorm, theta );
 		Vector3f position = center + ( r * unit * radius );
 
		// TODO: Figure out how best to interpret texture coordinates for this!
 
		//Vector2f texcoords;
		//texcoords.x = uStep * static_cast<float>( slice );
		//texcoords.y = vStep * static_cast<float>( stack );
 
		AddVertex( position, vnorm/*, texcoords*/ );
	}
 
    // Generate all of the indices according to the specified input parameters.

	for ( unsigned int x = 1; x <= slices; x++ ) {
 		m_pGeometry->AddIndex( baseVertex + 0 );
 		m_pGeometry->AddIndex( baseVertex + x );
 		m_pGeometry->AddIndex( baseVertex + x+1 );
	}
}
//--------------------------------------------------------------------------------
void GeometryActor::DrawCylinder( const Vector3f& p1, const Vector3f& p2, float r1, float r2, unsigned int stacks, unsigned int slices )
{
	Cone3f cone( p1, r1, p2, r2 );

	if ( stacks < 2 ) stacks = 2;
	if ( slices < 4 ) slices = 4;

	unsigned int baseVertex = m_pGeometry->GetVertexCount();

	// Build up a vertex evaluator, which will allow us to 'sample' a vertex's
	// attributes in a particular location.  In this case, we are defining an
	// evaluator that works in a 2D space.  In addition, we are provide the
	// evaluator with a Cone3f to be it's 'model' and we tell it its vertex
	// type - both through template parameters.

	VertexEvaluator2f< Cone3f, BasicVertexDX11::Vertex > evaluator;

	// Once the evaluator is created, then we add some AttributeEvaluators to 
	// it.  These objects are also templated according to the model and vertex
	// types, and they provide an 'evaluate' method to calculate a single vertex 
	// attribute that attribute is written directly into a passed in vertex.  We
	// add them to a list, and just iterate over it to create a complete vertex.

	evaluator.Evaluators.push_back( std::make_shared<SurfacePositionFromCone<BasicVertexDX11::Vertex>>() );
	evaluator.Evaluators.push_back( std::make_shared<SurfaceNormalFromCone<BasicVertexDX11::Vertex>>() );
	evaluator.Evaluators.push_back( std::make_shared<TexcoordsFromCone<BasicVertexDX11::Vertex>>() );
	evaluator.Evaluators.push_back( std::make_shared<ConstantColorFromCone<BasicVertexDX11::Vertex>>( m_Color ) );

	// Now we provide the evaluator with the instance of its model type.  This will
	// be passed to the attribute evaluators so they can determine what value they
	// should have.

	evaluator.SetModel( cone );

	// Next we create a tessellator object, and provide our vertex evaluator type
	// and the receiving object type as template parameters.  These two types 
	// basically describe the 'vertex producer' and the 'vertex consumer'.  The
	// tessellator also takes a few parameters to define how we want it to do the
	// sampling over its space.

	GridTessellator2f<
		VertexEvaluator2f< Cone3f, BasicVertexDX11::Vertex >, 
		DrawIndexedExecutorDX11<BasicVertexDX11::Vertex>
	> tessellator( stacks, slices );
	
	// The tessellator then fires off its activities using the supplied vertex
	// attribute evaluator and containers.  We simply specify the parameters 
	// indicating the ranges we want to sweep through the 2D manifold and the
	// rest is done for us.  In this case, we are tessellating a sphere, so we
	// pass in [0,2*pi] for theta and [0,pi] for phi.

	tessellator.TessellateTriGrid<BasicVertexDX11::Vertex>( 
		evaluator, *m_pGeometry,
		0.0f, static_cast<float>( 2.0 * GLYPH_PI ),
		0.0f, 1.0f );
}
//--------------------------------------------------------------------------------
//
//void GeometryActor::DrawCylinder( const Vector3f& p1, const Vector3f& p2, float r1, float r2, unsigned int stacks, unsigned int slices )
//{
//	if ( stacks < 2 ) stacks = 2;
//	if ( slices < 4 ) slices = 4;
//
//	// Define the axis of the cylinder, and find its length.
//	Vector3f v = p1 - p2;
//	Vector3f vnorm = Vector3f::Normalize( v );
//	float l = v.Magnitude();
//
//	Vector3f up = Vector3f( 0.0f, 1.0f, 0.0f );
//
//	// Find the unit vector perpendicular to the axis, that is pointing closest to the (0,1,0) direction.
//	// If the axis points in the (0,1,0) direction, then use (1,0,0) as the unit vector.
//	Vector3f unit;
//	if ( vnorm == up || vnorm == -up ) {
//		unit = Vector3f( 1.0f, 0.0f, 0.0f );
//	} else {
//		Vector3f temp = Vector3f::Cross( vnorm, up );
//		unit = Vector3f::Cross( temp, vnorm );
//		unit.Normalize();
//	}
//
//	float sliceStep = static_cast<float>( 2.0 * GLYPH_PI ) / static_cast<float>( slices );
//	float stackStep = l / static_cast<float>( stacks );
//	float radiusStep = ( r1 - r2 ) / static_cast<float>( stacks );
//
//	float uStep = 2.0f / static_cast<float>( slices );
//	float vStep = 2.0f / static_cast<float>( stacks );
//
//	float normalSlope = ( r2 - r1 ) / l;
//
//	// Generate all of the vertices according to the specified input parameters.
//
//	unsigned int baseVertex = m_pGeometry->GetVertexCount();
//	Matrix3f r;
//
//	for ( unsigned int stack = 0; stack <= stacks; stack++ ) {
//		for ( unsigned int slice = 0; slice <= slices; slice++ ) {
//
//			float theta = sliceStep * static_cast<float>( slice );
//			float height = stackStep * static_cast<float>( stack );
//			float currentRadius = r2 + radiusStep * static_cast<float>( stack );
//
//			// Calculate the position, which is a combination of an offset to the p2 endpoint,
//			// shifted along the axis of the cylinder, and a rotated and scaled radius component.
//			r.RotationEuler( vnorm, theta );
//			Vector3f position = p2 + ( vnorm * height ) + ( r * unit * currentRadius );
//
//			// Calculate the normal vector, which is dependent on the delta in radii at each
//			// end of the cylinder.
//			Vector3f normal = r * unit * currentRadius + vnorm * normalSlope * currentRadius;
//			normal.Normalize();
//
//			Vector2f texcoords;
//			texcoords.x = uStep * static_cast<float>( slice );
//			texcoords.y = vStep * static_cast<float>( stack );
//
//			AddVertex( position, normal, texcoords );
//		}
//	}
//
//	// Generate all of the indices according to the specified input parameters.
//
//	for ( unsigned int z = 0; z < stacks; z++ ) {
//		for ( unsigned int x = 0; x < slices; x++ ) {
//
//			m_pGeometry->AddIndex( baseVertex + (z+0)*(slices+1) + x+0 );
//			m_pGeometry->AddIndex( baseVertex + (z+0)*(slices+1) + x+1 );
//			m_pGeometry->AddIndex( baseVertex + (z+1)*(slices+1) + x+0 );
//
//			m_pGeometry->AddIndex( baseVertex + (z+0)*(slices+1) + x+1 );
//			m_pGeometry->AddIndex( baseVertex + (z+1)*(slices+1) + x+1 );
//			m_pGeometry->AddIndex( baseVertex + (z+1)*(slices+1) + x+0 );
//		}
//	}
//
//}
//--------------------------------------------------------------------------------
void GeometryActor::DrawBox( const Vector3f& center, const Vector3f& xdir, const Vector3f& ydir, const Vector3f& zdir, const Vector3f& extents )
{
	// Generate all of the vertices according to the specified input parameters.

	unsigned int baseVertex = m_pGeometry->GetVertexCount();

	Vector3f position;
	Vector3f normal;
	Vector2f tex;

	Vector3f x = xdir * extents.x;
	Vector3f y = ydir * extents.y;
	Vector3f z = zdir * extents.z;

	Vector2f tex00 = Vector2f( 0.0f, 0.0f );
	Vector2f tex01 = Vector2f( 0.0f, 1.0f );
	Vector2f tex10 = Vector2f( 1.0f, 0.0f );
	Vector2f tex11 = Vector2f( 1.0f, 1.0f );

	// Front face (+Z)
	DrawRect( center + z, xdir, ydir, Vector2f( extents.x, extents.y ) );

	// Back face (-Z)
	DrawRect( center - z, -xdir, ydir, Vector2f( extents.x, extents.y ) );

	// Right face (+X)
	DrawRect( center + x, -zdir, ydir, Vector2f( extents.z, extents.y ) );

	// Left face (-X)
	DrawRect( center - x, zdir, ydir, Vector2f( extents.z, extents.y ) );

	// Upper face (+Y)
	DrawRect( center + y, xdir, -zdir, Vector2f( extents.x, extents.z ) );

	// Lower face (-Y)
	DrawRect( center - y, xdir, zdir, Vector2f( extents.x, extents.z ) );

}
//--------------------------------------------------------------------------------
void GeometryActor::DrawBox( const Vector3f& center, const Vector3f& extents )
{
	DrawBox( center, Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ), Vector3f( 0.0f, 0.0f, 1.0f ), extents );
}
//--------------------------------------------------------------------------------
void GeometryActor::DrawRect( const Vector3f& center, const Vector3f& xdir, const Vector3f& ydir, const Vector2f& extents )
{
	// Generate all of the vertices according to the specified input parameters.

	unsigned int baseVertex = m_pGeometry->GetVertexCount();

	Vector3f position;
	Vector3f normal;
	Vector2f tex;

	Vector3f x = xdir * extents.x;
	Vector3f y = ydir * extents.y;

	Vector2f tex00 = Vector2f( 0.0f, 0.0f );
	Vector2f tex01 = Vector2f( 0.0f, 1.0f );
	Vector2f tex10 = Vector2f( 1.0f, 0.0f );
	Vector2f tex11 = Vector2f( 1.0f, 1.0f );

	// Use a common normal vector for each vertex
	normal = Vector3f::Cross( xdir, ydir );

	// Top left vertex
	position = center + x + y;
	tex = tex00;
	AddVertex( position, normal, tex );

	// Top right vertex
	position = center - x + y;
	tex = tex10;
	AddVertex( position, normal, tex );

	// Bottom right vertex
	position = center - x - y;
	tex = tex11;
	AddVertex( position, normal, tex );

	// Bottom right vertex
	position = center + x - y;
	tex = tex01;
	AddVertex( position, normal, tex );


	// Add two triangles to create the rectangular face.

	m_pGeometry->AddIndex( baseVertex + 0 );
	m_pGeometry->AddIndex( baseVertex + 1 );
	m_pGeometry->AddIndex( baseVertex + 2 );

	m_pGeometry->AddIndex( baseVertex + 0 );
	m_pGeometry->AddIndex( baseVertex + 2 );
	m_pGeometry->AddIndex( baseVertex + 3 );

}
//--------------------------------------------------------------------------------
void GeometryActor::UseSolidMaterial()
{
	GetBody()->SetMaterial( m_pSolidMaterial );
}
//--------------------------------------------------------------------------------
void GeometryActor::UseTexturedMaterial( ResourcePtr texture )
{
	GetBody()->SetMaterial( m_pTexturedMaterial );

	// Set the texture to be used if the passed in texture is not null...
	if ( nullptr != texture ) {

		ShaderResourceParameterWriterDX11* pWriter = 
			dynamic_cast<ShaderResourceParameterWriterDX11*> (
				m_pTexturedMaterial->Parameters.GetRenderParameter( std::wstring( L"ColorTexture" ) )
			);

		pWriter->SetValue( texture );
	}
}
//--------------------------------------------------------------------------------