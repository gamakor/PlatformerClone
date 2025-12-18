//
// Created by ddcha on 12/9/2025.
//
#pragma once

#ifndef PLATFORMERCLONE_KOR_LIB_H
#define PLATFORMERCLONE_KOR_LIB_H

#include <cstring>
#include <iostream>
#include <sys/stat.h>

//#####################################
//          DEFINES
//#####################################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#define EXPORT_FN __declspec(dllexport)
#endif

#define BIT(x) 1 << (x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

//#####################################
//          Logging
//#####################################
enum TextColor{
    TEXT_COLOR_BLACK,
 TEXT_COLOR_RED,
 TEXT_COLOR_GREEN,
 TEXT_COLOR_YELLOW,
 TEXT_COLOR_BLUE,
 TEXT_COLOR_MAGENTA,
 TEXT_COLOR_CYAN,
 TEXT_COLOR_WHITE,
 TEXT_COLOR_BRIGHT_BLACK,
 TEXT_COLOR_BRIGHT_RED,
 TEXT_COLOR_BRIGHT_GREEN,
 TEXT_COLOR_BRIGHT_YELLOW,
 TEXT_COLOR_BRIGHT_BLUE,
 TEXT_COLOR_BRIGHT_MAGENTA,
 TEXT_COLOR_BRIGHT_CYAN,
 TEXT_COLOR_BRIGHT_WHITE,
 TEXT_COLOR_COUNT
};

template<typename... Args>
void _log(char* prefix, const char* msg,TextColor color,Args...args)
{
    static char* TextColorTable[TEXT_COLOR_COUNT] =
    {
        "\x1b[30m", // TEXT_COLOR_BLACK
        "\x1b[31m", // TEXT_COLOR_RED
        "\x1b[32m", // TEXT_COLOR_GREEN
        "\x1b[33m", // TEXT_COLOR_YELLOW
        "\x1b[34m", // TEXT_COLOR_BLUE
        "\x1b[35m", // TEXT_COLOR_MAGENTA
        "\x1b[36m", // TEXT_COLOR_CYAN
        "\x1b[37m", // TEXT_COLOR_WHITE
        "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
        "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
        "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
        "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
        "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
        "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
        "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
        "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
      };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[color], prefix, msg);

    char textBuffer[8912] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("Warn: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("Error: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define SM_ASSERT(x, msg, ...)    \
{                                 \
if(!(x))                        \
{                               \
SM_ERROR(msg, ##__VA_ARGS__); \
DEBUG_BREAK();                \
SM_ERROR("Assertion HIT!")    \
}                               \
}

//#####################################
//          BUMP ALLOCATOR
//#####################################
struct BumpAllocator {
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator MakeBumpAllocator(size_t size)
{
    BumpAllocator ba = {};
    ba.memory = (char*)calloc(1,size);

    if (ba.memory)
    {
        ba.capacity = size;

    }else
    {
        SM_ERROR("Failed to allocate memory");
    }
    return ba;
}
char* BumpAllocate(BumpAllocator* allocator, size_t size)
{
    char* result = 0;
    size_t AllignedSize = (size + 7) & ~7;
    if (allocator->used + AllignedSize <= allocator->capacity)
    {
        result = allocator->memory + allocator->used;
        allocator->used += AllignedSize;
    }else
        SM_ASSERT(false,"Out of memory");
    return result;
}

//#####################################
//          FILE I/O
//#####################################

long long GetTimeStamp(char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_mtime;
}

bool FileExists(const char* filepath)
{
    SM_ASSERT(filepath,"Filepath is null");

    auto file = fopen(filepath, "rb");
    if (!file)
        return false;

    fclose(file);
    return true;
}

long GetFileSize(const char* filepath)
{
    SM_ASSERT(filepath,"Filepath is null");

    long fileSize = 0;
    auto file = fopen(filepath, "rb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s",filepath);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fclose(file);
    return fileSize;
}

//Reads a fle into a supplied buffer.

char* ReadFile(char* filePath,int*fileSize,char*buffer)
{
    SM_ASSERT(filePath,"Filepath is null");
    SM_ASSERT(fileSize,"Filesize is null");
    SM_ASSERT(buffer,"Buffer is null");

    *fileSize = 0;
    auto file = fopen(filePath, "rb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s",filePath);
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(buffer,0,*fileSize);
    fread(buffer,sizeof(char),*fileSize,file);
    fclose(file);

    return buffer;
}

char* ReadFile(char* filePath,int* filesize, BumpAllocator* bumpAllocator)
{
    char* file = nullptr;
    long tempFileSize = GetFileSize(filePath);

    if (tempFileSize)
    {
        char* buffer = BumpAllocate(bumpAllocator, tempFileSize+1);
        file = ReadFile(filePath,filesize,buffer);
    }
    return file;
}

void WriteFile(char* filePath,char* buffer,int fileSize)
{
    SM_ASSERT(filePath,"Filepath is null");
    SM_ASSERT(buffer,"Buffer is null");
    SM_ASSERT(fileSize,"Filesize is null");

    auto file = fopen(filePath, "wb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s",filePath);
        return;
    }
    fwrite(buffer,sizeof(char),fileSize,file);
    fclose(file);
}

bool copy_file(char* fileName,char* outputName,char* buffer)
{
    int fileSize = 0;
    char* data = ReadFile(fileName,&fileSize,buffer);

    auto outputFile = fopen(outputName, "wb");
    if (!outputFile)
    {
        SM_ERROR("Failed to open file %s",outputName);
        return false;
    }
    int result = fwrite(data,sizeof(char),fileSize,outputFile);

    if (result != fileSize)
    {
        SM_ERROR("Failed to write file %s",outputName);
        return false;
    }
    fclose(outputFile);
    return true;
}

bool copy_file(char* fileName,char* outputName,BumpAllocator* bumpAllocator)
{
    int fileSize = 0;
    long tempFileSize = GetFileSize(fileName);

    if (tempFileSize)
    {
        char* buffer = BumpAllocate(bumpAllocator, tempFileSize+1);
        return copy_file(fileName,outputName,buffer);
    }
    return false;
}

//#####################################
//          Math
//#####################################

struct Vec2 {
    float x,y;
};

struct IVec2 {
    int x,y;
};

struct Vec4 {
    union {
        float values[4];
        struct {
          float x,y,z,w;
        };
        struct {
            float r,g,b,a;
        };
    };
};

struct Mat4 {
    union {
        Vec4 values[4];
        struct {
            float ax,bx,cx,dx;
            float ay,by,cy,dy;
            float az,bz,cz,dz;
            float aw,bw,cw,dw;
        };
    };
};




#endif //PLATFORMERCLONE_KOR_LIB_H