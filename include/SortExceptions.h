#pragma once
#include <stdexcept>
#include <string>
#include <cstddef>

// Базовий клас користувацьких винятків бібліотеки сортування
class SortException : public std::runtime_error
{
public:
    explicit SortException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

// Помилка операції над порожнім масивом (пошук екстремумів, статистик)
class EmptyArrayException : public SortException
{
private:
    std::string operation_;

public:
    explicit EmptyArrayException(const std::string& operation)
        : SortException("Cannot perform operation on an empty array: " + operation),
          operation_(operation)
    {
    }

    const std::string& operation() const noexcept
    {
        return operation_;
    }
};

// Некоректний діапазон індексів для сортування підмасиву
class InvalidRangeException : public SortException
{
private:
    std::size_t left_;
    std::size_t right_;
    std::size_t arraySize_;

public:
    InvalidRangeException(std::size_t left, std::size_t right, std::size_t arraySize)
        : SortException("Invalid sort range [" + std::to_string(left) + ", " +
                        std::to_string(right) + "] for array of size " +
                        std::to_string(arraySize)),
          left_(left),
          right_(right),
          arraySize_(arraySize)
    {
    }

    std::size_t left() const noexcept { return left_; }
    std::size_t right() const noexcept { return right_; }
    std::size_t arraySize() const noexcept { return arraySize_; }
};

// Перевищення максимальної місткості масиву
class ArrayCapacityException : public SortException
{
private:
    std::size_t capacity_;
    std::size_t requested_;

public:
    ArrayCapacityException(std::size_t capacity, std::size_t requested)
        : SortException("Array capacity exceeded: capacity=" +
                        std::to_string(capacity) + ", requested=" +
                        std::to_string(requested)),
          capacity_(capacity),
          requested_(requested)
    {
    }

    std::size_t capacity() const noexcept { return capacity_; }
    std::size_t requested() const noexcept { return requested_; }
};

// Некоректний параметр вибору алгоритму сортування
class InvalidAlgorithmException : public SortException
{
private:
    int algorithmId_;

public:
    explicit InvalidAlgorithmException(int algorithmId)
        : SortException("Unknown sorting algorithm ID: " + std::to_string(algorithmId)),
          algorithmId_(algorithmId)
    {
    }

    int algorithmId() const noexcept { return algorithmId_; }
};
