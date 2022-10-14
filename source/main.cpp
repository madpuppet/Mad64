// Using SDL and standard IO
#include "common.h"

int main(int argc, char *args[])
{
    auto app = new Application();
    app->MainLoop();
    delete app;
    return 0;
}




