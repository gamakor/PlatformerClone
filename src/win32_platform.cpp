//
// Created by ddcha on 12/10/2025.
//
#pragma once
#include "platform.h"
#include "Kor_Lib.h"
#include "input.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "../3rd Party Lib/wglext.h"
#define GL_GLEXT_PROTOTYPES
#include "../3rd Party Lib/glcorearb.h"

//Windows Globals
static HWND window;
static HDC dc;

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

        case WM_CLOSE:
        {
            isRunning = false;
        }break;

        case WM_SIZE:
        {
            RECT rect = {};
            GetClientRect(window, &rect);
            input->screenSizeX = rect.right - rect.left;
            input->screenSizeY = rect.bottom - rect.top;
        } break;

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

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    //Fake Window init OpenGL
    {
        window = CreateWindowA(title, title,
                                    WS_OVERLAPPEDWINDOW,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    width, height,
                                    NULL, NULL,
                                    instance, NULL);
        if (!window)
        {
            SM_ASSERT(false,"Failed to create window");
            return false;
        }

        HDC fakeDC = GetDC(window);
        if (!fakeDC)
        {
            SM_ASSERT(false,"Failed to get device context");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cAlphaBits = 8;

        int pixelFormat = ChoosePixelFormat(fakeDC, &pfd);
        if (!pixelFormat)
        {
            SM_ASSERT(false,"Failed to choose pixel format");
            return false;
        }

        if (!SetPixelFormat(fakeDC, pixelFormat, &pfd))
        {
            SM_ASSERT(false,"Failed to set pixel format");
            return false;
        }

        //Create A handle to a fake OpenGL Rendering Context
        HGLRC fakeGLContext = wglCreateContext(fakeDC);
        if (!fakeGLContext)
        {
            SM_ASSERT(false,"Failed to create fake OpenGL context");
            return false;
        }
        if (!wglMakeCurrent(fakeDC, fakeGLContext))
        {
            SM_ASSERT(false,"Failed to make fake OpenGL context current");
            return false;
        }

         wglChoosePixelFormatARB =
            (PFNWGLCHOOSEPIXELFORMATARBPROC)PlatformLoadGLFunction("wglChoosePixelFormatARB");
         wglCreateContextAttribsARB =
            (PFNWGLCREATECONTEXTATTRIBSARBPROC)PlatformLoadGLFunction("wglCreateContextAttribsARB");

        if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)
        {
            SM_ASSERT(false,"Failed to load OpenGL functions");
            return false;
        }

        //Clean up of fake window.
        wglMakeCurrent(fakeDC, NULL);
        wglDeleteContext(fakeGLContext);
        ReleaseDC(window, fakeDC);
    }
    //Cant reuse the dame Device context
    //because we already called "SetPixel Format"

    //Actual GL Init

    {
        //Add Border size of the window
        RECT borderRect ={};
        AdjustWindowRectEx(&borderRect,WS_OVERLAPPEDWINDOW,0,0);

        width += borderRect.right - borderRect.left;
        height += borderRect.bottom - borderRect.top;
    }

    window = CreateWindowA(title, title,
                                    WS_OVERLAPPEDWINDOW,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    width, height,
                                    NULL, NULL,
                                    instance, NULL);
    if (!window)
    {
        SM_ASSERT(false,"Failed to create window");
        return false;
    }

    dc = GetDC(window);
    if (!dc)
    {
        SM_ASSERT(false,"Failed to get device context");
        return false;
    }

    const int pixelAttribs[] ={
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
     WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
     WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
     WGL_SWAP_METHOD_ARB,    WGL_SWAP_COPY_ARB,
     WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
     WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
     WGL_COLOR_BITS_ARB,     32,
     WGL_ALPHA_BITS_ARB,     8,
     WGL_DEPTH_BITS_ARB,     24,
     0
        };// Terminate with 0, otherwise OpenGL will throw an Error!

    UINT numPixelFormats;
    int pixelFormat = 0;
    if(!wglChoosePixelFormatARB(dc, pixelAttribs,
                                0, // Float List
                                1, // Max Formats
                                &pixelFormat,
                                &numPixelFormats))
    {
        SM_ASSERT(0, "Failed to wglChoosePixelFormatARB");
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd = {0};
    DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if(!SetPixelFormat(dc, pixelFormat, &pfd))
    {
        SM_ASSERT(0, "Failed to SetPixelFormat");
        return true;
    }

    const int contextAttribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0 // Terminate the Array
      };

    HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
    if(!rc)
    {
        SM_ASSERT(0, "Failed to crate Render Context for OpenGL");
        return false;
    }

    if(!wglMakeCurrent(dc, rc))
    {
        SM_ASSERT(0, "Faield to wglMakeCurrent");
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

void* PlatformLoadGLFunction(char* funcName)
{
    PROC proc = wglGetProcAddress(funcName);

    if (!proc)
    {
        static HMODULE opengl32 = LoadLibraryA("opengl32.dll");
        proc = GetProcAddress(opengl32, funcName);
       if (!proc)
       {
           SM_ASSERT(false,"Failed to load gl function %s","glCreateProgram");
           return nullptr;
       }
    }

    return (void*)proc;
}

void PlatformSwapBuffers()
{
    SwapBuffers(dc);
}

void* platform_load_dynamic_library(char* dll)
{
    HMODULE result = LoadLibraryA(dll);
    SM_ASSERT(result, "Failed to load dynamic library: %s",dll);

    return result;
}
void* platform_load_dynamic_function(void* dll,char* funName)
{
    FARPROC proc = GetProcAddress((HMODULE)dll, funName);
    SM_ASSERT(proc, "Failed to load dynamic library function: %s",funName);
    return (void*)proc;
}
bool platform_free_dynamic_library(void* dll)
{
    BOOL freeResult = FreeLibrary((HMODULE)dll);
    SM_ASSERT(freeResult, "Failed to free library");

    return (bool)freeResult;
}