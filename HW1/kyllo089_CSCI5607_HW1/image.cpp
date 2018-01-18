#include "image.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <vector>
#include "Kernel.h"

#define _USE_MATH_DEFINES

#include <math.h>

/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
	int b = 0; //which byte to write to
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
		}
	}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	
	width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
    
    //memcpy(data.raw, src.data.raw, num_pixels);
    *data.raw = *src.data.raw;
}

Image::Image (char* fname){

	int numComponents; //(e.g., Y, YA, RGB, or RGBA)
	data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	

	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}

void Image::AddNoise (double factor)
{
	int x, y;
	Pixel randPix;
	Component r, g, b, a;

	if (factor >= 0.0 && factor <= 1.0)
	{
		for (x = 0; x < Width(); x++)
		{
			for (y = 0; y < Height(); y++)
			{
				Pixel p = GetPixel(x, y);
				randPix = PixelRandom();

				//manually lerp each component
				r = ComponentClamp((1.0 - factor) * p.r + factor * randPix.r);
				g = ComponentClamp((1.0 - factor) * p.g + factor * randPix.g);
				b = ComponentClamp((1.0 - factor) * p.b + factor * randPix.b);
				a = ComponentClamp((1.0 - factor) * p.a + factor * randPix.a);

				GetPixel(x, y) = Pixel(r, g, b, a);
			}
		}
	}
}

void Image::Brighten (double factor)
{
	int x,y;
	if (factor >= 0.0 && factor <= 1.0)
	{
		for (x = 0; x < Width(); x++)
		{
			for (y = 0; y < Height(); y++)
			{
				Pixel p = GetPixel(x, y);
				Pixel scaled_p = p*factor;
				GetPixel(x, y) = scaled_p;
			}
		}
	}
}


void Image::ChangeContrast (double factor)
{
	int x, y;
	double aveL = 0.0;
	Pixel p, newPix;
	Component r, g, b;

	//1. calc average Luminance
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x, y);

			aveL += p.Luminance();
		}
	}
	aveL /= (Width()*Height());

	//2. compare each pixel's Luminance against the average L
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x, y);

			//3. interpolated or extrapolate from/away from ave L
			r = ComponentClamp(factor*(p.r - aveL) + aveL);
			g = ComponentClamp(factor*(p.g - aveL) + aveL);
			b = ComponentClamp(factor*(p.b - aveL) + aveL);

			newPix = Pixel(r, g, b);
			GetPixel(x, y) = newPix;
		}
	}
}


void Image::ChangeSaturation(double factor)
{
	//bounds check s.t. factor = [0, 1]
	if (factor >= -1.0 && factor <= 1.0)
	{
		int x, y;
		Pixel p, newPix;
		Component pL, r, g, b;

		for (x = 0; x < Width(); x++)
		{
			for (y = 0; y < Height(); y++)
			{
				p = GetPixel(x, y);
				pL = p.Luminance();

				r = ((1 - factor)*pL + (factor*p.r));
				g = ((1 - factor)*pL + (factor*p.g));
				b = ((1 - factor)*pL + (factor*p.b));

				newPix = Pixel(r, g, b);
				GetPixel(x, y) = newPix;
			}
		}
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for Saturation.\n");
	}
}


Image* Image::Crop(int x, int y, int w, int h)
{
	//bounds check x and y
	if (x >= 0 && y >= 0 && y < Height() && x < Width())
	{
		Image* newIm = new Image(w, h);
		int xSrc, ySrc, xStep = 1, yStep = 1, xDest = 0, yDest = 0;
		Pixel p;
		bool outOfBounds = false;

		//decrement only if on the far edge(s)
		if (x == Width()-1)
		{
			xStep = -1;
			xDest = w - 1;
		}
		if (y == Height()-1)
		{
			yStep = -1;
			yDest = h - 1;
		}

		for (xSrc = x; (xStep > 0 ? xSrc < (x + w) : xSrc > (x - w)); xSrc += xStep)
		{
			if (xSrc > Width() || xSrc < 0)
			{
				outOfBounds = true;
			}

			for (ySrc = y; (yStep > 0 ? ySrc < (y + h) : ySrc >(y - h)); ySrc += yStep)
			{				
				if (ySrc > Height() || ySrc < 0)
				{
					outOfBounds = true;
				}

				if (outOfBounds)
				{
					p = Pixel(0, 0, 0); //fill in out of bounds as black pixels
				}
				else
				{
					p = GetPixel(xSrc, ySrc);
				}

				newIm->GetPixel(xDest, yDest) = p;
				yDest += yStep;
				outOfBounds = false;
			}
			yStep > 0 ? yDest = 0 : yDest = h - 1; //reset destination y coord
			xDest += xStep;
		}

		return newIm;
	}// bounds check IF
	else
	{
		fprintf(stderr, "\nInvalid argument for Crop.\n");
	}

	return NULL;
}


