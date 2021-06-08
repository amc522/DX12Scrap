#include "Application.h"

int main(int argc, const char **argv)
{
    scrap::Application app;
    while(app)
    {
        app.update();
    }

    return 0;
}