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

using namespace openvdb;

void createAndWriteGrid() {
	float backgroundValue = 2.0;
	FloatGrid::Ptr grid = FloatGrid::create(backgroundValue);

	CoordBBox indexBB(Coord(-20, -20, -20), Coord(20, 20, 20));

	grid = tools::createLevelSetSphere<FloatGrid>(
	/*radius=*/100.0, /*center=*/Vec3f(1.5, 2, 3),
	/*voxel size=*/1.5, /*width=*/2.0);

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

	// Convert the level set sphere to a narrow-band fog volume, in which
	// interior voxels have value 1, exterior voxels have value 0, and
	// narrow-band voxels have values varying linearly from 0 to 1.

	float outside = grid->background(); //2
	float width = 2.0 * outside;       //4. Why are we doing this?

	// Visit and update all of the grid's active values, which correspond to
	// voxels on the narrow band.
	// This can be used over the timestep to update the active voxels in narrow
	// band and apply force according to the equation.
	for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
		float dist = iter.getValue();
		iter.setValue((outside - dist) / width); //((2-dist)/4
	}

	// Visit all of the grid's inactive tile and voxel values and update the values
	// that correspond to the interior region.
	for (FloatGrid::ValueOffIter iter = grid->beginValueOff(); iter; ++iter) {
		if (iter.getValue() < 0.0) {
			iter.setValue(1.0);
			iter.setValueOff();
		}
		if (iter.getValue() > 0.0) {
			iter.setValue(0.0);
			iter.setValueOff();
		}
	}
	// Set exterior voxels to 0.
//	grid->setBackground(0.0);

//	grid->setName("LevelSetSphere");

	io::File output_file("new_grids.vdb");

	GridPtrVec grids;
	grids.push_back(grid);

	output_file.write(grids);
	output_file.close();
}

int main() {
	// Initialize the OpenVDB library.  This must be called at least
	// once per program and may safely be called multiple times.
	initialize();
	// Create an empty floating-point grid with background value 0.
//	createAndWriteGrid();

	modifyGrid();

}

