cmake_minimum_required(VERSION 3.3)
project(VolumeSculptor)

find_package( VTK REQUIRED )
include(${VTK_USE_FILE})

#find_package(CGAL QUIET COMPONENTS Core )
#
#if ( CGAL_FOUND )
#
#    include( ${CGAL_USE_FILE} )
#
#    include( CGAL_CreateSingleSourceCGALProgram )
#
    include_directories (BEFORE "../../include")
#
    include_directories (BEFORE "include")
#
##    create_single_source_cgal_program( "array_convex_hull_2.cpp" )
##    create_single_source_cgal_program( "ch_from_cin_to_cout.cpp" )
##    create_single_source_cgal_program( "ch_graham_anderson.cpp" )
##    create_single_source_cgal_program( "ch_timing.cpp" )
##    create_single_source_cgal_program( "convex_hull_yz.cpp" )
##    create_single_source_cgal_program( "iostream_convex_hull_2.cpp" )
##    create_single_source_cgal_program( "vector_convex_hull_2.cpp" )
#
#else()
#
#    message(STATUS "This program requires the CGAL library, and will not be compiled.")
#
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES trial.cpp)
add_executable(VolumeSculptor ${SOURCE_FILES})


if(VTK_LIBRARIES)
    target_link_libraries(VolumeSculptor ${VTK_LIBRARIES})
else()
    target_link_libraries(VolumeSculptor vtkHybrid vtkWidgets)
endif()