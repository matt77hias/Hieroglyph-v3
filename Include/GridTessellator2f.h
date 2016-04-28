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
// GridTessellator2f
//
// This class facilitates the generation of vertices and indices that span a grid
// over a 2D manifold.  The grid is simply defined by a pair of integer values
// that indicate the number of rows and columns in the grid.  Then the vertices
// are produced at each grid point, and the indices are added to indicate the 
// primitive connections between vertices.
//--------------------------------------------------------------------------------
#ifndef GridTessellator2f_h
#define GridTessellator2f_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "DrawIndexedExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class TEvaluator, class TContainer>
	class GridTessellator2f
	{
	public:
		GridTessellator2f( unsigned int rows, unsigned int cols ) :
			m_rows( rows ),
			m_cols( cols ) {};

		template <class TVertex>
		void GenerateVertices( TEvaluator& e, TContainer& c, float xmin, float xmax, float ymin, float ymax )
		{
			float colStep = static_cast<float>( xmax-xmin ) / static_cast<float>( m_cols );
			float rowStep = static_cast<float>( ymax-ymin ) / static_cast<float>( m_rows );

			// Iterate over the grid and add a vertex to the container at each
			// corresponding point.

			TVertex v;

			for ( unsigned int row = 0; row <= m_rows; row++ ) {
				for ( unsigned int col = 0; col <= m_cols; col++ ) {

					float x = colStep * static_cast<float>( col );
					float y = rowStep * static_cast<float>( row );

					e.evaluate( v, x, y );
					c.AddVertex( v );
				}
			}
		};

		template <class TVertex>
		void TessellateTriGrid( TEvaluator& e, TContainer& c, float xmin, float xmax, float ymin, float ymax )
		{
			// Grab the starting vertex of the container, to ensure we can generate
			// our primitives properly.

			unsigned int baseVertex = c.GetVertexCount();

			// Generate the vertices at all grid points.

			GenerateVertices<TVertex>( e, c, xmin, xmax, ymin, ymax );

			// Generate all of the indices needed for the primitive connectivity 
			// according to the desired grid size.

			for ( unsigned int z = 0; z < m_rows; z++ ) {
				for ( unsigned int x = 0; x < m_cols; x++ ) {

					c.AddIndex( baseVertex + (z+0)*(m_cols+1) + x+0 );
					c.AddIndex( baseVertex + (z+0)*(m_cols+1) + x+1 );
					c.AddIndex( baseVertex + (z+1)*(m_cols+1) + x+0 );

					c.AddIndex( baseVertex + (z+0)*(m_cols+1) + x+1 );
					c.AddIndex( baseVertex + (z+1)*(m_cols+1) + x+1 );
					c.AddIndex( baseVertex + (z+1)*(m_cols+1) + x+0 );
				}
			}
		};

		template <class TVertex>
		void TessellateLineGrid( TEvaluator& e, TContainer& c, float xmin, float xmax, float ymin, float ymax )
		{
			// Since we will be using lines as our primitives, configure the 
			// container accordingly.

			c.SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

			// Grab the starting vertex of the container, to ensure we can generate
			// our primitives properly, then generate the vertices at all grid points.

			unsigned int baseVertex = c.GetVertexCount();
			GenerateVertices<TVertex>( e, c, xmin, xmax, ymin, ymax );

			// Produce grid lines horizontally first, followed by vertical afterwards.

			for ( unsigned int z = 0; z < m_rows+1; z++ ) {
				for ( unsigned int x = 0; x < m_cols; x++ ) {

					c.AddIndex( baseVertex + (z+0)*(m_cols+1) + x+0 );
					c.AddIndex( baseVertex + (z+0)*(m_cols+1) + x+1 );
				}
			}

			for ( unsigned int x = 0; x < m_cols+1; x++ ) {
				for ( unsigned int z = 0; z < m_rows; z++ ) {

					c.AddIndex( baseVertex + (z+0)*(m_cols+1) + x+0 );
					c.AddIndex( baseVertex + (z+1)*(m_cols+1) + x+0 );
				}
			}
		};

		// Update the grid size to be used during tessellation.

		void SetGridSize( unsigned int rows, unsigned int cols ) {
			
			assert( rows > 0 );
			assert( cols > 0 );
			
			m_rows = rows;
			m_cols = cols;
		};

	protected:
		unsigned int m_rows;
		unsigned int m_cols;
	};
};
//--------------------------------------------------------------------------------
#endif // GridTessellator2f_h
