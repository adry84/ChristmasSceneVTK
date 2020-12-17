#include "ChristmasScene.h"
#include "AnimationTimerCallback.h"
#include "Observer.h"
namespace ChristmasSceneVTK{


ChristmasScene::ChristmasScene() {
    namedColors = vtkSmartPointer<vtkNamedColors>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
}

void ChristmasScene::draw() {
    initBackgroundRenderer();
	initSnowRenderer();
	initSceneRenderer();
	initTextRenderer();

    renderWindow->SetSize(widthWindow, heightWindow);
    renderWindow->SetNumberOfLayers(4);
    renderWindow->AddRenderer(backgroundRenderer);
    renderWindow->AddRenderer(snowRenderer);
    renderWindow->AddRenderer(sceneRenderer);
    renderWindow->AddRenderer(textRenderer);
	
	adjustBackgroundCamera();


    renderWindow->Render();
    
    textActor->SetDisplayPosition(widthWindow / 2.0 - textActor->GetWidth() / 2.0, textActor->GetHeight() * 40.);
    textRenderer->SetDraw(true);

    addLights();

	performAnimation();
}

void ChristmasScene::performAnimation() {
	 auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);
	interactor->Initialize();
    vtkSmartPointer<AnimationTimerCallback> cb = vtkSmartPointer<AnimationTimerCallback>::New();
    cb->init(this);
    interactor->AddObserver(vtkCommand::TimerEvent, cb);
    int timerId = interactor->CreateRepeatingTimer(100);
    vtkSmartPointer<Observer> observer = vtkSmartPointer<Observer>::New();
    interactor->AddObserver(vtkCommand::KeyPressEvent, observer);
    interactor->Start();
}

void ChristmasScene::addLights() {
    sceneCamera = sceneRenderer->GetActiveCamera();
    auto sceneLight = vtkSmartPointer<vtkLight>::New();
    sceneLight->SetFocalPoint(sceneCamera->GetFocalPoint());
    sceneLight->SetPosition(sceneCamera->GetPosition());
    sceneLight->SetColor(namedColors->GetColor3d("Blue").GetData());
    sceneRenderer->AddLight(sceneLight);
}

void ChristmasScene::initTextRenderer() {
	textRenderer = vtkSmartPointer<vtkRenderer>::New();
    
    textRenderer->SetLayer(3);
    textRenderer->SetViewport(0.0, 0.0, 1., 1.);
	textActor = CreateTextActor(namedColors);
    textRenderer->AddActor(textActor);
	textRenderer->SetDraw(false);
}

void ChristmasScene::initSceneRenderer() {
	sceneRenderer = vtkSmartPointer<vtkRenderer>::New();
    sceneRenderer->SetLayer(2);
    sceneRenderer->SetViewport(0.0, 0.0, 1., 1.);
	
	pineTreeAssembly = vtkSmartPointer<vtkAssembly>::New();
    pineTreeAssembly->AddPart(DrawCylinder(namedColors));

    int indexBranch = branchNumber;

    while (indexBranch >= 0) {
        auto coneActor = DrawCone(namedColors,
            indexBranch + 1.5,
            (1.1 * indexBranch) + 2.
        );
        coneActor->SetPosition(0.0, (1.3 * (branchNumber - indexBranch)) + 7.0, 0);
        pineTreeAssembly->AddPart(
            coneActor
        );
        indexBranch--;
    }

    indexBranch = branchNumber;
    while (indexBranch >= 0) {
        auto actor = DrawSphere(namedColors, 0.75);
        actor->SetPosition((indexBranch + 1.5) * sin(360 / (indexBranch + 1)),
            (1.8 * (branchNumber - indexBranch)) + 2.5, 0.0
        );
        pineTreeAssembly->AddPart(
            actor
        );
        indexBranch--;
    }

    star = DrawStar(namedColors);
	
	sceneRenderer->AddActor(pineTreeAssembly);
    sceneRenderer->AddActor(star);
}

