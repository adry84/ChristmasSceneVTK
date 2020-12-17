#include "AnimationTimerCallback.h"

namespace  ChristmasSceneVTK {

void AnimationTimerCallback::init(ChristmasScene* christmasScene) {
	this->christmasScene = christmasScene;
    this->starPosition[0] = christmasScene->star->GetPosition()[0];
    this->starPosition[1] = christmasScene->star->GetPosition()[1];
    this->starPosition[2] = christmasScene->star->GetPosition()[2];
    isStopped = false;

}

void AnimationTimerCallback:: Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId, void* vtkNotUsed(callData)) {
    if (isStopped) {
        return;
    }
    if (vtkCommand::TimerEvent == eventId)
    {

        MovePineTree();

        ++this->timerCount;

        if (this->timerCount > cameraAnimStartTime && this->timerCount < cameraAnimTimeout) {
            MoveCamera();
        }
        if (this->timerCount < starAnimTimeout) {
            MoveStar();
        }
        else if (this->timerCount == starAnimTimeout) {
            christmasScene->star->RotateY(45.0);
            christmasScene->star->SetPosition(0., 18.5, 2.);
        }
        if (this->timerCount < snowFallTimeout) {
            MoveSowFlakes();
        }
        else if (this->timerCount == snowFallTimeout) {
            christmasScene->snowRenderer->SetDraw(false);
        }
        christmasScene->renderWindow->Render();

    }
}

void AnimationTimerCallback::MoveStar() {
    double t = 0.5 * timerCount * 3.14;
    cout << "MoveStar [" << starPosition[0] << "," << starPosition[1] << "," << starPosition[2] << "]" << endl;
    starPosition[0] = a * exp(b * t) * cos(t);
    starPosition[1] = a * exp(b * t) * sin(t);
    starPosition[2] = t / 2;
    christmasScene->star->SetPosition(starPosition);
}

void AnimationTimerCallback::MoveSowFlakes() {
    double t = 1. / snowFallTimeout * timerCount;
    double posY = t < 0.5 ? 2 * t * t : 2 * t * (2 - t) - 1; //easeout anim
    vtkProp3DCollection* parts = christmasScene->snowFlakesAssembly->GetParts();

    vtkCollectionSimpleIterator iterator;
    vtkProp3D* prop3D;
    for (parts->InitTraversal(iterator); (prop3D =
        parts->GetNextProp3D(iterator)); )
    {
        if (prop3D != NULL)
        {
            auto actor = vtkActor::SafeDownCast(prop3D);
            double* snowPosition = actor->GetPosition();
            snowPosition[1] = snowPosition[1] - ((rand() % 100) / 10. + 300.) * posY;
            if (snowPosition[1] < 34) {
                snowPosition[1] = 2478.;
            }
            cout << "snowPosition[1] : " << snowPosition[1] << endl;
            actor->RotateZ((rand() % 1800) / 10.);
            actor->RotateX((rand() % 1800) / 10.);
            actor->RotateY((rand() % 1800) / 10.);
            actor->SetPosition(snowPosition);
        }
    }

    christmasScene->snowRenderer->Render();
}

void AnimationTimerCallback::MovePineTree() {
    double* viewUp = christmasScene->sceneCamera->GetViewUp();
    christmasScene->pineTreeAssembly->RotateWXYZ(rotationAngle, viewUp[0], viewUp[1], viewUp[2]);
}

void AnimationTimerCallback::MoveCamera() {
    // Camera Parameters ///////////////////////////////////////////////////
    double* focalPoint = christmasScene->sceneCamera->GetFocalPoint();
    double* viewUp = christmasScene->sceneCamera->GetViewUp();
    double* position = christmasScene->sceneCamera->GetPosition();
    double axis[3];
    axis[0] = -christmasScene->sceneCamera->GetViewTransformMatrix()->GetElement(0, 0);
    axis[1] = -christmasScene->sceneCamera->GetViewTransformMatrix()->GetElement(0, 1);
    axis[2] = -christmasScene->sceneCamera->GetViewTransformMatrix()->GetElement(0, 2);


    // Build The transformation /////////////////////////////////////////////////
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();

    double center[3] = { 0., 0., 0. }; // cylinderActor->GetCenter();
    transform->Translate(center);
    transform->RotateWXYZ(rotationAngle, viewUp); // Azimuth
    //transform->RotateWXYZ(0, axis);   // Elevation
    transform->Translate(-center[0], -center[1], -center[2]);

    double newPosition[3];
    transform->TransformPoint(position, newPosition); // Transform Position
    double newFocalPoint[3];
    transform->TransformPoint(focalPoint, newFocalPoint); // Transform Focal Point

    christmasScene->sceneCamera->SetPosition(newPosition);
    christmasScene->sceneCamera->SetFocalPoint(newFocalPoint);

    // Orhthogonalize View Up //////////////////////////////////////////////////
    christmasScene->sceneCamera->OrthogonalizeViewUp();
    christmasScene->sceneRenderer->ResetCameraClippingRange();

    christmasScene->renderWindow->Render();
}

}


