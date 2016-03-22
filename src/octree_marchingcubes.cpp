/*
//
// Created by naman on 3/17/16.
//

#include <vtkMarchingCubes.h>
#include "vtkActor.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkHyperOctreeSampleFunction.h"
#include "vtkSphere.h"
#include "vtkCamera.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkCompositePolyDataMapper.h"
#include <vtkImageData.h>

int main() {

	//Initialize a HyperOctree
	vtkHyperOctreeSampleFunction *source3d = vtkHyperOctreeSampleFunction::New();
	vtkSphere *f3d = vtkSphere::New(); //Create a Sphere
	f3d->SetRadius(1);
	f3d->SetCenter(11, 1, 0);
	source3d->SetImplicitFunction(f3d); //set the equation to generate points
	source3d->SetThreshold(0.2);

	f3d->Delete();

	source3d->SetDimension(3);
	source3d->SetWidth(3);
	source3d->SetHeight(3);
	source3d->SetDepth(4);  // Depth of the tree along the z axis
	source3d->SetLevels(7); // 10
	source3d->SetMinLevels(0);
	source3d->SetOrigin(10, 0, 0);

	source3d->Update();

	double isoValue = 0.5;

	vtkSmartPointer<vtkImageData> volume =
			vtkSmartPointer<vtkImageData>::New();

//	volume->DeepCopy->(source3d->GetOutput());

	volume->DeepCopy(source3d->GetOutput());
	source3d->Delete();

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
*/
