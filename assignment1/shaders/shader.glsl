#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform vec4 u_Color;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_FragPos; // for the light

void main()
{
    v_Color = u_Color;
    v_Normal = a_Normal;
    v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 a_Color;

in vec4 v_Color;
in vec3 v_Normal; 
in vec3 v_FragPos; 

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(0, 0, 2.3);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec4 result = vec4(ambient + diffuse, 1.0) * v_Color;
    a_Color = result;

}


