#pragma once

#include <asio.hpp>
#include <async.h>
#include <cstdio>
#include <iostream>

using asio::ip::tcp;

///
/// \brief The RaiiHelper class RAII - хэлпер для упрощения работы с функциями библиотеки async
///
class AsyncHelper
{
public:
    ///
    /// \brief AsyncHelper Коснструктор
    /// \param bulkCommandsLimit Размер блока команд
    ///
    AsyncHelper(std::size_t bulkCommandsLimit): m_handler {async::connect(bulkCommandsLimit)} {}

    ///
    /// \brief AsyncHelper Удалённый конструктор копирования
    ///
    AsyncHelper(const AsyncHelper &) = delete;

    ///
    /// \brief AsyncHelper Конструктор перемещения
    ///
    AsyncHelper(AsyncHelper &&) noexcept = default;

    ///
    /// \brief ~AsyncHelper Деструктор. Делает disconnect
    ///
    ~AsyncHelper() { async::disconnect(m_handler); }

    AsyncHelper &operator=(const AsyncHelper &) = delete;
    AsyncHelper &operator=(AsyncHelper &&) = default;

    ///
    /// \brief receive Вызывает метод async::receive для своего хэндлера
    /// \param data указатель начало последовательного блока передаваемых данных
    /// \param size размер блока данных
    ///
    void receive(const char * const data, std::size_t size) const;

private:
    async::print_handler_t m_handler;
};

///
/// \brief session_reader корутина для чтения из сокета
/// \param socket - клиентский сокет
/// \param bulkCommandsLimit размер блока команд
/// \return
///
asio::awaitable<void> session_reader(tcp::socket socket, std::size_t bulkCommandsLimit)
{
    try {
        auto clientSocket = std::move(socket);
        auto asyncHandler = AsyncHelper {bulkCommandsLimit};
        for (std::string read_msg;;) {
            std::size_t n = co_await asio::async_read_until(clientSocket,
                                                            asio::dynamic_buffer(read_msg, 1024), "\n", asio::use_awaitable);

            asyncHandler.receive(read_msg.c_str(), n - 1);
            read_msg.erase(0, n);
        }
    } catch (std::exception& e) {
        std::cerr << "session_reader exception" << e.what() << std::endl;
    }
}

///
/// \brief listener Корутина ассинхроной обработки  входящих tcp-соединений
/// \param executor Конктекст выыполнения
/// \param serverPort  Порт TCP-сервера
/// \param bulkCommandsLimit Размер блока команд
/// \return
///
asio::awaitable<void> listener(asio::io_context &executor, std::uint16_t serverPort, std::size_t bulkCommandsLimit)
{
    tcp::acceptor acceptor(executor, {tcp::v6(), serverPort});
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
        asio::co_spawn(executor, session_reader(std::move(socket), bulkCommandsLimit), asio::detached);
    }
}

inline void AsyncHelper::receive(const char * const data, std::size_t size) const
{
    async::receive(m_handler, data, size);
}
