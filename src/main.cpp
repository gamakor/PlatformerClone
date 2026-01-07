#include "Kor_Lib.h"
#include "platform.h"
#include "input.h"
#include "game.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "../3rd Party Lib/glcorearb.h"

KeyCodeID KeyCodeLookupTable[KEY_COUNT];

//Windows Platform
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"
#include <chrono>
// #############################################################################
//                           Game DLL Stuff
// #############################################################################
// This is the function pointer to update_game in game.cpp
typedef decltype(GameUpdate) update_game_type;
static update_game_type* update_game_ptr;

// #############################################################################
//                           Cross Platform Functions
// #############################################################################

double get_delta_time();
void reload_game_dll(BumpAllocator* transientStorage);




int main()
{
    //init timestamp
    get_delta_time();
    
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

    gameState = (GameState*)BumpAllocate(&persistentStorage,sizeof(GameState));
    {
        if (!gameState)
        {
            SM_ERROR("Failed to allocate gamestate");
            return -1;
        }
    }

    platform_fill_keycode_lookup_table();
    PlatformCreateWindow("Platformer Clone", 1280, 720);
    platform_set_vsync(true);

    InitGL(&transientStorage);
    while(isRunning)
    {
        float dt =get_delta_time();
        reload_game_dll(&transientStorage);
        PlatformUpdateWindow();
        GameUpdate(gameState,renderData,input,dt);
        GLRender(&transientStorage);
        PlatformSwapBuffers();

        transientStorage.used = 0;
    }
    return 0;
}

void GameUpdate(GameState* gameStateIn, RenderData* renderDataIn,Input* inputIN,float deltaTime)
{
    update_game_ptr(gameStateIn,renderDataIn,inputIN,deltaTime);
}
double get_delta_time()
{
    //only executed once when entering the function (static)

    static auto lastTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();

    //seconds
    double delta = std::chrono::duration<double>(currentTime - lastTime).count();
    lastTime = currentTime;
    return delta;
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
