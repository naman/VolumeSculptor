//
// Created by gautam on 23/3/16.
//

#include <openvdb/openvdb.h>
#include <openvdb/math/Math.h>
#include <openvdb/Types.h>
#include <openvdb/tree/NodeManager.h>
#include <openvdb/tools/SignedFloodFill.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <iostream>

using namespace openvdb;

// // Populate the given grid with a narrow-band level set representation of a sphere.
// // The width of the narrow band is determined by the grid's background value.
// // (Example code only; use tools::createSphereSDF() in production.)
// template<class GridType> void makeSphere(GridType& grid, float radius, const openvdb::Vec3f& c)
// {
//     typedef typename GridType::ValueType ValueT;
//     // Distance value for the constant region exterior to the narrow band
//     const ValueT outside = grid.background();
//     // Distance value for the constant region interior to the narrow band
//     // (by convention, the signed distance is negative in the interior of
//     // a level set)
//     const ValueT inside = -outside;
//     // Use the background value as the width in voxels of the narrow band.
//     // (The narrow band is centered on the surface of the sphere, which
//     // has distance 0.)
//     int padding = int(openvdb::math::RoundUp(openvdb::math::Abs(outside)));
//     // The bounding box of the narrow band is 2*dim voxels on a side.
//     int dim = int(radius + padding);
//     // Get a voxel accessor.
//     typename GridType::Accessor accessor = grid.getAccessor();
//     // Compute the signed distance from the surface of the sphere of each
//     // voxel within the bounding box and insert the value into the grid
//     // if it is smaller in magnitude than the background value.
//     openvdb::Coord ijk;
//     int &i = ijk[0], &j = ijk[1], &k = ijk[2];
//     for (i = c[0] - dim; i < c[0] + dim; ++i) {
//         const float x2 = openvdb::math::Pow2(i - c[0]);
//         for (j = c[1] - dim; j < c[1] + dim; ++j) {
//             const float x2y2 = openvdb::math::Pow2(j - c[1]) + x2;
//             for (k = c[2] - dim; k < c[2] + dim; ++k) {
//                 // The distance from the sphere surface in voxels
//                 const float dist = openvdb::math::Sqrt(x2y2
//                     + openvdb::math::Pow2(k - c[2])) - radius;
//                 // Convert the floating-point distance to the grid's value type.
//                 ValueT val = ValueT(dist);
//                 // Only insert distances that are smaller in magnitude than
//                 // the background value.
//                 if (val < inside || outside < val) continue;
//                 // Set the distance for voxel (i,j,k).
//                 accessor.setValue(ijk, val);
//             }
//         }
//     }
//     // Propagate the outside/inside sign information from the narrow band
//     // throughout the grid.
//     tools::signedFloodFill(grid);
// }


// void makeCylinder(FloatGrid::Ptr grid, float radius, const CoordBBox& indexBB, double h, float backgroundValue)
// {
//   typename FloatGrid::Accessor accessor = grid->getAccessor();

//   // outputGrid voxel sizes
//   for (Int32 i = indexBB.min().x(); i <= indexBB.max().x(); ++i) {
//     for (Int32 j = indexBB.min().y(); j <= indexBB.max().y(); ++j) {
//       for (Int32 k = indexBB.min().z(); k <= indexBB.max().z(); ++k) {
//         Vec3d p(i * h, j * h, k * h);

//         float distance = sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z()) - radius;

//         if (fabs(distance) < backgroundValue)
//           accessor.setValue(Coord(i, j, k), distance);
//       }
//     }
//   }
//   tools::signedFloodFill(grid);
//   grid->setTransform(openvdb::math::Transform::createLinearTransform(h));
// }

void createAndWriteGrid()
{
    float backgroundValue = 2.0;
    openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(backgroundValue);

    CoordBBox indexBB(Coord(-20, -20, -20), Coord(20, 20, 20));
    //makeCylinder(grid, 5.0f, indexBB, 0.5, backgroundValue);

    grid =   openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(
            /*radius=*/50.0, /*center=*/openvdb::Vec3f(1.5, 2, 3),
            /*voxel size=*/0.5, /*width=*/4.0);

    grid->setName("LevelSetSphere");

    openvdb::io::File file("/home/gautam/Desktop/mygrids.vdb");

    openvdb::GridPtrVec grids;
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




