void ChristmasScene::initSnowRenderer() {
	snowRenderer = vtkSmartPointer<vtkRenderer>::New();
    snowRenderer->SetLayer(1);
    snowRenderer->SetViewport(0.0, 0.0, widthWindow, heightWindow);
	 double columnWidth = widthWindow * 2. / snowFlakesPerLine;
    double linesNumber = (snowFlakesNumber / snowFlakesPerLine);
    double lineHeight = heightWindow * 2 / linesNumber;
    snowFlakesAssembly = vtkSmartPointer<vtkAssembly>::New();
    snowFlakesAssembly->SetPosition(0., 0., 0.);
    vtkSmartPointer<vtkActor> sfModelActor = DrawSnowFlakes(namedColors);
    sfModelActor->GetProperty()->SetColor(namedColors->GetColor3d("White").GetData());
    for (int indexSF = 0; indexSF < snowFlakesNumber; indexSF++) {
        vtkSmartPointer<vtkActor> sfActor = vtkSmartPointer<vtkActor>::New();
        sfActor->ShallowCopy(sfModelActor);
        sfActor->SetScale((rand() % 5 / 10.) + 0.5);
        sfActor->SetPosition(
            (columnWidth * floor(indexSF % snowFlakesPerLine)) + (rand() % int(columnWidth)),
            (lineHeight * (indexSF / 10)) + (rand() % int(lineHeight)),
            0.);
        snowFlakesAssembly->AddPart(
            sfActor
        );
    }
    snowRenderer->AddActor(snowFlakesAssembly);
	
	auto snowCamera = snowRenderer->GetActiveCamera();
    snowRenderer->ResetCamera();
    snowCamera->Zoom(2.0);
}

void ChristmasScene::adjustBackgroundCamera() {
    // Set up the background camera to fill the renderer with the image
    double origin[3];
    double spacing[3];
    int extent[6];
    backgroundImageData->GetOrigin(origin);
    backgroundImageData->GetSpacing(spacing);
    backgroundImageData->GetExtent(extent);

    auto bgCamera = backgroundRenderer->GetActiveCamera();
    bgCamera->ParallelProjectionOn();

    double xc = origin[0] + 0.5 * (extent[0] + extent[1]) * spacing[0];
    double yc = origin[1] + 0.5 * (extent[2] + extent[3]) * spacing[1];
    double yd = (extent[3] - extent[2] + 1) * spacing[1];
    double d = bgCamera->GetDistance();
    bgCamera->SetParallelScale(0.5 * yd);
    bgCamera->SetFocalPoint(xc, yc, 0.0);
    bgCamera->SetPosition(xc, yc, d);
}
void ChristmasScene::initBackgroundRenderer() {
	backgroundRenderer = vtkSmartPointer<vtkRenderer>::New();
    backgroundRenderer->SetLayer(0);
    backgroundRenderer->InteractiveOff();

    vtkSmartPointer<vtkJPEGReader> jpegReader = vtkSmartPointer<vtkJPEGReader>::New();
    jpegReader->SetFileName(backgroundFilename.c_str());
    jpegReader->Update();
    backgroundImageData = jpegReader->GetOutput();
    vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
    imageActor->SetInputData(backgroundImageData);
    backgroundRenderer->AddActor(imageActor);
}

vtkSmartPointer<vtkTextActor> ChristmasScene::CreateTextActor(vtkSmartPointer<vtkNamedColors> namedColors) {
    vtkSmartPointer<vtkTextActor> actor = vtkSmartPointer<vtkTextActor>::New();
    actor->SetInput(sceneText.c_str());
    actor->GetTextProperty()->SetJustificationToCentered();
    actor->GetTextProperty()->SetFontFamilyToCourier();
    actor->GetTextProperty()->SetFontSize(100);
    actor->GetTextProperty()->SetBold(true);
    actor->GetTextProperty()->SetColor(namedColors->GetColor3d("FireBrick").GetData());
    return actor;
}

