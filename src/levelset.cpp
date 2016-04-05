#include <boost/smart_ptr/shared_ptr.hpp>
#include <openvdb/Grid.h>
#include <openvdb/io/File.h>
#include <openvdb/math/Coord.h>
#include <openvdb/math/Vec3.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tree/TreeIterator.h>
#include <openvdb/Types.h>
#include <vector>

#include "../../../../Documents/openvdb/viewer/Viewer.h"

using namespace openvdb;
using namespace std;
using namespace openvdb_viewer;

float constantForce() {
	//Force in Newtons
	float k = -10;
	return k;
}

float gaussianForce() {
	/// For bumps, inside and outside ///
	float sigma = 1;
//	F = exp −||p−p 0 ||/2σ	2
	float k = 1;
	return k;
}

void createAndWriteGrid() {
	float backgroundValue = 2.0;
	FloatGrid::Ptr grid = FloatGrid::create(backgroundValue);

	CoordBBox indexBB(Coord(-20, -20, -20), Coord(20, 20, 20));

	grid = tools::createLevelSetSphere<FloatGrid>(
	/*radius=*/100.0, /*center=*/Vec3f(1.5, 2, 3),
	/*voxel size=*/1.5, /*width=*/0.5); //METERS

	grid->setName("LevelSetSphere");

	io::File file("grids.vdb");

	GridPtrVec grids;
	grids.push_back(grid);

	file.write(grids);
	file.close();
}

void modifyGrid() {
	io::File file("grids.vdb");
	file.open();
	GridBase::Ptr baseGrid;
	baseGrid = file.readGrid("LevelSetSphere");

	file.close();

	// "LevelSetSphere" is a FloatGrid,
	// so cast the generic grid pointer to a FloatGrid pointer.
	FloatGrid::Ptr grid = gridPtrCast<FloatGrid>(baseGrid);

	// Visit and update all of the grid's active values, which correspond to
	// voxels on the narrow band.

	math::Coord coord;
	coord.setX(41);
	coord.setY(56);
	coord.setZ(8);

	math::Coord coord1;
	coord1.setX(51);
	coord1.setY(66);
	coord1.setZ(18);

	math::Coord coord_;

	float F = constantForce();
	float F = gaussianForce();

	for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
		float dist = iter.getValue();
		coord_ = iter.getCoord();
		if (coord_ > coord && coord_ < coord1) {
			iter.setValue(dist - F);
		}
	}

	/*
	 x[0] = initial_narrow_band_voxel-value;
	 for (t = 0; t < 100; ++t) { 100s
	 x[t] = x[t-1]  - F;

	 //construct a grid, display it.
	 }
	 */

	io::File output_file("new_grids.vdb");

	GridPtrVec grids;
	grids.push_back(grid);

	output_file.write(grids);
	output_file.close();
}

void displayGrid() {
	openvdb_viewer::Viewer viewer = openvdb_viewer::init("Test", /*bg=*/
	false);

	GridCPtrVec allGrids;

	// Load VDB files.
	io::File file("new_grids.vdb");
	file.open();

	GridPtrVecPtr grids = file.getGrids();
	allGrids.insert(allGrids.end(), grids->begin(), grids->end());

	viewer.open();
	viewer.view(allGrids);

	openvdb_viewer::exit();
}

int main() {
	// Initialize the OpenVDB library.  This must be called at least
	// once per program and may safely be called multiple times.
	initialize();
	// Create an empty floating-point grid with background value 0.
//	createAndWriteGrid();

//	modifyGrid();
	displayGrid();
}

