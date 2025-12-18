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
    GLint screenSizeID;

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
bool InitGL(BumpAllocator* transientStorage)
{
    LoadOpenGLFunctions();

    glDebugMessageCallback(&gl_debug_callback,nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize =0;
    char* vertShader = ReadFile("C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.vert",&fileSize,transientStorage);
    char* fragShader = ReadFile("C:/Users/ddcha/CLionProjects/PlatformerClone/assets/shaders/quad.frag",&fileSize,transientStorage);

    if (!vertShader || !fragShader)
    {
        SM_ASSERT(false,"Failed to load shaders");
        return false;
    }

    glShaderSource(vertexShaderID, 1, &vertShader, nullptr);
    glShaderSource(fragmentShaderID, 1, &fragShader, nullptr);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    //Checks success of vert shader
    {
        int success;

        char shaderInfoLog[512];
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShaderID, 512, NULL, shaderInfoLog);
            SM_ASSERT(false,"Failed to compile vertex shader: %s\n",shaderInfoLog);
        }
    }

    //Check success of frag shader
    {
        int success;

        char fragInfoLog[512];
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShaderID, 512, NULL, fragInfoLog);
            SM_ASSERT(false,"Failed to compile vertex shader: %s\n",fragInfoLog);
        }
    }

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

        stbi_image_free(data);
    }


    //Transform Storage Buffer
    {
        glGenBuffers(1,&glContext.transformSBOID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,glContext.transformSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(Transform)* MAX_TRANSFORMS,
                            renderData->transforms,GL_DYNAMIC_DRAW);

    }

    //Uniforms

    {
        glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");
    }

    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); //disable multisampling


    //Depth testing
    glEnable(GL_DEPTH_TEST);;
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID);

    return true;
}

void GLRender()
{
    glClearColor(119.0f/225.0f, 33.0f/255.0f, 110.0f/255.0f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, input->screenSizeX, input->screenSizeY);


    //Copy screen Size to the GPU
    Vec2 screenSize = {(float)input->screenSizeX, (float)input->screenSizeY};
    glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

    //Opaque Objects
    {
        //Copy Transforms to the GPU
        glBufferSubData(GL_SHADER_STORAGE_BUFFER,0,sizeof(Transform)* renderData->transformCount,
                        renderData->transforms);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transformCount);

        //reset for the next frame
        renderData->transformCount = 0;
    }

}