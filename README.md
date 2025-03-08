# SearchServer

## Описание

**SearchServer** — выполняет поиск в текстовых документах по ключеывым словам. Программа исключает из анализа "минус-слова" (т.е. слова, которые не должны учитывать при поиске) и выводит документы в порядке релевантности, и по рейтингу. 

Этот проект был создан с целью продемонстрировать умение работы с алгоритмами поиска, использование стандартной библиотеки C++ (STL) для обработки текстовых данных и организации поисковой логики.

## Стек технологий
- **C++** (стандарт C++17 или выше)
- **STL** (Standard Template Library) — используется для хранения и манипуляции данными (например, `std::vector`, `std::map`, `std::unordered_map`, и т.д.)
- Алгоритмы сортировки и поиска из стандартной библиотеки.

## 🚀 Установка и запуск

### Системные требования
- Операционная система: Linux / macOS / Windows
- Компилятор C++ с поддержкой стандарта C++17 или выше (например, `g++` или `clang++`).
- Минимальная версия C++: **C++17**.

### 🔧 Инструкция по развёртыванию
1. Склонируйте репозиторий:
    ```
    git clone git@github.com:centayrus/search-engine.git
    ```
2. Перейдите в каталог проекта:
    ```
    cd search-engine
    ```
3. Соберите проект с помощью скрипта на платформах Windows(GitBash/WSL)/Linux/MacOS:
    ```
    build.sh или ./build.sh
    ```
4. Запустите программу:
- Linux/MacOS:
    ```
    ./build/program
    ```
- Windows
    ```
    build\program.exe
    ```
Демонстрационный запуск выведет заранее введенные данные