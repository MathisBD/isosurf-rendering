#shader vertex
#version 430 core

// model view projection matrix    
uniform mat4 u_camera;

in vec3 vertPosition;
in vec3 vertNormal;

out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
    fragPosition = vertPosition;
    fragNormal = vertNormal;
    gl_Position = u_camera * vec4(vertPosition, 1.0);
}


#shader fragment
#version 430 core

uniform vec3 u_color;
uniform vec3 u_lightDirection;

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 outColor;

void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 light = normalize(u_lightDirection);
    float diffuse = max(dot(normal, -light), 0);

    outColor = vec4(u_color, 1.0) * diffuse;
}