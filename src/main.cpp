#include "applications/tetra_app.h"
#include "applications/cube_app.h"


int main()
{
    auto app = CubeApp();
    app.MainLoop();
    return 0;
}