#include <gtest/gtest.h>
#include "SortableArray.h"
#include "SortExceptions.h"
#include "ScopeLogger.h"
#include <limits>
#include <stdexcept>
#include <vector>
#include <string>

// 1. Створення коректного масиву та дотримання інваріантів
TEST(SortableArrayTest, CreatesValidArrayAndInvariants)
{
    SortableArray arr(5);
    EXPECT_EQ(arr.capacity(), 5u);
    EXPECT_EQ(arr.size(), 0u);
    EXPECT_TRUE(arr.empty());

    arr.add(12.5);
    arr.add(-4.0);
    EXPECT_EQ(arr.size(), 2u);
    EXPECT_FALSE(arr.empty());
    EXPECT_DOUBLE_EQ(arr.at(0), 12.5);
    EXPECT_DOUBLE_EQ(arr.at(1), -4.0);
}

// 2. Перевірка нульової місткості
TEST(SortableArrayTest, RejectsZeroCapacity)
{
    EXPECT_THROW(SortableArray(0), std::invalid_argument);
}

// 3. Сценарій помилки 1: Вихід за межі індексу (std::out_of_range)
TEST(SortableArrayTest, ThrowsOutOfRangeOnInvalidIndex)
{
    SortableArray arr(3);
    arr.add(1.0);
    EXPECT_THROW(arr.at(1), std::out_of_range);
    EXPECT_THROW(arr.at(100), std::out_of_range);
    EXPECT_THROW(arr.setAt(2, 5.0), std::out_of_range);
}

// 4. Сценарій помилки 2: Порожній масив для екстремумів та статистик (EmptyArrayException)
TEST(SortableArrayTest, ThrowsOnEmptyArrayExtremum)
{
    SortableArray arr(4);
    EXPECT_THROW(arr.min(), EmptyArrayException);
    EXPECT_THROW(arr.max(), EmptyArrayException);
    EXPECT_THROW(arr.mean(), EmptyArrayException);
    EXPECT_THROW(arr.median(), EmptyArrayException);
    EXPECT_THROW(arr.sort(), EmptyArrayException);
}

// 5. Сценарій помилки 3: Некоректний діапазон сортування (InvalidRangeException)
TEST(SortableArrayTest, ThrowsOnInvalidSortRange)
{
    SortableArray arr(5);
    arr.add(10.0);
    arr.add(20.0);
    arr.add(30.0);

    try
    {
        arr.sortRange(2, 1, SortAlgorithm::QuickSort);
        FAIL() << "Expected InvalidRangeException";
    }
    catch (const InvalidRangeException& ex)
    {
        EXPECT_EQ(ex.left(), 2u);
        EXPECT_EQ(ex.right(), 1u);
        EXPECT_EQ(ex.arraySize(), 3u);
    }

    EXPECT_THROW(arr.sortRange(0, 5, SortAlgorithm::MergeSort), InvalidRangeException);
}

// 6. Сценарій помилки 4: Перевищення максимальної місткості (ArrayCapacityException)
TEST(SortableArrayTest, ThrowsOnCapacityOverflow)
{
    SortableArray arr(2);
    arr.add(1.0);
    arr.add(2.0);

    try
    {
        arr.add(3.0);
        FAIL() << "Expected ArrayCapacityException";
    }
    catch (const ArrayCapacityException& ex)
    {
        EXPECT_EQ(ex.capacity(), 2u);
        EXPECT_EQ(ex.requested(), 3u);
    }
}

// 7. Сценарій помилки 5: Некоректний параметр вибору алгоритму (InvalidAlgorithmException)
TEST(SortableArrayTest, ThrowsOnInvalidAlgorithmParameter)
{
    SortableArray arr(3);
    arr.add(5.0);
    arr.add(1.0);

    const auto badAlgo = static_cast<SortAlgorithm>(99);
    try
    {
        arr.sort(badAlgo);
        FAIL() << "Expected InvalidAlgorithmException";
    }
    catch (const InvalidAlgorithmException& ex)
    {
        EXPECT_EQ(ex.algorithmId(), 99);
    }
}

// 8. Відхилення NaN та Infinity
TEST(SortableArrayTest, RejectsNonFiniteValues)
{
    SortableArray arr(3);
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(arr.add(inf), std::invalid_argument);
    EXPECT_THROW(arr.add(-inf), std::invalid_argument);
    EXPECT_THROW(arr.add(nan), std::invalid_argument);
    EXPECT_EQ(arr.size(), 0u);
}

// 9. Перевірка коректності всіх чотирьох алгоритмів сортування
TEST(SortableArrayTest, SortsCorrectlyAllAlgorithms)
{
    const std::vector<double> input = {42.0, -7.5, 19.0, 0.0, 8.2, -7.5};
    const std::vector<double> expected = {-7.5, -7.5, 0.0, 8.2, 19.0, 42.0};

    for (SortAlgorithm algo : {SortAlgorithm::QuickSort, SortAlgorithm::MergeSort,
                               SortAlgorithm::HeapSort, SortAlgorithm::InsertionSort})
    {
        SortableArray arr(10);
        arr.replaceAndSort(input, algo);
        ASSERT_EQ(arr.size(), expected.size());
        EXPECT_TRUE(arr.isSorted());
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            EXPECT_DOUBLE_EQ(arr.at(i), expected[i]);
        }
    }
}

