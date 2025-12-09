#pragma once
#include "bmplib.h"

struct filter {
	int shift;
	int K;
	unsigned int size;
	int** H;
};


bool readFilter(filter& F, const char* filename);
void freeMemory(filter& F);
void print(const filter& F);

rgbImg conv(const rgbImg & img, const filter & F);
rgbImg rgb2gray(const rgbImg& img);
rgbImg sepia(const rgbImg& img);

void logo(rgbImg& img);
void toASCII(rgbImg& img, BYTE level = 200);
