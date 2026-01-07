//
// Created by ddcha on 12/15/2025.
//


#pragma once

#ifndef PLATFORMERCLONE_RENDER_INTERFACE_H
#define PLATFORMERCLONE_RENDER_INTERFACE_H
#include <vector>

#include "assets.h"
#include "Kor_Lib.h"


// #############################################################################
//                           Renderer Constants
// #############################################################################



// #############################################################################
//                           Renderer Structs
// #############################################################################
struct OrthographicCamera2D
{
    float zoom = 1.0f;
    Vec2 dimensions ;
    Vec2 position ;
};

struct Transform
{
    IVec2 atlasOffset;
    IVec2 spriteSize;
    Vec2 size;
    Vec2 position;

};

struct RenderData
{
    OrthographicCamera2D gameCamera;
    OrthographicCamera2D uiCamera;


    Array<Transform,1000> transforms;
};

// #############################################################################
//                           Renderer Globals
// #############################################################################

static RenderData* renderData;

// #############################################################################
//                           Renderer Utility
// #############################################################################
IVec2 screen_to_world(IVec2 screenPos)
{
    OrthographicCamera2D camera = renderData->gameCamera;

    int xPos = (float)screenPos.x /
               (float)input->screenSize.x *
               camera.dimensions.x; // [0; dimensions.x]

    // Offset using dimensions and position
    xPos += -camera.dimensions.x / 2.0f + camera.position.x;

    int yPos = (float)screenPos.y /
               (float)input->screenSize.y *
               camera.dimensions.y; // [0; dimensions.y]

    // Offset using dimensions and position
    yPos += camera.dimensions.y / 2.0f + camera.position.y;

    return {xPos, yPos};
}

// #############################################################################
//                           Renderer Functions
// #############################################################################
void draw_quad(Transform transform)
{
    renderData->transforms.add(transform);
}

void draw_quad(Vec2 pos, Vec2 size)
{
    Transform transform = {};
    transform.position = pos - size/2.0f;
    transform.size = size;
    transform.atlasOffset = {0,0};
    transform.spriteSize = {1,1};

    renderData->transforms.add(transform);
}

void DrawSprite(SpriteID spriteID, Vec2 position)
{
    Sprite sprite = GetSprite(spriteID);
    Transform transform = {};
    transform.position = position - vec_2(sprite.size)/2;
    transform.size = vec_2(sprite.size);
    transform.atlasOffset = sprite.atlasOffset;
    transform.spriteSize = sprite.size;

    renderData->transforms.add(transform);

}
void DrawSprite(SpriteID spriteID, IVec2 position)
{
    DrawSprite(spriteID,vec_2(position));
}




#endif //PLATFORMERCLONE_RENDER_INTERFACE_H