void Image::ExtractChannel(int channel)
{
	//bounds check so that channel = [0,2]
	if (channel >= 0 && channel < 3)
	{
		int x, y;
		Pixel p, newPix;

		for (x = 0; x < Width(); x++)
		{
			for (y = 0; y < Height(); y++)
			{
				p = GetPixel(x, y);

				switch (channel)
				{
				case 0: //r channel
					newPix = Pixel(p.r, 0, 0);
					break;
				case 1: //g channel
					newPix = Pixel(0, p.g, 0);
					break;
				case 2: //b channel
					newPix = Pixel(0, 0, p.b);
					break;
				default:
					break;
				}

				GetPixel(x, y) = newPix;
			}
		}
	}
}


void Image::Quantize (int nbits, int type)
{
	//bounds check greater than 1
	if (nbits > 1)
	{
		std::vector<float> binVec;
		float diffBetweenBins = 255.0 / (nbits - 1);
		float curValue = 0;

		//1. fill bin values
		for (int i = 0; i < nbits; i++)
		{
			binVec.push_back(curValue);
			curValue += diffBetweenBins;
		}

		//2. loop through pixels
		Pixel p, newPix;
		Component r, g, b;
		int x, y;

		for (x = 0; x < Width(); x++)
		{
			for (y = 0; y < Height(); y++)
			{
				p = GetPixel(x, y);

				//3. determine which bin
				r = ComponentClamp(findBinVal(binVec, p.r, type, x, y));
				g = ComponentClamp(findBinVal(binVec, p.g, type, x, y));
				b = ComponentClamp(findBinVal(binVec, p.b, type, x, y));

				//4. build new pixel
				newPix = Pixel(r, g, b);
				GetPixel(x, y) = newPix;
			}
		}
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for Quantize.\n");
	}
}

static int Bayer4[4][4] =
{
	{ 15,  7, 13,  5 },
	{ 3, 11,  1,  9 },
	{ 12,  4, 14,  6 },
	{ 0,  8,  2, 10 }
};

float Image::findBinVal(std::vector<float> binVec, float currentValue, int type, int i, int j)
{
	float binDiff, threshold;
	int currentBin, n = binVec.size(), truncValue = 0;
	
	for (currentBin = 1; currentBin < n; currentBin++)
	{
		if (currentValue <= binVec.at(currentBin))
		{
			binDiff = abs(binVec.at(currentBin) - binVec.at(currentBin - 1));
			
			if (type == RANDOM_DITHER)				//random dither
			{
				threshold = ComponentRandom();
			}
			else if (type == REGULAR_QUANTIZE)		//quantize
			{
				threshold = binVec.at(currentBin - 1) + (binDiff / 2.0);
			}
			else if (type == ORDERED_DITHER)		//ordered dither
			{
				i = i%4; //x-index into Bayer4
				j = j%4; //y-index into Bayer4
				currentValue = (int)currentValue % 16; //all values within matrix are < 16
				threshold = Bayer4[i][j];
			}
			else
			{
				fprintf(stderr, "\Invalid enum type passed into findBinVal().\n");
			}

			if (currentValue < threshold)
			{
				return binVec.at(currentBin - 1);
			}
			else
			{
				return binVec.at(currentBin);
			}
		}
	}

	return 0;
}

