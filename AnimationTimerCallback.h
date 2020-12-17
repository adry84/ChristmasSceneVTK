#ifndef __ANIMATION_TIMER_CALLBACK_H__
#define __ANIMATION_TIMER_CALLBACK_H__

#include <vtkCommand.h>
#include "ChristmasScene.h"
namespace ChristmasSceneVTK {
    class AnimationTimerCallback : public vtkCommand
    {
    public:
        static AnimationTimerCallback* New()
        {
            AnimationTimerCallback* cb = new AnimationTimerCallback;
            cb->timerCount = 0;
            cb->christmasScene = nullptr;
            return cb;
        }

        void init(ChristmasScene* christmasScene);
        virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId, void* vtkNotUsed(callData));


    private:
        int timerCount;
        bool isStopped = false;
        ChristmasScene* christmasScene;

        double starPosition[3] = { 0.0, 0.0, 0.0 };
        const double a = 0.05;
        const double b = 0.10;
        const double cameraAnimStartTime = 80.;
        const double cameraAnimTimeout = 120.;
        const double starAnimTimeout = 80.;
        const double snowFallTimeout = 60.;
        const int rotationAngle = 10;

        void MoveStar();
        void MoveSowFlakes();
        void MovePineTree();
        void MoveCamera();

    };
}
#endif