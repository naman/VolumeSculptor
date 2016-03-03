#include <iostream>
#include <CGAL/Simple_cartesian.h>

using namespace std;

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_2 Point_2;

int main(int argc, char const *argv[])
{
    Point_2 p(1,1);
    cout << p << endl;
    return 0;
}