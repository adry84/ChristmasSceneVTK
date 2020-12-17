#include "ChristmasScene.h"
#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

using namespace ChristmasSceneVTK;

int main(int argc, char* argv[])
{
    ChristmasScene& scene = ChristmasScene();
	scene.draw();
    return EXIT_SUCCESS;
}