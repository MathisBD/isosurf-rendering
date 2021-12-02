#shader vertex
#version 430 core

// model view projection matrices    
uniform mat4 u_worldToView;
uniform mat4 u_viewToScreen;

layout (location = 0) in vec3 vertPosition;

out vec3 fragViewPos;

void main()
{
    vec4 viewPos = u_worldToView * vec4(vertPosition, 1.0);
    fragViewPos = viewPos.xyz;
    gl_Position = u_viewToScreen * viewPos;
}


#shader fragment
#version 430 core

uniform vec3 u_color;
uniform vec3 u_lightDirection;

in vec3 fragViewPos;

layout (location = 0) out vec4 outColor;

void main()
{
    vec3 xTangent = dFdx(fragViewPos);
    vec3 yTangent = dFdy(fragViewPos);
    vec3 normal = normalize(cross(xTangent, yTangent));
    
    vec3 light = normalize(u_lightDirection);
    float diffuse = max(dot(normal, -light), 0);
    
    float ambient = 0.4;

    outColor = vec4(u_color, 1.0) * (ambient + diffuse);
}