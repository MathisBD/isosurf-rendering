#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <chrono>
#include <math.h>

// layout of a vertex : X Y grayscale
float vertices[] = {
    0.f, 0.5f, 1.0f,
    0.5f, -0.5f, 0.8f,
    -0.5f, -0.5f, 0.0f
};

const char* vertexShaderSource = R"glsl(
    #version 460 core
    
    in vec2 position;
    in float gray;

    out float Gray;

    void main()
    {
        Gray = gray;
        gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    }

)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 460 core

    in float Gray;

    out vec4 outColor;

    void main()
    {
        outColor = vec4(1.0 - Gray, 1.0 - Gray, 1.0 - Gray, 1.0);
    }
)glsl";

GLuint compileShader(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // check for compile errors
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        // print the error log
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        printf("[-] Shader compilation error\n%s", buffer);
    }
    return shader;
}

int main()
{
    // init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // init GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    
    // create the Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create the Vertex Buffer Object
    // and load the vertices
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // compile shaders
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    // link the shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), 0);
    
    GLint grayAttrib = glGetAttribLocation(shaderProgram, "gray");
    glEnableVertexAttribArray(grayAttrib);
    glVertexAttribPointer(grayAttrib, 1, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), (void*)(2 * sizeof(float)));
    
    // GLFW event loop
    while(!glfwWindowShouldClose(window))
    {   glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}