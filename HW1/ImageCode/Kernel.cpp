#include "Kernel.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>


/*-------------------------------------------------------
Kernel Class
-------------------------------------------------------*/
Kernel::Kernel()
{
	size = 3;
	buildArray();
}

Kernel::Kernel(int s)
{
	size = s;
	buildArray();
}

Kernel::~Kernel()
{
	destroyArray();
}

void Kernel::buildArray()
{
	valuesArray = new float*[size];
	int i;

	for (i = 0; i < size; i++)
	{
		valuesArray[i] = new float[size];
	}
}

void Kernel::destroyArray()
{
	int i;

	for (i = 0; i < size; i++)
	{
		delete[] valuesArray[i];
	}

	if (valuesArray != NULL)
	{
		delete[] valuesArray;
	}

	valuesArray = NULL;
}

void Kernel::insertValue(const int i, const int j, float v)
{
	valuesArray[i][j] = v;
}

Image* Kernel::applyKernel(Image* image)
{
	int iWidth = image->Width(), iHeight = image->Height();
	Image* newIm = new Image(iWidth, iHeight);
	int i, j;

	for (i = 0; i < iWidth; i++)
	{
		for (j = 0; j < iHeight; j++)
		{
			newIm->GetPixel(i, j) = applyToPixel(image, i, j);
		}
	}

	return newIm;
}

Pixel Kernel::applyToPixel(Image* image, const int i, const int j)
{
	int ki = 0, kj = 0; //indices in kernel (valuesArray)
	//Pixel p = Pixel(0, 0, 0); //start with all zeros
	int w = image->Width(), h = image->Height();
	int x, y, xx, yy;
	Pixel iPix;
	float r = 0.0, g = 0.0, b = 0.0, a = 0.0, vArrVal = 0.0;

	for (x = (i - (size / 2)); x <= (i + (size / 2)); x++)
	{
		for (y = (j - (size / 2)); y <= (j + (size / 2)); y++)
		{
			//check for out-of-bounds
			if (x < 0) //left side
			{
				xx = w + x; //wrap around to right side
			}
			else if (x >= w) //right side
			{
				xx = x - w; //wrap around to the left side
			}
			else
			{
				xx = x;
			}

			if (y < 0) //top
			{
				yy = h + y; //wrap around to bottom
			}
			else if (y >= h) //bottom
			{
				yy = y - h; //wrap around to top
			}
			else
			{
				yy = y;
			}

			iPix = image->GetPixel(xx, yy);
			vArrVal = valuesArray[ki][kj];

			r += vArrVal * iPix.r;
			g += vArrVal * iPix.g;
			b += vArrVal * iPix.b;
			a += vArrVal * iPix.a;

			//fprintf(stderr, "\nKernel value (%i , %i) : %f", ki, kj, vArrVal);
			//fprintf(stderr, "\n(x , y) = (%i , %i) : ", x, y);
			//fprintf(stderr, "\n\tr = %f \n\tg = %f \n\tb = %f \n\ta = %f", r, g, b, a);

			kj++;
		}
		ki++;
		kj = 0;
	}

	Component newR = ComponentClamp((int)r);
	Component newG = ComponentClamp((int)g);
	Component newB = ComponentClamp((int)b);
	Component newA = ComponentClamp((int)a);

	Pixel p = Pixel(newR, newG, newB, newA);

	
	//p.Print();
	return p;
}

int Kernel::getSize()
{
	return size;
}

void Kernel::print()
{
	for (int i = 0; i < size; i++)
	{
		fprintf(stderr, "\n");
		for (int j = 0; j < size; j++)
		{
			fprintf(stderr, "\t%f |", valuesArray[i][j]);
		}
	}
}