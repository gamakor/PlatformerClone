#version 430 core

//structs
struct Transform
{
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 scale;
    vec2 position;


};


//input
layout(std430,binding =0) buffer TransformSBO
{
    Transform transforms[];
};

uniform vec2 screenSize;

//output
layout (location =0) out vec2 textureCoordsOut;



void main()
{
    Transform transform = transforms[gl_InstanceID];

    // Generating Vertices on the GPU
    // mostly because we have a 2D Engine

    // OpenGL Coordinates
    // -1/ 1                1/ 1
    // -1/-1                1/-1
    vec2 vertices[6] =
    {
    // Top Left
    transform.position,

    // Bottom Left
    vec2(transform.position + vec2(0.0, transform.scale.y)),

    // Top Right
    vec2(transform.position + vec2(transform.scale.x, 0.0)),

    // Top Right
    vec2(transform.position + vec2(transform.scale.x,0.0)),

    // Bottom Left
    vec2(transform.position + vec2(0.0, transform.scale.y)),

    // Bottom Right
    transform.position + transform.scale
    };

    int left = transform.atlasOffset.x;
    int right = transform.atlasOffset.x + transform.spriteSize.x;
    int top = transform.atlasOffset.y;
    int  bottom =transform.atlasOffset.y + transform.spriteSize.y;

    vec2 textureCoords[6] =
    {
        vec2(left,top),
        vec2(left,bottom),
        vec2(right,top),
        vec2(right,top),
        vec2(left, bottom),
        vec2(right,bottom),
    };




    //Normalize Pos

    {
        vec2 vertexPos = vertices[gl_VertexID];
        vertexPos.y = -vertexPos.y + screenSize.y;
        vertexPos =  2.0 * (vertexPos/screenSize) - 1.0;
        gl_Position = vec4(vertexPos, 0.0,1.0);
    }
    textureCoordsOut = textureCoords[gl_VertexID];
}