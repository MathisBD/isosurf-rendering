#shader vertex
#version 430 core

// model view projection matrices    
uniform mat4 u_worldToView;
uniform mat4 u_viewToScreen;

in vec3 vertPosition;
in vec3 vertNormal;

void main()
{
    gl_Position = u_viewToScreen * u_worldToView * vec4(vertPosition, 1.0);
}


#shader fragment
#version 430 core

uniform vec3 u_color;

out vec4 outColor;

void main()
{
    outColor = vec4(u_color, 1.0);
}