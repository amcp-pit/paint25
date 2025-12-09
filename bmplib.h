#pragma once
#include <cstdint>

typedef unsigned char BYTE;
typedef std::uint16_t WORD;
typedef std::uint32_t DWORD;
typedef std::int32_t LONG;

#pragma pack(push,1)
struct BITMAPFILEHEADER {
	WORD  bfType; // Тип файла
	DWORD bfSize; // Размер файла
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits; // Смещение до поля данных
};

struct BITMAPINFOHEADER {
	DWORD biSize;     // размер структуры в байтах
	LONG  biWidth;    // ширина в пикселях
	LONG  biHeight;   // высота в пикселях
	WORD  biPlanes;   // всегда должно быть 1
	WORD  biBitCount; // Кол-во бит на цвет 0 | 1 | 2 | 4 | 8 | 16 | 24 | 32 | 48 | 64
	DWORD biCompression;   // есть или нет сжатие
	DWORD biSizeImage;     // размер изображения
	LONG  biXPelsPerMeter; // горизонтальное разрешение
	LONG  biYPelsPerMeter; // вертикальное разрешение
	DWORD biClrUsed;       // кол-во цветов (в том случае, если используется таблица цветов)
	DWORD biClrImportant;  // кол-во существенных цветов
};
#pragma pack(pop)

struct RGB {
	BYTE red;
	BYTE green;
	BYTE blue;
};

struct rgbImg {
	unsigned int width;
	unsigned int height;
	RGB** pixels;
};

void print(const BITMAPFILEHEADER& bmfh);
void print(const BITMAPINFOHEADER& x);
rgbImg createImg(unsigned int width, unsigned int height);
void freeMemory(rgbImg& img);
bool readBMP(const char* fname, rgbImg& img);
bool saveBMP(const rgbImg& img, const char* filename);
