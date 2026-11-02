#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>
#include "SortExceptions.h"

// Антипатерн 1: генерування винятку примітивного типу int
void throwPrimitiveInt()
{
    throw 404;
}

// Антипатерн 2: генерування винятку типу std::string
void throwRawString()
{
    throw std::string("String exception: invalid array state");
}

// Антипатерн 3: втрата похідного типу (object slicing) через throw ex;
void demonstrateObjectSlicing(bool useBadRethrow)
{
    try
    {
        throw InvalidRangeException(5, 2, 4);
    }
    catch (const std::exception& ex)
    {
        if (useBadRethrow)
        {
            throw ex; // ПОМИЛКА: створює копію базового std::exception (object slicing)
        }
        throw; // ПРАВИЛЬНО: повторно поширює оригінальний InvalidRangeException
    }
}

int main()
{
    std::cout << "=== Example 1: int exception ===\n";
    try
    {
        throwPrimitiveInt();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught std::exception: " << ex.what() << '\n';
    }
    catch (int code)
    {
        std::cout << "std::exception handler skipped -> Caught via catch(int): " << code << '\n';
    }

    std::cout << "=== Example 2: std::string exception via catch(...) ===\n";
    try
    {
        throwRawString();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught std::exception: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "std::exception handler skipped -> Caught unknown exception via catch(...)\n";
    }

    std::cout << "=== Example 3: Standard/Custom hierarchy exception ===\n";
    try
    {
        throw InvalidRangeException(5, 2, 4);
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught via catch(const std::exception&): " << ex.what() << '\n';
    }

    std::cout << "=== Example 4: Object Slicing (throw ex; vs throw;) ===\n";
    try
    {
        demonstrateObjectSlicing(true);
    }
    catch (const InvalidRangeException& ex)
    {
        std::cout << "[throw ex;] Caught derived InvalidRangeException\n";
    }
    catch (const std::exception&)
    {
        std::cout << "[throw ex;] Caught base std::exception (derived InvalidRangeException sliced!)\n";
    }

    try
    {
        demonstrateObjectSlicing(false);
    }
    catch (const InvalidRangeException& ex)
    {
        std::cout << "[throw;   ] Caught derived InvalidRangeException: bounds ["
                  << ex.left() << ", " << ex.right() << "]\n";
    }

    return 0;
}
