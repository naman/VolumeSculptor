//
// Created by gautam on 22/3/16.
//

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
#include <vtkHyperOctreeSampleFunction.h>
#include <vtkMultiBlockDataSet.h>

int main (int, char *[])
{

    vtkMultiBlockDataSet *hds = vtkMultiBlockDataSet::New();
    hds->SetNumberOfBlocks(1);

    // 3D
    vtkHyperOctreeSampleFunction *source3d = vtkHyperOctreeSampleFunction::New();
    vtkSphere *f3d = vtkSphere::New();
    f3d->SetRadius(1);
    f3d->SetCenter(0, 0, 0);
    source3d->SetImplicitFunction(f3d);
    source3d->SetThreshold(0.2);

    f3d->Delete();

    source3d->SetDimension(3);
    source3d->SetWidth(10);
    source3d->SetHeight(10);
    source3d->SetDepth(10);
    source3d->SetLevels(7); // 10
    source3d->SetMinLevels(0);
    source3d->SetOrigin(-2, -2, -2);

    source3d->Update();

    hds->SetBlock(0, source3d->GetOutput());
//    vtkSmartPointer<vtkSphere> sphere =
//            vtkSmartPointer<vtkSphere>::New();
//
//    // Sample the function
//    vtkSmartPointer<vtkSampleFunction> sample =
//            vtkSmartPointer<vtkSampleFunction>::New();
//    sample->SetSampleDimensions(10,10,10);
//    sample->SetImplicitFunction(sphere);
//
//    double value = 2.0;
//    double xmin = -value, xmax = value,
//            ymin = -value, ymax = value,
//            zmin = -value, zmax = value;
//    sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

    // Create the 0 isosurface
    vtkSmartPointer<vtkContourFilter> contours =
            vtkSmartPointer<vtkContourFilter>::New();
    contours->SetInputConnection(source3d->GetOutputPort());
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


