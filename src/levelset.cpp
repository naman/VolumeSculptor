#include <boost/smart_ptr/shared_ptr.hpp>
#include <openvdb/Grid.h>
#include <openvdb/io/File.h>
#include <openvdb/math/Coord.h>
#include <openvdb/math/Vec3.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/Types.h>
#include <vector>

using namespace openvdb;

void createAndWriteGrid()
{
    float backgroundValue = 2.0;
    FloatGrid::Ptr grid = FloatGrid::create(backgroundValue);

    CoordBBox indexBB(Coord(-20, -20, -20), Coord(20, 20, 20));

    grid =   tools::createLevelSetSphere<FloatGrid>(
            /*radius=*/50.0, /*center=*/ Vec3f(1.5, 2, 3),
            /*voxel size=*/0.5, /*width=*/4.0);

    grid->setName("LevelSetSphere");

    io::File file("mygrids.vdb");

    GridPtrVec grids;
    grids.push_back(grid);

    file.write(grids);
    file.close();
}



int main()
{
    // Initialize the OpenVDB library.  This must be called at least
    // once per program and may safely be called multiple times.
    initialize();
    // Create an empty floating-point grid with background value 0.
    createAndWriteGrid();

}




































