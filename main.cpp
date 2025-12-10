#include <iostream>
#include "Kor_Lib.h"

//Platform Globals
static bool isRunning = true;

//Platform Funtions
bool PlatformCreateWindow(const char* title, int width, int height);
void PlatformUpdateWindow();

//Windows Platform
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
//Windows Globals
static HWND window;

//Platform Implementations
LRESULT CALLBACK WindowsWindowCallback(HWND window, UINT msg,
                                        WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (msg)
    {
        case WM_DESTROY:
        {
            isRunning = false;
        }break;
        default:
        {
            result= DefWindowProcA(window, msg, wParam, lParam);
        } ;
    }
    return result;
}
bool PlatformCreateWindow(const char* title, int width, int height)
{
    HINSTANCE instance = GetModuleHandleA(NULL);

    WNDCLASSA wc = {0};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);     //Icon look
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);       //How the curor will look
    wc.lpfnWndProc = WindowsWindowCallback;                        //Callback for input
    wc.lpszClassName = title;


    if (!RegisterClassA(&wc))
        return false;

    window = CreateWindowA(title, title,
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                width, height,
                                NULL, NULL,
                                instance, NULL);
    if (!window)
    {
        return false;
    }
    ShowWindow(window, SW_SHOWDEFAULT);
    return true;
}

void PlatformUpdateWindow()
{
    MSG msg;
    while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}
#endif


int main()
{
    PlatformCreateWindow("Platformer Clone", 1280, 720);
    std::cout << "Hello, World!" << std::endl;
    while(isRunning)
    {
        PlatformUpdateWindow();
        SM_TRACE("Hello, World!");
        SM_ASSERT(false,"Test");


    }
    return 0;
}