vtkSmartPointer<vtkActor> ChristmasScene::DrawSnowFlakes(vtkSmartPointer<vtkNamedColors> namedColors) {

    vtkSmartPointer<vtkJPEGReader> jpegReader = vtkSmartPointer<vtkJPEGReader>::New();
    jpegReader->SetFileName(snowFilename.c_str());

    // Convert the image to hsv so that we can threshold on value
    vtkSmartPointer<vtkImageRGBToHSV> toHSV = vtkSmartPointer<vtkImageRGBToHSV>::New();
    toHSV->SetInputConnection(jpegReader->GetOutputPort());

    // Just work with the value
    vtkSmartPointer<vtkImageExtractComponents> extractImage = vtkSmartPointer<vtkImageExtractComponents>::New();
    extractImage->SetInputConnection(toHSV->GetOutputPort());
    extractImage->SetComponents(2);

    // Threshold to a black/white image
    vtkSmartPointer<vtkImageThreshold> threshold1 = vtkSmartPointer<vtkImageThreshold>::New();
    threshold1->SetInputConnection(extractImage->GetOutputPort());
    threshold1->ThresholdByUpper(230);
    threshold1->SetInValue(255);
    threshold1->SetOutValue(0);
    threshold1->Update();

    // Place a seed in each corner and label connected pixels with 255
    threshold1->UpdateInformation();

    int* extent = threshold1->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
    vtkSmartPointer<vtkImageSeedConnectivity> connect = vtkSmartPointer<vtkImageSeedConnectivity>::New();
    connect->SetInputConnection(threshold1->GetOutputPort());
    connect->SetInputConnectValue(255);
    connect->SetOutputConnectedValue(255);
    connect->SetOutputUnconnectedValue(0);
    connect->AddSeed(extent[0], extent[2]);
    connect->AddSeed(extent[1], extent[2]);
    connect->AddSeed(extent[1], extent[3]);
    connect->AddSeed(extent[0], extent[3]);

    // Smooth a little before clipping
    vtkSmartPointer<vtkImageGaussianSmooth> smooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    smooth->SetDimensionality(2);
    smooth->SetStandardDeviation(1.0, 1.0);
    smooth->SetInputConnection(connect->GetOutputPort());

    vtkSmartPointer<vtkImageShrink3D> shrink = vtkSmartPointer<vtkImageShrink3D>::New();
    shrink->SetInputConnection(smooth->GetOutputPort());
    shrink->SetShrinkFactors(1, 1, 1);
    shrink->AveragingOn();

    // Convert the image to polydata
    vtkSmartPointer<vtkImageDataGeometryFilter> geometry = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    geometry->SetInputConnection(shrink->GetOutputPort());

    // Create texture coordinates
    vtkSmartPointer<vtkTextureMapToPlane> geometryTexture = vtkSmartPointer<vtkTextureMapToPlane>::New();
    geometryTexture->SetInputConnection(geometry->GetOutputPort());
    geometryTexture->SetOrigin(0, 0, 0);
    geometryTexture->SetPoint1(extent[1], 0, 0);
    geometryTexture->SetPoint2(0, extent[3], 0);

    // Clip the geometry
    vtkSmartPointer<vtkClipPolyData> clip = vtkSmartPointer<vtkClipPolyData>::New();
    clip->SetInputConnection(geometryTexture->GetOutputPort());
    clip->SetValue(5.5);
    clip->GenerateClipScalarsOff();
    clip->InsideOutOff();
    clip->InsideOutOn();

    vtkSmartPointer<vtkTriangleFilter> triangles = vtkSmartPointer<vtkTriangleFilter>::New();
    triangles->SetInputConnection(clip->GetOutputPort());

    vtkSmartPointer<vtkDecimatePro> decimate = vtkSmartPointer<vtkDecimatePro>::New();
    decimate->SetInputConnection(triangles->GetOutputPort());
    decimate->BoundaryVertexDeletionOn();
    decimate->SetDegree(100);
    decimate->PreserveTopologyOn();
    decimate->SetTargetReduction(.99);

    vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
    extrude->SetInputConnection(decimate->GetOutputPort());
    extrude->SetExtrusionTypeToNormalExtrusion();
    extrude->SetScaleFactor(-10);

    vtkSmartPointer<vtkPolyDataMapper> map = vtkSmartPointer<vtkPolyDataMapper>::New();
    map->SetInputConnection(extrude->GetOutputPort());
    map->ScalarVisibilityOff();

    vtkSmartPointer<vtkTexture> imageTexture = vtkSmartPointer<vtkTexture>::New();
    imageTexture->InterpolateOn();
    imageTexture->SetInputConnection(jpegReader->GetOutputPort());

    vtkSmartPointer<vtkActor> clipart = vtkSmartPointer<vtkActor>::New();
    clipart->SetMapper(map);
    clipart->SetTexture(imageTexture);
	
    return clipart;
}

