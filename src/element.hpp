//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//
#ifndef __PATCHMAN_ELEMENT_HPP__
#define __PATCHMAN_ELEMENT_HPP__

/*! \file */

#include <cstddef>
#include <memory>

#include "collapsedArrayArray.hpp"

namespace pman {

class Patch;

class Element {

public:
	enum Type {
	    POINT = 0,
	    LINE,
	    TRIANGLE,
	    QUADRANGLE,
	    POLYGON,
	    TETRAHEDRON,
	    HEXAHEDRON,
	    PYRAMID,
	    PRISM,
	    POLYHEDRON
	};

	/*!
		Hasher for the ids.

		Since ids are unique, the hasher can be a function that
		takes an id and cast it to a size_t.

		The hasher is defined as a struct, because a struct can be
		passed as an object into metafunctions (meaning that the type
		deduction for the template paramenters can take place, and
		also meaning that inlining is easier for the compiler). A bare
		function would have to be passed as a function pointer.
		To transform a function template into a function pointer,
		the template would have to be manually instantiated (with a
		perhaps unknown type argument).

	*/
	struct IdHasher {
		/*!
			Function call operator that casts the specified
			value to a size_t.

			\tparam U type of the value
			\param value is the value to be casted
			\result Returns the value casted to a size_t.
		*/
		template<typename U>
		constexpr std::size_t operator()(U&& value) const noexcept
		{
			return static_cast<std::size_t>(std::forward<U>(value));
		}
	};

	Element();
	Element(const int &id);
	Element(const int &id, Patch *patch);

	Element(Element&& other) = default;
	Element& operator=(Element&& other) = default;

	Patch * get_patch() const;
	void set_patch(Patch *patch);
	int get_patch_dimension() const;

	void set_id(const int &id);
	int get_id() const;
	
	void set_local_id(int id);
	int get_local_id() const;
	
	void set_type(Element::Type type);
	Element::Type get_type() const;
	
	void set_connect(std::unique_ptr<int[]> connect);
	void unset_connect();
	int* get_connect() const;

	void set_centroid(double * const centroid);
	double * get_centroid() const;

	int get_face_count() const;
	static int get_face_count(Element::Type type);

	int get_vertex_count() const;
	static int get_vertex_count(Element::Type type);
	int get_vertex(const int &vertex) const;

protected:
	static const int NULL_ELEMENT_ID;

private:
	Patch *m_patch;

	int m_id;
	int m_local_id;

	double *m_centroid;

	Element::Type m_type;
	
	std::unique_ptr<int[]> m_connect;

	Element(const Element &other) = delete;
	Element& operator = (const Element &other) = delete;

};

}

#endif
