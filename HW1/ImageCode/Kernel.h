/*********
* Kernel
*********/

#ifndef KERNEL_INCLUDED
#define KERNEL_INCLUDED

#include <assert.h>
#include <stdio.h>
#include "image.h"
#include "pixel.h"
#include <stdlib.h>
#include <vector>

class Kernel
{
public:
	Kernel();
	Kernel(int s);
	virtual ~Kernel();
	Image* applyKernel(Image* image);
	void insertValue(const int i, const int j, float val);
	int getSize();
	void print();

private:
	int size;
	float** valuesArray;

	void buildArray();
	void destroyArray();
	Pixel applyToPixel(Image* image, const int i, const int j);
};

#endif