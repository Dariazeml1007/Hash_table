# Оптимизация хэш-таблицы
## Гарри Поттер и философский камень
Частота появления "Harry" - 1327, "Ron" - 429, "Hermione" - 270. Откуда инфоррмация ? - Хэш - таблица нашла:)
## Начнем искать горячие точки

Для начала уберем пунктуацию и переведем все буквы в строчные отдельной программой (папка tools prepare_words.cpp), чтобы замерять именно поиск. Воспользуемся valgrind для анализа :

![Первый замер](callgrind/time_1.png)

Первая функция hash 

```c
unsigned long hash(const char *key)
{
    assert(key);

    unsigned long hash = 5381;

    for (size_t i = 0; key[i] != '\0'; i++)

         hash = ((hash << 5) + hash) + (size_t) (key[i]); // hash * 33 + c


    return hash;
}
```

Для ускорения хэш-функции будем использовать  _mm_crc32_u64

```c
uint32_t hash_intrinsic(const char* word)
{
    uint64_t hash = 0;
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word)));     //process only 4 byte
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 8)));
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 16)));
    hash = _mm_crc32_u64(hash, *((const uint64_t*)(word + 24)));
    return (uint32_t)hash;
}
```

![Второй замер](callgrind/time_2.png)

Сразу виден результат 


| Функция                | До оптимизации | После оптимизации | Улучшение |
|------------------------|----------------|-------------------|-----------|
| `hash()`              | 34.23%         | 19.92%            | ↓ 42%     |
