#include "SortableArray.h"
#include "SortExceptions.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <utility>

void SortableArray::validateValue(double value)
{
    if (!std::isfinite(value))
    {
        throw std::invalid_argument("Array element must be a finite number (not NaN or Infinity)");
    }
}

void SortableArray::validateAlgorithm(SortAlgorithm algorithm)
{
    const int id = static_cast<int>(algorithm);
    if (id < static_cast<int>(SortAlgorithm::QuickSort) ||
        id > static_cast<int>(SortAlgorithm::InsertionSort))
    {
        throw InvalidAlgorithmException(id);
    }
}

SortableArray::SortableArray(std::size_t capacity)
    : capacity_(capacity)
{
    if (capacity == 0)
    {
        throw std::invalid_argument("SortableArray capacity must be greater than zero");
    }
    data_.reserve(capacity);
}

std::size_t SortableArray::size() const noexcept
{
    return data_.size();
}

std::size_t SortableArray::capacity() const noexcept
{
    return capacity_;
}

bool SortableArray::empty() const noexcept
{
    return data_.empty();
}

bool SortableArray::isSorted() const noexcept
{
    return std::is_sorted(data_.begin(), data_.end());
}

void SortableArray::clear() noexcept
{
    data_.clear();
}

double SortableArray::at(std::size_t index) const
{
    if (index >= data_.size())
    {
        throw std::out_of_range("SortableArray index out of range");
    }
    return data_[index];
}

// Strong guarantee
void SortableArray::setAt(std::size_t index, double value)
{
    if (index >= data_.size())
    {
        throw std::out_of_range("SortableArray index out of range");
    }
    validateValue(value);
    data_[index] = value;
}

// Strong guarantee
void SortableArray::add(double value)
{
    validateValue(value);
    if (data_.size() >= capacity_)
    {
        throw ArrayCapacityException(capacity_, data_.size() + 1);
    }
    data_.push_back(value);
}

double SortableArray::min() const
{
    if (data_.empty())
    {
        throw EmptyArrayException("min");
    }
    return *std::min_element(data_.begin(), data_.end());
}

double SortableArray::max() const
{
    if (data_.empty())
    {
        throw EmptyArrayException("max");
    }
    return *std::max_element(data_.begin(), data_.end());
}

double SortableArray::mean() const
{
    if (data_.empty())
    {
        throw EmptyArrayException("mean");
    }
    const double sum = std::accumulate(data_.begin(), data_.end(), 0.0);
    const double result = sum / static_cast<double>(data_.size());
    if (!std::isfinite(result))
    {
        throw std::overflow_error("Arithmetic overflow while computing mean");
    }
    return result;
}

double SortableArray::median() const
{
    if (data_.empty())
    {
        throw EmptyArrayException("median");
    }
    std::vector<double> temp = data_;
    std::sort(temp.begin(), temp.end());
    const std::size_t n = temp.size();
    if (n % 2 == 1)
    {
        return temp[n / 2];
    }
    return 0.5 * (temp[n / 2 - 1] + temp[n / 2]);
}

void SortableArray::quickSortImpl(std::vector<double>& arr, int low, int high)
{
    if (low >= high) return;
    const double pivot = arr[low + (high - low) / 2];
    int i = low;
    int j = high;
    while (i <= j)
    {
        while (arr[i] < pivot) ++i;
        while (arr[j] > pivot) --j;
        if (i <= j)
        {
            std::swap(arr[i], arr[j]);
            ++i;
            --j;
        }
    }
    if (low < j) quickSortImpl(arr, low, j);
    if (i < high) quickSortImpl(arr, i, high);
}

void SortableArray::mergeSortImpl(std::vector<double>& arr, int left, int right)
{
    if (left >= right) return;
    const int mid = left + (right - left) / 2;
    mergeSortImpl(arr, left, mid);
    mergeSortImpl(arr, mid + 1, right);

    std::vector<double> merged;
    merged.reserve(static_cast<std::size_t>(right - left + 1));
    int i = left, j = mid + 1;
    while (i <= mid && j <= right)
    {
        if (arr[i] <= arr[j]) merged.push_back(arr[i++]);
        else merged.push_back(arr[j++]);
    }
    while (i <= mid) merged.push_back(arr[i++]);
    while (j <= right) merged.push_back(arr[j++]);

    for (int k = 0; k < static_cast<int>(merged.size()); ++k)
    {
        arr[left + k] = merged[k];
    }
}

