#!/bin/bash

# Определяем, какая ОС
case "$(uname -s)" in
    Linux*)   os="linux" ;;
    Darwin*)  os="macos" ;;
    MINGW*|CYGWIN*|MSYS*) os="windows" ;;
    *)        os="unknown" ;;
esac

# Устанавливаем расширение исполняемого файла
exe_ext=""
if [[ "$os" == "windows" ]]; then
    exe_ext=".exe"
fi

# Находим все .cpp файлы
compile_files=$(find . -name "*.cpp" -exec printf '%s ' {} +)

# Путь для сборки
build_path="./build"
mkdir -p "$build_path"

# Вывод информации
echo "Operating system detected: $os"
echo "Compile start..."

# Компиляция
g++ -Wall -Werror -Wextra -Wpedantic -Wformat=2 -Wcast-align -Wnull-dereference \
    -fdiagnostics-all-candidates -Werror --std=c++20 -g3 -O0 \
    $compile_files -o "$build_path/program$exe_ext"

# Проверяем, успешна ли компиляция
if [[ $? -eq 0 ]]; then
    echo "Compile completed successfully!"
    echo "Launch the program: $build_path/program$exe_ext"
else
    echo "Compilation failed!"
    exit 1
fi

