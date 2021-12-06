#include "applications/tetra_app.h"
#include "applications/cube_app.h"
#include "thread_pool.h"


int main()
{
    auto app = TetraApp();
    app.MainLoop();
    return 0;
}