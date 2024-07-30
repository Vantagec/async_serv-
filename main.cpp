#include <iostream>
#include <utility>
#include <asio.hpp>
#include <async.h>
#include <streamredirect.hpp>
#include "version.h"
#include "bulkserver.hpp"

// Использование пространства имен std

void printUsage(const char *programName) {
    std::cout << "Usage:" << std::endl;
    std::cout << programName << " <port> <bulk size>" << std::endl;
}

int main(int argc, const char *argv[]) {
    std::cout << "Async app version: " << PROJECT_VERSION << std::endl;

    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }

    const auto serverPort = static_cast<std::uint16_t>(std::stoi(argv[1]));
    const auto bulkCommandsLimit = std::stoi(argv[2]);

    if (bulkCommandsLimit < 1) {
        std::cout << "Bulk size must be greater than 0" << std::endl;
        return 0;
    }

    try {
        asio::io_context ioContext(1);

        // Установка сигналов для корректной остановки сервера
        asio::signal_set signals(ioContext, SIGINT, SIGTERM);
        signals.async_wait([&](const asio::error_code&, int) { ioContext.stop(); });

        // Запуск сервера
        asio::co_spawn(ioContext, listener(ioContext, serverPort, bulkCommandsLimit), asio::detached);

        // Запуск обработчика контекста ввода-вывода
        ioContext.run();
    } 
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

