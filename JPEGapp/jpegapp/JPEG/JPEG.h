
#ifndef JPEGAPP_JPEG_H
#define JPEGAPP_JPEG_H

#include "ReadFile.h"
#include "Huffman.h"
#include <algorithm>
#include <vector>
#include <cmath>
using namespace std;

struct Component
{
    unsigned char id;
    unsigned char sampheigth;
    unsigned char sampwidth;
    unsigned char qtnum;
    unsigned char ACnum;
    unsigned char DCnum;
    signed long DC = 0;
    signed long outDC = 0;
};

class JPEG
{
    const string compnames[6] = {"", "Y", "Cb", "Cr", "I", "Q"};
    const unsigned char neMARKER[10] = {0x0, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7};
    const unsigned char ZIGZAG[8][8] = {
        0,  1,  5,  6, 14, 15, 27, 28,
        2,  4,  7, 13, 16, 26, 29, 42,
        3,  8, 12, 17, 25, 30, 41, 43,
        9, 11, 18, 24, 31, 40, 44, 53,
        10, 19, 23, 32, 39, 45, 52, 54,
        20, 22, 33, 38, 46, 51, 55, 60,
        21, 34, 37, 47, 50, 56, 59, 61,
        35, 36, 48, 49, 57, 58, 62, 63
    };
    // шаблон функций маркеров
    typedef void (JPEG::*jpegfunc)();
    // массив функций обработчиков маркеров по их адресам
    jpegfunc MARKERS[256];
private:
    // файл
    ReadFile * fin;
    // Чтение байтов из файла
    unsigned long long readNumFromFile(unsigned char length);
    unsigned long long cursor;
    // Точность сэмпла
    unsigned char sampleprecision;
    // Высота x Ширина изображения
    unsigned short height;
    unsigned short width;
    // Компоненты Изображения
    vector<Component *> components;
    // массивы для типов/адресов маркеров встречающихся в файле
    vector<unsigned char> markerstype;
    vector<unsigned long long int> markersaddr;
    // Интервал обнуления
    unsigned long long restartinterval = 0xFFFFFFFFFFFFFFFF;
    // Коды Хаффмана
    map <unsigned char, Huffman *> AC;
    map <unsigned char, Huffman *> DC;
    // Таблицы Квантизации
    map <unsigned char, unsigned char **> QT;
    map <unsigned char, unsigned char> qtprec;
    // Поиск маркеров
    void findMarkers();
    // Проверка длины маркера
    unsigned short findLength();
    // Обработчики маркеров
    void defaultMarker();
    void markerSOI();
    void markerEOI();
    void markerDRI();
    void markerSOF();
    void markerDHT();
    void markerSOS();
    void markerDQT();
    // Курсок на начало данных
    unsigned long long datacursor = 0;
    // Достаёт следующую матрицу
    signed long ** getNextTable();
    // Количество прочтённых MCU
    unsigned long long mcunum;
    // ID кодируемого компонента
    unsigned char ccid;
    // Переменные для кодирования
    unsigned char hs;
    unsigned char ws;
    unsigned long long buffer;
    unsigned short bitlength;
    unsigned long long outbuffer;
    unsigned short outbitlength;
    unsigned long long numberofmcu;
    unsigned long long mcuw;
    unsigned long long mcuh;
    unsigned char hsample;
    unsigned char wsample;
    // Функции для кодирования
    void extendBuffer();
    unsigned char byteFromOutBuffer();
    void genMCUNumber();
    // Стартовая установка декодирования
    void decodeStart();
    // Шаг декодирования
    void step();
    // Собрать статистику по файлу
    map<unsigned char, map <bool, map<unsigned char, unsigned long long>>> stats;
    void genStats();
    // Получить байты маркера после аддреса
    vector <unsigned char> getMarker(unsigned char marker, unsigned long long addr);
    // Сгенерировать информацию в формате JPEG
    vector <unsigned char> genJPEGData(map<unsigned char, Huffman *> &DC, map<unsigned char, Huffman *> &AC, map<unsigned char, Component> &comp, bool compressed = false);
    // Считывание и генерация кода Хаффмана для перекодировки
    void genCoding(string &codingfile, map<unsigned char, Huffman *> &DC, map<unsigned char, Huffman *> &AC, map<unsigned char, Component> &comp);
    bool compressed = false;
public:
    JPEG(string filename, bool compressed = false);
    ~JPEG();
    void saveClearJpeg();
    void compressJPEG(string codingfile);
    void decompressJPEG(string codingfile);
    void decodeTables();
    void getStats();
    static unsigned char getCategory(signed long a);
};


#endif //JPEGAPP_JPEG_H
