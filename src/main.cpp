#include "applications/sandbox.h"


int main()
{
    auto app = Sandbox();
    app.Initialize();
    app.MainLoop();
    app.Cleanup();
    return 0;
}