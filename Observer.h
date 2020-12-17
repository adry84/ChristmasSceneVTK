#ifndef __CHRISTMAS_OBSERVER_H__
#define __CHRISTMAS_OBSERVER_H__

namespace ChristmasSceneVTK {

    class Observer : public vtkCommand {
    public:
        static Observer* New() {
            return new Observer;
        }

        void Execute(vtkObject* caller, unsigned long eventId, void* idontnow)
        {
            //vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
            //std::string key = interactor->GetKeySym();
            if (isMoving) {
                cout << "isMoving set to false " << endl;
                isMoving = false;
                moveTime = 0.;
                return;
            }
            else {
                cout << "isMoving set to true " << endl;
                isMoving = true;
            }
        }

    private:
        bool isMoving = false;
        unsigned long moveTime = 0;
    };
}

#endif