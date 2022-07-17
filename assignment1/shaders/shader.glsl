#type vertex
#version 450 core
// I am using the same file for the vertex and fragement shader
// The Shader class knows which is which because of preprocessor commands (i.e. type X)

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

struct Camera   {
    mat4 viewProjection;
    vec3 position;
};

uniform Camera u_Camera;
uniform mat4 u_Transform;
uniform mat4 lightSpaceMatrix;

out vec4 v_Normal;
out vec3 v_FragPos; // for the light
out vec2 v_TexCoord;
out vec3 v_reflectedVector;
out vec4 v_LightSpace;

void main()
{
    v_TexCoord = a_TexCoord;
    v_Normal = vec4(transpose(inverse(mat3(u_Transform))) * a_Normal, 1.0);
    v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
    v_LightSpace = lightSpaceMatrix * vec4(v_FragPos, 1.0);

    gl_Position = u_Camera.viewProjection * u_Transform * vec4(a_Position, 1.0);
    gl_PointSize = 7.0;

    vec3 viewVector = normalize(v_FragPos - u_Camera.position);
    v_reflectedVector = reflect(viewVector, normalize(v_Normal).xyz);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 a_Color;

in vec4 v_Normal; 
in vec3 v_FragPos; 
in vec2 v_TexCoord;
in vec3 v_reflectedVector;
in vec4 v_LightSpace;

uniform sampler2D ourTexture;
uniform samplerCube u_CubeMap;

struct Light {
    vec3 position;
    vec4 color;
    float ambientStrength;
};

struct Material {
    vec4 color;
    float shininess;
}; 

uniform Light u_Light;
uniform Material u_Material;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform int u_allowShadows;

float ShadowCalculation(vec4 fragPosLightSpace);
void main()
{
    vec3 color = texture(ourTexture, v_TexCoord).rgb * u_Material.color.xyz;

    vec3 lightColor = u_Light.color.xyz;
    vec3 ambient = u_Light.ambientStrength * lightColor;

    // Diffuse lighting
    vec4 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_Light.position - v_FragPos);
    float diff = max(dot(norm.xyz, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // calculate shadow
    float shadow = u_allowShadows != 0 ? ShadowCalculation(v_LightSpace) : 0.3f;               
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;    

    a_Color = vec4(lighting, 1.0);

    // For metal shininess
    if (u_Material.shininess > 0.1) {
        vec4 reflectedColor = texture(u_CubeMap, v_reflectedVector);
        vec4 result = vec4(ambient + diffuse, 1.0) * texture(ourTexture, v_TexCoord) * u_Material.color;
        a_Color = mix(vec4(lighting, 1.0), reflectedColor, u_Material.shininess);
    }   
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


