
// * This code is for visualizing an octree storing a 3d implicit sphere.
// * Multiblockdataset has been used for storing multiple data sets for visualization.
// * Colored mapping is done as a fancy way to visualize the octree dataset
// *
//
//
//

/*
#include "vtkActor.h"
#include "vtkCellData.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkHyperOctreeSurfaceFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkHyperOctreeSampleFunction.h"
#include "vtkSphere.h"
#include "vtkCamera.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkCompositePolyDataMapper.h"

int main(int argc, char *argv[]) {
	// Standard rendering classes
	vtkRenderer *renderer = vtkRenderer::New();
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(renderer);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	vtkMultiBlockDataSet *hds = vtkMultiBlockDataSet::New();
	hds->SetNumberOfBlocks(1);

	// 3D
	vtkHyperOctreeSampleFunction *source3d = vtkHyperOctreeSampleFunction::New();
	vtkSphere *f3d = vtkSphere::New();
	f3d->SetRadius(1);
	f3d->SetCenter(11, 1, 0);
	source3d->SetImplicitFunction(f3d);
	source3d->SetThreshold(0.2);

	f3d->Delete();

	source3d->SetDimension(3);
	source3d->SetWidth(2);
	source3d->SetHeight(3);
	source3d->SetDepth(4);
	source3d->SetLevels(7); // 10
	source3d->SetMinLevels(0);
	source3d->SetOrigin(10, 0, 0);

	source3d->Update(); // Update now, make things easier with a debugger

	hds->SetBlock(0, source3d->GetOutput());

	vtkHyperOctreeSurfaceFilter *surface = vtkHyperOctreeSurfaceFilter::New();

	vtkCompositeDataPipeline *exec = vtkCompositeDataPipeline::New();
	// Make sure we call SetExecutive right after the filter creation and
	// before the SetInput call.
	surface->SetExecutive(exec);
	surface->SetInput(hds);
	hds->Delete();
	exec->Delete();

	surface->Update(); //So that we can call GetRange() on the scalars

	// This creates a blue to red lut.
	vtkLookupTable *lut = vtkLookupTable::New();
	lut->SetHueRange(0.667, 0.0);

	vtkCompositePolyDataMapper *mapper = vtkCompositePolyDataMapper::New();
	mapper->SetInputConnection(0, surface->GetOutputPort(0));
	mapper->SetLookupTable(lut);

	if (source3d->GetOutput()->GetLeafData() != 0) {
		if (source3d->GetOutput()->GetLeafData()->GetScalars() != 0) {
			mapper->SetScalarRange(source3d->GetOutput()->GetLeafData()->
					GetScalars()->GetRange());
		}
	}

	source3d->Delete();

	vtkActor *actor = vtkActor::New();
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	renderer->SetBackground(0.5, 0.5, 0.5);
	renWin->SetSize(300, 300);
	vtkCamera *cam = renderer->GetActiveCamera();
	renderer->ResetCamera();
	cam->Azimuth(180);

	renWin->Render();
	iren->Start();

	// Cleanup
	renderer->Delete();
	renWin->Delete();
	iren->Delete();

	mapper->Delete();
	actor->Delete();
	surface->Delete();
	lut->Delete();

	return 0;
}
*/