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

int implicit_sphere() {

    /*
     * Samples an implicit sphere in a Voxel Grid
     * 0 level isosurface using contour filter
     * Applies a color mapping to each contour
     * Visualize
     * */

    vtkSmartPointer<vtkSphere> sphere =
            vtkSmartPointer<vtkSphere>::New();

    // Sample the function
    vtkSmartPointer<vtkSampleFunction> sample =
            vtkSmartPointer<vtkSampleFunction>::New();
    sample->SetSampleDimensions(10, 10, 10);
    sample->SetImplicitFunction(sphere);

    double value = 2.0;
    double xmin = -value, xmax = value,
            ymin = -value, ymax = value,
            zmin = -value, zmax = value;
    sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

    // Create the 0 isosurface
    vtkSmartPointer<vtkContourFilter> contours =
            vtkSmartPointer<vtkContourFilter>::New();
    contours->SetInputConnection(sample->GetOutputPort());
    contours->GenerateValues(1, 1, 1);

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

