/*=========================================================================

				Project 8 for CIS 410 (W18)
				by Jacob Brown 3/3/2018

===========================================================================*/
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkInteractorStyle.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkOpenGLPolyDataMapper.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkDataSetReader.h>
#include <vtkContourFilter.h>

#include <vtkRectilinearGrid.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkHedgeHog.h>
#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>
#include <vtkMaskPoints.h>>
#include <vtkPlaneSource.h>
#include <vtkStreamTracer.h>
#include <vtkRungeKutta4.h>
#include <vtkLineSource.h>

int main()
{
	vtkDataSetReader *rdr = vtkDataSetReader::New();
	rdr->SetFileName("proj8.vtk");
	rdr->Update();

	vtkSmartPointer<vtkRenderWindow> renWin = 
		vtkSmartPointer<vtkRenderWindow>::New();

	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	vtkCamera * camera = NULL;

	//******************************************
	//		Render 1 - Isosurface
	//******************************************

	// Contour Filter 1
	vtkContourFilter *cf1 = vtkContourFilter::New();
	cf1->SetValue(0, 2.5);
	cf1->SetInputConnection(rdr->GetOutputPort());
	cf1->Update();

	// Contour Filter 2
	vtkContourFilter *cf2 = vtkContourFilter::New();
	cf2->SetValue(0, 5.0);
	cf2->SetInputConnection(rdr->GetOutputPort());
	cf2->Update();

	// CF1 Mapper
	vtkSmartPointer<vtkPolyDataMapper> cf1Mapper = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cf1Mapper->SetInputConnection(cf1->GetOutputPort());

	// CF2 mapper
	vtkSmartPointer<vtkPolyDataMapper> cf2Mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cf2Mapper->SetInputConnection(cf2->GetOutputPort());

	// First actor
	vtkSmartPointer<vtkActor> cf1Actor =
		vtkSmartPointer<vtkActor>::New();
	cf1Actor->SetMapper(cf1Mapper);
	cf1Actor->GetMapper()->ScalarVisibilityOff();
	cf1Actor->GetProperty()->SetColor((double)(76 / 255), 1.0, 0.0);

	// Second actor
	vtkSmartPointer<vtkActor> cf2Actor = 
		vtkSmartPointer<vtkActor>::New();
	cf2Actor->SetMapper(cf2Mapper);

	// Renderer 1
	vtkSmartPointer<vtkRenderer> ren1 = 
		vtkSmartPointer<vtkRenderer>::New();
	renWin->AddRenderer(ren1);

	camera = ren1->GetActiveCamera();

	ren1->SetViewport(0.0, 0.0, 0.5, 0.5);

	// Add actors to first renderer
	ren1->AddActor(cf1Actor);
	ren1->AddActor(cf2Actor);
	ren1->ResetCamera();

	//******************************************
	//		Render 2 - Three Slices
	//******************************************

	// Three planes
	vtkSmartPointer<vtkPlane> plane1 = 
		vtkSmartPointer<vtkPlane>::New();
	plane1->SetOrigin(0, 0, 0);
	plane1->SetNormal(1, 0, 0);

	vtkSmartPointer<vtkPlane> plane2 = 
		vtkSmartPointer<vtkPlane>::New();
	plane2->SetOrigin(0, 0, 0);
	plane2->SetNormal(0, 1, 0);

	vtkSmartPointer<vtkPlane> plane3 = 
		vtkSmartPointer<vtkPlane>::New();
	plane3->SetOrigin(0, 0, 0);
	plane3->SetNormal(0, 0, 1);

	// Plane cutters
	vtkSmartPointer<vtkCutter> planeCut1 = 
		vtkSmartPointer<vtkCutter>::New();
	planeCut1->SetInputConnection(rdr->GetOutputPort());
	planeCut1->SetCutFunction(plane1);

	vtkSmartPointer<vtkCutter> planeCut2 = 
		vtkSmartPointer<vtkCutter>::New();
	planeCut2->SetInputConnection(rdr->GetOutputPort());
	planeCut2->SetCutFunction(plane2);

	vtkSmartPointer<vtkCutter> planeCut3 = 
		vtkSmartPointer<vtkCutter>::New();
	planeCut3->SetInputConnection(rdr->GetOutputPort());
	planeCut3->SetCutFunction(plane3);

	// Three mappers
	vtkSmartPointer<vtkPolyDataMapper> cutMapper1 = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cutMapper1->SetInputConnection(planeCut1->GetOutputPort());
	cutMapper1->ScalarVisibilityOn();
	cutMapper1->SetScalarModeToUsePointData();
	cutMapper1->SetColorModeToMapScalars();
	cutMapper1->SetScalarRange(rdr->GetOutput()->GetPointData()->GetScalars()->GetRange());

	vtkSmartPointer<vtkPolyDataMapper> cutMapper2 = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cutMapper2->SetInputConnection(planeCut2->GetOutputPort());
	cutMapper2->ScalarVisibilityOn();
	cutMapper2->SetScalarModeToUsePointData();
	cutMapper2->SetColorModeToMapScalars();
	cutMapper2->SetScalarRange(rdr->GetOutput()->GetPointData()->GetScalars()->GetRange());

	vtkSmartPointer<vtkPolyDataMapper> cutMapper3 = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cutMapper3->SetInputConnection(planeCut3->GetOutputPort());
	cutMapper3->ScalarVisibilityOn();
	cutMapper3->SetScalarModeToUsePointData();
	cutMapper3->SetColorModeToMapScalars();
	cutMapper3->SetScalarRange(rdr->GetOutput()->GetPointData()->GetScalars()->GetRange());

	// Three actors
	vtkSmartPointer<vtkActor> cutActor1 = 
		vtkSmartPointer<vtkActor>::New();
	cutActor1->SetMapper(cutMapper1);
	cutActor1->RotateX(30);
	cutActor1->RotateY(30);

	vtkSmartPointer<vtkActor> cutActor2 = 
		vtkSmartPointer<vtkActor>::New();
	cutActor2->SetMapper(cutMapper2);
	cutActor2->RotateX(30);
	cutActor2->RotateY(30);

	vtkSmartPointer<vtkActor> cutActor3 = 
		vtkSmartPointer<vtkActor>::New();
	cutActor3->SetMapper(cutMapper3);
	cutActor3->RotateX(30);
	cutActor3->RotateY(30);

	// Renderer 2
	vtkSmartPointer<vtkRenderer> ren2 = 
		vtkSmartPointer<vtkRenderer>::New();
	renWin->AddRenderer(ren2);

	ren2->SetViewport(0.0, 0.5, 0.5, 1.0);

	// Add actors to second renderer
	ren2->AddActor(cutActor1);
	ren2->AddActor(cutActor2);
	ren2->AddActor(cutActor3);
	ren2->ResetCamera();

	//******************************************
	//		Render 3 - Hedgehog Glyphs
	//******************************************

	// Sets the "grad" fieldData active (thank you, Manish)
	rdr->GetOutput()->GetPointData()->SetActiveAttribute("grad", vtkDataSetAttributes::VECTORS);

	vtkSmartPointer<vtkMaskPoints> pointMask = 
		vtkSmartPointer<vtkMaskPoints>::New();
	pointMask->SetInputConnection(rdr->GetOutputPort());
	pointMask->SetOnRatio(150);
	pointMask->RandomModeOn();

	vtkSmartPointer<vtkArrowSource> arrowSource =
		vtkSmartPointer<vtkArrowSource>::New();

	// Hedgehog glyphs
	vtkSmartPointer<vtkGlyph3D> hedgehog = 
		vtkSmartPointer<vtkGlyph3D>::New();
	hedgehog->SetSourceConnection(arrowSource->GetOutputPort());
	hedgehog->SetInputConnection(pointMask->GetOutputPort());
	hedgehog->OrientOn();
	hedgehog->SetVectorModeToUseVector();

	// Hedgehog mapper
	vtkSmartPointer<vtkPolyDataMapper> hedgehogMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	hedgehogMapper->SetInputConnection(hedgehog->GetOutputPort());
	hedgehogMapper->SetScalarRange(rdr->GetOutput()->GetPointData()->GetScalars()->GetRange());

	// Hedgehog actor
	vtkSmartPointer<vtkActor> hedgehogActor =
		vtkSmartPointer<vtkActor>::New();
	hedgehogActor->SetMapper(hedgehogMapper);

	// Renderer 3
	vtkSmartPointer<vtkRenderer> ren3 =
		vtkSmartPointer<vtkRenderer>::New();
	renWin->AddRenderer(ren3);

	ren3->SetViewport(0.5, 0.0, 1.0, 0.5);

	// Add hedgehog actor to renderer
	ren3->AddActor(hedgehogActor);
	ren3->ResetCamera();

	//******************************************
	//		Render 4 - Streamlines
	//******************************************
	 
	// Probably unnecesary
	rdr->GetOutput()->GetPointData()->SetActiveAttribute("grad", vtkDataSetAttributes::VECTORS);

	// Seeds
	vtkSmartPointer<vtkLineSource> seedsLine = 
		vtkSmartPointer<vtkLineSource>::New();
	seedsLine->SetPoint1(-9, 0, 0);
	seedsLine->SetPoint2(9, 0, 0);
	seedsLine->SetResolution(18);
	
	// Streamline tracer
	vtkSmartPointer<vtkStreamTracer> slTracer = 
		vtkSmartPointer<vtkStreamTracer>::New();
	slTracer->SetSourceConnection(seedsLine->GetOutputPort());
	slTracer->SetInputConnection(rdr->GetOutputPort());
	slTracer->SetMaximumPropagation(50);
	slTracer->SetIntegratorTypeToRungeKutta4();

	// Streamline mapper
	vtkSmartPointer<vtkPolyDataMapper> slMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	slMapper->SetInputConnection(slTracer->GetOutputPort());
	slMapper->SetScalarRange(rdr->GetOutput()->GetPointData()->GetScalars()->GetRange());
	slMapper->SetScalarModeToUsePointData();

	// Streamline actor
	vtkSmartPointer<vtkActor> slActor = 
		vtkSmartPointer<vtkActor>::New();
	slActor->SetMapper(slMapper);

	// Renderer 4
	vtkSmartPointer<vtkRenderer> ren4 =
		vtkSmartPointer<vtkRenderer>::New();
	renWin->AddRenderer(ren4);

	ren4->SetViewport(0.5, 0.5, 1.0, 1.0);

	// Add streamline actor to scene
	ren4->AddActor(slActor);
	ren4->ResetCamera();
	  
	renWin->Render();
	renWin->SetWindowName("Jacob Brown - Project 8");
	renWin->SetSize(600, 600);
	  
	iren->Start();

	return EXIT_SUCCESS;
}