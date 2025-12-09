#include "imgtools.h"
#include <iostream>
#include <fstream>

void freeMemory(filter& F) {
	if (F.H != nullptr) {
		for (unsigned int i = 0; i < F.size; ++i)
			delete[] F.H[i];
		delete[] F.H;
		F.H = nullptr;
		F.size = 0;
	}
}

bool readFilter(filter& F, const char* filename) {
	std::ifstream inFile(filename);
	if (!inFile) return false;
	freeMemory(F);
	inFile >> F.shift >> F.K >> F.size;
	if ((F.size == 0) || (F.size % 2 == 0)) {
		inFile.close();
		return false;
	}
	// Выделить память под ядро свертки
	// Прочитать ядро из файла
	F.H = new int* [F.size];
	for (unsigned int i = 0; i < F.size; ++i) {
		F.H[i] = new int[F.size];
		for (unsigned int col = 0; col < F.size; ++col)
			inFile >> F.H[i][col];
	}
	inFile.close();
	return true;
}

void print(const filter& F) {
	std::cout << F.shift << " + 1 / " << F.K << " * [\n";
	for (unsigned int i = 0; i < F.size; ++i) {
		for (unsigned int j = 0; j < F.size; ++j) {
			std::cout << F.H[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << "]\n";
}

BYTE treshhold(int x) {
	x = (x < 0) ? 0 : (x > 255 ? 255 : x);
	return static_cast<BYTE>(x);
}

rgbImg conv(const rgbImg& img, const filter& F) {
	rgbImg newimg = createImg(img.width, img.height);
	// Применим фильтр
	unsigned int r = (F.size - 1) / 2;
	for (unsigned int row = r; row < img.height - r; ++row) {
		for (unsigned int col = r; col < img.width - r; ++col) {
			int tmpR = 0;
			int tmpG = 0;
			int tmpB = 0;

			for (unsigned int i = 0; i < F.size; ++i) {
				for (unsigned int j = 0; j < F.size; ++j) {
					tmpR += F.H[i][j] * img.pixels[row + i - r][col + j - r].red;
					tmpG += F.H[i][j] * img.pixels[row + i - r][col + j - r].green;
					tmpB += F.H[i][j] * img.pixels[row + i - r][col + j - r].blue;
				}
			}

			tmpR /= F.K;
			tmpG /= F.K;
			tmpB /= F.K;

			tmpR += F.shift;
			tmpG += F.shift;
			tmpB += F.shift;

			tmpR = treshhold(tmpR);
			tmpG = treshhold(tmpG);
			tmpB = treshhold(tmpB);

			RGB pixel = { tmpR, tmpG, tmpB };
			newimg.pixels[row][col] = pixel;
		}
	}

	return newimg;
}

rgbImg rgb2gray(const rgbImg& img) {
	rgbImg newimg = createImg(img.width, img.height);
	for (unsigned int row = 0; row < img.height; ++row) {
		for (unsigned int col = 0; col < img.width; ++col) {
			RGB x = img.pixels[row][col];
			int pixel = 0.299 * x.red + 0.587 * x.green + 0.114 * x.blue;
			x.blue = x.red = x.green = treshhold(pixel);
			newimg.pixels[row][col] = x;
		}
	}
	return newimg;
}

rgbImg sepia(const rgbImg& img) {
	rgbImg newimg = createImg(img.width, img.height);
	for (unsigned int row = 0; row < img.height; ++row) {
		for (unsigned int col = 0; col < img.width; ++col) {
			RGB x = img.pixels[row][col];
			RGB y;
			y.red = treshhold(0.393 * x.red + 0.769 * x.green + 0.189 * x.blue);
			y.green = treshhold(0.349 * x.red + 0.686 * x.green + 0.168 * x.blue);
			y.blue = treshhold(0.272 * x.red + 0.534 * x.green + 0.131 * x.blue);
			newimg.pixels[row][col] = y;
		}
	}
	return newimg;
}


void logo(rgbImg& img) {
	if (img.width > 25 && img.height > 25) {
		// add logo
		RGB brush = { 255, 0, 0 };
		for (unsigned int row = 0; row < 20; ++row) {
			img.pixels[row][0] = brush;
			img.pixels[row][19] = brush;
		}
		for (unsigned int col = 0; col < 20; ++col) {
			img.pixels[0][col] = brush;
			img.pixels[19][col] = brush;
		}
		for (unsigned int i = 0; i < 20; ++i) {
			img.pixels[i][i] = brush;
			img.pixels[i][19 - i] = brush;
		}
	}
}

void toASCII(rgbImg& img, BYTE level) {
	for (unsigned int row = 0; row < img.height; ++row) {
		for (unsigned int col = 0; col < img.width; ++col) {
			//std::cout << (int)(img.pixels[row][col].blue) << " ";
			RGB tmp = img.pixels[row][col];
			if (tmp.red < level) {
				std::cout << " ";
			}
			else {
				std::cout << (char)0x95;
			}
		}
		std::cout << std::endl;
	}
}
