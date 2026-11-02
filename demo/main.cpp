#include <iostream>
#include <iomanip>
#include <exception>
#include "SortableArray.h"
#include "SortExceptions.h"
#include "ScopeLogger.h"

void printArray(const SortableArray& arr, const std::string& label)
{
    std::cout << label << " (size=" << arr.size() << ", capacity=" << arr.capacity() << "): [";
    for (std::size_t i = 0; i < arr.size(); ++i)
    {
        std::cout << arr.at(i) << (i + 1 < arr.size() ? ", " : "");
    }
    std::cout << "]\n";
}

// Рівень 3 — виникнення винятку під час пакетної заміни та сортування
void level3()
{
    ScopeLogger logger("level3");
    std::cout << "Executing sort on capacity-overflow batch...\n";
    SortableArray arr(4);
    arr.replaceAndSort({10.0, 20.0, 30.0, 40.0, 50.0, 60.0}, SortAlgorithm::QuickSort);
}

// Рівень 2 — перехоплення за const&, журналювання та повторне генерування через throw;
void level2()
{
    ScopeLogger logger("level2");
    try
    {
        level3();
    }
    catch (const std::exception& ex)
    {
        std::cout << "[LOG] level2 caught: " << ex.what() << '\n';
        throw; // Зберігає оригінальний поліморфний тип ArrayCapacityException
    }
}

// Рівень 1 — проміжний виклик
void level1()
{
    ScopeLogger logger("level1");
    level2();
}

int main()
{
    std::cout << "=== [Stage 1-3] SortableArray Basic Operations & Exceptions ===\n";
    try
    {
        SortableArray arr(8);
        for (double v : {42.0, 15.0, 8.0, 99.0, 23.0})
        {
            arr.add(v);
        }
        printArray(arr, "Initial array");
        std::cout << std::fixed << std::setprecision(2)
                  << "Min element: " << arr.min()
                  << ", Max element: " << arr.max()
                  << ", Mean: " << arr.mean() << '\n';

        arr.sort(SortAlgorithm::QuickSort);
        printArray(arr, "After QuickSort range [0..4]");

        SortableArray emptyArr(4);
        emptyArr.min();
    }
    catch (const EmptyArrayException& ex)
    {
        std::cout << "Caught EmptyArrayException: " << ex.what()
                  << " (operation: " << ex.operation() << ")\n";
    }

    std::cout << "\n=== [Stage 4] Strong Exception Safety Guarantee (prepare-then-commit) ===\n";
    SortableArray safeArr(6);
    for (double v : {50.0, 10.0, 40.0, 20.0, 30.0}) safeArr.add(v);
    printArray(safeArr, "Before failed operation");

    try
    {
        safeArr.sortCopyAndReplace(4, 1, SortAlgorithm::MergeSort);
    }
    catch (const InvalidRangeException& ex)
    {
        std::cout << "Caught InvalidRangeException: " << ex.what() << '\n';
    }
    printArray(safeArr, "After failed operation (state preserved!)");

    std::cout << "\n=== [Stage 5] Stack Unwinding & RAII (ScopeLogger) ===\n";
    try
    {
        level1();
    }
    catch (const ArrayCapacityException& ex)
    {
        std::cout << "[MAIN] Caught ArrayCapacityException: capacity="
                  << ex.capacity() << ", requested=" << ex.requested() << '\n';
    }
    catch (const SortException& ex)
    {
        std::cout << "[MAIN] Caught SortException: " << ex.what() << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << "[MAIN] Caught std::exception: " << ex.what() << '\n';
    }

    std::cout << "Program continues normally.\n";
    return 0;
}
