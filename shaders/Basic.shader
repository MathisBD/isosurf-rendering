#shader vertex
#version 430 core
    
uniform vec2 u_cameraCenterPos;
uniform vec2 u_cameraViewSize;

in vec3 vertPosition;
in vec3 vertColor;

out vec3 fragColor;

void main()
{
    fragColor = vertColor;

    vec3 position = vertPosition;
    position.xy -= u_cameraCenterPos;
    position.xy /= 0.5*u_cameraViewSize;
    gl_Position = vec4(position, 1.0);
}


#shader fragment
#version 430 core

in vec3 fragColor;

out vec4 outColor;

void main()
{
    outColor = vec4(fragColor, 1.0);
}