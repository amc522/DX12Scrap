#include "Application.h"
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE /*hInstance*/,
                   _In_opt_ HINSTANCE /*hPrevInstance*/,
                   _In_ PWSTR /*lpCmdLine*/,
                   _In_ int /*nCmdShow*/)
{
    scrap::Application app;
    while(app)
    {
        app.update();
    }

    return 0;
}