#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkLookupTable.h"

#include "vtkPolyDataMapper.h"
#include "vtkTimerLog.h"
#include "vtkHyperOctreeSampleFunction.h"
#include "vtkHyperOctreeContourFilter.h"
#include "vtkXMLHyperOctreeWriter.h"
#include "vtkXMLHyperOctreeReader.h"
#include "vtkSphere.h"
#include "vtkCamera.h"

#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkCompositePolyDataMapper.h"
#include "vtkHyperOctreeSurfaceFilter.h"

#include "vtkCharArray.h"


int octree_2() {

    /*
    * This example creates, saves to disk, reloads from disk, and then draws
    * a vtkHyperOctree. The purpose is to check that disk IO of HyperOctrees
    * works.
    */
    /*
        * This code is for visualizing an octree storing a 3d implicit sphere.
        * Multiblockdataset has been used for storing multiple data sets for visualization.
        * Colored mapping is done as a fancy way to visualize the octree dataset
     */


    int dimension = 3;
    int levels = 5;
    int skipreader = 0;
    int binary = 2;
    int compressed = 1;
    int showcontour = 1;
    int ncontours = 3;
    int rewrite = 0;
    int interactive = 1;


    //-----------------------------------------------------------------
    vtkTimerLog *timer = vtkTimerLog::New();

    //-----------------------------------------------------------------
    // Standard rendering classes
    vtkRenderer *renderer = vtkRenderer::New();
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    //-----------------------------------------------------------------
    // Generate the data

    vtkHyperOctreeSampleFunction *source = vtkHyperOctreeSampleFunction::New();
    vtkSphere *f = vtkSphere::New();
    f->SetRadius(0.1);
    f->SetCenter(1, 1, 1);
    source->SetImplicitFunction(f);
    source->SetThreshold(0.2);
    f->Delete();

    source->SetDimension(dimension);
    source->SetWidth(2);
    source->SetHeight(3);
    source->SetDepth(4);
    source->SetLevels(levels);
    source->SetMinLevels(0);

    cout << "update source..." << endl;
    timer->StartTimer();
    source->Update(); // Update now, make things easier with a debugger
    timer->StopTimer();
    cout << "source updated" << endl;
    cout << "source time=" << timer->GetElapsedTime() << " s" << endl;

    //Adds some field data to the HyperOctree to test IO of field data
    vtkFieldData *fd = source->GetOutput()->GetFieldData();
    vtkCharArray *ca = vtkCharArray::New();
    ca->InsertNextValue('T');
    ca->InsertNextValue('E');
    ca->InsertNextValue('S');
    ca->InsertNextValue('T');
    ca->SetName("FDTestArray");
    fd->AddArray(ca);
    ca->Delete();

    //------------------------------------------------------------------
    // Test saving it to file
    vtkXMLHyperOctreeWriter *writerX = vtkXMLHyperOctreeWriter::New();
    writerX->SetInputConnection(0, source->GetOutputPort(0));
    writerX->SetFileName("HyperOctreeSample.vto");
    writerX->SetDataModeToAscii();
    if (binary == 1) {
        writerX->SetDataModeToBinary();
    }
    if (binary == 2) {
        writerX->SetDataModeToAppended();
    }
    if (!compressed) {
        writerX->SetCompressor(NULL);
    }

    cout << "update writerX..." << endl;
    timer->StartTimer();
    writerX->Write();
    timer->StopTimer();
    cout << "HyperOctree written" << endl;
    cout << "writerX time=" << timer->GetElapsedTime() << " s" << endl;
    writerX->Delete();

    //------------------------------------------------------------------
    // Test reading back the saved file

    vtkXMLHyperOctreeReader *readerX = vtkXMLHyperOctreeReader::New();
    readerX->SetFileName("HyperOctreeSample.vto");

    cout << "update readerX..." << endl;
    timer->StartTimer();
    readerX->Update();
    timer->StopTimer();
    cout << "readerX updated" << endl;
    cout << "readerX time=" << timer->GetElapsedTime() << " s" << endl;

    if (rewrite) {
        writerX = vtkXMLHyperOctreeWriter::New();
        writerX->SetInputConnection(0, readerX->GetOutputPort(0));
        writerX->SetFileName("HyperOctreeSample2.vto");
        writerX->SetDataModeToAscii();
        if (binary == 1) {
            writerX->SetDataModeToBinary();
        }
        if (binary == 2) {
            writerX->SetDataModeToAppended();
        }
        if (!compressed) {
            writerX->SetCompressor(NULL);
        }
        writerX->Write();
        writerX->Delete();
        cout << "HyperOctree written again" << endl;
    }

    // -----------------------------------------------------------------
    // Display the results with either contour or surface
    vtkHyperOctreeContourFilter *contour = vtkHyperOctreeContourFilter::New();
    contour->SetNumberOfContours(ncontours);
    contour->SetValue(0, 0.5);
    if (ncontours > 1) {
        contour->SetValue(1, 4.0);
    }
    if (ncontours > 2) {
        contour->SetValue(2, 8.0);
    }

    vtkMultiBlockDataSet *hds = vtkMultiBlockDataSet::New();
    hds->SetNumberOfBlocks(1);

    if (skipreader) {
        hds->SetBlock(0, source->GetOutput());
        contour->SetInputConnection(0, source->GetOutputPort(0));
    }
    else {
        hds->SetBlock(0, readerX->GetOutput());
        contour->SetInputConnection(0, readerX->GetOutputPort(0));
    }

    source->Delete();
    readerX->Delete();

    cout << "update contour..." << endl;
    timer->StartTimer();
    contour->Update();
    timer->StopTimer();
    cout << "contour updated" << endl;
    cout << "contour time=" << timer->GetElapsedTime() << " s" << endl;

    vtkHyperOctreeSurfaceFilter *surface = vtkHyperOctreeSurfaceFilter::New();
    vtkCompositeDataPipeline *exec = vtkCompositeDataPipeline::New();
    // Make sure we call SetExecutive right after the filter creation and
    // before the SetInput call.
    surface->SetExecutive(exec);
    surface->SetInputData(hds);
    exec->Delete();
    hds->Delete();

    // This creates a blue to red lut.
    vtkLookupTable *lut = vtkLookupTable::New();
    lut->SetHueRange(0.667, 0.0);

    vtkPolyDataMapper *cmapper = vtkPolyDataMapper::New();
    cmapper->SetInputConnection(0, contour->GetOutputPort(0));
    cmapper->SetLookupTable(lut);
    cmapper->SetScalarModeToUseCellData();

    vtkCompositePolyDataMapper *smapper = vtkCompositePolyDataMapper::New();
    smapper->SetInputConnection(0, surface->GetOutputPort(0));
    smapper->SetLookupTable(lut);
    smapper->SetScalarModeToUseCellData();

    if (contour->GetOutput()->GetCellData() != 0) {
        if (contour->GetOutput()->GetCellData()->GetScalars() != 0) {
            smapper->SetScalarRange(contour->GetOutput()->GetCellData()->
                    GetScalars()->GetRange());
        }
    }
    surface->Delete();
    contour->Delete();

    vtkActor *actor = vtkActor::New();
    if (showcontour) {
        actor->SetMapper(cmapper);
    }
    else {
        actor->SetMapper(smapper);
    }
    renderer->AddActor(actor);

    // Standard testing code.
    renderer->SetBackground(0.5, 0.5, 0.5);
    renWin->SetSize(300, 300);
    vtkCamera *cam = renderer->GetActiveCamera();
    renderer->ResetCamera();
    cam->Azimuth(180);
    renWin->Render();

    int retVal = 1;

    if (interactive) {
        iren->Start();
    }


    // Cleanup

    actor->Delete();
    smapper->Delete();
    cmapper->Delete();
    lut->Delete();

    iren->Delete();
    renWin->Delete();
    renderer->Delete();

    timer->Delete();

    //regresstest and IOCxxTests have opposite senses
    return !retVal;
}