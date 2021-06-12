#include "Application.h"

#include <Windows.h>
#include <array>
#include <locale>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

int WINAPI wWinMain(_In_ HINSTANCE /*hInstance*/,
                    _In_opt_ HINSTANCE /*hPrevInstance*/,
                    _In_ PWSTR /*lpCmdLine*/,
                    _In_ int /*nCmdShow*/)
{
    // Setting up the system preferred locale, which is specified by using and empty string. The locale will inform how
    // to format numbers, times, dates, etc. The default locale is the 'C' locale, which doesn't do any specialized
    // formatting for numbers.
    std::locale::global(std::locale(""));

    std::shared_ptr<spdlog::logger> scrapLogger;
    {
        // setup a logger for the Visual Studio output window and for standard console out
        std::array<spdlog::sink_ptr, 2> sinks;
        sinks[0] = std::make_shared<spdlog::sinks::stdout_sink_mt>();
        sinks[1] = std::make_shared<spdlog::sinks::msvc_sink_mt>();

        scrapLogger = std::make_shared<spdlog::logger>("scrap", std::begin(sinks), std::end(sinks));
        spdlog::set_default_logger(scrapLogger);
    }

    scrap::Application app;
    while(app)
    {
        app.update();
        scrapLogger->flush();
    }

    return 0;
}