#include <boost/smart_ptr/shared_ptr.hpp>
#include <openvdb/Grid.h>
#include <openvdb/io/File.h>
#include <openvdb/math/Coord.h>
#include <openvdb/math/Transform.h>
#include <openvdb/math/Vec3.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tools/LevelSetAdvect.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/VelocityFields.h>
#include <openvdb/tree/TreeIterator.h>
#include <openvdb/tree/ValueAccessor.h>
#include <openvdb/Types.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace openvdb;
using namespace openvdb::tools;
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

vector<double> gaussianForce(int n, double mean, double stdev) {
	/// For bumps, inside and outside ///
	random_device rd;
	mt19937 gen(rd());

	normal_distribution<> dis(mean, stdev);

	vector<double> v(n);
	for (int i = 0; i < n; ++i) {
		double number = dis(gen);
		cout << "number" << number << endl;
		v[i] = number;
	}
	return v;
}

void createAndWriteGrid() {
	float backgroundValue = 2.0;
	FloatGrid::Ptr grid = FloatGrid::create(backgroundValue);

	CoordBBox indexBB(Coord(-20, -20, -20), Coord(20, 20, 20));

	grid = createLevelSetSphere<FloatGrid>(
	/*radius=*/100.0, /*center=*/Vec3f(1.5, 2, 3),
	/*voxel size=*/1.5, /*width=*/2); //METERS

	grid->setName("LevelSetSphere");

	io::File file("grids.vdb");

	GridPtrVec grids;
	grids.push_back(grid);

	file.write(grids);
	file.close();
}

void writeGrid(FloatGrid::Ptr grid, string name) {
	io::File output_file(name);

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
	GridSampler<FloatGrid::ConstAccessor, BoxSampler> fastSampler(accessor,
			(*grid).transform());

	// Compute the value of the grid at a location in index space.
	float worldValue = fastSampler.isSample(openvdb::Vec3R(x, y, z));
	return worldValue;
}

void modifyGrid() {
	FloatGrid::Ptr grid = readGrid();

	// Visit and update all of the grid's active values, which correspond to
	// voxels on the narrow band.

	Vec3d bck(0, 0, 0);
	Vec3DGrid::Ptr vel = Vec3DGrid::create(bck);
	Vec3dGrid::Accessor accessor = (*vel).getAccessor();

	int x = 41;
	int y = 56;
	int z = 8;
	int r = 10;

	math::Coord min(x - r, y - r, z - r);
	math::Coord max(x + r, y + r, z + r);

	math::CoordBBox box(min, max);

	/*	for (float i = x - r; i < x + r; ++i) {
	 for (float j = y - r; j < y + r; ++j) {
	 for (float k = z - r; k < z + r; ++k) {
	 if ((i - x) * (i - x) + (j - y) * (j - y) + (k - z) * (k - z)
	 - r * r < 0) {
	 math::Coord ijk;
	 ijk.setX(i);
	 ijk.setY(j);
	 ijk.setZ(k);
	 float mag = sqrt(i * i + j * j + k * k);
	 accessor.setValue(ijk, Vec3d(i/mag, j/mag, k/mag));
	 }

	 }
	 }
	 }*/

	math::Coord coord;

	for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
		float dist = iter.getValue();
		coord = iter.getCoord();

		if (box.isInside(coord)) {
			float dist_2 = (coord.x()) * (coord.x()) + (coord.y()) * (coord.y())
					+ (coord.z()) * (coord.z());

			math::Coord ijk;
			ijk.setX(coord.x());
			ijk.setY(coord.y());
			ijk.setZ(coord.z());
			accessor.setValue(ijk, Vec3d(dist_2, dist_2, dist_2));
		}
	}

	DiscreteField<Vec3dGrid> field(*vel);

	LevelSetAdvection<FloatGrid, DiscreteField<Vec3dGrid> > advection(*grid,
			field, NULL);

	cout << advection.advect(0, 4) << endl;

	writeGrid(grid, "adve.vdb");
}

/*
 void modifyUsingGauss() {
 FloatGrid::Ptr grid = readGrid();

 int iSecret;

 initialize random seed:
 srand(time(NULL));

 int n = 100;
 vector<double> normals = gaussianForce(n, 150, 40);

 int x = 41;
 int y = 56;
 int z = 8;

 int r = 20;
 math::Coord min(x - r, y - r, z - r);
 math::Coord max(x + r, y + r, z + r);

 math::CoordBBox box(min, max);
 math::Coord coord;

 for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
 float val = iter.getValue();
 coord = iter.getCoord();

 if (box.isInside(coord)) {
 iSecret = rand() % (n - 1) + 0;
 iter.setValue(val * normals[iSecret]);
 }
 }

 writeGrid(grid);
 }
 */

void volumeAdvectConstantVelocity() {

	///
	/// Uses LevelSetAdvection to advect the grid.
	///

	FloatGrid::Ptr grid = readGrid();

	Vec3DGrid::Ptr vel = Vec3DGrid::create(Vec3d(0, 0, 0));
	Vec3dGrid::Accessor accessor = (*vel).getAccessor();

	int x = 41;
	int y = 56;
	int z = 8;

	int r;
	cout << "Enter Radius of BBox: " << endl;
	cin >> r;

	math::Coord min(x - r, y - r, z - r);
	math::Coord max(x + r, y + r, z + r);

	math::CoordBBox box(min, max);
	math::Coord coord;

	double vx, vy, vz;
	cout << "Enter vectors in (x,y,z) : " << endl;
	cin >> vx >> vy >> vz;

	Vec3d vector(vx, vy, vz);

	vector.normalize();

	cout << vector.x() << vector.y() << vector.z() << endl;
	for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
		float val = iter.getValue();
		coord = iter.getCoord();

		if (box.isInside(coord)) {

			accessor.setValue(coord, vector);
		}
	}

	/*
	 DiscreteField<Vec3dGrid> field(*vel);
	 VolumeAdvection<DiscreteField<Vec3dGrid> > advection(field, NULL);

	 Vec3DGrid::Ptr mask = Vec3DGrid::create(Vec3d(0, 0, 0));

	 advection.advect(grid, 0.1);
	 */

	DiscreteField<Vec3dGrid> field(*vel);

	LevelSetAdvection<FloatGrid, DiscreteField<Vec3dGrid> > advection(*grid,
			field, NULL);

	cout << advection.advect(0, 10) << endl;

	writeGrid(grid, "advect.vdb");
}

