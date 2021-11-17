#include "application.h"


int main()
{
    auto app = Application();
    app.mainLoop();
    app.cleanup();
    return 0;
}