// 10. Перевірка обчислень із плаваючою комою за допомогою EXPECT_NEAR
TEST(SortableArrayTest, CalculatesMeanAndMedianWithTolerance)
{
    SortableArray arr(5);
    arr.add(0.1);
    arr.add(0.2);
    arr.add(0.3);
    arr.add(0.4);

    EXPECT_NEAR(arr.mean(), 0.25, 1e-12);
    EXPECT_NEAR(arr.median(), 0.25, 1e-12);
    EXPECT_NEAR(arr.min(), 0.1, 1e-12);
    EXPECT_NEAR(arr.max(), 0.4, 1e-12);
}

// 11. Сильна гарантія для add()
TEST(ExceptionSafetyTest, AddPreservesStateOnCapacityOverflow)
{
    SortableArray arr(2);
    arr.add(10.0);
    arr.add(20.0);

    EXPECT_THROW(arr.add(30.0), ArrayCapacityException);

    ASSERT_EQ(arr.size(), 2u);
    EXPECT_DOUBLE_EQ(arr.at(0), 10.0);
    EXPECT_DOUBLE_EQ(arr.at(1), 20.0);
}

// 12. Сильна гарантія для sortRange()
TEST(ExceptionSafetyTest, SortRangePreservesStateOnInvalidAlgorithm)
{
    SortableArray arr(4);
    arr.add(40.0);
    arr.add(10.0);
    arr.add(30.0);

    EXPECT_THROW(
        arr.sortRange(0, 2, static_cast<SortAlgorithm>(77)),
        InvalidAlgorithmException
    );

    ASSERT_EQ(arr.size(), 3u);
    EXPECT_DOUBLE_EQ(arr.at(0), 40.0);
    EXPECT_DOUBLE_EQ(arr.at(1), 10.0);
    EXPECT_DOUBLE_EQ(arr.at(2), 30.0);
}

// 13. Сильна гарантія для replaceAndSort() при перевищенні місткості
TEST(ExceptionSafetyTest, ReplaceAndSortStrongGuaranteeOnCapacityError)
{
    SortableArray arr(3);
    arr.add(5.0);
    arr.add(15.0);

    const std::vector<double> tooMany = {1.0, 2.0, 3.0, 4.0};
    EXPECT_THROW(
        arr.replaceAndSort(tooMany, SortAlgorithm::QuickSort),
        ArrayCapacityException
    );

    ASSERT_EQ(arr.size(), 2u);
    EXPECT_DOUBLE_EQ(arr.at(0), 5.0);
    EXPECT_DOUBLE_EQ(arr.at(1), 15.0);
}

// 14. Сильна гарантія для replaceAndSort() при некоректному алгоритмі
TEST(ExceptionSafetyTest, ReplaceAndSortStrongGuaranteeOnInvalidAlgo)
{
    SortableArray arr(4);
    arr.add(40.0);
    arr.add(10.0);
    arr.add(30.0);

    const std::vector<double> replacement = {8.0, 15.0, 23.0};
    EXPECT_THROW(
        arr.replaceAndSort(replacement, static_cast<SortAlgorithm>(-5)),
        InvalidAlgorithmException
    );

    // Початковий стан [40, 10, 30] повністю збережений
    ASSERT_EQ(arr.size(), 3u);
    EXPECT_DOUBLE_EQ(arr.at(0), 40.0);
    EXPECT_DOUBLE_EQ(arr.at(1), 10.0);
    EXPECT_DOUBLE_EQ(arr.at(2), 30.0);
}

// 15. Успішне виконання replaceAndSort()
TEST(ExceptionSafetyTest, ReplaceAndSortUpdatesSuccessfully)
{
    SortableArray arr(4);
    arr.add(100.0);

    arr.replaceAndSort({30.0, 10.0, 20.0}, SortAlgorithm::MergeSort);
    ASSERT_EQ(arr.size(), 3u);
    EXPECT_DOUBLE_EQ(arr.at(0), 10.0);
    EXPECT_DOUBLE_EQ(arr.at(1), 20.0);
    EXPECT_DOUBLE_EQ(arr.at(2), 30.0);
}

// 16. Перевірка порядку знищення локальних об'єктів при розкручуванні стека
TEST(ExceptionSafetyTest, StackUnwindingDestroysScopeLoggersInReverseOrder)
{
    std::vector<std::string> events;
    auto runLevels = [&events]() {
        ScopeLogger l1("L1", &events);
        {
            ScopeLogger l2("L2", &events);
            throw InvalidRangeException(3, 1, 2);
        }
    };

    EXPECT_THROW(runLevels(), InvalidRangeException);

    const std::vector<std::string> expected = {
        "ENTER:L1", "ENTER:L2", "EXIT:L2", "EXIT:L1"
    };
    ASSERT_EQ(events.size(), expected.size());
    EXPECT_EQ(events, expected);
}

// 17. Негативний самоконтроль (Етап 8): вимкнений за замовчуванням тест,
// який демонструє порушення сильної гарантії в методі replaceAndSortUnsafe()
TEST(ExceptionSafetyTest, DISABLED_UnsafeSortAndReplaceViolatesStrongGuarantee)
{
    SortableArray arr(4);
    arr.add(40.0);
    arr.add(10.0);
    arr.add(30.0);

    const std::vector<double> replacement = {8.0, 15.0, 23.0};
    EXPECT_THROW(
        arr.replaceAndSortUnsafe(replacement, static_cast<SortAlgorithm>(99)),
        InvalidAlgorithmException
    );

    ASSERT_EQ(arr.size(), 3u);
    EXPECT_DOUBLE_EQ(arr.at(0), 40.0);
    EXPECT_DOUBLE_EQ(arr.at(1), 10.0);
    EXPECT_DOUBLE_EQ(arr.at(2), 30.0);
}
