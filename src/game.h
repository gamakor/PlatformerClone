//
// Created by ddcha on 12/17/2025.
//
#pragma once
#ifndef PLATFORMERCLONE_GAME_H
#include "input.h"
#include "Kor_Lib.h"
#include "render_interface.h"
#include "vector"

#define PLATFORMERCLONE_GAME_H


// #############################################################################
//                           Game Globals
// #############################################################################

constexpr int FPS = 60;
constexpr float UPDATE_DELAY = 1.0f / FPS;
constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILE_SIZE = 8;

constexpr IVec2 WORLD_GRID = {WORLD_WIDTH/TILE_SIZE, WORLD_HEIGHT/TILE_SIZE};
// #############################################################################
//                           Game Structs
// #############################################################################
enum GameInputType {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    JUMP,

    MOUSE_LEFT, MOUSE_RIGHT,

    Game_Input_Count
};

struct KeyMapping {
   std::vector <KeyCodeID> keys{};
};

struct Tile {
    int neighbourMask;
    bool isVisible;
};

struct Player {
    IVec2 pos;
    IVec2 prevPos;
    Vec2 speed;
    Vec2 inheritedSpeed;
};

struct Solid {
    SpriteID spriteID;
    IVec2 pos;
    IVec2 prevPos;
    Vec2 remainder;
    Vec2 speed;
    int keyframeIndex;
    Array<IVec2,2> keyframes;
};

struct GameState {

    float updateTimer;

    bool initialized = false;

    Player player;
    Array<Solid,20> solids;

    Array<IVec2,21> tileCoords;
    Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y];
    KeyMapping keyMappings[Game_Input_Count];
};



// #############################################################################
//                           Game Structs
// #############################################################################
static GameState* gameState;
// #############################################################################
//                           Game Functions
// #############################################################################

extern "C"
{
    EXPORT_FN void GameUpdate(GameState* gameStateIn,RenderData* renderDataIn,Input* inputIN,float deltaTime);
}


#endif //PLATFORMERCLONE_GAME_H