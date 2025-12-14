#include "Kor_Lib.h"
#include "platform.h"
#include "input.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "3rd Party Lib/glcorearb.h"

//Windows Platform
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"



int main()
{

    BumpAllocator transientStorage = MakeBumpAllocator(MB(50));
    input.screenSizeX = 1280;
    input.screenSizeY = 720;
    PlatformCreateWindow("Platformer Clone", 1280, 720);

    InitGL(&transientStorage);
    while(isRunning)
    {
        PlatformUpdateWindow();
        GLRender();
        PlatformSwapBuffers();
    }
    return 0;
}