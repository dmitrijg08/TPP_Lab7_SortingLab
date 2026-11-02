#pragma once
#include <vector>
#include <cstddef>

enum class SortAlgorithm : int
{
    QuickSort = 1,
    MergeSort = 2,
    HeapSort = 3,
    InsertionSort = 4
};

class SortableArray
{
private:
    std::vector<double> data_;
    std::size_t capacity_;

    static void validateValue(double value);
    static void validateAlgorithm(SortAlgorithm algorithm);

    static void quickSortImpl(std::vector<double>& arr, int low, int high);
    static void mergeSortImpl(std::vector<double>& arr, int left, int right);
    static void heapSortImpl(std::vector<double>& arr, int left, int right);
    static void insertionSortImpl(std::vector<double>& arr, int left, int right);
    static void executeSort(std::vector<double>& arr, std::size_t left, std::size_t right, SortAlgorithm algorithm);

public:
    // Конструктор із перевіркою інваріанта capacity > 0
    explicit SortableArray(std::size_t capacity);

    // No-throw guarantee
    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;
    bool isSorted() const noexcept;
    void clear() noexcept;

    // Strong guarantee (генерує std::out_of_range при виході за межі)
    double at(std::size_t index) const;
    void setAt(std::size_t index, double value);

    // Strong guarantee (генерує ArrayCapacityException або std::invalid_argument)
    void add(double value);

    // Пошук екстремумів та статистик (генерує EmptyArrayException для порожнього масиву)
    double min() const;
    double max() const;
    double mean() const;
    double median() const;

    // Strong guarantee: сортування всього масиву або діапазону [left, right] через копію та swap
    void sort(SortAlgorithm algorithm = SortAlgorithm::QuickSort);
    void sortRange(std::size_t left, std::size_t right, SortAlgorithm algorithm = SortAlgorithm::QuickSort);
    void sortCopyAndReplace(std::size_t left, std::size_t right, SortAlgorithm algorithm);

    // Strong guarantee: багатокрокова заміна набору елементів із миттєвим сортуванням
    void replaceAndSort(const std::vector<double>& newElements, SortAlgorithm algorithm);

    // Basic guarantee (навмисно некоректна реалізація для етапу 8 — порушує сильну гарантію)
    void replaceAndSortUnsafe(const std::vector<double>& newElements, SortAlgorithm algorithm);
};
