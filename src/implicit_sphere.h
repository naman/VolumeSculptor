

///////////////////////////////////////////////////////////////////////////
//
/// @file implicit_sphere.h
///
/// @brief Taking a vtksphere, Sampling it. Sampling is being done by calculating the implicit function value and the gradient of every point in and around the sphere. Then the sampled values are stored in a Volume grid. Then we calculate 0 contour of the function. Contouring finds contour for a specified value , or it can generate contour for a given range of values.
///


#include <vtkSmartPointer.h>

#include <vtkSampleFunction.h>
#include <vtkContourFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>

#include <vtkSphere.h>
#include <vtkMarchingContourFilter.h>

int implicit_sphere() {

    vtkSmartPointer<vtkSphere> sphere =
            vtkSmartPointer<vtkSphere>::New();
//    sphere->SetCenter(-2,-2, -2);
//    sphere->SetCenter(-2,-2, -2);

    // Sample the function
    vtkSmartPointer<vtkSampleFunction> sample =
            vtkSmartPointer<vtkSampleFunction>::New();
    sample->SetSampleDimensions(50, 50, 50);

    sample->SetImplicitFunction(sphere);

    double value = 2.0;
    double xmin = -value, xmax = value,
            ymin = -value, ymax = value,
            zmin = -value, zmax = value;
    sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

    // Create the 0 isosurface
    vtkSmartPointer<vtkMarchingContourFilter> contours =
            vtkSmartPointer<vtkMarchingContourFilter>::New();
    contours->SetInputConnection(sample->GetOutputPort());
//    contours->GenerateValues(10, 0, 10);
    contours->GenerateValues(1, 0, 0);

    // Map the contours to graphical primitives
    vtkSmartPointer<vtkPolyDataMapper> contourMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(contours->GetOutputPort());
    contourMapper->ScalarVisibilityOff();

    // Create an actor for the contours
    vtkSmartPointer<vtkActor> contourActor =
            vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    renderer->AddActor(contourActor);
    renderer->SetBackground(.2, .3, .4);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

