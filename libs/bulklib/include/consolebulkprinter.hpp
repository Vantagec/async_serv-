#pragma once

#include <consoleprinter.hpp>
#include <iostream>
#include "bulkprinter.hpp"

namespace bulk_defs {

/// @brief Класс для печати блока команд в консоль
class ConsoleBulkPrinter: public hw_io::ConsolePrinter, public BulkPrinter
{
public:
    using hw_io::ConsolePrinter::ConsolePrinter;
    ~ConsoleBulkPrinter() {std::cout << std::flush;}

    /// @brief Перегрузка абстрактной функции для печати блока команд в консоль
    /// @param bulk shared_ptr на блок команд
    void printBulk(std::shared_ptr<const Bulk> bulk) override final
    {
        print(BulkPrinter::bulkToString(*bulk));
    }
};

}
