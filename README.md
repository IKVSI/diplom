# Диплом
## Тема: Модуль архивации JPEG файлов

### Проект программы находится в папке "JPEGapp"

* в папке "JPEGapp/jpegapp" находятся исходники малых программ написанных на C++ и соответствующие Cmake файлы для правильной компиляции.

* в папке "JPEGapp/Scripts" находятся скомпилированные програмы и

## Программа jpegtable.exe [file].jpg

Принимает на вход jpeg файл и генерирует файлы квантованных матриц на каждую компоненту цветового пространства

## Программа jpegclear.exe [file].jpg

Принимает на вход файл jpeg и удаляет из него лишние маркеры, для того чтобы оставить только минимальный набор нужный для декодирования файла

## Программа jpegstats.exe [file].jpg

принимает на вход файл и сохраняет в файл [file].jpg.stats статистику использования категорий в квантованных матрицах для последующей генерации кодов Хаффмана

## Программа jpegcompress.exe [coding].table [file].jpg

принимает на вход 2 файла, один с кодировкой Хаффмана, которой должен быть перекодирован второй файл [file].jpg с удалением его собственной кодировки

## Программа jpegdecompress.exe [coding].table [file].jpg

Также принимает на вход 2 файла, один с кодировкой, которой был закодирован [file].jpg и сам файл, для его восстановления

## Скрипт compress.py [folder]

1. Принимает на вход папку в которой размещены jpeg файлы
2. Создаёт в ней папку JPEG
3. Отбирает файлы JPEG нужной структуры и копирует их в JPEG
4. Собирает статистику по файлам скопированным в JPEG используя jpegstats.exe
5. Создаёт папку COMPRESS в folder
6. Туда сохраняет основанный на собранной статистике код Хаффмана в COMPRESS/coding.file
7. Применяет программу jpegcompress.exe с файлом coding.table для перекодирования каждого файла из папки JPEG
8. Сохраняет эти файлы в COMPRESS

## Скрипт decompress.py [folder]

1. Принимает на вход папку в которой размещены сжатые предыдущим алгоритмом файлы
2. Создаёт в ней папку DECOMPRESS
3. Используя находящийся в папке coding.table при помощи decompress.exe восстанавливает закодированные фалйы
4. Сохраняет эти файлы в DECOMPRESS

### Скрипты fileload.py, gentables.py, gettables.py использовались для взаимодействия с базой данных

### В папке "Диплом" находятся текст диплома и связанные с ним документы

### В папке "Преддипломная практика" текст отчёта и документы по практике

### В папке "Doc" некоторая полезные ресурсы