vtkSmartPointer<vtkActor> ChristmasScene::DrawStar(vtkSmartPointer<vtkNamedColors> namedColors) {
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(starFilename.c_str());
    reader->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    double color[4];
    namedColors->GetColor("gold", color);
    actor->GetProperty()->SetColor(color);
    actor->SetPosition(0.0, 10, 0.0);
    actor->RotateZ(90.0);
    actor->RotateX(90.0);
    actor->SetScale(0.3);

    return actor;
}



vtkSmartPointer<vtkActor> ChristmasScene::DrawCone(vtkSmartPointer<vtkNamedColors> namedColors, double radius, double height)
{
    vtkSmartPointer<vtkConeSource> source = vtkSmartPointer<vtkConeSource>::New();
    source->SetResolution(8);
    source->SetRadius(radius);
    source->SetHeight(height);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(source->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    double color[4];
    namedColors->GetColor("green", color);
    actor->GetProperty()->SetColor(color);
    actor->RotateZ(90.0);
    actor->RotateY(0.0);

    actor->GetProperty()->SetSpecular(0.0);
    actor->GetProperty()->SetAmbient(0.5);
    actor->GetProperty()->SetDiffuse(1.0);
    actor->GetProperty()->SetSpecularPower(100);
    return actor;
}

vtkSmartPointer<vtkActor> ChristmasScene::DrawCylinder(vtkSmartPointer<vtkNamedColors> namedColors)
{
    vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
    cylinderSource->SetHeight(3.0);
    cylinderSource->SetRadius(1.0);
    cylinderSource->SetResolution(8);
    vtkSmartPointer<vtkPolyDataMapper> cylinderMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cylinderMapper->SetInputConnection(cylinderSource->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(cylinderMapper);
    double brownColor[4];
    namedColors->GetColor("brown", brownColor);
    actor->GetProperty()->SetColor(brownColor[0], brownColor[1], brownColor[2]);
    actor->RotateX(0.0);
    actor->RotateY(0.0);
    //ApplyLight(actor, namedColors);
    return actor;
}

vtkSmartPointer<vtkActor> ChristmasScene::DrawSphere(vtkSmartPointer<vtkNamedColors> namedColors, double radius)
{
    vtkSmartPointer<vtkSphereSource> source = vtkSmartPointer<vtkSphereSource>::New();
    source->SetThetaResolution(8);
    source->SetPhiResolution(16);
    source->SetRadius(radius);
    vtkSmartPointer<vtkShrinkPolyData> shrink = vtkSmartPointer<vtkShrinkPolyData>::New();
    shrink->SetInputConnection(source->GetOutputPort());
    shrink->SetShrinkFactor(0.8);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(shrink->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);
    return actor;
}

}