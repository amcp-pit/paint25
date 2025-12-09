#include <iostream>
#include <fstream>

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

void print(const BITMAPFILEHEADER& bmfh) {
	std::cout << bmfh.bfType << "\n";
	std::cout << bmfh.bfSize << "\n";
	std::cout << bmfh.bfOffBits << "\n";
}

void print(const BITMAPINFOHEADER& x) {
	std::cout << "biSize = " << x.biSize << "\n";
	std::cout << "biWidth = " << x.biWidth << "\n";
	std::cout << "biHeight = " << x.biHeight << "\n";
	std::cout << "biPlanes = " << x.biPlanes << "\n";
	std::cout << "biBitCount = " << x.biBitCount << "\n";
	std::cout << "biCompression = " << x.biCompression << "\n";
	std::cout << "biSizeImage = " << x.biSizeImage << "\n";
	std::cout << "biXPelsPerMeter = " << x.biXPelsPerMeter << "\n";
	std::cout << "biYPelsPerMeter = " << x.biYPelsPerMeter << "\n";
	std::cout << "biClrUsed = " << x.biClrUsed << "\n";
	std::cout << "biClrImportant = " << x.biClrImportant << "\n";
}

rgbImg createImg(unsigned int width, unsigned int height) {
	rgbImg img;
	img.width = width;
	img.height = height;
	img.pixels = new RGB* [height];
	for (unsigned int row = 0; row < height; ++row) {
		img.pixels[row] = new RGB[width];
	}
	return img;
}

void freeMemory(rgbImg& img) {
	for (unsigned int row = 0; row < img.height; ++row) {
		delete[] img.pixels[row];
	}
	delete[] img.pixels;
	img.pixels = nullptr;
	img.width = 0;
	img.height = 0;
}

bool readBMP(const char* fname, rgbImg& img) {
	std::ifstream inFile(fname, std::ios_base::binary);
	if (!inFile)
		return false;
	BITMAPFILEHEADER bmfh;
	inFile.read( (char*) & bmfh, sizeof(bmfh));
	print(bmfh);
	if (bmfh.bfType != 256 * int('M') + int('B')) { // 19778
		inFile.close();
		return false;
	}
	
	BITMAPINFOHEADER bmih;
	inFile.read((char*)&bmih, sizeof(bmih));
	print(bmih);

	if (bmih.biCompression != 0) {
		inFile.close();
		return false;
	}
	
	freeMemory(img);
	img = createImg(bmih.biWidth, bmih.biHeight);

	if (bmih.biBitCount == 24) {
		// TODO чтение содержимого изображения из файла
		RGB tmp;
		BYTE offset_array[16] = { 0 };
		const int offset = (img.width % 4) ? (4 - (3 * img.width)%4) : 0;
		for (unsigned int row = img.height; row > 0; --row) {
			for (unsigned int col = 0; col < img.width; ++col) {
				inFile.read((char*)&tmp, sizeof(tmp));
				std::swap(tmp.blue, tmp.red);
				img.pixels[row - 1][col] = tmp;
			}
			inFile.read((char*)offset_array, offset);
		}
	}
	else {
		inFile.close();
		return false;
	}

	inFile.close();
	return true;
}

bool saveBMP(const rgbImg& img, const char* filename) {
	std::ofstream outBMP(filename, std::ios_base::binary);
	if (!outBMP) return false;

	BITMAPFILEHEADER bmfh;
	char bfType[] = { 'B', 'M' };
	bmfh.bfType = *((WORD*)bfType);
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = bmfh.bfOffBits + img.height * img.width * 3;
	outBMP.write((char*) & bmfh, sizeof(BITMAPFILEHEADER));

	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = img.width;
	bmih.biHeight = img.height;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = 0;
	bmih.biSizeImage = bmfh.bfSize - bmfh.bfOffBits;
	bmih.biXPelsPerMeter = 1;
	bmih.biYPelsPerMeter = 1;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;
	outBMP.write((char*)&bmih, sizeof(BITMAPINFOHEADER));

	const int offset = (img.width % 4) ? (4 - (3 * img.width) % 4) : 0;
	BYTE offset_array[4] = { 0 };

	for (unsigned int row = img.height; row > 0; --row) {
		for(unsigned int col = 0; col < img.width; ++col){
			RGB tmp = img.pixels[row-1][col];
			std::swap(tmp.blue, tmp.red);
			outBMP.write((char*)&tmp, sizeof(tmp));
		}
		outBMP.write((char*)offset_array, offset);
	}
	outBMP.close();
	return true;
}

int main(int argc, char * argv[])
{
	if (argc != 2) {
		std::cout << "Use:\n\tpaint25 filename\n";
		return 0;
	}
	rgbImg img = {0, 0, nullptr};
	if (!readBMP(argv[1], img)) {
		std::cout << "Can not read BMP file '" << argv[1] << "'\n";
		return 1;
	}
	// Обработка файла
	/*
	BYTE level = 200;
	for (unsigned int row = 0; row < img.height; ++row) {
		for (unsigned int col = 0; col < img.width; ++col) {
			//std::cout << (int)(img.pixels[row][col].blue) << " ";
			RGB tmp = img.pixels[row][col];
			if ( tmp.red < level) {
				std::cout << " ";
			}
			else {
				std::cout << (char)0x95;
			}
		}
		std::cout << std::endl;
	}
	*/

	if (img.width > 25 && img.height > 25) {
		// add logo
		RGB brush = { 255, 255, 255 };
		for (unsigned int row = 0; row < 20; ++row) {
			img.pixels[row][0] = brush;
			img.pixels[row][19] = brush;
		}
		for (unsigned int col = 0; col < 20; ++col) {
			img.pixels[0][col] = brush;
			img.pixels[19][col] = brush;
		}
		for (unsigned int i = 0; i < 20; ++i){
			img.pixels[i][i] = brush;
			img.pixels[i][19 - i] = brush;
		}
	}

	// Сохранение файла
	if (!saveBMP(img, "output.bmp")) {
		std::cout << "Can not save to file 'output.bmp'\n";
	}
	freeMemory(img);
	return 0;
}
