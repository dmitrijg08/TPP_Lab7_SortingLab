# SortingLab — Лабораторна робота № 7 (Варіант 9: Сортування масивів)

## 1. Опис проєкту та інваріанти класу `SortableArray`
Статична бібліотека `SortLib` реалізує клас `SortableArray` для зберігання, статистичного аналізу та сортування числових масивів (`QuickSort`, `MergeSort`, `HeapSort`, `InsertionSort`).

**Інваріанти класу `SortableArray`:**
1. `capacity_ > 0` — максимальна місткість масиву завжди додатна.
2. `0 <= data_.size() <= capacity_` — поточна кількість елементів не перевищує `capacity_`.
3. `std::isfinite(data_[i]) == true` для всіх `0 <= i < size()` — масив не містить `NaN` або `±Infinity`.

## 2. Ієрархія винятків
- `std::invalid_argument` — нульова місткість у конструкторі або значення `NaN`/`Inf`.
- `std::out_of_range` — вихід індексу за межі `[0, size() - 1]`.
- `SortException` (успадкований від `std::runtime_error`):
  - `EmptyArrayException` — виклик `min()`, `max()`, `mean()`, `median()`, `sort()` для порожнього масиву.
  - `InvalidRangeException` — некоректні межі `[left, right]` для сортування піддіапазону.
  - `ArrayCapacityException` — перевищення максимальної місткості `capacity_`.
  - `InvalidAlgorithmException` — невідомий ідентифікатор алгоритму сортування.

## 3. Дослідження розкручування стека (Stack Unwinding)
**Очікувана послідовність повідомлень до запуску:**
```
[ENTER] level1
[ENTER] level2
[ENTER] level3
Executing sort on capacity-overflow batch...
[EXIT ] level3
[LOG] level2 caught: Array capacity exceeded: capacity=4, requested=6
[EXIT ] level2
[EXIT ] level1
[MAIN] Caught ArrayCapacityException: capacity=4, requested=6
Program continues normally.
```
**Порівняння з фактичним результатом:** фактичний вивід `SortDemo.exe` повністю збігається з очікуваним. Локальні об'єкти `ScopeLogger` знищуються у зворотному порядку їх створення (LIFO).

## 4. Висновок щодо антипатернів (`int` та `std::string` проти `std::exception`)
Генерування примітивних типів (`int`) або `std::string` унеможливлює їх перехоплення через стандартний поліморфний обробник `catch (const std::exception& ex)` та позбавляє єдиного інтерфейсу `what()`. Використання класів, похідних від `std::exception`, забезпечує централізовану обробку, розширюваність ієрархії та передачу структурованих діагностичних даних без ризику `object slicing` при використанні `throw;`.