void SortableArray::heapSortImpl(std::vector<double>& arr, int left, int right)
{
    std::make_heap(arr.begin() + left, arr.begin() + right + 1);
    std::sort_heap(arr.begin() + left, arr.begin() + right + 1);
}

void SortableArray::insertionSortImpl(std::vector<double>& arr, int left, int right)
{
    for (int i = left + 1; i <= right; ++i)
    {
        double key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

void SortableArray::executeSort(std::vector<double>& arr, std::size_t left, std::size_t right, SortAlgorithm algorithm)
{
    validateAlgorithm(algorithm);
    switch (algorithm)
    {
    case SortAlgorithm::QuickSort:
        quickSortImpl(arr, static_cast<int>(left), static_cast<int>(right));
        break;
    case SortAlgorithm::MergeSort:
        mergeSortImpl(arr, static_cast<int>(left), static_cast<int>(right));
        break;
    case SortAlgorithm::HeapSort:
        heapSortImpl(arr, static_cast<int>(left), static_cast<int>(right));
        break;
    case SortAlgorithm::InsertionSort:
        insertionSortImpl(arr, static_cast<int>(left), static_cast<int>(right));
        break;
    }
}

// Strong guarantee: prepare-then-commit
void SortableArray::sortCopyAndReplace(std::size_t left, std::size_t right, SortAlgorithm algorithm)
{
    if (data_.empty() || left > right || right >= data_.size())
    {
        throw InvalidRangeException(left, right, data_.size());
    }
    validateAlgorithm(algorithm);

    // 1. Підготовка нового стану в тимчасовій копії
    std::vector<double> temp = data_;
    executeSort(temp, left, right, algorithm);

    // 2. Фіксація нового стану без генерації винятків (noexcept)
    data_.swap(temp);
}

// Strong guarantee
void SortableArray::sortRange(std::size_t left, std::size_t right, SortAlgorithm algorithm)
{
    sortCopyAndReplace(left, right, algorithm);
}

// Strong guarantee
void SortableArray::sort(SortAlgorithm algorithm)
{
    if (data_.empty())
    {
        throw EmptyArrayException("sort");
    }
    sortCopyAndReplace(0, data_.size() - 1, algorithm);
}

// Strong guarantee: багатокрокова заміна та сортування за принципом prepare-then-commit
void SortableArray::replaceAndSort(const std::vector<double>& newElements, SortAlgorithm algorithm)
{
    if (newElements.size() > capacity_)
    {
        throw ArrayCapacityException(capacity_, newElements.size());
    }
    validateAlgorithm(algorithm);

    std::vector<double> temp;
    temp.reserve(capacity_);
    for (double val : newElements)
    {
        validateValue(val);
        temp.push_back(val);
    }

    if (!temp.empty())
    {
        executeSort(temp, 0, temp.size() - 1, algorithm);
    }

    // Неподільна фіксація стану
    data_.swap(temp);
}

// Навмисно некоректна реалізація для демонстрації порушення сильної гарантії (Етап 8)
void SortableArray::replaceAndSortUnsafe(const std::vector<double>& newElements, SortAlgorithm algorithm)
{
    // Помилка: модифікуємо поточний стан ДО перевірки алгоритму та повної валідації
    data_.clear();
    for (double val : newElements)
    {
        validateValue(val);
        if (data_.size() >= capacity_)
        {
            throw ArrayCapacityException(capacity_, newElements.size());
        }
        data_.push_back(val);
    }

    if (!data_.empty())
    {
        // Якщо тут виникне InvalidAlgorithmException, попередні елементи вже втрачено!
        executeSort(data_, 0, data_.size() - 1, algorithm);
    }
}
