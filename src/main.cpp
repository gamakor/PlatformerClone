#include "Kor_Lib.h"
#include "platform.h"
#include "input.h"
#include "game.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "../3rd Party Lib/glcorearb.h"

//Windows Platform
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"

// #############################################################################
//                           Game DLL Stuff
// #############################################################################
// This is the function pointer to update_game in game.cpp
typedef decltype(GameUpdate) update_game_type;
static update_game_type* update_game_ptr;

// #############################################################################
//                           Cross Platform Functions
// #############################################################################
void reload_game_dll(BumpAllocator* transientStorage);




int main()
{

    BumpAllocator transientStorage = MakeBumpAllocator(MB(50));
    BumpAllocator persistentStorage = MakeBumpAllocator(MB(50));

    input = (Input*)BumpAllocate(&persistentStorage,sizeof(Input));
    if (!input)
    {
        SM_ASSERT(false,"Failed to allocate input memory");
        return -1;
    }

    renderData = (RenderData*)BumpAllocate(&persistentStorage,sizeof(RenderData));

    if (!renderData)
    {
        SM_ASSERT(false,"Failed to allocate renderdata memory");
        return -1;
    }

    input->screenSizeX = 1280;
    input->screenSizeY = 720;
    PlatformCreateWindow("Platformer Clone", 1280, 720);

    InitGL(&transientStorage);
    while(isRunning)
    {
        reload_game_dll(&transientStorage);
        PlatformUpdateWindow();
        GameUpdate(renderData,input);
        GLRender();
        PlatformSwapBuffers();

        transientStorage.used = 0;
    }
    return 0;
}

void GameUpdate(RenderData* renderDataIn,Input* inputIN)
{
    update_game_ptr(renderDataIn,inputIN);
}

void reload_game_dll(BumpAllocator* transientStorage)
{
    static void* gameDLL;
    static long long lastEditTimestampGameDLL = 0;

    long long currentTimestampGameDLL = GetTimeStamp("game.dll");
    if (currentTimestampGameDLL > lastEditTimestampGameDLL)
    {
        if (gameDLL)
        {
            bool freeResult = platform_free_dynamic_library(gameDLL);
            SM_ASSERT(freeResult, "Failed to free dynamic library");
            gameDLL = nullptr;
            SM_TRACE("Freed game.dll");
        }


        while (!copy_file("game.dll", "game_load.dll",transientStorage))
        {
            Sleep(10);
        }
        SM_TRACE("Coppied game.dll in game_load.dll");

        gameDLL = platform_load_dynamic_library("game_load.dll");
        SM_ASSERT(gameDLL, "Failed to load game_load.dll");

        update_game_ptr = (update_game_type*)platform_load_dynamic_function(gameDLL,"GameUpdate");
        SM_ASSERT(update_game_ptr, "Failed to load game_load.dll");
        lastEditTimestampGameDLL = currentTimestampGameDLL;
    }
}