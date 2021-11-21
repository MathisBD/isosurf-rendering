#include "rendering/gl_errors.h"
#include <GL/glew.h>
#include <iostream>


void GLClearErrors() 
{
    while(glGetError());    
}

bool GLLogCall(const char* function, const char* file, int line) 
{
    unsigned int count = 0;
    while (GLenum error = glGetError()) {
        std::cout << "[-] GL Error " << error << std::endl;
        std::cout << "\t" << function << std::endl;
        std::cout << "\t" << file << ":" << line << std::endl;
        count++;
    }    
    return count == 0;
}