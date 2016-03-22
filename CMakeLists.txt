cmake_minimum_required(VERSION 3.3)
project(VolumeSculptor)

SET(VTK_DIR "/home/naman/Documents/VTK-7.0.0/" CACHE PATH "VTK directory override" FORCE)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        src/main.cpp
        src/implicit_sphere.h
        src/octree_3D.h
        src/marching_sample.h
        src/octree_render.h
        src/octree_2.h
        src/ImplicitPolyDataDistance.h
        src/SDT_Octree.h
        )

add_executable(VolumeSculptor ${SOURCE_FILES})

if(VTK_LIBRARIES)
    target_link_libraries(VolumeSculptor ${VTK_LIBRARIES})
else()
    target_link_libraries(VolumeSculptor vtkHybrid vtkWidgets)
endif()

