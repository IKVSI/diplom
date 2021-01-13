
#ifndef JPEGAPP_JPEG_H
#define JPEGAPP_JPEG_H

#include "ReadFile.h"
#include "Huffman.h"
#include <algorithm>
#include <csignal>
#include <vector>
using namespace std;


class JPEG
{
    // шаблон функций маркеров
    typedef void (JPEG::*jpegfunc)();
    // массив функций обработчиков маркеров по их адресам
    jpegfunc MARKERS[256];
private:
    // массивы для типов/адресов маркеров встречающихся в файле
    vector<unsigned char> markerstype;
    vector<unsigned long long int> markersaddr;
    // проверка крайних маркеров SOI/EOI
    bool startimage = false;
    bool endimage = false;
    // адрес и длина чтения из файла
    unsigned long long int cursor = 0;
    unsigned long long int readlength = 1;
    // коды Хаффмана
    map<unsigned short, Huffman *> DC;
    map<unsigned short, Huffman *> AC;
    unsigned short hufsize = 16;
    // обработчики маркеров
    void defaultMarker();
    void ignoreMarker();
    void SOIMarker();
    void EOIMarker();
    void DHTMarker();
    // проверка на 0xFF есть ли маркер
    bool isMarker();
    // запуск обработчика маркера
    void runMarker();
    // чтение длины маркера
    unsigned long long int markerLength();
    // запись маркера в массив
    void pushMarker(unsigned char marker, unsigned long long int addr);
    // файл
    ReadFile * fin;
public:
    JPEG(string filename);
    ~JPEG();
    // поиск маркеров в файле
    void findMarkers();
    // обрезка информационных и прочих маркеров не влияющих на изображение
    // и сохранение в новый файл name.jpg -> name.clear.jpg
    void saveClearJpeg();
};


#endif //JPEGAPP_JPEG_H
