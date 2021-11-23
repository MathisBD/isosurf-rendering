#shader vertex
#version 430 core

// model view projection matrix    
uniform mat4 u_MVP;

in vec3 vertPosition;
in vec3 vertColor;

out vec3 fragColor;

void main()
{
    fragColor = vertColor;
    gl_Position = u_MVP * vec4(vertPosition, 1.0);
}


#shader fragment
#version 430 core

in vec3 fragColor;

out vec4 outColor;

void main()
{
    outColor = vec4(fragColor, 1.0);
}