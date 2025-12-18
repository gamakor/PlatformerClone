//
// Created by ddcha on 12/15/2025.
//


#pragma once

#ifndef PLATFORMERCLONE_RENDER_INTERFACE_H
#define PLATFORMERCLONE_RENDER_INTERFACE_H
#include "assets.h"
#include "Kor_Lib.h"


// #############################################################################
//                           Renderer Constants
// #############################################################################
constexpr int MAX_TRANSFORMS = 1000;


// #############################################################################
//                           Renderer Structs
// #############################################################################
struct OrthographicCamera2D
{
    float zoom = 1.0f;
    Vec2 dimensions = {0.0f,0.0f};
    Vec2 position = {0.0f,0.0f};
};

struct Transform
{
    IVec2 atlasOffset;
    IVec2 spriteSize;
    Vec2 scale;
    Vec2 position;

};

struct RenderData
{
    OrthographicCamera2D camera;
    OrthographicCamera2D uiCamera;

    int transformCount;
    Transform transforms[MAX_TRANSFORMS];
};

// #############################################################################
//                           Renderer Globals
// #############################################################################

static RenderData* renderData;

// #############################################################################
//                           Renderer Functions
// #############################################################################


void DrawSprite(SpriteID spriteID, Vec2 position, Vec2 scale)
{
    Sprite sprite = GetSprite(spriteID);
    Transform transform = {};
    transform.position = position;
    transform.scale = scale;
    transform.atlasOffset = sprite.atlasOffset;
    transform.spriteSize = sprite.spriteSize;

    renderData->transforms[renderData->transformCount++] = transform;

}

#endif //PLATFORMERCLONE_RENDER_INTERFACE_H