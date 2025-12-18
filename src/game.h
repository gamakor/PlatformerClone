//
// Created by ddcha on 12/17/2025.
//
#pragma once
#ifndef PLATFORMERCLONE_GAME_H
#include "input.h"
#include "Kor_Lib.h"
#include "render_interface.h"

#define PLATFORMERCLONE_GAME_H


// #############################################################################
//                           Game Globals
// #############################################################################
// #############################################################################
//                           Game Structs
// #############################################################################
// #############################################################################
//                           Game Functions
// #############################################################################

extern "C"
{
    EXPORT_FN void GameUpdate(RenderData* renderDataIn,Input* inputIN);
}
#endif //PLATFORMERCLONE_GAME_H