#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    TexCoords = aPos;
    vec4 pos = proj * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  

#type fragment
#version 450 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}

