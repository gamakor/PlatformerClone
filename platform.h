//
// Created by ddcha on 12/10/2025.
//

#pragma once

#ifndef PLATFORMERCLONE_PLATFORM_H
#define PLATFORMERCLONE_PLATFORM_H

#endif //PLATFORMERCLONE_PLATFORM_H

//Platform Globals
static bool isRunning = true;

//Platform Funtions
bool PlatformCreateWindow(const char* title, int width, int height);
void PlatformUpdateWindow();
void* PlatformLoadGLFunction(char* funcName);
void PlatformSwapBuffers();