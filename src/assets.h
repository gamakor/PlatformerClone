//
// Created by ddcha on 12/15/2025.
//
#pragma once
#ifndef PLATFORMERCLONE_ASSETS_H
#define PLATFORMERCLONE_ASSETS_H
#include "Kor_Lib.h"

//#####################################
//          Assets Constants
//#####################################

//#####################################
//          Assets Structs
//#####################################
enum SpriteID
{
    SPRITE_DICE,


    SPRITE_COUNT

};

struct Sprite
{
    IVec2 atlasOffset;
    IVec2 spriteSize;
};
//#####################################
//          Assets Functions
//#####################################

Sprite GetSprite(SpriteID spriteID)
{
    Sprite sprite = {};
    switch (spriteID)
    {
        case SPRITE_DICE:
        {
            sprite.atlasOffset = {16,0};
            sprite.spriteSize = {16,16};
        }
    }

    return sprite;
}


#endif //PLATFORMERCLONE_ASSETS_H