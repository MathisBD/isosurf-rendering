#pragma once
#include <assert.h>


void GLClearErrors();
// returns true if all is fine, false if an error was found
bool GLLogCall(const char* function, const char* file, int line);

#define GLCall(x) GLClearErrors();\
    x;\
    assert(GLLogCall(#x, __FILE__, __LINE__))
