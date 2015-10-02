//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//
#ifndef DISABLE_OCTREE
#ifndef __PATCHMAN_PATCH_OCTREE_HPP__
#define __PATCHMAN_PATCH_OCTREE_HPP__

/*! \file */

#include "patch.hpp"

#include "Class_Para_Tree.hpp"

#include <assert.h>
#include <cstddef>
#include <vector>

namespace pman {

struct OctreeLevelInfo{
    int    level;
    double h;
    double area;
    double volume;
};

class PatchOctree : public Patch {

public:
	PatchOctree(const int &id, const int &dimension, std::array<double, 3> origin,
			double length, double dh);

	~PatchOctree();

	long get_cell_octant(const long &id) const;
	int get_cell_level(const long &id);

	/*!
		\brief Gets the octree associated with the patch.

		\tparam dimension is the dimension of the octree. It MUST match the dimension
		of the patch.
		\result A pointer to the octree associated to the patch.
	*/
	template<int dimension>
	Class_Para_Tree<dimension> * get_tree()
	{
		assert(dimension == get_dimension());

		if (dimension == 2) {
			return dynamic_cast<Class_Para_Tree<dimension> *>(&m_tree_2D);
		} else {
			return dynamic_cast<Class_Para_Tree<dimension> *>(&m_tree_3D);
		}
	}

protected:
	std::array<double, 3> & _get_opposite_normal(std::array<double, 3> &normal);
	bool _update(vector<uint32_t> &cellMapping);
	bool _mark_cell_for_refinement(const long &id);
	bool _mark_cell_for_coarsening(const long &id);
	bool _enable_cell_balancing(const long &id, bool enabled);

private:
	long m_nInternalCells;
	long m_nGhostCells;

	std::unordered_map<long, long, Element::IdHasher> m_cell_to_octant;

	Class_Para_Tree<2> m_tree_2D;
	Class_Para_Tree<3> m_tree_3D;

	vector<double> m_tree_dh;
	vector<double> m_tree_area;
	vector<double> m_tree_volume;

	std::vector<std::array<double, 3> > m_normals;

	void update_vertices();
	void import_vertices();
	void reload_vertices();

	void update_cells();
	void import_cells();
	void reload_cells();

	void update_interfaces();
	void import_interfaces();
	void reload_interfaces();
};

}

#endif
#endif