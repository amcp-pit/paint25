#include "bmplib.h"
#include <iostream>
#include <fstream>

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
	img.pixels = new RGB * [height];
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
	inFile.read((char*)&bmfh, sizeof(bmfh));
	//print(bmfh);
	if (bmfh.bfType != 256 * int('M') + int('B')) { // 19778
		inFile.close();
		return false;
	}

	BITMAPINFOHEADER bmih;
	inFile.read((char*)&bmih, sizeof(bmih));
	//print(bmih);

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
		const int offset = (img.width % 4) ? (4 - (3 * img.width) % 4) : 0;
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
	outBMP.write((char*)&bmfh, sizeof(BITMAPFILEHEADER));

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
		for (unsigned int col = 0; col < img.width; ++col) {
			RGB tmp = img.pixels[row - 1][col];
			std::swap(tmp.blue, tmp.red);
			outBMP.write((char*)&tmp, sizeof(tmp));
		}
		outBMP.write((char*)offset_array, offset);
	}
	outBMP.close();
	return true;
}
