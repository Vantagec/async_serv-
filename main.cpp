#include <iostream>
#include <utility>
#include <async.h>
#include <streamredirect.hpp>
#include "version.h"
#include "bulkserver.hpp"

// using namespace std;

int main(int argc, const char *argv[])
{
    std::cout << "Async app version: " << PROJECT_VERSION << std::endl;

    const auto bulkSize = argc;
    if (bulkSize != 3) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " <port> <bulk size>" << std::endl;
        return 1;
    }

    // обработка параметра командной строки с числом команд в блоке
    const std::uint16_t serverPort = std::atoi(argv[1]);

    // обработка параметра командной строки с числом команд в блоке
    const int bulkCommandsLimit = std::atoi(argv[2]);
    if (bulkCommandsLimit < 1)
        return 0;

    try {
        asio::io_context io_context(1);

        asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io_context.stop(); });

        asio::co_spawn(io_context, listener(io_context, serverPort, bulkCommandsLimit), asio::detached);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }

    return 0;

}
