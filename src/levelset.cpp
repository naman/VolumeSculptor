#include <boost/smart_ptr/shared_ptr.hpp>
#include <openvdb/Grid.h>
#include <openvdb/io/File.h>
#include <openvdb/math/Coord.h>
#include <openvdb/math/Vec3.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tools/LevelSetAdvect.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VelocityFields.h>
#include <openvdb/tree/TreeIterator.h>
#include <openvdb/Types.h>
#include <stddef.h>
#include <vector>

using namespace openvdb;
using namespace std;

FloatGrid::Ptr readGrid() {
	io::File file("grids.vdb");
	file.open();
	GridBase::Ptr baseGrid;
	baseGrid = file.readGrid("LevelSetSphere");

	file.close();

	// "LevelSetSphere" is a FloatGrid,
	// so cast the generic grid pointer to a FloatGrid pointer.
	FloatGrid::Ptr grid = gridPtrCast<FloatGrid>(baseGrid);

	return grid;
}

float constantForce() {
	//Force in Newtons
	float k = -10;
	return k;
}

/*float gaussianForce() {
 /// For bumps, inside and outside ///
 float sigma = 1;
 float k = 1;
 float mean = 1;

 random_device rd;
 mt19937 gen(rd());

 // values near the mean are the most likely
 // standard deviation affects the dispersion of generated values from the mean
 normal_distribution<> d(mean, sigma);


 return k;
 }*/

void createAndWriteGrid() {
	float backgroundValue = 2.0;
	FloatGrid::Ptr grid = FloatGrid::create(backgroundValue);

	CoordBBox indexBB(Coord(-20, -20, -20), Coord(20, 20, 20));

	grid = tools::createLevelSetSphere<FloatGrid>(
	/*radius=*/100.0, /*center=*/Vec3f(1.5, 2, 3),
	/*voxel size=*/1.5, /*width=*/2); //METERS

	grid->setName("LevelSetSphere");

	io::File file("grids.vdb");

	GridPtrVec grids;
	grids.push_back(grid);

	file.write(grids);
	file.close();
}


void writeGrid(FloatGrid::Ptr grid) {
	io::File output_file("new_grids.vdb");

	GridPtrVec grids;
	grids.push_back(grid);

	output_file.write(grids);
	output_file.close();
}


float fetchGridValue(FloatGrid::Ptr grid, int x, int y, int z) {

	// Request a value accessor for accelerated access.
	// (Because value accessors employ a cache, it is important to declare
	// one accessor per thread.)
	FloatGrid::ConstAccessor accessor = (*grid).getConstAccessor();

	// Instantiate the GridSampler template on the accessor type and on
	// a box sampler for accelerated trilinear interpolation.
	tools::GridSampler<FloatGrid::ConstAccessor, tools::BoxSampler> fastSampler(
			accessor, (*grid).transform());

	// Compute the value of the grid at a location in index space.
	float worldValue = fastSampler.isSample(openvdb::Vec3R(x, y, z));
	return worldValue;
}

void modifyGrid() {
	FloatGrid::Ptr grid = readGrid();

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
	//float F = gaussianForce();

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
	writeGrid(grid);
}

/*
 void displayGrid() {
 //openvdb_viewer::Viewer viewer = openvdb_viewer::init("Test",
 //false);

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
 */


void selectROI() {

}

void advectGrid() {
	FloatGrid::Ptr grid = readGrid();
	tools::DiscreteField<tools::StaggeredBoxSampler> (*field);

	tools::LevelSetAdvection<FloatGrid, tools::DiscreteField, tools::InterruptT>(grid, field, NULL);
	writeGrid(grid);

}

int main() {
	// Initialize the OpenVDB library.  This must be called at least
	// once per program and may safely be called multiple times.
	initialize();

	advectGrid();

	// Create an empty floating-point grid with background value 0.
//	createAndWriteGrid();

//	createField();
//	modifyGrid();
//	displayGrid();
//	selectROI();
}

