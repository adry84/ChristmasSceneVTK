#ifndef __CHRISTMAS_SCENE_H__
#define __CHRISTMAS_SCENE_H__
//Lib VTK
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkVectorText.h>
#include <vtkImageCast.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkTextProperty.h>
#include <vtkImageMapper3D.h>
#include <vtkImageData.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkConeSource.h> 
#include <vtkSphereSource.h> 
#include <vtkShrinkPolyData.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkProp3D.h>
#include <vtkProgrammableFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkJPEGReader.h>
#include <vtkOBJReader.h>
#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkLight.h>
#include <vtkPropCollection.h>
#include <vtkTexture.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkImageRGBToHSV.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageThreshold.h>
#include <vtkImageSeedConnectivity.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageShrink3D.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkTextureMapToPlane.h>
#include <vtkClipPolyData.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkProp3DCollection.h>
#include <vtkObject.h>
#include <vtkProp3D.h>

namespace ChristmasSceneVTK {

class ChristmasScene  {
	public:
		
		ChristmasScene();
		void draw();

		//window
		vtkSmartPointer<vtkRenderWindow> renderWindow;

		//renderers
		vtkSmartPointer<vtkRenderer> backgroundRenderer;
		vtkSmartPointer<vtkRenderer> snowRenderer;
		vtkSmartPointer<vtkRenderer> textRenderer;
		vtkSmartPointer<vtkRenderer> sceneRenderer;
		//actors
		vtkSmartPointer<vtkActor> star;
		vtkSmartPointer<vtkTextActor> textActor;
		vtkSmartPointer<vtkAssembly> pineTreeAssembly;
		vtkSmartPointer<vtkAssembly> snowFlakesAssembly;
		//camera
		vtkCamera* sceneCamera;

	private:
		const double heightWindow = 1280.;
		const double widthWindow = 1920.;
		const int branchNumber = 8;

		const int snowFlakesNumber = 200;
		const int snowFlakesPerLine = 10;
		const std::string backgroundFilename = "../christmasTexture.jpg"; 
		const std::string starFilename = "../star.obj"; 
		const std::string snowFilename = "../snowflakes2.jpg";
		const std::string sceneText = "Merry Christmas";

		vtkSmartPointer<vtkTextActor> CreateTextActor(vtkSmartPointer<vtkNamedColors> namedColors);
		vtkSmartPointer<vtkActor> DrawSnowFlakes(vtkSmartPointer<vtkNamedColors> namedColors);
		vtkSmartPointer<vtkActor> DrawStar(vtkSmartPointer<vtkNamedColors> namedColors) ;
		vtkSmartPointer<vtkActor> DrawCone(vtkSmartPointer<vtkNamedColors> namedColors, double radius, double height);
		vtkSmartPointer<vtkActor> DrawCylinder(vtkSmartPointer<vtkNamedColors> namedColors);
		vtkSmartPointer<vtkActor> DrawSphere(vtkSmartPointer<vtkNamedColors> namedColors, double radius);
		void initBackgroundRenderer();
		void adjustBackgroundCamera() ;
		void initSnowRenderer();
		void initSceneRenderer();
		void initTextRenderer();
		void addLights();
		void performAnimation() ;
		
		
		vtkSmartPointer<vtkNamedColors> namedColors;
		vtkSmartPointer<vtkImageData> backgroundImageData ;
		
		
};
}
#endif