void Tokenize(const string& str, vector<string>& tokens,
		const string& delimiters = " ") {
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void convertMeshtoVol() {
	// ifstream is used for reading files
	// We'll read from a file called Sample.dat
	string input;
	cin >> input;

	ifstream inf(input);

	// If we couldn't open the input file stream for reading
	if (!inf) {
		// Print an error and exit
		cerr << "Uh oh, cube.obj could not be opened for reading!" << endl;
		exit(1);
	}
	vector<Vec3f> points;
	vector<Vec3I> quads;

	string strInput;
	float x, y, z;
	string::size_type sz;

	while (inf) {
		// read stuff from the file into a string and print it
		getline(inf, strInput);
		vector<string> tokens;
		Tokenize(strInput, tokens);

		cout << tokens[0] << " " << tokens[1] << " " << tokens[2] << " "
				<< tokens[3] << endl;

		x = stof(tokens[1], &sz);
		y = stof(tokens[2], &sz);
		z = stof(tokens[3], &sz);

		if (strInput[0] == 'v') {
			points.push_back(Vec3f(x, y, z));
		}
		if (strInput[0] == 'f') {
			quads.push_back(Vec3I(x, y, z));
		}
	}

	math::Transform::Ptr xform = math::Transform::createLinearTransform();

	QuadAndTriangleDataAdapter<Vec3f, Vec3I> mesh(points, quads);

	FloatGrid::Ptr grid = tools::meshToVolume<FloatGrid>(mesh, *xform);

	grid = meshToLevelSet<FloatGrid>(*xform, points, quads);

	cout << "output " << endl;
	string output;
	cin >> output;
	writeGrid(grid, output);

	// When inf goes out of scope, the ifstream
	// destructor',gims[ will close the file
}

void volumeAdvectGaussVelocity() {

	///
	/// Uses LevelSetAdvection to advect the grid.
	///

	int n = 50000;

	double mean;
	double dev;

	cout << "Mean and Deviation" << endl;
	cin >> mean >> dev;

	vector<double> normals = gaussianForce(n, mean, dev);

	FloatGrid::Ptr grid = readGrid();

	Vec3DGrid::Ptr vel = Vec3DGrid::create(Vec3d(0, 0, 0));
	Vec3dGrid::Accessor accessor = (*vel).getAccessor();

	int x = 0;
	int y = 0;
	int z = 0;

	int r;
	cout << "Enter Radius of BBox: " << endl;
	cin >> r;

	math::Coord min(x - r, y - r, z - r);
	math::Coord max(x + r, y + r, z + r);

	math::CoordBBox box(min, max);
	math::Coord coord;

	int i = 0;
	for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
		float val = iter.getValue();
		coord = iter.getCoord();
		if (val == 0)
			cout << coord.x() << " " << coord.y() << " " << coord.z();

		if (box.isInside(coord)) {
			double dx = normals[i]; //random number generated using a gaussian distribution
			Vec3d vector(dx, dx, dx);

//			vector.normalize();
			cout << vector.x() << vector.y() << vector.z() << endl;
			i++;
			accessor.setValue(coord, vector); //accessor points to the velocity grid index which contains coord
		}
	}

	DiscreteField<Vec3dGrid> field(*vel);

	LevelSetAdvection<FloatGrid, DiscreteField<Vec3dGrid> > advection(*grid,
			field, NULL);

	cout << advection.advect(0, 10) << endl;

	writeGrid(grid, "advect.vdb");
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

void advectGrid() {
	FloatGrid::Ptr grid = readGrid();

	Vec3d bck(0, 0, 0);
	Vec3DGrid::Ptr vel = Vec3DGrid::create(bck);

	Vec3dGrid::Accessor accessor = (*vel).getAccessor();

	math::Coord coord;

	for (FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
		float dist = iter.getValue();
		//iter.setValue((float)(outside - dist)/(float)width);
		coord = iter.getCoord();
		float dist_2 = (coord.x()) * (coord.x()) + (coord.y()) * (coord.y())
				+ (coord.z()) * (coord.z());

		math::Coord ijk;
		ijk.setX(coord.x());
		ijk.setY(coord.y());
		ijk.setZ(coord.z());
		accessor.setValue(ijk, Vec3d(dist_2, dist_2, dist_2));
	}

	DiscreteField<Vec3dGrid> field(*vel);

	LevelSetAdvection<FloatGrid, DiscreteField<Vec3dGrid> > advection(*grid,
			field, NULL);

	cout << advection.advect(0, 1) << endl;

	writeGrid(grid, "advect.vdb");
}

int main() {
	// Initialize the OpenVDB library.  This must be called at least
	// once per program and may safely be called multiple times.
	initialize();
	//createAndWriteGrid();
	//modifyUsingNormal();
//	selectROI();
//	advectGrid();
//	volumeAdvect();
//	volumeAdvectGaussVelocity();
	// Create an empty floating-point grid with background value 0.
//	createField();
//	modifyGrid();
//	displayGrid();
//	selectROI();

	convertMeshtoVol();
}

