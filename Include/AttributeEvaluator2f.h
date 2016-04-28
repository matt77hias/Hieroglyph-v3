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
// Evaluator
//
// This class is based on the talk "Inheritance Is The Base Class of Evil" by
// Sean Parent at the Going Native 2013 C++ conference, available for free viewing here:
// http://channel9.msdn.com/Events/GoingNative/2013/Inheritance-Is-The-Base-Class-of-Evil
// 
// The main concept is anonymize an input concrete object as long as it provides 
// an appropriate interface desired for this functionality.  In this case, we are
// specifying that an Evaluator instance can wrap around any object that provides
// an evaluate method with the signature of the appropriate 'model' to evaluate
// and a vertex type, plus the (x,y) location to evaluate at.
//
// This Evaluator template is specialized over the model and vertex types, letting
// the user choose the vertex type and object to be tessellated as needed for a
// given situation.  Then creating a list of evaluator instances for building a 
// custom vertex becomes very easy, and in fact only requires the user to provide
// a std::vector< Evaluator< ModelType, VertexType > > and then push back any
// particular elements that you want.  This is a very natural way to work with the
// evaluators, and allows for a very flexible input object type choice.
//--------------------------------------------------------------------------------
#ifndef AttributeEvaluator2f_h
#define AttributeEvaluator2f_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// The class template is parameterized by the model (input data type) and the 
	// vertex (output data type).  This ensures that only the right type of 
	// objects are compatible with the Evaluator (i.e. ones that use the right 
	// model and produce the right type of vertex).

	template <class TModel, class TVertex>
	class Evaluator
	{
	public:
		// The public constructor parameterized by the concrete input object type.
		// This allows Evaluator instances to be created by template parameter 
		// deduction without requiring the user to explicitly name the type. This
		// initializes the 'm_self' member a shared_ptr to our internal model 
		// type, which is explained further below.
		template <typename T>
		Evaluator( T x ) : m_self( std::make_shared< model< T > >( std::move( x ) ) ) { }

		// The only other public method provides the interface that we want all
		// of the Evaluator objects to have.  This is just a pass through method
		// to the wrapped input object.
		void evaluate( TModel& model, TVertex& v, float x, float y ) {
			m_self->evaluate( model, v, x, y );
		}

	private:
		// An abstract base class that gives us a common way to refer to our input
		// object.  This is actually providing the basis for polymorphically 
		// accessing that input object.
		struct concept
		{
			virtual ~concept() { };
			virtual void evaluate( TModel& model, TVertex& v, float x, float y ) const = 0;
		};

		// This template class specializes our concept class, and is parameterized
		// by the input object type.  Generally speaking, this class forms a mini-
		// inheritance hierarchy, but it is totally transparent to the user.  He
		// doesn't need to inherit from any particular base class.  This makes 
		// things much easier on the client side.
		template <typename T>
		struct model : public concept 
		{
			// The constructor moves the input object (if possible) to its internal
			// copy.
			model( T x ) :
				m_data( std::move( x ) ) { }

			// The only interface available for interacting with the original object
			// is provided by the polymorphic base concept class.
			virtual void evaluate( TModel& model, TVertex& v, float x, float y ) const {
				m_data.evaluate( model, v, x, y );
			}

			// The model keeps a *copy* of the original data.
			T m_data;
		};

		// We reference the internal object through a shared pointer to a *const* concept.
		std::shared_ptr<const concept> m_self;
	};
};
//--------------------------------------------------------------------------------
#endif // AttributeEvaluator2f_h
