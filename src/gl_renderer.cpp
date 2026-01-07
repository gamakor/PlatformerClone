//
// Created by ddcha on 12/13/2025.
//

#include "gl_renderer.h"
#include "render_interface.h"
#include "input.h"

//To load PNG Files
#define STB_IMAGE_IMPLEMENTATION
#include "../3rd Party Lib/stb_image.h"

// #############################################################################
//                           OpenGL Constants
// #############################################################################

const char* TEXTUREPATH = "C:/Users/ddcha/CLionProjects/PlatformerClone/assets/textures/TEXTURE_ATLAS.png";



// #############################################################################
//                           OpenGL Structs
// #############################################################################


struct GLContext {
    GLuint programID;
    GLuint textureID;
    GLuint transformSBOID;
    GLuint screenSizeID;
    GLuint orthoProjectionID;


    long long textureTimestamp;
    long long shaderTimestamp;



};

// #############################################################################
//                           OpenGL Globals
// #############################################################################
static GLContext glContext;

// #############################################################################
//                           OpenGL Functions
// #############################################################################
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                         GLsizei length, const GLchar* message, const void* user)
{
    if(severity == GL_DEBUG_SEVERITY_LOW ||
       severity == GL_DEBUG_SEVERITY_MEDIUM ||
       severity == GL_DEBUG_SEVERITY_HIGH)
    {
        SM_ASSERT(false, "OpenGL Error: %s", message);
    }
    else
    {
        SM_TRACE((char*)message);
    }
}

GLuint gl_create_shader(int shaderType, char* shaderPath, BumpAllocator* transientStorage)
{
    int fileSize =0;
    char* vertShader = ReadFile(shaderPath,&fileSize,transientStorage);
    if (!vertShader)
    {
        SM_ASSERT(false,"Failed to load shader: %s", shaderPath);
    }
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &vertShader, nullptr);
    glCompileShader(shaderID);

    //test if shader complied successfully
    {
        int success;
        char shaderLog[2048] ={};

        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderID, 2048, NULL, shaderLog);
            SM_ASSERT(false,"Failed to compile shader: %s\n",shaderLog);
            return 0;
        }
    }

    return shaderID;
}



bool InitGL(BumpAllocator* transientStorage)
{
    LoadOpenGLFunctions();

    glDebugMessageCallback(&gl_debug_callback,nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    //need to fix this so it works with in the project only and doesnt need an exact path

    GLuint vertexShaderID = gl_create_shader(GL_VERTEX_SHADER,
        "C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.vert",transientStorage);
    GLuint fragmentShaderID = gl_create_shader(GL_FRAGMENT_SHADER,
        "C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.frag",transientStorage);

    if (!vertexShaderID || !fragmentShaderID)
    {
        SM_ASSERT(false,"Failed to load shaders");
        return false;
    }

    long long timestampVert = GetTimeStamp("C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.vert");
    long long timestampFrag = GetTimeStamp("C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.frag");
    glContext.shaderTimestamp = max(timestampVert,timestampFrag);

    glContext.programID = glCreateProgram();
    glAttachShader(glContext.programID, vertexShaderID);
    glAttachShader(glContext.programID, fragmentShaderID);
    glLinkProgram(glContext.programID);


    glDetachShader(glContext.programID, vertexShaderID);
    glDetachShader(glContext.programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);


    //Has to be done
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    {
        //Texture Loading STBI
        int width, height, nrChannels;
        unsigned char* data = stbi_load(TEXTUREPATH, &width, &height, &nrChannels, 0);

        if (!data)
        {
            SM_ASSERT(false,"Failed to load texture");
            return false;
        }

        glGenTextures(1,&glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);

        // set the texture wrapping/filtering options (on the currently bound texture object
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, data);

        glContext.textureTimestamp = GetTimeStamp(TEXTUREPATH);

        stbi_image_free(data);
    }


    //Transform Storage Buffer
    {
        glGenBuffers(1,&glContext.transformSBOID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,glContext.transformSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(Transform)* renderData->transforms.maxElements,
                            renderData->transforms.elements,GL_DYNAMIC_DRAW);

    }

    //Uniforms

    {
        glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");
        glContext.orthoProjectionID = glGetUniformLocation(glContext.programID, "orthoProjection");


    }

    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); //disable multisampling


    //Depth testing
    glEnable(GL_DEPTH_TEST);;
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID);

    return true;
}

void GLRender(BumpAllocator* transientStorage)
{
    //texture hot reloading
    {
        long long currentTimestamp = GetTimeStamp(TEXTUREPATH);

        if (currentTimestamp > glContext.textureTimestamp)
        {
           glActiveTexture(GL_TEXTURE0);
            int width, height, nrChannels;
            char* data = (char*)stbi_load(TEXTUREPATH, &width, &height, &nrChannels, 4);
            if (data)
            {
                glContext.textureTimestamp = currentTimestamp;
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
        }
    }
    //shader hot reloading
    {
        long long timestampVert = GetTimeStamp("C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.vert");
        long long timestampFrag = GetTimeStamp("C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.frag");
        if (timestampVert > glContext.shaderTimestamp || timestampFrag > glContext.shaderTimestamp)
        {

            GLuint vertexShaderID = gl_create_shader(GL_VERTEX_SHADER, "C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.vert", transientStorage);
            GLuint fragmentShaderID = gl_create_shader(GL_FRAGMENT_SHADER, "C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.frag", transientStorage);

            if (!vertexShaderID || !fragmentShaderID)
            {
                SM_ASSERT(false,"Failed to load shaders");
                return;
            }

            glAttachShader(glContext.programID, vertexShaderID);
            glAttachShader(glContext.programID, fragmentShaderID);
            glLinkProgram(glContext.programID);

            glDetachShader(glContext.programID, vertexShaderID);
            glDetachShader(glContext.programID, fragmentShaderID);
            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);

            glContext.shaderTimestamp = max(timestampVert,timestampFrag);
        }
    }








    glClearColor(119.0f/225.0f, 33.0f/255.0f, 110.0f/255.0f, 1.0f);
    glClearDepth(-1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, input->screenSize.x, input->screenSize.y);


    //Copy screen Size to the GPU
    Vec2 screenSize = {(float)input->screenSize.x, (float)input->screenSize.y};
    glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

    OrthographicCamera2D camera = renderData->gameCamera;
    Mat4 orthoProjection = orthographic_projection(camera.position.x - camera.dimensions.x / 2.0f,
                                                   camera.position.x + camera.dimensions.x / 2.0f,
                                                   camera.position.y - camera.dimensions.y / 2.0f,
                                                   camera.position.y + camera.dimensions.y / 2.0f);
    glUniformMatrix4fv(glContext.orthoProjectionID, 1, GL_FALSE, &orthoProjection.ax);

    //Opaque Objects
    {



            // Use .data() to get the pointer to the array
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData->transforms.count,
                            renderData->transforms.elements);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transforms.count);

            // Clear the vector for the next frame
            renderData->transforms.clear();
        }
    }

