#include <iostream>
#include <fstream>
#include <cstring>

#include "bmplib.h"
#include "imgtools.h"

int main(int argc, char * argv[])
{
	if (argc < 4) {
		std::cout << "Use:\n\tpaint25 key inputfilename outputfilename\n";
		std::cout << "\t key = -logo \t add logo\n";
		std::cout << "\t key = -f filter \t apply filter\n";
		std::cout << "\t key = -ascii \t convert to ascii art\n"; 
		std::cout << "\t key = -gray \t convert to grayscale\n";
		return 0;
	}
	char* inputfilename = argv[argc - 2];
	char* outfilename = argv[argc - 1];

	rgbImg img = {0, 0, nullptr};
	if (!readBMP(inputfilename, img)) {
		std::cout << "Can not read BMP file '" << inputfilename << "'\n";
		return 1;
	}
	rgbImg newimg = { 0 ,0, nullptr };
	// Обработка файла
	if (strcmp(argv[1], "-logo") == 0) {
		std::swap(img, newimg);
		logo(newimg);
	}
	else if (strcmp(argv[1], "-f") == 0) {
		filter F = { 0, 0 ,0, nullptr };
		if (!readFilter(F, argv[2])) {
			std::cout << "Can not read filter\n";
			freeMemory(img);
			return 1;
		}
		newimg = conv(img, F);
	}
	else if (strcmp(argv[1], "-ascii") == 0) {
	}
	else if (strcmp(argv[1], "-gray") == 0) {
		newimg = rgb2gray(img);
	}
	else if (strcmp(argv[1], "-sepia") == 0) {
		newimg = sepia(img);
	}

	// Сохранение файла
	if (newimg.pixels != nullptr) {
		if (!saveBMP(newimg, outfilename)) {
			std::cout << "Can not save to file '" << outfilename << "'\n";
		}
		freeMemory(newimg);
	}

	freeMemory(img);
	return 0;
}
