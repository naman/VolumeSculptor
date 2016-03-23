/*
 * This program fails, vtkHyperOctree expects a implicit. I have tried calculating signed shortest distance field and passed
 * into a HyperOctree. But that doesn't work.
 * 
 * */

#include <vtkVersion.h>
#include <vtkImplicitPolyDataDistance.h>

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkHyperOctreeSampleFunction.h>
#include <vtkContourFilter.h>


int SDT_Octree() {
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(1.0f);
    sphereSource->Update();
    vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    sphereMapper->ScalarVisibilityOff();
    vtkSmartPointer<vtkActor> sphereActor =
            vtkSmartPointer<vtkActor>::New();
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetOpacity(.3);
    sphereActor->GetProperty()->SetColor(1, 0, 0);

    vtkSmartPointer<vtkImplicitPolyDataDistance> implicitPolyDataDistance =
            vtkSmartPointer<vtkImplicitPolyDataDistance>::New();
    implicitPolyDataDistance->SetInput(sphereSource->GetOutput());

    // Setup a grid
    vtkSmartPointer<vtkPoints> points =
            vtkSmartPointer<vtkPoints>::New();
    float step = 0.1;
    for (float x = -2.0; x <= 2.0; x += step) {
        for (float y = -2.0; y <= 2.0; y += step) {
            for (float z = -2.0; z <= 2.0; z += step) {
                points->InsertNextPoint(x, y, z);
            }
        }
    }

    // Add distances to each point
    vtkSmartPointer<vtkFloatArray> signedDistances =
            vtkSmartPointer<vtkFloatArray>::New();
    signedDistances->SetNumberOfComponents(1);
    signedDistances->SetName("SignedDistances");

    // Evaluate the signed distance function at all of the grid points
    for (vtkIdType pointId = 0; pointId < points->GetNumberOfPoints(); ++pointId) {
        double p[3];
        points->GetPoint(pointId, p);
        float signedDistance = implicitPolyDataDistance->EvaluateFunction(p);
        signedDistances->InsertNextValue(signedDistance);
    }

    vtkSmartPointer<vtkPolyData> polyData =
            vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->GetPointData()->SetScalars(signedDistances);

    vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
            vtkSmartPointer<vtkVertexGlyphFilter>::New();

    // 3D
    vtkHyperOctreeSampleFunction *source3d = vtkHyperOctreeSampleFunction::New();
    source3d->SetInputDataObject(polyData);
    source3d->SetThreshold(0.2);

    source3d->SetDimension(3);
    source3d->SetWidth(10);
    source3d->SetHeight(10);
    source3d->SetDepth(10);

//    cout << source3d->GetLevels() << endl; // returns 5 then how is the next line working?!

    source3d->SetLevels(8); // Change it back to 7.
    source3d->SetMinLevels(0);
    source3d->SetOrigin(-2, -2, -2); //Origin vs Center?

    source3d->Update();

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

    return 0;
}