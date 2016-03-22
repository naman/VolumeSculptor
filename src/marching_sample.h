#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkVoxelModeller.h>
#include <vtkSphereSource.h>
#include <vtkImageData.h>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

int marching_sample() {
    vtkSmartPointer<vtkImageData> volume =
            vtkSmartPointer<vtkImageData>::New();
    double isoValue;

    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetPhiResolution(20);
    sphereSource->SetThetaResolution(20);
    sphereSource->Update();

    double bounds[6];
    sphereSource->GetOutput()->GetBounds(bounds);
    for (unsigned int i = 0; i < 6; i += 2) {
        double range = bounds[i + 1] - bounds[i];
        bounds[i] = bounds[i] - .1 * range;
        bounds[i + 1] = bounds[i + 1] + .1 * range;
    }
    vtkSmartPointer<vtkVoxelModeller> voxelModeller =
            vtkSmartPointer<vtkVoxelModeller>::New();
    voxelModeller->SetSampleDimensions(50, 50, 50);
    voxelModeller->SetModelBounds(bounds);
    voxelModeller->SetScalarTypeToFloat();
    voxelModeller->SetMaximumDistance(.1);

    voxelModeller->SetInputConnection(sphereSource->GetOutputPort());
    voxelModeller->Update();
    isoValue = 0.5;
    volume->DeepCopy(voxelModeller->GetOutput());

    vtkSmartPointer<vtkMarchingCubes> surface =
            vtkSmartPointer<vtkMarchingCubes>::New();

    surface->SetInputData(volume);
    surface->ComputeNormalsOn();
    surface->SetValue(0, isoValue);

    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(surface->GetOutputPort());
    mapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    renderer->AddActor(actor);

    renderWindow->Render();
    interactor->Start();
    return EXIT_SUCCESS;
}