void Image::RandomDither (int nbits)
{
	if (nbits > 1)
	{
		Quantize(nbits, RANDOM_DITHER); //type = RANDOM_DITHER
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for Random Dither.\n");
	}
}


void Image::OrderedDither(int nbits)
{
	if (nbits > 1)
	{
		Quantize(nbits, ORDERED_DITHER); //type = ORDERED_DITHER
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for Ordered Dither.\n");
	}
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

// Wiki page on FloydSteinberg Dithering:
// https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
void Image::FloydSteinbergDither(int nbits)
{
	if (nbits > 1)
	{
		std::vector<float> binVec;
		float diffBetweenBins = 255.0 / (nbits - 1);
		float curValue = 0;

		//1. fill bin values
		for (int i = 0; i < nbits; i++)
		{
			binVec.push_back(curValue);
			curValue += diffBetweenBins;
		}

		//2. loop through pixels
		Pixel p, newPix, pixError;
		Component r, g, b;
		int x, y, w = Width(), h = Height();
		int type = REGULAR_QUANTIZE;

		for (x = 0; x < w; x++)
		{
			for (y = 0; y < h; y++)
			{
				p = GetPixel(x, y);

				//3. determine which bin
				r = ComponentClamp(findBinVal(binVec, p.r, type, x, y));
				g = ComponentClamp(findBinVal(binVec, p.g, type, x, y));
				b = ComponentClamp(findBinVal(binVec, p.b, type, x, y));

				//4. build new pixel
				newPix = Pixel(r, g, b);
				GetPixel(x, y) = newPix;

				//5. find error (difference)
				pixError = p + (-1.0)*newPix;

				//6. distribute error to right/below pixels if not on right/botto edges
				if (x > 0 && x < w - 1 && y < h - 1)
				{
					GetPixel(x + 1, y) = GetPixel(x + 1, y) + (ALPHA * pixError);
					GetPixel(x - 1, y + 1) = GetPixel(x - 1, y + 1) + (BETA * pixError);
					GetPixel(x, y + 1) = GetPixel(x, y + 1) + (GAMMA * pixError);
					GetPixel(x + 1, y + 1) = GetPixel(x + 1, y + 1) + (GAMMA * pixError);
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for FloydSteinberg Dither.\n");
	}
}

float Image::gaussianValue(int x, int y, int kernelSize)
{
	int center = kernelSize / 2;
	//1. find distance from center of kernel (center @ (kernelSize/2 , kernelSize/2)
	//float distance = findDistance(x, y, center, center);

	//2. calculate
	//return (1.0 / sqrt(2 * M_PI)) * exp(-pow(distance, 2) / 2.0);
	x = abs(x - center);
	y = abs(y - center);
	return  (1.0 / (2 * M_PI)) * exp((pow(x, 2) + pow(y,2))/ -2.0);
}

float Image::findDistance(int x1, int x2, int y1, int y2)
{
	float rad = pow(x1 - x2, 2) + pow(y1 - y2, 2);
	return sqrt(rad);
}

void Image::Blur(int n)
{
	if (n % 2 != 0) //odd number dimensions only
	{
		Kernel * ed = new Kernel(n);
		int kSize = ed->getSize();
		int i, j;
		
		//build kernel
		for (i = 0; i < kSize; i++)
		{
			for (j = 0; j < kSize; j++)
			{
				ed->insertValue(i, j, gaussianValue(i, j, n));
			}
		}
		ed->print();

		//Image* newIm = ed->applyKernel(this);
		//copyIntoImage(newIm);
		ed->~Kernel();
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for Blur.\n");
	}
}

void Image::Sharpen(int n)
{
	if (n % 2 != 0) //odd number dimensions only
	{
		Kernel * ed = new Kernel(n);
		int kSize = ed->getSize();
		int i, j;

		//build kernel
		for (i = 0; i < kSize; i++)
		{
			for (j = 0; j < kSize; j++)
			{
				ed->insertValue(i, j, -1.0); //fill with negatives
			}
		}

		Image* newIm = ed->applyKernel(this);
		copyIntoImage(newIm);
		ed->~Kernel();
	}
	else
	{
		fprintf(stderr, "\nInvalid argument for Sharpen.\n");
	}
}

void Image::EdgeDetect()
{
	Kernel* ed = new Kernel();
	int kSize = ed->getSize();
	int i, j;

	//build kernel
	for (i = 0; i < kSize; i++)
	{
		for (j = 0; j < kSize; j++)
		{
			ed->insertValue(i, j, -1.0); //fill with negatives
		}
	}
	ed->insertValue(kSize / 2, kSize / 2, 8.0); //made center 8.0

	Image* newIm = ed->applyKernel(this);
	copyIntoImage(newIm);
	ed->~Kernel();
}

void Image::copyIntoImage(Image* image)
{
	int x, y;
	
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			GetPixel(x, y) = image->GetPixel(x, y);
		}
	}
	
}

Image* Image::Scale(double sx, double sy)
{
	/* WORK HERE */
	return NULL;
}

float Image::rotateX(double angle, float x, float y)
{
	return (x*cos(angle)) - (y*sin(angle));
}

float Image::rotateY(double angle, float x, float y)
{
	return (x*sin(angle)) + (y*cos(angle));
}

Image* Image::Rotate(double angle)
{
	Image* newI;
	int newWidth = 0, newHeight = 0;
	int origWidth = Width(), origHeight = Height();

	//convert from degrees to radians
	angle *= M_PI / 180.0;

	//1. determine size of newI
	//translate from screen/pixel space to image space (image center at (0,0))
	//rotate translated (0,0) corner
	float x1 = rotateX(angle, -0.5*origWidth, -0.5*origHeight);
	float y1 = rotateY(angle, -0.5*origWidth, -0.5*origHeight);
	//rotate translated (w,0) corner
	float x2 = rotateX(angle, 0.5*origWidth, -0.5*origHeight);
	float y2 = rotateY(angle, 0.5*origWidth, -0.5*origHeight);
	//rotate translated (w,h) corner
	float x3 = rotateX(angle, 0.5*origWidth, 0.5*origHeight);
	float y3 = rotateY(angle, 0.5*origWidth, 0.5*origHeight);
	//rotate translated (0,h)
	float x4 = rotateX(angle, -0.5*origWidth, 0.5*origHeight);
	float y4 = rotateY(angle, -0.5*origWidth, 0.5*origHeight);

	//compare diagonals
	int diag1x = (int) ceil(abs(x1 - x3));
	int diag1y = (int) ceil(abs(y1 - y3));
	int diag2x = (int) ceil(abs(x2 - x4));
	int diag2y = (int) ceil(abs(y2 - y4));

	(diag1x > diag2x) ? newWidth = diag1x : newWidth = diag2x;
	(diag1y > diag2y) ? newHeight = diag1y : newHeight = diag2y;

	newI = new Image(newWidth, newHeight);

	//2. fill newI with all black
	for (int i = 0; i < newWidth; i++)
	{
		for (int j = 0; j < newHeight; j++)
		{
			newI->GetPixel(i, j) = Pixel(0, 0, 0);
		}
	}

	//3. rotate all of orig pixels
	int newX = 0.0, newY = 0.0;

	for (int x = 0; x < origWidth; x++)
	{
		for (int y = 0; y < origHeight; y++)
		{
			//rotate translated pixels
			newX = (int) rotateX(angle, x - (0.5*origWidth), y - (0.5*origHeight));
			newY = (int) rotateY(angle, x - (0.5*origWidth), y - (0.5*origHeight));

			//translate back to screen space (pixel space?)
			newX += 0.5*newWidth;
			newY += 0.5*newHeight;

			//fprintf(stderr, "newX: %d   newY: %d\n", newX, newY);

			if (newX < 0 || newX >= newWidth || newY < 0 || newY >= newHeight)
			{
				fprintf(stderr, "***newX || newY out of bounds of newI***\n");
			}
			else
			{
				newI->GetPixel((int)newX, (int)newY) = GetPixel(x, y);
			}
		}
	}

	return newI;
}

void Image::Fun()
{
	/* WORK HERE */
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}


Pixel Image::Sample (double u, double v){
    /* WORK HERE */
	return Pixel();
}