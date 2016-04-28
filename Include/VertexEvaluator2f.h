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
// VertexEvaluator2f
//
// The vertex evaluator collects a number of AttributeEvaluator2f instances and
// then applies them all based on a given 'model'.  This allows the users of this
// class to use it without direct knowledge of which model it is using.
//--------------------------------------------------------------------------------
#ifndef VertexEvaluator2f_h
#define VertexEvaluator2f_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "AttributeEvaluator2f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class TModel, class TVertex>
	class VertexEvaluator2f
	{
	public:
		void SetModel( TModel& model ) {
			m_Model = model;
		};

		void evaluate( TVertex& v, float x, float y ) {
			for ( auto& pEvaluator : Evaluators ) {
				pEvaluator.evaluate( m_Model, v, x, y );
			}
		};

		std::vector< Evaluator< TModel, TVertex > > Evaluators;

	protected:
		TModel m_Model;
	};
};
//--------------------------------------------------------------------------------
#endif // VertexEvaluator2f_h
//--------------------------------------------------------------------------------