#include "applications/sandbox.h"


int main()
{
    auto app = Sandbox();
    app.initialize();
    app.mainLoop();
    app.cleanup();
    return 0;
}