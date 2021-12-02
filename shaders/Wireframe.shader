#shader vertex
#version 430 core

// model view projection matrices    
uniform mat4 u_worldToView;
uniform mat4 u_viewToScreen;

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertColor;

out vec3 fragColor;

void main()
{
    fragColor = vertColor;
    gl_Position = u_viewToScreen * u_worldToView * vec4(vertPosition, 1.0);
}


#shader fragment
#version 430 core

uniform vec3 u_color;

in vec3 fragColor;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragColor, 1.0);
}