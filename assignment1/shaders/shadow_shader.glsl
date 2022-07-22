#type vertex
#version 450 core
// I am using the same file for the vertex and fragement shader
// The Shader class knows which is which because of preprocessor commands (i.e. type X)

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 lightSpaceMatrix;
uniform mat4 u_Transform;

void main()
{
    gl_Position = lightSpaceMatrix * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

void main()
{         
   //gl_FragDepth = gl_FragCoord.z;
}  

