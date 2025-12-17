# Отчёт по лабораторной работе №4

## 1. Цель работы
Реализовать динамический менеджер памяти с выбором свободного блока по стратегии **Next Fit** и проверить его работу на наборе операций выделения/освобождения.

## 2. Что такое Next Fit 
Стратегия **Next Fit** — это вариация “первого подходящего” выделения, но старт поиска выполняется **с позиции последнего удачного выделения**.

За счёт этого:
- при большом списке блоков уменьшается число просмотренных элементов в среднем;
- поведение становится более “круговым” (поиск обходит список и возвращается к началу только при необходимости).

## 3. Логика работы выделения/освобождения

### 3.1. Псевдокод поиска (Next Fit)
Ниже — упрощённая схема поиска блока подходящего размера `size`:

```text
start = lastAlloc
curr  = start

do
  if curr.free && curr.span >= size:
     return curr
  curr = curr.next
  if curr == heapEnd:
     curr = heapStart
while curr != start

return NULL
```

### 3.2. Что происходит при Alloc
- выбирается кандидат через Next Fit;
- если блок “слишком большой”, он **разрезается** на занятый и остаток (новый свободный блок);
- точка `lastAlloc` обновляется, чтобы следующий поиск начинался ближе к “месту работы”.

### 3.3. Что происходит при Free
- блок помечается как свободный;
- выполняется **слияние соседних** свободных блоков (уменьшение фрагментации);
- при необходимости корректируется `lastAlloc`, если он указывал на блок, который был поглощён при merge.

## 4. Внесённые изменения в проект
Изменения удобно воспринимать как набор правок по файлам.

| Файл | Суть изменения | Зачем это нужно |
| --- | --- | --- |
| `Eco.MemoryManager1Lab/HeaderFiles/CEcoMemoryManager1Lab.h` | добавлено поле `m_pLastAlloc` | хранит позицию последнего выделения для Next Fit |
| `Eco.MemoryManager1Lab/SharedFiles/IEcoMemoryManager1.h` | добавлено описание структуры блока `MemoryBlock` | единый формат хранения метаданных блока и связности списка |
| `Eco.MemoryManager1Lab/SourceFiles/CEcoMemoryManager1Lab.c` | инициализация кучи и “первого блока”, установка `m_pLastAlloc` | подготовка менеджера к обработке запросов |
| `Eco.MemoryManager1Lab/SourceFiles/CEcoMemoryAllocator1Lab.c` | реализованы `Alloc`/`Free` с поддержкой Next Fit, split/merge | непосредственно стратегия выделения и борьба с фрагментацией |
| `MySimpleEcoOS/SourceFiles/MySimpleEcoOS.c` | тестовый сценарий (серия Alloc/Free + вывод статуса) | демонстрация корректной работы |

## 5. Ключевые фрагменты (референс)
Ниже приведены характерные части, показывающие идею реализации. Фрагменты можно рассматривать как “опорные”, а не как полный листинг.

### 5.1. Метаданные блока

```c
typedef struct MemoryBlock {
    bool_t free;
    struct MemoryBlock* next;
    struct MemoryBlock* prev;
} MemoryBlock;
```

### 5.2. Поиск блока (Next Fit)

```c
static MemoryBlock* findBlock(CEcoMemoryManager1Lab_623E1838* pCMe, uint32_t size) {
    MemoryBlock* pBlock = (MemoryBlock*)pCMe->m_pLastAlloc;
    MemoryBlock* pStart = pBlock;

    for (;;) {
        if (pBlock->free == 1 && (uint32_t)((char_t*)pBlock->next - (char_t*)pBlock) >= size) {
            return pBlock;
        }

        pBlock = pBlock->next;
        if ((char_t*)pBlock == pCMe->m_pHeapEnd) {
            pBlock = (MemoryBlock*)pCMe->m_pHeapStart;
        }
        if (pBlock == pStart) {
            break;
        }
    }

    return 0;
}
```

### 5.3. Деление и слияние

```c
static void splitBlock(CEcoMemoryManager1Lab_623E1838* pCMe, MemoryBlock* pBlock, uint32_t size) {
    MemoryBlock* pNewBlock = (MemoryBlock*)((char_t*)pBlock + size);
    pNewBlock->free = 1;
    pNewBlock->next = pBlock->next;
    pNewBlock->prev = pBlock;

    if ((char_t*)pBlock->next != pCMe->m_pHeapEnd) {
        pBlock->next->prev = pNewBlock;
    }
    pBlock->next = pNewBlock;
}

static void mergeBlocks(CEcoMemoryManager1Lab_623E1838* pCMe, MemoryBlock* pBlock) {
    MemoryBlock* pBlockNext = pBlock->next;
    pBlock->next = pBlockNext->next;
    if ((char_t*)pBlock->next != pCMe->m_pHeapEnd) {
        pBlock->next->prev = pBlock;
    }
    if (pCMe->m_pLastAlloc == (char_t*)pBlockNext) {
        pCMe->m_pLastAlloc = (char_t*)pBlock;
    }
}
```

## 6. Проверка работоспособности
Тестовый сценарий включает:
- последовательные `Alloc` разных размеров;
- освобождение не в порядке выделения;
- повторный `Alloc` крупного блока после освобождений.

Критерии корректности:
- `Alloc` возвращает валидный указатель при наличии ресурса;
- после `Free` объём свободной памяти увеличивается;
- при освобождении соседних блоков выполняется консолидация;
- точка старта поиска меняется и соответствует концепции Next Fit.

## 7. Сборка 

### <img width="612" height="445" alt="image" src="https://github.com/user-attachments/assets/4902d197-865d-4acb-ac9c-cab4dc6000e8" />

